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

#include <QObject>
#include <QStringList>
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
		QStringList receivers() const;
		void reemitAddedSignals(reemitSignalsEnum);
		// dbus services
		BusServiceList * getBusServicelist() const;
		// receiver interfaces
		OrgLiriDevManagerReceiverInterface * getDeviceManagerReceiver(const QString& instance) const;
		OrgLiriExecutionReceiverInterface  * getExecutionEngineReceiver(const QString& instance) const;
		// control interfaces
		OrgLiriDevManagerControlInterface * getDeviceManagerControl() const;
		OrgLiriExecutionControlInterface  * getExecutionEngineControl() const;

	private:
		void startedExecutionEngine();
		void startedDeviceManager();
		QStringList parseIntrospect();
	private:
		BusServiceList* busServiceList;
		OrgLiriDevManagerControlInterface* devicemanager_controlinterface;
		OrgLiriExecutionControlInterface* execution_controlinterface;
		QMap< QString, OrgLiriDevManagerReceiverInterface* > devmanlist;
		QMap< QString, OrgLiriExecutionReceiverInterface* > executionlist;

	private Q_SLOTS:
		void slotServiceUnregistered(const QString& service, bool system);
		void slotServiceRegistered(const QString& service, bool system);
		void deviceAddedSlot(const QString& rid);
		void deviceRemovedSlot(const QString& rid);
		void deviceAddedExecutionSlot(const QString& rid);
		void deviceRemovedExecutionSlot(const QString& rid);

	Q_SIGNALS:
		// state of a framework component changed
		void devicemanagerStateChanged(int state);
		void executionengineStateChanged(int state);

		// device added/removed
		void deviceAdded(const QString& uid);
		void deviceRemoved(const QString& uid);
		void deviceAddedExecution(const QString& rid);
		void deviceRemovedExecution(const QString& rid);

		// some notification signals from the execution engine
		void executed(const QString& rid, int result, const QString &executed);
		void modeChanged(const QString& rid, const QString &oldmode, const QString &newmode);
		void targetChanged(int state, const QString &targetid);
		void profilesLoaded(const QString& rid);
};

#endif
