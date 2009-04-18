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
#ifndef LIRI_DEVICELISTMODEL_H_
#define LIRI_DEVICELISTMODEL_H_

#include <QAbstractItemModel>
#include <QModelIndex>
#include <kicon.h>
#include <QString>
#include <QVariant>
#include <QList>

class OrgLiriDevManagerReceiverInterface;
class BusConnection;
class DeviceListModel;
class trLiriClass;

enum DeviceListModelRoles {
	CaptionRole = Qt::UserRole,
	StatusRole,
	RemoteRole,
	RemoteUIDRole,
	RIDRole
};

class DeviceInfo : public QObject {
	Q_OBJECT
	public:
		DeviceInfo(DeviceListModel* model, BusConnection* busconnection, int rid);
		~DeviceInfo();
		KIcon* icon;
		QString text_;
		QString caption;
		QString status;
		QString remote;
		QString remoteuid;
		QString tooltip; // connected, udi, rid etc
		int rid;
		int remotestate;
		int receiverstate;
		bool updatetextflag;
	private:
		DeviceListModel* model;
		BusConnection* busconnection;
		void updatetext();
	private Q_SLOTS:
		void receiverStateChanged(int state);
		void remoteStateChanged(int state);
		void key(const QString &keycode, const QString &keyname, uint channel, int pressed);
};

class DeviceListModel : public QAbstractListModel {
     Q_OBJECT
public:
	DeviceListModel(BusConnection* busconnection);
	~DeviceListModel();
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	void changed(DeviceInfo*); // called by DeviceInfo
	trLiriClass* trLiriMessages;
	KIcon iconRunning;
	KIcon iconUnknown;
	KIcon iconInit;
	KIcon iconKey;
private Q_SLOTS:
	void deviceAdded(int rid);
	void deviceRemoved(int rid);
private:
	QList<DeviceInfo*> list;
	BusConnection* busconnection;
};

#endif
