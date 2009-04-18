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
#include <QDebug>

#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <kdesktopfile.h>
#include <KDE/KLocale>

#include "config.h"
#include "DriversList.h"

#include "businterconnect/DaemonConnection.h"
#include "businterconnect/Dbus_proxy_receiverloadremote.h"
#include "businterconnect/Dbus_proxy_receiverlist.h"
#include "businterconnect/Dbus_proxy_receiver.h"

DriverList::DriverList(DaemonConnection* dconnection) : QAbstractListModel(0), dconnection(dconnection) {
	connect(dconnection, SIGNAL(receiverStateChanged(int, int)), SLOT(receiverStateChanged(int, int)));
	if (dconnection->getReceiverListInterface()) {
		QDBusReply<QList<int> > instancelistReply = dconnection->getReceiverListInterface()->getAllInstances();
		QList<int> instancelist = instancelistReply.value();
		foreach(int instanceNr, instancelist) {
			OrgLiriReceiverInterface* ri = dconnection->getReceiverInterface(instanceNr);
			receiverStateChanged(instanceNr, ri->ReceiverState());
		}
	}
}

DriverList::~DriverList() {
}

void DriverList::receiverStateChanged(int instance, int state) {
	if (state == LIRIOK_ready) {
		OrgLiriReceiverInterface* ri = dconnection->getReceiverInterface(instance);
		if (!ri) return;

		QStringList keys;
		keys << QLatin1String("remotereceiver.id");
		QStringList values = ri->getSettings(keys);

		if (values.size() != 1) return;

		DriverInfo driverinfo;
		driverinfo.instance = instance;
		driverinfo.id = values[0];

		if (dconnection->getReceiverLoadRemoteInterface())
			driverinfo.remote = dconnection->getReceiverLoadRemoteInterface()->getDefaultRemote(driverinfo.id);

		beginInsertRows(QModelIndex(), drivers.size(), drivers.size());
		drivers.append(driverinfo);
		endInsertRows();
	} else {
		for (int i=drivers.size()-1; i >= 0 ; --i)
			if (drivers[i].instance == instance) {
				beginRemoveRows(QModelIndex(), i, i);
				drivers.removeAt(i);
				endRemoveRows();
			}
	}
}

/**************** For model access ****************/
int DriverList::rowCount(const QModelIndex &) const {
	return drivers.size();
}

int DriverList::columnCount(const QModelIndex &) const {
	return 2;
}

Qt::ItemFlags DriverList::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	if (index.column() == 1 && dconnection->getReceiverLoadRemoteInterface())
		return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
	else
		return QAbstractItemModel::flags(index);
}

bool DriverList::setData ( const QModelIndex & index, const QVariant & value, int role) {
	if (index.isValid() && role == Qt::EditRole && index.column() == 1) {
		if (drivers[index.row()].remote != value.value<QString>()) {
			drivers[index.row()].remote = value.value<QString>();
			if (dconnection->getReceiverLoadRemoteInterface()) {
				dconnection->getReceiverLoadRemoteInterface()->setDefaultReceiverID(
					drivers[index.row()].remote, drivers[index.row()].id);
				emit dataChanged(index, index);
				return true;
			}
		}
	}
	return false;
}

QVariant DriverList::data(const QModelIndex &index, int role) const {
	QVariant theData;

	if ( !index.isValid() ) {
		return QVariant();
	}

	switch ( role ) {
		case Qt::EditRole:
		case Qt::DisplayRole:
			if (index.column() == 0) {
				theData.setValue( drivers[index.row()].id );
			} else if (index.column() == 1) {
				theData.setValue( drivers[index.row()].remote );
			}
			break;
		default:
			break;
	}

	return theData;
}

QVariant DriverList::headerData(int section, Qt::Orientation orientation,
                         int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		if (section==0)
		return i18n("ReceiverID");
		else if (section==1)
		return i18n("Remote Descriptor");
	}

    return QVariant();
}
