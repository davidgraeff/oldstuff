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
#include <kcategorizedsortfilterproxymodel.h>

#include "config.h"
#include "ActionsModel.h"

ActionsModel::ActionsModel(DaemonConnection* dconnection, ExecutionConnection* econnection) :
	QAbstractListModel(0), dconnection(dconnection), econnection(econnection) { }

ActionsModel::~ActionsModel() {
	clear();
}

void ActionsModel::clear() {
	foreach(ActionInfo* ai, actions)
		delete ai;
	actions.clear();
	reset();
}

void ActionsModel::refresh() {
	clear();

	if (!econnection->getExecutionInterface() || !dconnection->getReceiverListInterface()) return;
	QList<int> allinstances = dconnection->getReceiverListInterface()->getAllInstances();
	OrgLiriExecutionInstanceInterface* instanceObject;
	foreach (int instance, allinstances) {
		instanceObject = econnection->getExecutionInstanceInterface(instance);
		// actions
		QStringList actionslist = instanceObject->Actions();
		foreach (QString actionid, actionslist) {
			QStringList actionidd = actionid.split(QChar(46));
			if (actionidd.size() != 4) continue;
			ActionInfo* rainfo = new ActionInfo;
			rainfo->instance = instance;
			rainfo->instancename = i18n("Instance: %1", rainfo->instance);
			if (actionidd[0].size()) rainfo->name = i18n("Mode: %1 ", actionidd[0]);
			rainfo->name += i18n("Key: %1, KeyState: %2", actionidd[1], actionidd[2]);
			rainfo->tooltip = i18n("Actions: %1", actionidd[3]);
			actions.append(rainfo);
		}
	}

	reset();
}

/**************** For model access ****************/
int ActionsModel::rowCount(const QModelIndex &) const {
	return actions.size();
}

QVariant ActionsModel::data(const QModelIndex &index, int role) const {
	if ( !index.isValid() ) {
		return QVariant();
	}

	ActionInfo* info = actions[index.row()];

	switch ( role ) {
		case Qt::DisplayRole:
			return info->name;
		case Qt::ToolTipRole:
			return info->tooltip;
			break;
		case KCategorizedSortFilterProxyModel::CategoryDisplayRole:
			return info->instancename;
			break;
		case KCategorizedSortFilterProxyModel::CategorySortRole:
			return info->instance;
			break;
		default:
			break;
	}
	return QVariant();
}

QVariant ActionsModel::headerData(int section, Qt::Orientation orientation,
                         int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		if (section==0)
			return i18n("Mode");
		else if (section==1)
			return i18n("Key");
		else if (section==2)
			return i18n("KeyState");
		else if (section==3)
			return i18n("Actions");
	}

	return QVariant();
}

