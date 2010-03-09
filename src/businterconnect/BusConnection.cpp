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
#include "businterconnect/BusConnection.h"
#include "businterconnect/BusServiceList.h"
#include "businterconnect/Control_DevManager_dbusproxy.h"
#include "businterconnect/Control_Execution_dbusproxy.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"
#include "businterconnect/Receiver_Execution_dbusproxy.h"
/* common liri stuff */
#include "config.h"

#include <QSet>
#include <QStringList>
/* dbus */
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtXml/QtXml>

BusConnection::BusConnection(QObject* parent) : QObject(parent) {
	busServiceList = new BusServiceList();
	devicemanager_controlinterface = 0;
	execution_controlinterface = 0;

	// emit signals for new/removes services on the bus
	connect (busServiceList, SIGNAL( regService(const QString&, bool) ),
		SLOT( slotServiceRegistered(const QString&, bool) ));
	connect (busServiceList, SIGNAL( unregService(const QString&, bool) ),
		SLOT( slotServiceUnregistered(const QString&, bool) ));

	// reemit signals for existing services on the bus
	const QSet<QString> servicesSystem = busServiceList->getServices(true);
	foreach (QString servicename, servicesSystem)
		slotServiceRegistered(servicename, true);
	const QSet<QString> servicesSession = busServiceList->getServices(false);
	foreach (QString servicename, servicesSession)
		slotServiceRegistered(servicename, false);
}

BusConnection::~BusConnection() {
	delete busServiceList;
	delete devicemanager_controlinterface;
	delete execution_controlinterface;
	qDeleteAll(devmanlist.values());
	qDeleteAll(executionlist.values());
}

int BusConnection::deviceManagerState() {
	return ((devicemanager_controlinterface != 0) ?  LIRI_RUNNING :  LIRI_UNKNOWN);
}

int BusConnection::executionEngineState() {
	return ((execution_controlinterface != 0) ?  LIRI_RUNNING :  LIRI_UNKNOWN);
}

QStringList BusConnection::receivers() const {
	return devmanlist.keys();
}

void BusConnection::reemitAddedSignals(reemitSignalsEnum sig) {
	QStringList list;
	switch (sig) {
		case reemitDeviceManager:
			list = devmanlist.keys();
			foreach(QString rid, list) emit deviceAdded(rid);
			break;
		case reemitExecutionEngine:
			list = executionlist.keys();
			foreach(QString rid, list) emit deviceAddedExecution(rid);
			break;
		default:
			break;
	}
}

BusServiceList *  BusConnection::getBusServicelist() const {
	return busServiceList;
}

OrgLiriDevManagerReceiverInterface * BusConnection::getDeviceManagerReceiver(const QString& instance) const {
	return devmanlist.value(instance);
}

OrgLiriExecutionReceiverInterface  * BusConnection::getExecutionEngineReceiver(const QString& instance) const {
	return executionlist.value(instance);
}

OrgLiriDevManagerControlInterface * BusConnection::getDeviceManagerControl() const {
	return devicemanager_controlinterface;
}

OrgLiriExecutionControlInterface  * BusConnection::getExecutionEngineControl() const {
	return execution_controlinterface;
}

void BusConnection::deviceAddedSlot(const QString& rid) {
	// be on the safe side, although this slot should only be called if these
	// prerequires are fulfilled anyway
	if (!devicemanager_controlinterface) return;

	// is this a duplicate?
	if (devmanlist.contains(rid)) return;

	// create device-manager device bus object. Bus objectpath: /org/liri/Devicelist/N
	QString devicename = QLatin1String(LIRI_DBUS_OBJECT_RECEIVERS"/") + rid;
	OrgLiriDevManagerReceiverInterface* tmp;
	tmp = new OrgLiriDevManagerReceiverInterface(
		QLatin1String(LIRI_DBUS_SERVICE_DEVMAN),
		devicename,
		QDBusConnection::systemBus() );

	// connection established?
	if (!tmp->isValid()) {
		qWarning() << devicename << "Interface invalid" << tmp->lastError();
		delete tmp; tmp = 0;
		return;
	}

	// create new entry
	devmanlist[rid] = tmp;

	emit deviceAdded(rid);
}


void BusConnection::deviceAddedExecutionSlot(const QString& rid) {
	// be on the safe side, although this slot should only be called if these
	// prerequires are fulfilled anyway
	if (!devicemanager_controlinterface || !execution_controlinterface) return;

	// is this a duplicate?
	if (executionlist.contains(rid)) return;

	// create execution device bus object. Bus objectpath: /org/liri/Devicelist/N
	QString devicename = QLatin1String(LIRI_DBUS_OBJECT_RECEIVERS"/") + rid;
	OrgLiriExecutionReceiverInterface* tmp;
	tmp = new OrgLiriExecutionReceiverInterface(
		QLatin1String(LIRI_DBUS_SERVICE_EXECUTION),
		devicename,
		QDBusConnection::sessionBus() );

	// connection established?
	if (!tmp->isValid()) {
		qWarning() << devicename << "Interface invalid" << tmp->lastError();
		delete tmp; tmp = 0;
		return;
	}

	// create new entry
	executionlist[rid] = tmp;

	emit deviceAddedExecution(rid);
}

void BusConnection::deviceRemovedSlot(const QString& rid) {
	// only do something if this rid is in our map
	QMap< QString, OrgLiriDevManagerReceiverInterface* >::iterator it = devmanlist.find(rid);
	if (it == devmanlist.end()) return;

	// make no sense to have an execution-engine device but no
	// (physical) device-manager device. Although this should also
	// be emitted by the execution engine, we do it here, too.
	deviceRemovedExecutionSlot(rid);

	// emit signals
	emit deviceRemoved(rid);
	if (*it) emit deviceRemovedExecution(rid);

	// remove
	delete *it;
	devmanlist.erase(it);
}

void BusConnection::deviceRemovedExecutionSlot(const QString& rid) {
	// only do something if this rid is in our map
	QMap< QString, OrgLiriExecutionReceiverInterface* >::iterator it = executionlist.find(rid);
	if (it == executionlist.end()) return;

	// emit signals
	emit deviceRemovedExecution(rid);

	// remove
	delete *it;
	executionlist.erase(it);
}

void BusConnection::slotServiceUnregistered(const QString& service, bool system) {
	// device manager shutdown
	if (system && service == QLatin1String(LIRI_DBUS_SERVICE_DEVMAN)) {
		// emit signals
		emit devicemanagerStateChanged(LIRI_UNKNOWN);

		// free control interface
		delete devicemanager_controlinterface;
		devicemanager_controlinterface = 0;

		// remove all entries one after the other
		QStringList rids = devmanlist.keys();
		foreach(QString rid, rids) deviceRemovedSlot(rid);
	} else
	// execution engine shutdown
	if (!system && service == QLatin1String(LIRI_DBUS_SERVICE_EXECUTION)) {
		// emit signals
		emit executionengineStateChanged(LIRI_UNKNOWN);

		// free control interface
		delete execution_controlinterface;
		execution_controlinterface = 0;

		// remove all entries one after the other
		QStringList rids = executionlist.keys();
		foreach(QString rid, rids) deviceRemovedExecutionSlot(rid);
	}
}

void BusConnection::slotServiceRegistered(const QString& service, bool system) {
	// device manager detected
	if (system && service == QLatin1String(LIRI_DBUS_SERVICE_DEVMAN) && !devicemanager_controlinterface) {
		startedDeviceManager();
	} else
	// execution engine detected
	if (!system && service == QLatin1String(LIRI_DBUS_SERVICE_EXECUTION) && !execution_controlinterface) {
		startedExecutionEngine();
	}
}

void BusConnection::startedExecutionEngine() {
	// create control object
	execution_controlinterface = new OrgLiriExecutionControlInterface(
		QLatin1String(LIRI_DBUS_SERVICE_EXECUTION),
		QLatin1String(LIRI_DBUS_OBJECT_RECEIVERS), QDBusConnection::sessionBus());

	// connection established?
	if (!execution_controlinterface->isValid()) {
		qWarning() << "execution_controlinterface Interface invalid" << execution_controlinterface->lastError();
		delete execution_controlinterface;
		execution_controlinterface = 0;
		return;
	}

	// connect signals
	connect(execution_controlinterface, SIGNAL ( executed(int, int, const QString &) ),
		SIGNAL( executed(int, int, const QString &) ));

	connect(execution_controlinterface, SIGNAL ( modeChanged(int, const QString &, const QString &) ),
		SIGNAL( modeChanged(int, const QString &, const QString &) ));

	connect(execution_controlinterface, SIGNAL ( targetChanged(int, const QString &) ),
		SIGNAL( targetChanged(int, const QString &) ));

	connect(execution_controlinterface, SIGNAL ( profilesLoaded(int) ),
		SIGNAL( profilesLoaded(int) ));

	connect(execution_controlinterface, SIGNAL ( deviceAddedExecution(const QString&) ),
		SLOT( deviceAddedExecutionSlot(const QString&) ));

	connect(execution_controlinterface, SIGNAL ( deviceRemovedExecution(const QString&) ),
		SLOT( deviceRemovedExecutionSlot(const QString&) ));

	// add devices (have to be known be the device manager already!)
	QStringList list = devmanlist.keys();
	foreach(QString rid, list) deviceAddedExecutionSlot(rid);

	// emit signal
	emit executionengineStateChanged(LIRI_RUNNING);
}

void BusConnection::startedDeviceManager() {
	// create control object
	devicemanager_controlinterface = new OrgLiriDevManagerControlInterface(
		QLatin1String(LIRI_DBUS_SERVICE_DEVMAN),
		QLatin1String(LIRI_DBUS_OBJECT_RECEIVERS),
		QDBusConnection::systemBus());

	// connection established?
	if (!devicemanager_controlinterface->isValid()) {
		qWarning() << "devicemanager_controlinterface Interface invalid" <<
			devicemanager_controlinterface->lastError();
		delete devicemanager_controlinterface;
		devicemanager_controlinterface = 0;
		return;
	}

	// connect signals
	connect(devicemanager_controlinterface, SIGNAL ( deviceAdded(const QString&) ),
		SLOT( deviceAddedSlot(const QString&) ));

	connect(devicemanager_controlinterface, SIGNAL ( deviceRemoved(const QString&) ),
		SLOT( deviceRemovedSlot(const QString&) ));

	// add devices through parsing dbus introsspect xml
	QStringList list = parseIntrospect();
	foreach(QString rid, list) deviceAddedSlot(rid);

	// emit signal
	emit devicemanagerStateChanged(LIRI_RUNNING);
}

QStringList BusConnection::parseIntrospect() {
	QStringList tmp;
	QDBusInterface iface(QLatin1String(LIRI_DBUS_SERVICE_DEVMAN), QLatin1String(LIRI_DBUS_OBJECT_RECEIVERS),
		QLatin1String("org.freedesktop.DBus.Introspectable"), QDBusConnection::systemBus());
	if (!iface.isValid()) {
		QDBusError err(iface.lastError());
		qDebug() << "Cannot introspect child nodes (devices): interface invalid";
		return tmp;
	}

	QDBusReply<QString> xml = iface.call(QLatin1String("Introspect"));
	if (!xml.isValid()) {
		QDBusError err(xml.error());
		if (err.isValid()) {
			qDebug() << "Cannot introspect child nodes (devices): Call to object failed";
		} else {
			qDebug() << "Cannot introspect child nodes (devices): Invalid XML";
		}
		return tmp;
	}

	QDomDocument doc;
	doc.setContent(xml);

	QDomElement node = doc.documentElement();
	QDomElement child = node.firstChildElement();
	while (!child.isNull()) {
		QString name = child.attribute(QLatin1String("name"));
		if (child.tagName() == QLatin1String("node") && name[0].isNumber()) {
			tmp.append(name);
		}
		child = child.nextSiblingElement();
	}
	return tmp;
}
