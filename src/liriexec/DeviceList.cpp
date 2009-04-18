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
#include "DeviceList.h"
#include "ExecutionJob.h"
#include "actions/ActionGroup.h"
#include "device/DeviceInstance.h"
#include "device/TargetList.h"
#include "businterconnect/BusConnection.h"
#include "businterconnect/BusServiceList.h"
#include "config.h"
#include <QDebug>
#include <QStringList>

DeviceList::DeviceList(QDBusConnection* dbus, BusConnection* busconnection) : dbus(dbus), busconnection(busconnection) {
	connect(busconnection, SIGNAL( deviceAdded(int) ), SLOT( deviceAdded(int) ));
	connect(busconnection, SIGNAL( deviceRemoved(int) ), SLOT( deviceRemoved(int) ));
	connect(busconnection, SIGNAL( devicemanagerStateChanged(int) ), SLOT( devicemanagerStateChanged(int) ));
	devicemanagerStateChanged(busconnection->deviceManagerState());
}

DeviceList::~DeviceList() {
	emit abortExecutionJobs();
	executionjobs.waitForDone();
	qDeleteAll(instances);
	qDeleteAll(targetlists);
}

void DeviceList::instanceLoadedProfiles(DeviceInstance* dev) {
	emit profilesLoaded(dev->getInstance());
}

void DeviceList::addTargetList(BusServiceWithOptions busservice) {
	if (targetlists.contains(busservice.serviceid)) return; // do not add if already added
	TargetList* targetlist = new TargetList(busconnection->getBusServicelist(),
		busservice.serviceid, busservice.options);
	connect(targetlist,SIGNAL( targetChanged(int, const QString &) ),
		SIGNAL( targetChanged(int, const QString &) ));
	targetlists.insert(busservice.serviceid, targetlist);
}

TargetList* DeviceList::getTargetList(const QString &target) {
	return targetlists.value(target, 0);
}

QStringList DeviceList::getTargets() {
	QStringList list;
	// insert all services into a list and return that
	foreach(TargetList* targetlist, targetlists) {
		QString serviceid = targetlist->getServiceid();
		QList<bool> actives = targetlist->getActiveMask();
		QStringList services = targetlist->getAllServices();
		Q_ASSERT(services.size()==actives.size());
		// Because we want to return just a string list, we have to 'stack' some information together
		// serviceid := bus://servicename as given by application profiles. E.g.: "session://org.mpris."
		// FORMAT: serviceid@service1 [last character is 0 or 1 (active mask)]
		// Example: "session://org.mpris.@org.mpris.amarok1"
		for(int i=0;i<actives.size();++i) {
			list.append(serviceid + QLatin1Char('@') + services[i] + QString::number(actives[i]));
		}
	}
	return list;
}

void DeviceList::setTarget(const QString &targetid, bool active) {
	//targetid format: bus://abstract_servicename@bus://concret_servicename
	QStringList arg = targetid.split(QLatin1Char('@'));
	if (arg.size() != 2) { qDebug() << "DeviceList::setTarget wrong format"; return; }

	TargetList* targetlist = targetlists.value(arg[0]);
	if (!targetlist) { qDebug() << "DeviceList::setTarget target not found"; return; }

	if (active)
		targetlist->activate(targetlist->getPos(arg[1]), false);
	else
		targetlist->deactivate(targetlist->getPos(arg[1]));
}

void DeviceList::stopJobs() {
	/* if there is still an execution job running due to repeating actions, abort it now */
	emit abortExecutionJobs();
}

void DeviceList::startJob(ActionGroup* actiongroup) {
	ExecutionJob* ejob = new ExecutionJob(actiongroup);
	connect(this, SIGNAL(abortExecutionJobs()), ejob, SLOT(abort()));
	executionjobs.start(ejob);

}

void DeviceList::devicemanagerStateChanged(int state) {
	if (state == LIRI_UNKNOWN) {
		qDebug() << "Lost connection to device manager! ExecutionEngine: Clear";
		qDeleteAll(instances);
		instances.clear();
		emit abortExecutionJobs();
	} else if (state == LIRI_RUNNING) {
		QList<int> receivers = busconnection->receivers();
		foreach(int rid, receivers) deviceAdded(rid);
	}
}

void DeviceList::deviceRemoved(int rid) {
	if (rid < instances.size()) {
		delete instances[rid];
		instances[rid] = 0;
		emit deviceRemovedExecution(rid);
	}
}

void DeviceList::deviceAdded(int rid) {
	if (rid >= instances.size()) instances.resize(rid+1);
	if (instances.value(rid,0)) return; //only add if not already added
	instances[rid] = new DeviceInstance(dbus, this, busconnection, rid);
	emit deviceAddedExecution(rid);
}
