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
#ifndef LIRI_DAEMON_CONNECTION_H_
#define LIRI_DAEMON_CONNECTION_H_

#include <QString>
#include <QObject>
#include <QList>
#include <QMap>

class BusServiceList;
class OrgLiriDevManagerReceiverInterface;
class OrgLiriExecutionReceiverInterface;
class OrgLiriDevManagerControlInterface;
class OrgLiriExecutionControlInterface;

enum reemitSignalsEnum {
	reemitDeviceManager,
	reemitExecutionEngine
};

class BusConnection: public QObject {
	Q_OBJECT
	public:
		BusConnection(QObject* parent = 0);
		~BusConnection();
		int deviceManagerState();
		int executionEngineState();
		QList<int> receivers() const;
		void reemitAddedSignals(reemitSignalsEnum);
		// dbus services
		BusServiceList * getBusServicelist() const;
		// receiver interfaces
		OrgLiriDevManagerReceiverInterface * getDeviceManagerReceiver(int instance) const;
		OrgLiriExecutionReceiverInterface  * getExecutionEngineReceiver(int instance) const;
		// control interfaces
		OrgLiriDevManagerControlInterface * getDeviceManagerControl() const;
		OrgLiriExecutionControlInterface  * getExecutionEngineControl() const;

	private:
		void startedExecutionEngine();
		void startedDeviceManager();
		QList<int> parseIntrospect();
	private:
		BusServiceList* busServiceList;
		OrgLiriDevManagerControlInterface* devicemanager_controlinterface;
		OrgLiriExecutionControlInterface* execution_controlinterface;
		QMap< int, OrgLiriDevManagerReceiverInterface* > devmanlist;
		QMap< int, OrgLiriExecutionReceiverInterface* > executionlist;

	private Q_SLOTS:
		void slotServiceUnregistered(const QString& service, bool system);
		void slotServiceRegistered(const QString& service, bool system);
		void deviceAddedSlot(int rid);
		void deviceRemovedSlot(int rid);
		void deviceAddedExecutionSlot(int rid);
		void deviceRemovedExecutionSlot(int rid);

	Q_SIGNALS:
		// state of a framework component changed
		void devicemanagerStateChanged(int state);
		void executionengineStateChanged(int state);

		// device added/removed
		void deviceAdded(int rid);
		void deviceRemoved(int rid);
		void deviceAddedExecution(int rid);
		void deviceRemovedExecution(int rid);

		// some notification signals from the execution engine
		void executed(int rid, int result, const QString &executed);
		void modeChanged(int rid, const QString &oldmode, const QString &newmode);
		void targetChanged(int state, const QString &targetid);
		void profilesLoaded(int rid);
};

#endif
