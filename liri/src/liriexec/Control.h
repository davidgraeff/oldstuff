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
#ifndef LIRI_CONTROL_H_
#define LIRI_CONTROL_H_

#include <QString>
#include <QObject>
#include <QSocketNotifier>
#include <QDateTime>
#include <QStringList>

class QDBusConnection;
class DeviceList;

class Control: public QObject {
	Q_OBJECT
	public:
		Control(QDBusConnection* dbus, DeviceList* devicelist);
		~Control();

		void quit();
		QString started();
		QString version();

		QStringList getTargets();
		void setTarget(const QString &targetid, bool active);

		// Unix signal handlers.
		static void intSignalHandler(int unused);
		static void termSignalHandler(int unused);

	private Q_SLOTS:
		// Unix->Qt signal handlers.
		void handleSigInt();
		void handleSigTerm();

	Q_SIGNALS:
		void executed(int receiverinstance, int result, const QString &executedstring);
		void modeChanged(int receiverinstance, const QString &oldmode, const QString &newmode);
		void targetChanged(int state, const QString &targetid);
		void deviceAddedExecution(int rid);
		void deviceRemovedExecution(int rid);
		void profilesLoaded(int rid);

	private:
		static int sigintFd[2];
		static int sigtermFd[2];

		QSocketNotifier *snInt;
		QSocketNotifier *snTerm;

		QDBusConnection* dbus;

		QDateTime startedtimedate;
		DeviceList* devicelist;

		bool quitFlag;
};

#endif