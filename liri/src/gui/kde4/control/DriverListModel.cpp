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

#include <QStringList>
#include <KDE/KLocale>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <kdesktopfile.h>
#include <kconfiggroup.h>

#include "config.h"
#include "DriverListModel.h"


DriverListModel::DriverListModel() {
	reload();
}

DriverListModel::~DriverListModel() {

}

void DriverListModel::reload() {
	drivers.clear();

	//get all installed drivers
	QDir receiverDir(QLatin1String(LIRI_SYSTEM_DRIVER_DESCRIPTION_DIR));
	QStringList receiversList = receiverDir.entryList(QStringList()<<QLatin1String("*.desktop"));
	foreach(QString file, receiversList) {
		QString id = QFileInfo(file).baseName();
		KDesktopFile dfile(receiverDir.absoluteFilePath(file));
		KConfigGroup gr = dfile.desktopGroup();
		DriverTextLine dr;
		dr.id = id;
		if (gr.hasKey("X-Driver-Version"))
			dr.version = gr.readEntry("X-Driver-Version");
		else
			dr.version = QLatin1String("0.0");
		dr.name = dfile.readName();
		dr.description = dfile.readComment();
		drivers.append(dr);
	}
}

int DriverListModel::rowCount(const QModelIndex &) const {
	return drivers.size();
}

QVariant DriverListModel::data(const QModelIndex &index, int role) const {
	if ( !index.isValid() ) {
		return QVariant();
	}

	const DriverTextLine* line = &(drivers[index.row()]);

	switch ( role ) {
		case Qt::DisplayRole:
			switch (index.column()) {
				case 0: return line->id;
				case 1: return line->version;
				case 2: return line->name;
				default: break;
			}
			break;
		case Qt::ToolTipRole:
			return line->description;
			break;
		default:
			break;
	}
	return QVariant();
}

int DriverListModel::columnCount(const QModelIndex &) const {
	return 3;
}

QVariant DriverListModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole) return QVariant();

	if (orientation == Qt::Horizontal) {
		switch (section) {
			case 0: return i18n("ID");
			case 1: return i18n("Version");
			case 2: return i18n("Name");
			default: break;
		}
	}

	return QVariant();
}
