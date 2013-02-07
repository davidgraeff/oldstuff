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
#define LIRI_LOGFILE LIRI_LOGFILE_EXECUTION
#include "logging.h"
#include "Control.h"
#include "DeviceList.h"
#include "businterconnect/BusConnection.h"

#include <QDebug>
#include <QCoreApplication>
#include <QStringList>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <stdlib.h> // EXIT_FAILURE

void copyfiles(const QString& system, const QString& user, const QString& filter, bool replace) {
		QStringList temp;
		QDir systemdir(system);
		QDir userdir = QDir::homePath() + QLatin1String("/") + user;
		userdir.mkpath(userdir.path());
		temp = systemdir.entryList(QStringList()<<filter, QDir::Readable | QDir::NoDotAndDotDot | QDir::Files);
		foreach (QString file, temp) {
			if (replace) QFile::remove(userdir.absoluteFilePath(file));
			if (!QFile::exists(userdir.absoluteFilePath(file))) {
				QFile::copy(systemdir.absoluteFilePath(file), userdir.absoluteFilePath(file));
				qDebug() << "Copy to" << userdir.absoluteFilePath(file);
			}
		}
}

int main( int argc, char * argv[]) {
	int exitcode = 0;

	/* install message handlers */
	qInstallMsgHandler(liriMessageOutput);

	/* command line arguments */
	QCoreApplication app(argc, argv);

	/* help command line argument? */
	if (QCoreApplication::arguments().contains(QLatin1String("--help"))) {
		std::cout << "This program belongs to the liri framework.\nliriexec will execute all commands as the current user and register to the session bus of that user.\nIt reacts to loaded remote files of the lirid process and loads appropriate desktop and application profiles.\nCommand line options: --help --firststart --replace --exitaftercopy" << std::endl;
		return EXIT_SUCCESS;
	}

	qDebug() << ABOUT_SUMMARY << ABOUT_VERSION;

	/* settings */
	QString settingsfile = QDir::home().absoluteFilePath(QLatin1String(LIRI_USER_CONFIG_FILE));
	QSettings settings(settingsfile, QSettings::IniFormat);
	qDebug() << "Settings file" << settingsfile;

	/* copy desktop/application profiles and loader files */
	if (QCoreApplication::arguments().contains(QLatin1String("--firststart"))
		|| !settings.value(QLatin1String("firststarted")).toBool()) {
		settings.setValue(QLatin1String("firststarted"), true);

		bool replace = QCoreApplication::arguments().contains(QLatin1String("--replace"));
		qDebug() << "First start! Copying profile files to user directory. Replace:" << replace;

		qDebug() << "Copy loader files";
		copyfiles(QLatin1String(LIRI_SYSTEM_REMOTE_LOAD_PROFILES_DIR),
			QLatin1String(LIRI_HOME_REMOTE_LOAD_PROFILES_DIR), QLatin1String("*.loader"), replace);

		qDebug() << "Copy desktop-profile files";
		copyfiles(QLatin1String(LIRI_SYSTEM_DESKTOPPROFILES_DIR),
			QLatin1String(LIRI_HOME_DESKTOPPROFILES_DIR), QLatin1String("*.desktop"), replace);

		qDebug() << "Copy application-profile files";
		copyfiles(QLatin1String(LIRI_SYSTEM_APPPROFILES_DIR),
			QLatin1String(LIRI_HOME_APPPROFILES_DIR), QLatin1String("*.desktop"), replace);
	}

	if (QCoreApplication::arguments().contains(QLatin1String("--exitaftercopy"))) {
		return EXIT_SUCCESS;
	}

	/* bus */
	QDBusConnection conn = QDBusConnection::connectToBus(QDBusConnection::SessionBus, QLatin1String(LIRI_DBUS_SERVICE_EXECUTION));
	if ( !conn.isConnected() ) {
		qWarning() << "DBus Error:" << conn.lastError().name() << conn.lastError().message();
		exitcode = -1;
	} else if (!conn.registerService(QLatin1String(LIRI_DBUS_SERVICE_EXECUTION)) ) {
		qWarning() << "Another instance of this program is already running.";
		if (conn.lastError().name().size())
			qWarning() << "Error:" << conn.lastError().name() << conn.lastError().message();
		exitcode = -1;
	}

	/* start execution engine */
	Control* control;
	DeviceList* devicelist;
	BusConnection* connection;
	if (!exitcode) {
		/* framework bus connection */
		connection = new BusConnection();

		devicelist = new DeviceList(&conn, connection);

		control = new Control(&conn, devicelist);

		control->connect(devicelist, SIGNAL(targetChanged(int, const QString &)),
			SIGNAL(targetChanged(int, const QString &)));
		control->connect(devicelist, SIGNAL(executed(int, int, const QString &)),
			SIGNAL(executed(int, int, const QString &)));
		control->connect(devicelist, SIGNAL(modeChanged(int, const QString &, const QString &)),
			SIGNAL(modeChanged(int, const QString &, const QString &)));
		control->connect(devicelist, SIGNAL(deviceAddedExecution(int)),
			SIGNAL(deviceAddedExecution(int)));
		control->connect(devicelist, SIGNAL(deviceRemovedExecution(int)),
			SIGNAL(deviceRemovedExecution(int)));
		control->connect(devicelist, SIGNAL(profilesLoaded(int)),
			SIGNAL(profilesLoaded(int)));
	}

	// mainloop
	if ( !exitcode ) {
		exitcode = QCoreApplication::exec();

		qDebug() << "Clean up";
		delete control;
		delete devicelist;
		delete connection;
	}


	return exitcode;
}
