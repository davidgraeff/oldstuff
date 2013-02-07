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
#ifndef LIRI_RECEIVERINSTANCEBUSSERVICE_H_
#define LIRI_RECEIVERINSTANCEBUSSERVICE_H_

#include <QVariant>
#include <QString>
#include <QList>
#include <QSet>
#include <QStringList>
#include <QObject>
#include <QDBusConnection>
#include <QMutex>

class BusServiceList;

class TargetList: public QObject {
	Q_OBJECT
public:
	TargetList(const BusServiceList* busServiceList, const QString& serviceid,
		const QList< QString >& options, QObject* parent = 0);

	QDBusConnection& connection();

	const QString& getBus() const;
	const QString& getServiceid() const;
	const QString& getServicename() const;

	const QStringList &getActiveServices() const;
	const QStringList &getAllServices() const;
	const QList<bool> &getActiveMask() const;
	int getPos(const QString& servicename);

	int activateAll();
	int deactivateAll();
	int activate(int nr, bool onlyOne);
	int activateMultiple(QList<int> list);
	int activateNext();
	int activatePrevious();
	int deactivate(int nr);

	bool lockService(const QString& servicename);
	bool unlockService(const QString& servicename);

Q_SIGNALS:
	void targetChanged(int state, const QString &targetid);

private Q_SLOTS:
	void slotServiceUnregistered(const QString& service, bool system);
	void slotServiceRegistered(const QString& service, bool system);

private:
	// locked services
	QSet<QString> lockedServices;

	// options
	bool exactmatch;
	bool autostart;
	bool newservicesActive;
	bool systembus;

	// bus
	QDBusConnection conn;

	QString bus;
	QString servicename;

	// service identification string. Format: bus://servicename
	QString serviceid;

	// all fitting services, active services, service mask
	QStringList services;
	QStringList activeservices;
	QList<bool> activemask;
	QList<bool> oldactivemask;
	int lastActivatedService;
	void updateActiveServices();

	// multithread access
	mutable QMutex accessMutex;
	mutable QMutex lockMutex;

};
#endif


