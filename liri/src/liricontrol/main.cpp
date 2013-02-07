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
#include "ListenObject.h"

#include <QDebug>
#include <QCoreApplication>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include "businterconnect/BusConnection.h"
#include "businterconnect/Control_DevManager_dbusproxy.h"
#include "businterconnect/Control_Execution_dbusproxy.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"
#include "businterconnect/Receiver_Execution_dbusproxy.h"
#include "fileformats/RemoteFile.h"

#include <stdlib.h> // EXIT_FAILURE
#include <iostream>

/* command line arguments
   --devices: list of connected devices and state infos
   --drivers: installed drivers
   --remotes: installed remotes
   --reload-remotes: reload remotes for all devices
   --reload-profiles: reload profiles for all devices
   --quit-lirid: quit device daemon
   --quit-liriexec: quit liri execution engine
   --set-remoteuid: set default remote for a device
   -d DEV_NUM: all device operations are only for this device
*/

BusConnection* connection;

int exitnow(int result) {
	delete connection;
	connection = 0;
	return result;
}

void nodevicemanager() {
	qDebug() << "Device manager is not running!";
}

int main( int argc, char * argv[]) {
	/* command line arguments */
	QCoreApplication app(argc, argv);

	/* print out version of the program and information about running lirid, liriexec */
	connection = new BusConnection();

	bool did = false;
	if (QCoreApplication::arguments().contains(QLatin1String("--help"))) {
		qDebug() << "Usage: liricontrol [options] [DEV_NUM]";
		qDebug() << "  DEV_NUM: Operate only on this device. Look up the DEV_NUM with --devices";
		qDebug() << "  Options:";
		qDebug() << "  --help\t\tThis help";
		qDebug() << "  --messages\t\tLeave with CTRL-C; Show runtime messages of the framework";
		qDebug() << "  --devices\t\tDisplay all connected devices";
		qDebug() << "  --drivers\t\tDisplay all installed drivers";
		qDebug() << "  --remotes\t\tDisplay all installed remotes and their UIDs";
		qDebug() << "  --reload-remote\tReload remotes for DEV_NUM";
		qDebug() << "  --reload-profiles\tReload profiles for DEV_NUM";
		qDebug() << "  --quit-lirid\t\tQuit the device manager";
		qDebug() << "  --quit-liriexec\tQuit the execution engine";
		qDebug() << "  --status-devman\tDevice manager status: {STARTTIME} or NOTRUNNING";
		qDebug() << "  --status-execution\tExecution engine status: {STARTTIME} or NOTRUNNING";


		return exitnow(EXIT_SUCCESS);
	}

	if (QCoreApplication::arguments().contains(QLatin1String("--status-devman"))) {
		if (connection->deviceManagerState() == LIRI_RUNNING) {
			QString tmp = connection->getDeviceManagerControl()->started();
			std::cout << QDateTime::fromString(tmp, Qt::ISODate).toString(Qt::SystemLocaleShortDate).toAscii().data();
		} else
			std::cout << "NOTRUNNING";
		return exitnow(EXIT_SUCCESS);
	}

	if (QCoreApplication::arguments().contains(QLatin1String("--status-execution"))) {
		if (connection->executionEngineState() == LIRI_RUNNING) {
			QString tmp = connection->getExecutionEngineControl()->started();
			std::cout << QDateTime::fromString(tmp, Qt::ISODate).toString(Qt::SystemLocaleShortDate).toAscii().data();
		} else
			std::cout << "NOTRUNNING";
		return exitnow(EXIT_SUCCESS);
	}

	qDebug() << ABOUT_SUMMARY << ABOUT_VERSION;

	//get DEV_NUM
	QString device;
	if (QCoreApplication::arguments().last()[0].isNumber()) device = QCoreApplication::arguments().last();

	if (QCoreApplication::arguments().contains(QLatin1String("--devices"))) {
		did = true;
		if (connection->deviceManagerState()==LIRI_RUNNING) {
			QStringList instancelist = connection->receivers();
			foreach(QString instance, instancelist) {
				OrgLiriDevManagerReceiverInterface * const ri = connection->getDeviceManagerReceiver(instance);
				if (!ri) continue;
				QStringList keys;
				keys << QLatin1String("udi") << QLatin1String("remotereceiver.id") <<
					QLatin1String("driver.id") << QLatin1String("driver.version") <<
					QLatin1String("driver.blocking") << QLatin1String("remote.uid");
				QStringList values = ri->getSettings(keys);
				if (values.size() != keys.size()) continue;
				RemoteFile remotefile(values[5]);
				qDebug() << "Device Instance" << instance;
				qDebug() << "\tReceiver status:" << ri->ReceiverState();
				qDebug() << "\tRemote status:" << ri->RemoteState();
				qDebug() << "\tHAL udi:" << values[0];
				qDebug() << "\tReceiver id:" << values[1];
				qDebug() << "\tDriver id:" << values[2];
				qDebug() << "\tDriver version:" << values[3];
				qDebug() << "\tDriver blocking:" << values[4];
				qDebug() << "\tRemote uid:" << values[5];
				qDebug() << "\tRemote name:" << remotefile.getName();
			}
		} else {
			nodevicemanager();
		}
	}
	if (QCoreApplication::arguments().contains(QLatin1String("--drivers"))) {
		did = true;
		//get all installed drivers
		QDir driversDir(QString::fromLatin1(LIRI_SYSTEM_DRIVER_DESCRIPTION_DIR));
		QStringList driversList = driversDir.entryList(QStringList()<<QString::fromLatin1("*.desktop"));
		foreach(QString file, driversList) {
			QString id = QFileInfo(file).baseName();
			QSettings driverinfo(driversDir.absoluteFilePath(file), QSettings::IniFormat);
			driverinfo.beginGroup(QLatin1String("Desktop Entry"));
			qDebug() << "Driver" << id;
			qDebug() << "\tFilename:" << driversDir.absoluteFilePath(file);
			if (driverinfo.status() != QSettings::NoError) continue;
			qDebug() << "\tName:" << driverinfo.value(QLatin1String("Name")).toString();
			qDebug() << "\tComment:" << driverinfo.value(QLatin1String("Comment")).toString();
		}
	}
	if (QCoreApplication::arguments().contains(QLatin1String("--remotes"))) {
		did = true;
		QDir resystem(QLatin1String(LIRI_SYSTEM_REMOTES_DIR));
		QStringList filter; filter << QLatin1String("*.desktop");
		QStringList files = resystem.entryList(filter, QDir::Readable | QDir::NoDotAndDotDot | QDir::Files);

		foreach (QString file, files) {
			QString uid = DesktopFile::getUidOfFilename(file);
			RemoteFile re(uid);
			qDebug() << "Remote" << uid;
			qDebug() << "\tName" << re.getName();
			qDebug() << "\tComment" << re.getComment();
		}
	}
	if (QCoreApplication::arguments().contains(QLatin1String("--reload-remote"))) {
		qDebug() << "Reload Remote for" << device;
		did = true;
		if (connection->deviceManagerState()==LIRI_RUNNING) {
			OrgLiriDevManagerReceiverInterface * const ri = connection->getDeviceManagerReceiver(device);
			if (ri) {
				ri->reloadAssociatedRemote();
				qDebug() << "Remote reloaded!";
			} else {
				qDebug() << "Device not found!";
			}
		} else {
			nodevicemanager();
		}
	}
	if (QCoreApplication::arguments().contains(QLatin1String("--reload-profiles"))) {
		qDebug() << "Reload profiles for" << device;
		did = true;
		if (connection->executionEngineState()==LIRI_RUNNING) {
			OrgLiriExecutionReceiverInterface * const ri = connection->getExecutionEngineReceiver(device);
			if (ri) {
				ri->reload();
				qDebug() << "Profiles reloaded!";
			} else {
				qDebug() << "Device not found!";
			}
		} else {
			qDebug() << "Execution engine not running for" << getenv("USER");
		}
	}
	if (QCoreApplication::arguments().contains(QLatin1String("--quit-lirid"))) {
		did = true;
		if (connection->deviceManagerState()==LIRI_RUNNING) {
			connection->getDeviceManagerControl()->quit();
		}
		qDebug() << "Quit device daemon lirid!";
	}
	if (QCoreApplication::arguments().contains(QLatin1String("--quit-liriexec"))) {
		did = true;
		if (connection->executionEngineState()==LIRI_RUNNING) {
			connection->getExecutionEngineControl()->quit();
		}
		qDebug() << "Quit execution engine liriexec!";
	}

	if (QCoreApplication::arguments().contains(QLatin1String("--messages"))) {
		did = true;
		ListenObject* lo = new ListenObject(connection);
		QCoreApplication::exec();
		delete lo;
	}

	if (!did) {
		qDebug() << "Help for available commands with --help";
		qDebug() << "Status for user" << getenv("USER");
		if (connection->deviceManagerState() == LIRI_RUNNING) {
			QString tmp = connection->getDeviceManagerControl()->started();
			qDebug() << "\tDevice Manager: Running" <<
				QDateTime::fromString(tmp, Qt::ISODate).toString(Qt::SystemLocaleShortDate);
		} else
			qDebug() << "\tDevice Manager: Offline";
		if (connection->executionEngineState() == LIRI_RUNNING) {
			QString tmp = connection->getExecutionEngineControl()->started();
			qDebug() << "\tExecution Engine: Running" <<
				QDateTime::fromString(tmp, Qt::ISODate).toString(Qt::SystemLocaleShortDate);
		} else
			qDebug() << "\tExecution Engine: Offline";
	}

	return exitnow(EXIT_SUCCESS);
}
