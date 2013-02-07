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
#include "DP_Model.h"
#include "fileformats/DesktopProfileFile.h"
#include "fileformats/RemoteFile.h"
#include "fileformats/RemoteLoadProfilesFile.h"

#include <QDir>
#include <QString>
#include <QStringList>
#include <QDebug>

#include <KDE/KLocale>

#include "config.h"

DP_Model::DP_Model(QObject *parent) : QAbstractListModel(parent) {
	marker = 0;
	reload();
}

DP_Model::~DP_Model() {
	clear();
}

inline void DP_Model::mark(int i) {
	if (!marker)
		marked[i] = Qt::Unchecked;
	else {
		if (marker->isAll()) {
			marked[i] = Qt::PartiallyChecked;
		} else if (marker->contains(list[i]->getUid())) {
			marked[i] = Qt::Checked;
		} else {
			marked[i] = Qt::Unchecked;
		}
	}
}

void DP_Model::markControlled(RemoteFile* re) {
	delete marker;
	marker = 0;

	if (re)
		marker = new RemoteLoadProfilesFile(re->getUid());

	/* iterate over every desktop profile and check if it is controlled by re */
	/* Laufzeit in O ( n ), n:desktops.size, lookup if controlled with hash */
	for (int i = 0; i < list.size(); ++i) {
		mark(i);
	}

	reset();
}

void DP_Model::clear() {
	qDeleteAll(list);
	list.clear();
	decoration.clear();
	delete marker;
	marker = 0;
	reset();
}

void DP_Model::reload() {
	clear();

	QDir dphome = QDir::home();
	dphome.cd(QLatin1String(LIRI_HOME_DESKTOPPROFILES_DIR));
	QStringList filter; filter << QLatin1String("*.desktop");

	/* iterate over all desktop profiles and take application profile uids
	from those that are controlled by this remoteuid */
	QStringList files = dphome.entryList(filter, QDir::Readable | QDir::NoDotAndDotDot | QDir::Files);
	foreach (QString file, files) {
		DesktopProfileFile* dp = new DesktopProfileFile(DesktopFile::getUidOfFilename(file));
		/* is this application profile part of applicationprofiles */
		if (dp->getState() == DesktopFile::Valide) {
			list.append(dp);
			marked.append(Qt::Unchecked);
			decoration.append(KIcon(dp->getIconFilename()));
		} else {
			delete dp;
		}
	}
	reset();
}

int DP_Model::rowCount(const QModelIndex &) const {
	return list.count();
}

int DP_Model::columnCount(const QModelIndex &) const {
	return 2;
}

QVariant DP_Model::data(const QModelIndex &index, int role) const {
	QVariant theData;
	if ( !index.isValid() ) {
		return QVariant();
	}

	DesktopProfileFile* mi = list[index.row()];

	switch ( role ) {
		case Qt::DisplayRole:
			if (index.column() == 1)
				theData.setValue( mi->getName() );
			else
				theData.setValue( mi->getName() + QLatin1Char('\n') + mi->getComment() );
			break;
		case Qt::DecorationRole:
			theData.setValue( static_cast<QIcon>(decoration[index.row()]) );
			break;
		case Qt::ToolTipRole:
				theData.setValue( mi->getName() + QLatin1Char('\n') + mi->getComment() );
			break;
		case Qt::CheckStateRole:
			if (index.column() == 1 && marker)
				theData.setValue( marked[index.row()] );
			break;
		case Qt::UserRole:
			theData.setValue((void*)mi);
			break;
		default:
			break;
	}

	return theData;
}

QVariant DP_Model::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		if (section == 0)
			return i18n("Desktop Profiles");
	}
	return QVariant();
}

Qt::ItemFlags DP_Model::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

bool DP_Model::setData ( const QModelIndex & index, const QVariant & value, int role) {
	if (index.isValid() && role == Qt::CheckStateRole && marker && marked[index.row()] != Qt::PartiallyChecked) {
		marked[index.row()] = value.value<int>();
		if (marked[index.row()]) {
			marker->insert(list[index.row()]->getUid());
		} else {
			marker->remove(list[index.row()]->getUid());
		}
		marker->save();
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

void DP_Model::edit(DesktopProfileFile* newprofile) {
	Q_ASSERT(newprofile);
	int index = -1;
	for (int i=0;i<list.size();++i) if (list[i]->getUid() == newprofile->getUid()) { index=i;break; }

	// new
	if (index == -1) {
		index = list.size();
		beginInsertRows(QModelIndex(), index, index);
		list.append(newprofile);
		marked.append(Qt::Unchecked);
		mark(index);
		decoration.append(KIcon(newprofile->getIconFilename()));
		endInsertRows();
	} else { // edit
		if (newprofile == list[index]) return; // same object, do nothing
		delete list[index];
		list[index] = newprofile;
		decoration[index] = KIcon(newprofile->getIconFilename());
		mark(index);
		QModelIndex modelindex = createIndex(index,0);
		emit dataChanged(modelindex, modelindex);
	}
}

void DP_Model::remove(DesktopProfileFile* newprofile) {
	Q_ASSERT(newprofile);
	int index = -1;
	for (int i=0;i<list.size();++i) if (list[i]->getUid() == newprofile->getUid()) { index=i;break; }

	if (index == -1) return;

	beginRemoveRows(QModelIndex(), index, index);
	delete list[index];
	list.removeAt(index);
	decoration.removeAt(index);
	marked.removeAt(index);
	endRemoveRows();
}
