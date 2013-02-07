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
#include "../messages.h"

#include "ReceiversList.h"
#include <cstdio>

ReceiverList::ReceiverList() : QAbstractListModel(0) {
	//get all installed receivers
	QDir receiverDir(QLatin1String(LIRI_SYSTEM_DRIVER_DESCRIPTION_DIR));
	QStringList receiversList = receiverDir.entryList(QStringList()<<QLatin1String("*.desktop"));
	foreach(QString file, receiversList) {
		QString id = QFileInfo(file).baseName();
		KDesktopFile dfile(receiverDir.absoluteFilePath(file));
		ReceiverInfo* re = new ReceiverInfo();
		re->name = dfile.readName();
		re->description = dfile.readComment();
		receiverinfos.insert(id, re);
	}
}

ReceiverList::~ReceiverList() {
	foreach(Receiver* re, receivers)
		delete re;
	receivers.clear();
	foreach(ReceiverInfo* re, receiverinfos)
		delete re;
	receiverinfos.clear();
}

void ReceiverList::changed(int instance, int state, const QString& timeConnected, const QString& timeDisconnected, const QString& udi, const QString& driver, const QString& id) {
	Receiver* re = 0;
	ReceiverInfo* reinfo = 0;
	for (int i = 0; i < receivers.size(); ++i) {
		if (receivers[i]->instance == instance) {
			re = receivers[i];
			break;
		}
	}
	
	/* not found: create new */
	if (re == 0) {
		re = new Receiver();
		re->id = id;
		re->udi = udi;
		re->udi = re->udi.mid(re->udi.lastIndexOf(QLatin1Char('/'))+1);
		if (receiverinfos.contains(driver)) {
			reinfo = receiverinfos[driver];
			re->name = reinfo->name;
			re->description = reinfo->description;
			re->installed = true;
		} else {
			re->installed = false;
			re->name = driver;
		}
		receivers.prepend(re);
	}

	/* update */
	re->state = state;
	re->instance = instance;
	re->connectedtime = timeConnected;
	re->disconnectedtime = timeDisconnected;

	reset();
}

void ReceiverList::changedRemote(int instance, int state, const QString& remoteUid, const QString& remoteName) {
	Receiver* re = 0;
	for (int i = 0; i < receivers.size(); ++i) {
		if (receivers[i]->instance == instance) {
			re = receivers[i];
			break;
		}
	}

	if (!re) return;

	re->remoteuid = remoteUid;
	re->remotename = remoteName;
	re->remotestate = state;

	reset();
}

void ReceiverList::clear() {
	receivers.clear();
	reset();
}

/**************** For model access ****************/
int ReceiverList::rowCount(const QModelIndex &) const {
	return receivers.size();
}

int ReceiverList::columnCount(const QModelIndex &) const {
	return 5;
}

QVariant ReceiverList::data(const QModelIndex &index, int role) const {
	if ( !index.isValid() ) {
		return QVariant();
	}

	Receiver* re = receivers[index.row()];
	switch ( role ) {
		case Qt::DisplayRole:
			if (index.column() == 0)
				return re->id;
			else if (index.column() == 1)
				return trLiri.msg(re->state);
			else if (index.column() == 2)
				return trLiri.msg(re->remotestate);
			else if (index.column() == 3)
				return re->name;
			else if (index.column() == 4)
				return re->remotename;
			break;
		case Qt::ToolTipRole:
			if (re->state != LIRIOK_offline) {
				return i18n("Connected: %1\nHal ID: %2\nRemote uid: %3",
					re->connectedtime, re->udi, re->remoteuid);
			} else {
				return i18n("Connected: %1\nDisconnected: %2\nRemote uid: %3",
					re->connectedtime, re->disconnectedtime, re->remoteuid);
			}
			break;
		default:
			break;
	}
	return QVariant();
}

QVariant ReceiverList::headerData(int section, Qt::Orientation orientation,
                         int role) const {
     if (role != Qt::DisplayRole)
         return QVariant();

     if (orientation == Qt::Horizontal) {
		if (section==0)
			return i18n("Receiver\nID");
		else if (section==1)
			return i18n("Receiver\nState");
		else if (section==2)
			return i18n("Remote\nState");
		else if (section==3)
			return i18n("Driver\nName");
		else if (section==4)
			return i18n("Remote\nName");
	 }

    return QVariant();
}

