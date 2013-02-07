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
#include <KDE/KLocale>

#include "config.h"
#include "TargetListModel.h"
#include "businterconnect/BusConnection.h"
#include "businterconnect/Control_DevManager_dbusproxy.h"
#include "businterconnect/Control_Execution_dbusproxy.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"
#include "businterconnect/Receiver_Execution_dbusproxy.h"

TargetListModel::TargetListModel(BusConnection* busconnection) :
	QAbstractListModel(0), busconnection(busconnection), ci(0) {
	Q_ASSERT(busconnection);
	connect(busconnection, SIGNAL(executionengineStateChanged(int)),SLOT(executionengineStateChanged(int)));
	// in what state is the execution engine?
	executionengineStateChanged(busconnection->executionEngineState());
}

TargetListModel::~TargetListModel() {

}

int TargetListModel::rowCount(const QModelIndex &) const {
	return targetlist.size();
}

QVariant TargetListModel::data(const QModelIndex &index, int role) const {
	if ( !index.isValid() ) {
		return QVariant();
	}

	switch ( role ) {
		case Qt::DisplayRole:
			return targetlist[index.row()].concrete;
			break;
		case Qt::ToolTipRole:
			return targetlist[index.row()].name;
			break;
		case Qt::CheckStateRole:
			return targetlist[index.row()].checked;
			break;
		default:
			break;
	}
	return QVariant();
}

Qt::ItemFlags TargetListModel::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

bool TargetListModel::setData ( const QModelIndex & index, const QVariant & value, int role ) {
	if (index.isValid() && role == Qt::CheckStateRole) {
		Q_ASSERT(ci);
		targetlist[index.row()].checked = value.value<int>();
		QString targetid = targetlist[index.row()].abstract + QLatin1Char('@') + targetlist[index.row()].concrete;
		if (targetlist[index.row()].checked == Qt::Checked) { // a target should be activated
			ci->setTarget(targetid, true);
		} else if (targetlist[index.row()].checked == Qt::Unchecked) { // a target should be deactivated
			ci->setTarget(targetid, false);
		}
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

void TargetListModel::targetChanged(int state, const QString &targetid) {
	// FORMAT: abstract_serviceid@concrete_serviceid
	QStringList arg = targetid.split(QLatin1Char('@'));
	Q_ASSERT(arg.size()==2);
	// in list?
	for (int i=0;i<targetlist.size();++i) {
		if (targetlist[i].concrete == arg[1]) {
			if (state!=-1) { // modify entry
				targetlist[i].checked = ((state) ? Qt::Checked : Qt::Unchecked);
				return;
			} else { // remove entry if state==-1
				targetlist.removeAt(i);
				reset();
				return;
			}
		}
	}

	// not in list, prepare a new targetinfo structure
	TargetInfo target;
	target.abstract = arg[0];
	target.concrete = arg[1];
	target.name = target.concrete.mid(target.concrete.lastIndexOf(QLatin1Char('/')));
	target.checked = ((state) ? Qt::Checked : Qt::Unchecked);

	// add the structure to the model
	beginInsertRows(QModelIndex(), targetlist.size(), targetlist.size());
	targetlist.append(target);
	endInsertRows();
}

void TargetListModel::executionengineStateChanged(int state) {
	if (state == LIRI_RUNNING) {
		// set control interface for the execution engine if it is running
		ci = busconnection->getExecutionEngineControl();
		if (ci) {
			// connect signal to get information about target changes
			connect(ci, SIGNAL( targetChanged(int,const QString &) ), SLOT( targetChanged(int,const QString &) ));
			// get current targets
			QStringList targets = ci->getTargets();
			// last character of the targetid_ is 1 or 0 (active or not)
			foreach (QString targetid_, targets) {
				// first (active state): split the last character and convert it to an integer
				targetChanged(targetid_.right(1).toInt(), targetid_.left(targetid_.size()-1));
			}
		}
	} else if (state == LIRI_UNKNOWN) {
		targetlist.clear();
		reset(); // reset model
		ci = 0;
	}
}