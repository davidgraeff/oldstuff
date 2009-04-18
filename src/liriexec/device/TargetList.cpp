#include "liriexec/device/TargetList.h"
#include "businterconnect/BusServiceList.h"
#include "config.h"
#include <QDebug>

TargetList::TargetList(const BusServiceList* busServiceList, const QString& serviceid,
		const QList< QString >& options, QObject* parent) : QObject(parent), conn(QString()) {

	// serviceid, name and bus
	this->serviceid = serviceid;
	QStringList data = serviceid.split(QLatin1String("://"));
	Q_ASSERT(data.size() == 2);
	bus = data[0];
	servicename = data[1];

	// init int
	newservicesActive = true;
	lastActivatedService = -1;

	// options
	systembus = (bus == QLatin1String("system"));
	exactmatch = true;
	autostart = false;
	QList<int> activateLater;
	foreach (QString option, options) {
		if      (!option.size()) continue;
		else if (option == QLatin1String("lazy")) exactmatch = false;
		else if (option == QLatin1String("autostart")) autostart = true;
		else if (option == QLatin1String("single")) newservicesActive = false;
		else if (option[0].isNumber()) activateLater.append(option.toInt());
	}

	// dbus connection
	if (systembus)
		conn = QDBusConnection::systemBus();
	else
		conn = QDBusConnection::sessionBus();

	/* signals for updating the reachable-services list */
	connect (busServiceList, SIGNAL( regService(const QString&,bool) ),
		SLOT( slotServiceRegistered(const QString&,bool) ));
	connect (busServiceList, SIGNAL( unregService(const QString&,bool) ),
		SLOT( slotServiceUnregistered(const QString&,bool) ));

	/* inital go through service list */
	foreach (QString servicename, busServiceList->getServices(systembus))
		slotServiceRegistered(servicename, systembus);

	activateMultiple(activateLater);
}

QDBusConnection& TargetList::connection() {
	return conn;
}

const QString & TargetList::getServiceid() const {
	return serviceid;
}

const QString & TargetList::getBus() const {
	return bus;
}

const QString & TargetList::getServicename() const {
	return servicename;
}

// try to get the position of servicename in the service list
// this function is not called that often and therefore has only
// a runtime performance of O(n)
int TargetList::getPos(const QString& servicename) {
	for (int i=0; i < services.size(); ++i)
		if (services[i] == servicename) return i;
	return -1; // not found in the service list
}

const QStringList & TargetList::getActiveServices() const {
	return activeservices;
}

const QList<bool> & TargetList::getActiveMask() const {
	return activemask;
}

const QStringList & TargetList::getAllServices() const {
	return services;
}

int TargetList::activateAll() {
	// modify flags
	lastActivatedService = 0;
	newservicesActive = true;
	// update mask
	for(int i=0;i<activemask.size();++i) activemask[i] = true;
	// apply mask
	updateActiveServices();
	return LIRIOK_executed;
}

int TargetList::deactivateAll() {
	// modify flags
	lastActivatedService = 0;
	newservicesActive = false;
	// update mask
	for(int i=0;i<activemask.size();++i) activemask[i] = false;
	// apply mask
	updateActiveServices();
	return LIRIOK_executed;
}

int TargetList::activate(int nr, bool onlyOne) {
	// validate nr
	if (nr < 0 || nr >= activemask.size()) return LIRIERR_noservicewithnumber;
	// modify flags
	lastActivatedService = nr;
	// update mask
	if (onlyOne) for(int i=0;i<activemask.size();++i) activemask[i] = false;
	activemask[nr] = true;
	// apply mask
	updateActiveServices();
	return LIRIOK_executed;
}

int TargetList::activateMultiple(QList<int> list) {
	// update mask
	for(int i=0;i<list.size();++i) {
		if (i < 0 || i >= activemask.size()) return LIRIERR_noservicewithnumber;
		activemask[i] = true;
	}
	// apply mask
	updateActiveServices();
	return LIRIOK_executed;
}

int TargetList::deactivate(int nr) {
	// validate nr
	if (nr < 0 || nr >= activemask.size()) return LIRIERR_noservicewithnumber;
	// update mask
	activemask[nr] = false;
	// apply mask
	updateActiveServices();
	return LIRIOK_executed;
}

int TargetList::activateNext() {
	return activate(lastActivatedService+1, true);
}

int TargetList::activatePrevious() {
	return activate(lastActivatedService-1, true);
}

void TargetList::updateActiveServices() {
	QString targetid;
	activeservices.clear();
	for (int i=0;i<activemask.size();++i) {
		// append to active list if service active
		if (activemask[i]) activeservices.append(services[i]);
		// emit changed signals
		if (oldactivemask.size() <= i || oldactivemask[i] != activemask[i]) {

			accessMutex.lock();
			QString cache = services[i];
			accessMutex.unlock();

			targetid = serviceid + QLatin1Char('@') + cache;
			emit targetChanged(activemask[i], targetid);
		}
	}

	oldactivemask = activemask;
}

void TargetList::slotServiceUnregistered(const QString& service, bool system) {
	if (this->systembus != system) return;
	if (!exactmatch) {
		if(servicename.leftRef(servicename.size()) != service.leftRef(servicename.size())) return;
	} else {
		if (servicename != service) return;
	}

	QMutexLocker locker(&accessMutex);

	QString targetid;
	for (int i=services.size()-1; i>=0;--i) {
		if (services[i] == service) {
			targetid = serviceid + QLatin1Char('@') +  services[i];
			emit targetChanged(-1, targetid);
			services.removeAt(i);
			activemask.removeAt(i);
			qDebug() << "TargetList lost target" << service;
		}
	}
	updateActiveServices();
}

void TargetList::slotServiceRegistered(const QString& service, bool system) {
	if (this->systembus != system) return;
	if (!exactmatch) {
		if(servicename.leftRef(servicename.size()) != service.leftRef(servicename.size())) return;
	} else {
		if (servicename != service) return;
	}

	if (services.contains(service)) return;

	qDebug() << "TargetList found target" << service;
	accessMutex.lock();
	services.append(service);
	accessMutex.unlock();

	// append 0 or 1 for this service depending on newservicesActive
	activemask.append(newservicesActive);
	updateActiveServices();
}

bool TargetList::lockService(const QString& servicename) {
	QMutexLocker locker(&lockMutex);
	if (lockedServices.contains(servicename)) return false;
	lockedServices.insert(servicename);
	return true;
}

bool TargetList::unlockService(const QString& servicename) {
	QMutexLocker locker(&lockMutex);
	return lockedServices.remove(servicename);
}
