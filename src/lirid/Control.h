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
#ifndef DEVICELIST_H_
#define DEVICELIST_H_

#include <QObject>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QMap>
#include <QList>
#include <QByteArray>
#include <QSocketNotifier>
#include <QDateTime>

class Control : public QObject
{
	Q_OBJECT
	public:
		Control(QDBusConnection* conn);
		~Control();

		/* dbus interface methods (control) */
		void quit();
		void start();
		QString version();
		QString lastHalCallout();
		QString started();

		// Unix signal handlers.
		static void intSignalHandler(int unused);
		static void termSignalHandler(int unused);

	private:
		static int sigintFd[2];
		static int sigtermFd[2];

		QSocketNotifier *snInt;
		QSocketNotifier *snTerm;

		QDBusConnection* conn;

		QDateTime halcallout;
		QDateTime startedtimedate;

		bool quitFlag;

	private Q_SLOTS:
		// Unix->Qt signal handlers.
		void handleSigInt();
		void handleSigTerm();

	Q_SIGNALS: // SIGNALS
		void deviceAdded(int rid);
		void deviceRemoved(int rid);
		void shutdown();
};

#endif
