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

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusObjectPath>

class OrgFreedesktopHalManagerInterface;
class DeviceInstance;

class HalManagedDeviceList : public QObject
{
	Q_OBJECT
	public:
		HalManagedDeviceList(QDBusConnection* conn);
		~HalManagedDeviceList();
		bool connectedToHal();
		void shutdown();
	Q_SIGNALS:
		void deviceAdded(int rid);
		void deviceRemoved(int rid);
	private Q_SLOTS:
		void halDeviceAdded(const QString& udi);
		void halDeviceRemoved(const QString& udi);
		void slotServiceUnregisteredSystem(const QString&);
		void slotServiceRegisteredSystem(const QString&);
		void slotServiceOwnerChangedSystem(const QString&,const QString&,const QString&);
		void removeDevice(DeviceInstance* di);
	private:
		bool cancel;
		QDBusConnection* conn;
		/* All device instances. Accessible through their instanceNr */
		QMap<int, DeviceInstance*> instances;
		int instanceCounter;
		OrgFreedesktopHalManagerInterface* halinterface;
};

#endif