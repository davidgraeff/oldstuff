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
#ifndef DEVICEPLUGGING_H_
#define DEVICEPLUGGING_H_

#include <QString>
#include <QObject>
#include <QMap>
#include <QSocketNotifier>

class DeviceInstance;
struct udev_monitor;
struct udev;
struct udev_device;

enum ManagedDeviceListStateEnum {
	Invalid,
	Valid,
	ValidWithoutMonitoring
};

class ManagedDeviceList : public QObject
{
	Q_OBJECT
	public:
		ManagedDeviceList();
		~ManagedDeviceList();
		void setShutdownBehaviour(bool shutdownWithLastInstance);
		bool getShutdownBehaviour();
		ManagedDeviceListStateEnum getState();
		void processDevice(struct udev_device *dev);
	Q_SIGNALS:
		void deviceAdded(const QString& uid);
		void deviceRemoved(const QString& uid);
	private Q_SLOTS:
		void deviceReleased(DeviceInstance* di);
		void udevActivity ( int socket );
	public Q_SLOTS:
		void shutdown();
	private:
		bool m_shutdownWithLastInstance;
		/* All device instances. Accessible through their unique sys path */
		QMap<QString, DeviceInstance*> m_devices;
		QSocketNotifier* m_udevMonitorNotifier;
		ManagedDeviceListStateEnum m_state;
		struct udev_monitor *udev_mon;
		struct udev *udev;
		int udev_monitor_fd;
};

#endif