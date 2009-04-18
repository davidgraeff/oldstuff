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
#include "BusServiceList.h"

#include <QDBusConnectionInterface>
#include <QDBusConnection>
#include <QDebug>

BusServiceList::BusServiceList(QObject* parent) : QObject(parent) {
	//initial fill

	QDBusConnectionInterface *iface;
	//session
	iface = QDBusConnection::sessionBus().interface();
	connect (iface, SIGNAL(serviceUnregistered(const QString&)),
		this, SLOT(slotServiceUnregisteredSession(const QString&)));

	connect (iface, SIGNAL(serviceRegistered(const QString&)),
		this, SLOT(slotServiceRegisteredSession(const QString&)));

	connect (iface, SIGNAL(serviceOwnerChanged(const QString&,const QString&,const QString&)),
		this, SLOT(slotServiceOwnerChangedSession(const QString&,const QString&,const QString&)));


	//system
	iface = QDBusConnection::systemBus().interface();
	connect (iface, SIGNAL(serviceUnregistered(const QString&)),
		this, SLOT(slotServiceUnregisteredSystem(const QString&)));

	connect (iface, SIGNAL(serviceRegistered(const QString&)),
		this, SLOT(slotServiceRegisteredSystem(const QString&)));

	connect (iface, SIGNAL(serviceOwnerChanged(const QString&,const QString&,const QString&)),
		this, SLOT(slotServiceOwnerChangedSystem(const QString&,const QString&,const QString&)));

	if (QDBusConnection::sessionBus().interface()) {
		const QStringList serviceNames = QDBusConnection::sessionBus().interface()->registeredServiceNames();
		foreach (QString service, serviceNames)
			slotServiceRegisteredSession(service);
	}

	if (QDBusConnection::systemBus().interface()) {
		const QStringList serviceNames = QDBusConnection::systemBus().interface()->registeredServiceNames();
		foreach (QString service, serviceNames)
			slotServiceRegisteredSystem(service);
	}
}

const QSet<QString>& BusServiceList::getServices(bool system) const {
	QMutexLocker locker(&accessMutex);
	if (system)
		return servicesSystem;
	else
		return servicesSession;
}

void BusServiceList::slotServiceOwnerChangedSession(const QString& service,const QString& olds,const QString& news) {
	if (olds.isEmpty())
		slotServiceRegisteredSession(service);
	else if (news.isEmpty())
		slotServiceUnregisteredSession(service);
}

void BusServiceList::slotServiceUnregisteredSession(const QString& service) {
	accessMutex.lock();
	servicesSession.remove(service);
	accessMutex.unlock();
	emit unregService(service, false);
}

void BusServiceList::slotServiceRegisteredSession(const QString& service) {
	accessMutex.lock();
	servicesSession.insert(service);
	accessMutex.unlock();
	emit regService(service, false);
}

void BusServiceList::slotServiceOwnerChangedSystem(const QString& service,const QString& olds,const QString& news) {
	if (olds.isEmpty())
		slotServiceRegisteredSystem(service);
	else if (news.isEmpty())
		slotServiceUnregisteredSystem(service);
}

void BusServiceList::slotServiceUnregisteredSystem(const QString& service) {
	accessMutex.lock();
	servicesSystem.remove(service);
	accessMutex.unlock();
	emit unregService(service, true);
}

void BusServiceList::slotServiceRegisteredSystem(const QString& service) {
	accessMutex.lock();
	servicesSystem.insert(service);
	accessMutex.unlock();
	emit regService(service, true);
}
