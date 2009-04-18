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
#include <QDBusConnectionInterface>
#include <QDBusConnection>
#include "HalManagedDeviceList_dbusproxy.h"
#include "HalManagedDeviceList.h"
#include "device/DeviceInstance.h"
#include "config.h"

HalManagedDeviceList::HalManagedDeviceList(QDBusConnection* conn) : cancel(false), conn(conn), instanceCounter(1) {
	setObjectName(QLatin1String("HalManagedDeviceList"));

	qRegisterMetaType< QMap<QString,QString> >("QMap<QString,QString>");

	halinterface = 0;

	QDBusConnectionInterface *iface = QDBusConnection::systemBus().interface();
	connect (iface, SIGNAL(serviceUnregistered(const QString&)),
		this, SLOT(slotServiceUnregisteredSystem(const QString&)));

	connect (iface, SIGNAL(serviceRegistered(const QString&)),
		this, SLOT(slotServiceRegisteredSystem(const QString&)));

	connect (iface, SIGNAL(serviceOwnerChanged(const QString&,const QString&,const QString&)),
		this, SLOT(slotServiceOwnerChangedSystem(const QString&,const QString&,const QString&)));

	if (iface) {
		const QStringList serviceNames = iface->registeredServiceNames();
		foreach (QString service, serviceNames)
			slotServiceRegisteredSystem(service);
	}
}

HalManagedDeviceList::~HalManagedDeviceList() {
	delete halinterface;
	qDeleteAll(instances);
}

void HalManagedDeviceList::slotServiceOwnerChangedSystem(const QString& service,const QString& olds,const QString& news) {
	if (olds.isEmpty())
		slotServiceRegisteredSystem(service);
	else if (news.isEmpty())
		slotServiceUnregisteredSystem(service);
}

void HalManagedDeviceList::slotServiceUnregisteredSystem(const QString& service) {
	if (QLatin1String(LIRI_DBUS_SERVICE_HAL) != service)
		return;
	delete halinterface;
	halinterface = 0;
}

void HalManagedDeviceList::slotServiceRegisteredSystem(const QString& service) {
	if (QLatin1String(LIRI_DBUS_SERVICE_HAL) != service)
		return;

	delete halinterface;
	halinterface = new OrgFreedesktopHalManagerInterface(QLatin1String(LIRI_DBUS_SERVICE_HAL), QLatin1String(LIRI_DBUS_OBJECT_HAL), *conn );
	if (!halinterface->isValid()) {
		qWarning() << "Failed to contact HAL!";
		qDebug() << halinterface->lastError();
		delete halinterface;
		halinterface = 0;
	} else {
		/* connect signals for new/removed receivers */
		connect(halinterface, SIGNAL(DeviceAdded(const QString &)), this, SLOT(halDeviceAdded(const QString&)));
		connect(halinterface, SIGNAL(DeviceRemoved(const QString &)), this, SLOT(halDeviceRemoved(const QString&)));
		/* walk through all existing receivers */
		QStringList paths = halinterface->FindDeviceByCapability(QLatin1String("receiver"));
		foreach (QString path, paths) {
			halDeviceAdded(path);
		}
	}
}

bool HalManagedDeviceList::connectedToHal() {
	return (halinterface != 0);
}

void HalManagedDeviceList::shutdown() {
	cancel = true;
	foreach(DeviceInstance* device, instances)
		device->release();
}

void HalManagedDeviceList::halDeviceAdded(const QString& udi) {
	/* we don't take all */
	QString udii = udi.mid(udi.lastIndexOf(QLatin1Char('/'))+1);
	if (!udi.size() || !udii.size()) return;

	/* filter: we already have the parent node, ignore childs */
	foreach(DeviceInstance* instance, instances) {
		if (instance->ReceiverState() == LIRI_DEVICE_RUNNING &&
			instance->getUdi().leftRef(udi.size()) == udi)
			 return;
	}

	/* dbus hal connection to device interface */
 	QDBusInterface remoteApp( QLatin1String(LIRI_DBUS_SERVICE_HAL), QLatin1String(LIRI_DBUS_OBJECT_HALdev"/")+udii, QLatin1String(LIRI_DBUS_INTERFACE_HALdev), *conn );

	/* filter: only devices with capability "receiver" are allowed */
	if (!remoteApp.call( QLatin1String("QueryCapability"), QLatin1String("receiver") ).arguments()[0].toBool()) return;

	/* get settings from hal */
	QDBusReply<QVariantMap> reply = remoteApp.call( QLatin1String("GetAllProperties") );
	if ( !reply.isValid() ) {
		qWarning() << "DetectionWithHal: Get hal settings failed. " << reply.error().name()
		<< ", " << reply.error().message();
		return;
	}

	// create a driver instance
	DeviceInstance* device;
	device = new DeviceInstance(conn, instanceCounter++, udi);
	DeviceSettings* devset = device->deviceSettings();

	/* hal settings -> driver settings */
	QVariantMap halsettings = reply.value();
	QVariantMap::const_iterator i = halsettings.constBegin();
	while (i != halsettings.constEnd()) {
		devset->set(i.key(), i.value().toString());
		++i;
	}

	/* set remotereceiver.id! Important for loading the approriate remote */
	QString receiverID;
	receiverID = devset->get(QLatin1String("remotereceiver.driver"));
	receiverID += QLatin1String(".");
	receiverID += devset->get(QLatin1String("usb_device.vendor_id"));
	receiverID += QLatin1String(".");
	receiverID += devset->get(QLatin1String("usb_device.product_id"));
	devset->set(QLatin1String("remotereceiver.did"), receiverID);
	receiverID += QLatin1String(".")+QString::number(device->getInstanceNr());
	devset->set(QLatin1String("remotereceiver.duid"), receiverID);

	/* connect signals/slots */
	connect(device, SIGNAL( releasedDevice(DeviceInstance*) ),
			SLOT( removeDevice(DeviceInstance*) ));

	/* add to list */
	instances.insert(device->getInstanceNr(), device);

	emit deviceAdded(device->getInstanceNr());

	device->loaddriver();
}

void HalManagedDeviceList::halDeviceRemoved(const QString& udi) {
	/* get device */
	DeviceInstance* device = 0;

	foreach(DeviceInstance* instance, instances)
		if (instance->getUdi() == udi) {
			device = instance;
			break;
		}
	if (!device) return;

	/* release device */
	device->release();
}

void HalManagedDeviceList::removeDevice(DeviceInstance* device) {
	if (!device) return;

	instances.remove(device->getInstanceNr());

	emit deviceRemoved(device->getInstanceNr());

	/* free memory */
	delete device;

	if (instances.isEmpty() && cancel) QCoreApplication::exit(0);
}
