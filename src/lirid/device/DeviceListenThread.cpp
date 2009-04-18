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
#include "DeviceListenThread.h"
#include "DeviceInitThread.h"
#include "DeviceInstance.h"
#include "fileformats/RemoteFile.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include "config.h"

DeviceListenThread::DeviceListenThread(DeviceInstance* di) : di(di) {
	setObjectName(QLatin1String("DeviceListenThread")+QString::number(di->getInstanceNr()));
	timeoutKeyRelease = 0;
	timeoutListen = 0;
	memset(&lastkey, 0, sizeof(lastkey));
}

DeviceListenThread::~DeviceListenThread() {

}

int DeviceListenThread::getTimeout() {
	return timeoutListen;
}

void DeviceListenThread::run() {

	/* set some timeout values */
	// minimum LIRI_DRIVERINIT_TIMEOUT ms for the driver init timeout
	int timeoutInit = 0;
	if (di->deviceSettings()->has(QLatin1String("remotereceiver.timeout.init")))
		timeoutInit = di->deviceSettings()->get( QLatin1String("remotereceiver.timeout.init") ).toInt();
	if (timeoutInit < LIRI_DRIVERINIT_TIMEOUT) timeoutInit = LIRI_DRIVERINIT_TIMEOUT;

	// minimum LIRI_DRIVERLISTEN_TIMEOUT ms for the driver init timeout
	if (di->deviceSettings()->has(QLatin1String("remotereceiver.timeout.listen")))
		timeoutListen = di->deviceSettings()->get( QLatin1String("remotereceiver.timeout.listen") ).toInt();
	if (timeoutListen < LIRI_DRIVERLISTEN_TIMEOUT) timeoutListen = LIRI_DRIVERLISTEN_TIMEOUT;

	// minimum 50ms for the key release event timeout
	if (di->deviceSettings()->has(QLatin1String("remotereceiver.timeout.keyrelease")))
		timeoutKeyRelease = di->deviceSettings()->get( QLatin1String("remotereceiver.timeout.keyrelease") ).toInt();
	if (timeoutKeyRelease < 50) timeoutKeyRelease = 50;

	/* init device */
	DeviceInitThread* initThread = new DeviceInitThread(di);
	initThread->start();
	initThread->wait(timeoutInit);
	initThread->terminate();
	initThread->wait();
	int result = initThread->result;
	delete initThread;
	initThread = 0;

	di->updateReceiverState(result);

	if (result != LIRI_DEVICE_RUNNING) {
		qDebug() << "RID:" << di->getInstanceNr() << ", Device initialisating failed. Errorcode: " << result;
		return;
	}

	qDebug() << "RID:" << di->getInstanceNr() << ", Device initialised successfully";

	/* load remote */
	di->updateRemoteState(LIRI_REMOTE_RELOAD);

	/* listen to the device */
	listen();
}

void DeviceListenThread::remoteReloadCheck() {
	int remoteState = di->RemoteState();

	/* do we need to reload the remote? */
	if (remoteState != LIRI_REMOTE_RELOAD) {
		return;
	}

	/* get filename of the remote file */
	QString olduid = di->psettings.get(QLatin1String("remote.uid"));
	QString remoteuid = di->psettings.get(QLatin1String("remote.new.uid"));

	keys.clear();

	if (olduid.size() && !remoteuid.size()) {
		/* Unload */
		di->updateRemoteState(LIRI_REMOTE_UNLOADED);
		return;
	} else if (!remoteuid.size()) {
		/* Not specified */
		di->updateRemoteState(LIRI_REMOTE_NO);
		return;
	}

	/* load remote */
	RemoteFile remote(remoteuid);
	if (remote.getState() == DesktopFile::Valide)
	/* Loaded/Reloaded */
	{
		int size = remote.countKeys();
		// fill map with keys
		for (int i=0;i<size;++i) {
			keys.insert(remote.getKey(i).first.toAscii(), remote.getKey(i).second);
		}
		di->psettings.set(QLatin1String("remote.uid"), remote.getUid());
		di->updateRemoteState((olduid!=remote.getUid()) ? LIRI_REMOTE_LOADED: LIRI_REMOTE_RELOADED);
		qDebug() << "RID:" << di->getInstanceNr() << ", Remote loaded" << remoteuid;
	} else
	/* No success with loading the remote file */
	{
		qWarning() << "RID:" << di->getInstanceNr() << ", Remote uid invalid:" << remoteuid;
		di->updateRemoteState(LIRIERR_filename);
	}
}

void DeviceListenThread::keyevent(const liri::KeyCode& key) {
	/* get hex code */
	hexcode = QByteArray(key.keycode,key.keycodeLen).toHex();

	if (di->RemoteState() == LIRI_REMOTE_LOADED|| di->RemoteState() == LIRI_REMOTE_RELOADED) {
		keyname = keys[hexcode];
	} else {
		keyname.clear();
	}

	emit di->key(QString::fromAscii(hexcode), keyname, key.channel, key.pressed);
}

/* listening: listen thread */
void DeviceListenThread::listen() {
	liri::KeyCode key;
	int rState;
	int timeout = timeoutListen;

	while (1) {
		/* remotes */
		remoteReloadCheck();

		/* read from device */
		key = di->driver->listen(timeout);
		rState = di->ReceiverState();

		/* Abort Conditions */
		if (rState == LIRI_DEVICE_CANCEL) break;
		if (key.state < 0) {
			rState = key.state;
			break;
		}

		if (di->psettings.hasDebugText()) {
			qWarning() << di->psettings.getNextDebugText().c_str();
		}

		/* key event and synthetic key release event */
		if (key.state == 1 && lastkey.state == 1) {
			timeout = timeoutKeyRelease;
			if ( key != lastkey ) {
				lastkey.pressed = 0;
				keyevent(lastkey);
				keyevent(key);
				lastkey = key;
			}
		/* just synthetic key release event; no new key pressed */
		} else if (key.state == 0 && lastkey.state == 1) {
			lastkey.state = 0;
			lastkey.pressed = 0;
			timeout = timeoutListen;
			keyevent(lastkey);
		/* no lastkey event just the new one */
		} else if (key.state == 1 && lastkey.state == 0) {
			lastkey = key;
			timeout = timeoutKeyRelease;
			keyevent(key);
		}
	}

	/* if an error occurred and after 1 second still no LIRI_DEVICE_CANCELevent arrived
	deliver the error state to the devicelist */
	if (rState != LIRI_DEVICE_CANCEL) {
		msleep(1000);
	}

	if (di->ReceiverState() != LIRI_DEVICE_CANCEL) {
		di->updateReceiverState(rState);
	}
}
