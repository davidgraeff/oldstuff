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
#ifndef DEVICELISTENTHREAD_H_
#define DEVICELISTENTHREAD_H_

#include <QThread>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include "driver-interface.h" // for liri::KeyCode

class DeviceInstance;

class DeviceListenThread : public QThread {
	public:
		DeviceListenThread(DeviceInstance* di);
		~DeviceListenThread();
		void run();
		void listen();
		void remoteReloadCheck();
		int getTimeout();
	private:
		DeviceInstance* di;
		int timeoutListen;
		int timeoutKeyRelease;
		liri::KeyCode lastkey;
		QTimer quittimer;

		/* to not always reallocate */
		QByteArray hexcode;
		QString keyname;

		/* remote */
		QMap< QByteArray, QString > keys;

		inline void keyevent(const liri::KeyCode& key);

};

#endif
