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
#include "RemotesModel.h"
#include "fileformats/RemoteFile.h"

#include <QDir>
#include <QString>
#include <QStringList>
#include <QDebug>

#include <KDE/KLocale>

#include "config.h"

RemotesModel::RemotesModel(QObject *parent) : QAbstractListModel(parent) {
	icon = KIcon(QLatin1String("liri-client-event"));
	reload();
}

RemotesModel::~RemotesModel() {
	qDeleteAll(list);
}

void RemotesModel::reload() {
	qDeleteAll(list);
	list.clear();

	QDir resystem(QLatin1String(LIRI_SYSTEM_REMOTES_DIR));
	QStringList filter; filter << QLatin1String("*.desktop");
	QStringList files = resystem.entryList(filter, QDir::Readable | QDir::NoDotAndDotDot | QDir::Files);

	foreach (QString file, files) {
		RemoteFile* re = new RemoteFile(DesktopFile::getUidOfFilename(file));
		if (re->getState() == DesktopFile::Valide) {
			RemoteItem *item = new RemoteItem();
			item->text = re->getName();
			item->uid = re->getUid();
			item->icon = &icon;
			list.append(item);
		}
		delete re;
	}
}

int RemotesModel::getIndexOf(const QString& remoteuid) {
	for (int i=0;i<list.size();++i) {
		if (list[i]->uid == remoteuid) return i;
	}
	return -1;
}

QString RemotesModel::getUidOf(int index) {
	return list[index]->uid;
}

int RemotesModel::rowCount(const QModelIndex &) const {
	return list.size();
}

QVariant RemotesModel::data(const QModelIndex &index, int role) const {
	if ( !index.isValid() ) {
		return QVariant();
	}

	RemoteItem* item = static_cast<RemoteItem*>(list[index.row()]);

	switch ( role ) {
		case Qt::DisplayRole:
			return item->text;
			break;
		case Qt::DecorationRole:
			return *(static_cast<QIcon*>(item->icon));
			break;
		case Qt::ToolTipRole:
			return item->uid;
			break;
		case Qt::UserRole:
			return item->uid;
			break;
		default:
			break;
	}

	return QVariant();
}
