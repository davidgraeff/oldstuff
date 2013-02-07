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
#include "Remotes_Model.h"
#include "fileformats/RemoteFile.h"

#include <QDir>
#include <QString>
#include <QStringList>
#include <QDebug>

#include <KDE/KLocale>

#include "config.h"
/* reading directories and the home dir of the current user */
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h> //getpwuid
#include <unistd.h> //getuid

Remotes_Model::Remotes_Model(QObject *parent) : QAbstractListModel(parent) {
	decorationIcon = KIcon(QLatin1String("liri-client-event"));
	reload();
}

Remotes_Model::~Remotes_Model() {
	clear();
}

void Remotes_Model::clear() {
	qDeleteAll(list);
	list.clear();
}

void Remotes_Model::reload() {
	clear();

	QDir resystem(QLatin1String(LIRI_SYSTEM_REMOTES_DIR));
	QStringList filter; filter << QLatin1String("*.desktop");
	QStringList files = resystem.entryList(filter, QDir::Readable | QDir::NoDotAndDotDot | QDir::Files);

	foreach (QString file, files) {
		RemoteFile* re = new RemoteFile(DesktopFile::getUidOfFilename(file));
		/* is this application profile part of applicationprofiles */
		if (re->getState() == DesktopFile::Valide) {
			list.append(re);
		} else {
			delete re;
		}
	}
	reset();
}

int Remotes_Model::rowCount(const QModelIndex &) const {
	return list.count();
}

QVariant Remotes_Model::data(const QModelIndex &index, int role) const {
	QVariant theData;
	if ( !index.isValid() ) {
		return QVariant();
	}

	RemoteFile* mi = list[index.row()];

	switch ( role ) {
		case Qt::DisplayRole:
			theData.setValue( mi->getName()+ QLatin1Char('\n') + mi->getComment() );
			break;
		case Qt::DecorationRole:
			theData.setValue( static_cast<QIcon>(decorationIcon) );
			break;
		case Qt::ToolTipRole:
			theData.setValue( mi->getName()+ QLatin1Char('\n') + mi->getComment() );
			break;
		case Qt::UserRole:
			theData.setValue((void*)mi);
			break;
		default:
			break;
	}
	return theData;
}

QVariant Remotes_Model::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal && section == 0) {
			return i18n("Remotes");
	}

	return QVariant();
}

void Remotes_Model::edit(RemoteFile* newprofile) {
	Q_ASSERT(newprofile);
	int index = -1;
	for (int i=0;i<list.size();++i) if (list[i]->getUid() == newprofile->getUid()) { index=i;break; }

	// new
	if (index == -1) {
		index = list.size();
		beginInsertRows(QModelIndex(), index, index);
		list.append(newprofile);
		endInsertRows();
	} else { // edit
		if (newprofile == list[index]) return; // same object, do nothing
		delete list[index];
		list[index] = newprofile;
		QModelIndex modelindex = createIndex(index,0);
		emit dataChanged(modelindex, modelindex);
	}
}

void Remotes_Model::remove(RemoteFile* newprofile) {
	Q_ASSERT(newprofile);
	int index = -1;
	for (int i=0;i<list.size();++i) if (list[i]->getUid() == newprofile->getUid()) { index=i;break; }

	if (index == -1) return;

	beginRemoveRows(QModelIndex(), index, index);
	delete list[index];
	list.removeAt(index);
	endRemoveRows();
}
