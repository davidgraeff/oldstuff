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
#include "AP_Model.h"
#include "fileformats/ApplicationProfileFile.h"
#include "fileformats/DesktopProfileFile.h"

#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QDebug>

#include <KDE/KLocale>

#include "config.h"

AP_Model::AP_Model(QObject *parent) : QAbstractListModel(parent) {
	reload();
}

AP_Model::~AP_Model() {
	clear();
}

inline void AP_Model::mark(int i, DesktopProfileFile* dp) {
	if (!dp)
		marked[i] = Qt::Unchecked;
	else {
		if ( dp->contains(list[i]->getUid()) ) {
			marked[i] = Qt::Checked;
		} else {
			marked[i] = Qt::Unchecked;
		}
	}
}

void AP_Model::markUsed(DesktopProfileFile* dp) {
	/* iterate over every application profile and check if it is used by dp */
	/* Laufzeit in O ( n*k ), n:apps.size, k:desks.size */
	for (int i = 0; i < list.size(); ++i) {
		mark(i, dp);
	}
	marker = dp;
	reset();
}

void AP_Model::clear() {
	qDeleteAll(list);
	list.clear();
	marked.clear();
	decoration.clear();
	marker = 0;
}

void AP_Model::reload() {
	clear();

	QStringList filter;
	QDir aphome = QDir::home();
	aphome.cd(QLatin1String(LIRI_HOME_APPPROFILES_DIR));
	filter << QLatin1String("*.desktop");

	ApplicationProfileFile* ap;

	/* iterate over all desktop profiles and take application profile uids
	from those that are controlled by this remoteuid */
	QStringList files = aphome.entryList(filter, QDir::Readable | QDir::NoDotAndDotDot | QDir::Files);
	foreach (QString file, files) {
		ap = new ApplicationProfileFile(DesktopFile::getUidOfFilename(file));
		/* is this application profile part of ApplicationProfileFiles */
		if (ap->getState() == DesktopFile::Valide) {
			edit(ap);
		} else {
			delete ap;
		}
	}
	reset();
}

int AP_Model::rowCount(const QModelIndex &) const {
	return list.count();
}

QVariant AP_Model::data(const QModelIndex &index, int role) const {
	QVariant theData;
	if ( !index.isValid() ) {
	    return QVariant();
	}

	ApplicationProfileFile* mi = list[index.row()];
	switch ( role ) {
		case Qt::DisplayRole:
			theData.setValue( mi->getName() );
			break;
		case Qt::DecorationRole:
			theData.setValue( static_cast<QIcon>(decoration[index.row()]) );
			break;
		case Qt::ToolTipRole:
			theData.setValue( mi->getName()+ QLatin1Char('\n') + mi->getComment() );
			break;
		case Qt::CheckStateRole:
			if (marker)
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

QVariant AP_Model::headerData(int section, Qt::Orientation orientation,
                 int role) const {
     if (role != Qt::DisplayRole)
         return QVariant();

     if (orientation == Qt::Horizontal && section==0)
         return i18n("Application Profiles");

    return QVariant();
}

Qt::ItemFlags AP_Model::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

bool AP_Model::setData ( const QModelIndex & index, const QVariant & value, int role) {
	if (index.isValid() && role == Qt::CheckStateRole && marker) {
		marked[index.row()] = value.value<int>();
		if (marked[index.row()]) {
			marker->insert( list[index.row()]->getUid() );
		} else {
			marker->remove( list[index.row()]->getUid() );
		}
		emit saveDesktopfile(marker,false);
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

void AP_Model::edit(ApplicationProfileFile* newprofile) {
	Q_ASSERT(newprofile);
	int index = -1;
	for (int i=0;i<list.size();++i) if (list[i]->getUid() == newprofile->getUid()) { index=i;break; }

	// new
	if (index == -1) {
		index = list.size();
		beginInsertRows(QModelIndex(), index, index);
		list.append(newprofile);
		marked.append(Qt::Unchecked);
		decoration.append(KIcon(newprofile->getIconFilename()));
		mark(index, marker);
		endInsertRows();
	} else { // edit
		if (newprofile == list[index]) return; // same object, do nothing
		delete list[index];
		list[index] = newprofile;
		decoration[index] = KIcon(newprofile->getIconFilename());
		mark(index, marker);
		QModelIndex modelindex = createIndex(index,0);
		emit dataChanged(modelindex, modelindex);
	}
}

void AP_Model::remove(ApplicationProfileFile* newprofile) {
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
