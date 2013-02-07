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
#ifndef LIRI_DBUS_SERVICELIST_H_
#define LIRI_DBUS_SERVICELIST_H_

#include <QMutex>
#include <QObject>
#include <QSet>
#include <QString>

class BusServiceList: public QObject {
  Q_OBJECT
public:
	BusServiceList(QObject* parent = 0);

	const QSet<QString>& getServices(bool system) const;
	//bool findService(const QString& service, bool system) const;

private Q_SLOTS:
	void slotServiceUnregisteredSession(const QString&);
	void slotServiceRegisteredSession(const QString&);
	void slotServiceOwnerChangedSession(const QString&,const QString&,const QString&);
	void slotServiceUnregisteredSystem(const QString&);
	void slotServiceRegisteredSystem(const QString&);
	void slotServiceOwnerChangedSystem(const QString&,const QString&,const QString&);
Q_SIGNALS:
	void regService(const QString& service, bool system);
	void unregService(const QString& service, bool system);

private:
	// services
	QSet<QString> servicesSession;
	QSet<QString> servicesSystem;

	// multithread access
	mutable QMutex accessMutex;
};

#endif
