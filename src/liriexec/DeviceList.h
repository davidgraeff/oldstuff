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
#ifndef LIRI_RECEIVERLIST_H_
#define LIRI_RECEIVERLIST_H_

#include <QString>
#include <QObject>
#include <QMap>
#include <QStringList>
#include <QVector>
#include <QThreadPool>
#include "fileformats/ApplicationProfileFile.h"

class QDBusConnection;
class DeviceInstance;
class BusConnection;
class TargetList;

class DeviceList: public QObject {
	Q_OBJECT
	public:
		DeviceList(QDBusConnection* dbus, BusConnection* busconnection);
		~DeviceList();

		/* targets */
		QStringList getTargets();
		void setTarget(const QString &targetid, bool active);
		void addTargetList(BusServiceWithOptions busservice);
		TargetList* getTargetList(const QString &target);

		void startJob(ActionGroup* actiongroup);
		void stopJobs();

		void instanceLoadedProfiles(DeviceInstance*);

	private Q_SLOTS:
		void devicemanagerStateChanged(int state);
		void deviceAdded(QString rid);
		void deviceRemoved(QString rid);

	Q_SIGNALS:
		void executed(QString receiverinstance, int result, const QString &executedstring);
		void modeChanged(QString receiverinstance, const QString &oldmode, const QString &newmode);
		void targetChanged(int state, const QString &targetid);
		void deviceAddedExecution(QString rid);
		void deviceRemovedExecution(QString rid);
		void profilesLoaded(QString rid);

		void abortExecutionJobs();

	private:
		QDBusConnection* dbus;
		BusConnection* busconnection;
		QThreadPool executionjobs;
		QList<DeviceInstance*> instances;
		QMap< QString, TargetList* > targetlists;
};

#endif
