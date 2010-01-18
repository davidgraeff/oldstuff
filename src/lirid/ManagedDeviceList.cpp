/****************************************************************************
** This file is part of the linux remotes project
**
** Use this file under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#include <QDebug>
#include <QCoreApplication>
extern "C"
{
#include <libudev.h>
#include <poll.h>
#include <errno.h>
}

#include "ManagedDeviceList.h"
#include "device/DeviceInstance.h"
#include "config.h"

ManagedDeviceList::ManagedDeviceList() : m_shutdownWithLastInstance(false) {
	
	setObjectName(QLatin1String("ManagedDeviceList"));
	
	// connect to udev and set up monitor for new/removed receivers
	udev_mon = 0;
	udev = 0;
	udev_monitor_fd = 0;
	m_udevMonitorNotifier = 0;

	udev = udev_new ();
	if (udev == 0) {
		m_state = Invalid;
		qWarning() << "UDev failed:" << errno;
		return;
	}

	udev_mon = udev_monitor_new_from_netlink (udev, "udev");
	if (udev_mon == 0)
	{
		m_state = ValidWithoutMonitoring;
		qWarning() << "UDev monitor failed:" << errno;
		// Add filter to only receive events for the usb subsystem
	} else if (udev_monitor_filter_add_match_subsystem_devtype (udev_mon, "usb", NULL) != 0) {
		qWarning() << "UDev monitor filter failed:" << errno;
		udev_monitor_unref (udev_mon);
		// Enable receiving of udev events
	} else if (udev_monitor_enable_receiving (udev_mon) < 0) {
		qWarning() << "UDev monitor enabling failed:" << strerror(errno);
		udev_monitor_unref (udev_mon);
		udev_mon = 0;		
		// Get file descriptor
	} else if ((udev_monitor_fd = udev_monitor_get_fd(udev_mon)) < 0) {
		qWarning() << "Failed to get udev monitor fd.";
		udev_monitor_unref (udev_mon);
		udev_mon = 0;
		// Watch file descriptor
	} else {
		m_udevMonitorNotifier = new QSocketNotifier(udev_monitor_fd, QSocketNotifier::Read);
		connect(m_udevMonitorNotifier, SIGNAL(activated(int)), SLOT(udevActivity(int)));
		m_udevMonitorNotifier->setEnabled(true);
	}

	// enumerate all existing devices and add receivers
	struct udev_enumerate *devenum = udev_enumerate_new (udev);
	if (devenum == 0) {
		m_state = Invalid;
		return;
	}
	if (udev_enumerate_add_match_subsystem (devenum, "usb"))
	{
		udev_enumerate_unref (devenum);
		m_state = Invalid;
		return;
	}

	udev_enumerate_scan_devices (devenum);
	struct udev_list_entry *devlist = udev_enumerate_get_list_entry (devenum);
	struct udev_list_entry *deventry;
	udev_list_entry_foreach (deventry, devlist)
	{
		const char *path = udev_list_entry_get_name (deventry);
		struct udev_device *dev = udev_device_new_from_syspath (udev, path);
		/* DO SOMETHING */
		processDevice(dev);
		udev_device_unref (dev);
	}
	udev_enumerate_unref (devenum);

	m_state = Valid;
}

ManagedDeviceList::~ManagedDeviceList() {
	delete m_udevMonitorNotifier;
	close (udev_monitor_fd);
	
    if (udev_mon)
        udev_monitor_unref(udev_mon);

    if (udev)
        udev_unref(udev);

	udev = 0;
	udev_mon = 0;
	
	qDeleteAll(m_devices);
}

ManagedDeviceListStateEnum ManagedDeviceList::getState()
{
	return m_state;
}

void ManagedDeviceList::setShutdownBehaviour(bool shutdownWithLastInstance)
{
	m_shutdownWithLastInstance = shutdownWithLastInstance;
}

bool ManagedDeviceList::getShutdownBehaviour()
{
	return m_shutdownWithLastInstance;
}

void ManagedDeviceList::shutdown() {
	m_shutdownWithLastInstance = true;
	if (m_devices.size() == 0)
		QCoreApplication::exit(0);
	else {
		foreach(DeviceInstance* device, m_devices)
			device->release();
	}
}

void ManagedDeviceList::udevActivity ( int socket )
{
	Q_UNUSED(socket);
	struct udev_device *dev;
	if (!(dev = udev_monitor_receive_device(udev_mon))) {
		qDebug() <<"Failed to get udev device object from monitor.";
		return;
	}

	const char* devpath = udev_device_get_devpath(dev);
	if (!QString::fromAscii(devpath).startsWith(QLatin1String("/devices")))
		return;

	processDevice(dev);
	udev_device_unref(dev);
}

void ManagedDeviceList::processDevice(struct udev_device *dev)
{
    Q_ASSERT(dev);
	// Ignore child devices
	if (udev_device_get_property_value(dev, "ID_VENDOR_ID") == 0) return;

	// Ignore devices not belonging to liri
	const char* driver = udev_device_get_property_value(dev, "liri_receiver_driver");
	if (!driver) return;

    const char* action = udev_device_get_action(dev);
	const char* uid_t = udev_device_get_property_value(dev, "DEVPATH");
	if (!uid_t) return;
	const QString uid = QString::fromAscii(uid_t);

	if (strcmp(action,"remove")==0) {
		/* get device */
		DeviceInstance* device = m_devices.value(uid);
		if (!device) return;
		/* release device */
		device->release();
	} else { // add device
		DeviceInstance* device = m_devices.value(uid);
		// if device already present, do nothing */
		if (device) return;
		// create new device object with settings
		device = new DeviceInstance(uid);
		// Collect all udev information about the device and forward that
		// to the device object
		QMap<QString, QString> settings;
		struct udev_list_entry *proplist = udev_device_get_properties_list_entry(dev);
		while (proplist)
		{
			device->setSetting(QString::fromAscii(udev_list_entry_get_name(proplist)),
						QString::fromAscii(udev_list_entry_get_value(proplist)));
			proplist = udev_list_entry_get_next(proplist);
		}
		device->setSetting(QLatin1String("uid"),uid);
		connect(device, SIGNAL( releasedDevice(DeviceInstance*) ),
				SLOT( removeDevice(DeviceInstance*) ));

		/* add to list */
		m_devices.insert(uid, device);

		emit deviceAdded(uid);

		device->loaddriver();
	}
	

	qDebug() << "event" << "ID_VENDOR_ID" << udev_device_get_property_value(dev, "ID_VENDOR_ID") << action;
}

/** Remove device object from managed devices and release used ressources.

This method is called by the device object itself to signal that it no longer
is using any hardware ressources (or timed out in releasing those ressources)
and is ready to be removed.
*/
void ManagedDeviceList::deviceReleased(DeviceInstance* device) {
	if (!device) return;

	m_devices.remove(device->getUid());

	emit deviceRemoved(device->getUid());

	/* free memory */
	delete device;

	if (m_devices.isEmpty() && m_shutdownWithLastInstance)
		QCoreApplication::exit(0);
}
