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
#ifndef DEVICEINSTANCE_H_
#define DEVICEINSTANCE_H_

#include <QString>
#include <QObject>
#include <QDBusConnection>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

#include "driver-interface.h"
#include "DeviceListenThread.h"
#include "DeviceSettings.h"

/* For dlopen and driver loading, freeing */
typedef void liriDriver_Free(liri::IDriver* p);
typedef liri::IDriver* liriDriver_Create(DeviceSettings*);

/**
* \brief Instance representing a device
*
*  Responsible for loading the corresponding driver for the device,
*  providing dbus object access to this instance and controlling the
*  thread that listens to the device for key events.
*
* \sa DeviceListenThread, DeviceInitThread
**/
class DeviceInstance : public QObject
{
	Q_OBJECT
	friend class DeviceListenThread;
	friend class DeviceInitThread;
	public:
		DeviceInstance(QDBusConnection* conn, int instanceNr, const QString& udi);
		~DeviceInstance();

		/* release claimed device, release driver
		   if wait is set to false the terminating process is asynchronous.
		   When terminating completed the signal "released" will be emitted. */
		void release();

		/* load driver and start listen thread */
		bool loaddriver();

		/* access */
		const QString getUdi() const;
		const int getInstanceNr() const;
		DeviceSettings* deviceSettings();

		/* states */
		int ReceiverState();
		int RemoteState();
		void updateReceiverState(int receiverState);
		void updateRemoteState(int remoteState);

		void reloadAssociatedRemote();
		void setAssociatedRemote(const QString &remoteid);

		/* settings manipulation */
		QMap<QString,QString> getAllSettings();
		QStringList getSettings(const QStringList &keys);
		void setSettings(const QMap<QString,QString> &settings);

		void configAssociatedRemote();

	private:
		QString getConfigPath();
		void p_reloadAssociatedRemote();

	Q_SIGNALS:
		// a signal to the list to remove this instance
		void releasedDevice(DeviceInstance* di);
		void driverChangedSettings(const QMap<QString,QString> &changedsettings);
		void key(const QString &keycode, const QString &keyname, uint channel, int pressed);
		void receiverStateChanged(int state);
		void remoteStateChanged(int state);

	public Q_SLOTS:
		// called by the listen thread when it finished
		void listenfinished();

	private:
		// system bus connection
		QDBusConnection* conn;
		// device instance id, eg 1..n, 0=invalid
		int instanceNr;
		// settings and logging for communicating with pure c++-drivers
		DeviceSettings psettings;
		// the listen thread
		DeviceListenThread* listenThread;

		/* states */
		QString udi;
		int receiverState;
		int remoteState;
		QReadWriteLock lockReceiverState;
		QReadWriteLock lockRemoteState;

		/* dlopen, driver loading and freeing */
		liri::IDriver* driver;
		liriDriver_Free* free_driver;
		void* driverhandle;
};

#endif
