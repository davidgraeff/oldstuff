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
#include "config.h"
#define LIRI_LOGFILE LIRI_LOGFILE_DAEMON
#include "logging.h"
#include "Control.h"
#include "ManagedDeviceList.h"

#include <QDebug>
#include <QCoreApplication>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QStringList>
#include <signal.h> // for the signals
#include <stdlib.h> // EXIT_FAILURE
#include <stdio.h>

int main( int argc, char * argv[]) {
	/* install message handlers */
	qInstallMsgHandler(liriMessageOutput);

	/* command line arguments */
	QCoreApplication app(argc, argv);

	/* help command line argument? */
	if (QCoreApplication::arguments().contains(QLatin1String("--help"))) {
		std::cout << "This program belongs to the liri framework.\nlirid must run with root privileges to access usb and firewire devices as well as system configuration files and the system bus.\nIt manages all connected devices and propagate receiver events (key press, connecting, disconnecting) over the system bus.\nIf an appropriate remote descripion file is installed, lirid will also provide symbolic names (such as #PLAY) and not only keycodes." << std::endl;
		return EXIT_SUCCESS;
	}

	qDebug() << ABOUT_SUMMARY << ABOUT_VERSION;

	/* init dbus */
	QDBusConnection conn = QDBusConnection::connectToBus(QDBusConnection::SystemBus, QLatin1String(LIRI_DBUS_SERVICE_DEVMAN));
	if ( !conn.isConnected() ) {
		qWarning() << "DBus: Connection failed!";
		return -1;
	}

	/* claim name */
	if ( !conn.registerService(QLatin1String(LIRI_DBUS_SERVICE_DEVMAN)) ) {
		qWarning() << "DBus: Service registering failed! Propably another instance is already running?!" << conn.lastError().message();
		return -2;
	}

	/* list of receiver instances */
	Control* control = new Control(&conn);
	ManagedDeviceList* devicelist = new ManagedDeviceList();
	devicelist->connect(control,SIGNAL(shutdown()), SLOT(shutdown()));
	control->connect(devicelist, SIGNAL( deviceAdded(int) ), SIGNAL( deviceAdded(int) ));
	control->connect(devicelist, SIGNAL( deviceRemoved(int) ), SIGNAL( deviceRemoved(int) ));

	// mainloop
	int exitcode = QCoreApplication::exec();

	/* control clean up */
	qDebug() << "Shutting down: Main Controller";
	delete control;

	//* inform all drivers about our exit, wait for them and clean up */
	qDebug() << "Shutting down: Device List";
	delete devicelist;

	/* dbus clean */
	qDebug() << "Shutting down: Dbus Connection";
	conn.unregisterService(QLatin1String(LIRI_DBUS_SERVICE_DEVMAN));
	QDBusConnection::disconnectFromBus(QLatin1String(LIRI_DBUS_SERVICE_DEVMAN));

	return exitcode;
}
