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
#include "businterconnect/Dbus_proxy_rrc.h"

DriverList::DriverList(DaemonConnection* dconnection) : QAbstractListModel(0), dconnection(dconnection) {
	//get all installed drivers
	QDir receiverDir(QString::fromLatin1(LIRI_SYSTEM_DRIVER_DESCRIPTION_DIR));
	QStringList receiversList = receiverDir.entryList(QStringList()<<QString::fromLatin1("*.desktop"));
	foreach(QString file, receiversList) {
		QString id = QFileInfo(file).baseName();
		KDesktopFile dfile(receiverDir.absoluteFilePath(file));
		DriverInfo* re = new DriverInfo();
		re->name = dfile.readName();
		re->description = dfile.readComment();
		re->filename = file;
		re->id = id;
		if (dconnection->getRRCInterface())
			re->remote = dconnection->getRRCInterface()->getDefaultRemote(id);
		drivers.append(re);
	}
}

DriverList::~DriverList() {
	foreach(DriverInfo* re, drivers)
		delete re;
	drivers.clear();
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

	if (index.column() == 1)
		return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
	else
		return QAbstractItemModel::flags(index);
}

bool DriverList::setData ( const QModelIndex & index, const QVariant & value, int role) {
	if (index.isValid() && role == Qt::EditRole && index.column() == 1) {
		DriverInfo* re = drivers[index.row()];
		if (re->remote != value.value<QString>()) {
			re->remote = value.value<QString>();
			if (dconnection->getRRCInterface())
				dconnection->getRRCInterface()->setDefaultDriver(re->remote, re->id);
			emit dataChanged(index, index);
		}
		return true;
	}
	return false;
}

QVariant DriverList::data(const QModelIndex &index, int role) const {
	QVariant theData;

	if ( !index.isValid() ) {
		return QVariant();
	}

	DriverInfo* re = drivers[index.row()];
	switch ( role ) {
		case Qt::EditRole:
		case Qt::DisplayRole:
			if (index.column() == 0)
				theData.setValue( re->name );
			else if (index.column() == 1)
				theData.setValue( re->remote );
			break;
		case Qt::ToolTipRole:
			theData.setValue( i18n("-Driver-\nName: %1\nDescription: %2\nFile (Desc): %3", re->name, re->description, re->filename) );
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
		return i18n("Driver");
		else if (section==1)
		return i18n("Remote Descriptor");
	}

    return QVariant();
}
