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

#include "config.h"
#include "ExecutionListModel.h"

#include "businterconnect/BusConnection.h"
#include "businterconnect/Control_DevManager_dbusproxy.h"
#include "businterconnect/Control_Execution_dbusproxy.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"
#include "businterconnect/Receiver_Execution_dbusproxy.h"

/* read desktop and application profiles */
#include "liriexec/actions/ActionChangeMode.h"
#include "liriexec/actions/ActionChangeVariable.h"
#include "liriexec/actions/ActionChangeBusInstance.h"
#include "liriexec/actions/ActionProgram.h"
#include "liriexec/actions/ActionBus.h"
#include "liriexec/actions/Action.h"
#include "liriexec/actions/ActionGroup.h"
#include "fileformats/DesktopProfileFile.h"
#include "fileformats/ApplicationProfileFile.h"

ExecutionListModel::ExecutionListModel(BusConnection* busconnection) : busconnection(busconnection) {
	Q_ASSERT(busconnection);

	root = new ExecutionInfo();

	connect(busconnection, SIGNAL( deviceAddedExecution(const QString &) ), SLOT( deviceAddedExecution(const QString &) ));
	connect(busconnection, SIGNAL( deviceRemovedExecution(const QString &) ), SLOT( deviceRemovedExecution(const QString &) ));
	connect(busconnection, SIGNAL( profilesLoaded(const QString &) ), SLOT( changed(const QString &) ));

	QStringList receivers = busconnection->receivers();
	foreach(QString rid, receivers) deviceAddedExecution(rid);
}

ExecutionListModel::~ExecutionListModel() {
	delete root;
}

QModelIndex ExecutionListModel::parent ( const QModelIndex & index ) const {
	if (!index.isValid()) QModelIndex();

	ExecutionInfo *childItem = static_cast<ExecutionInfo*>(index.internalPointer());
	Q_ASSERT(childItem);

	ExecutionInfo *parentItem = childItem->parent;

	if (parentItem == root)
		return QModelIndex();

	Q_ASSERT(parentItem);
	return createIndex(parentItem->row, 0, parentItem);
}

QModelIndex ExecutionListModel::index ( int row, int column, const QModelIndex & parent ) const {
	if (!parent.isValid()) QModelIndex();
	ExecutionInfo* parentItem = static_cast<ExecutionInfo*>(parent.internalPointer());

	//parent: invisible root parent
	if (!parentItem) {
		return createIndex(row, column, root->childs[row]);
	} else
	//parent: valid
	{
		return createIndex(row, column, parentItem->childs[row]);
	}
}

int ExecutionListModel::rowCount(const QModelIndex &parent) const {
	if (!parent.isValid()) {
		return root->childs.size();
	} else {
		ExecutionInfo* parentItem = static_cast<ExecutionInfo*>(parent.internalPointer());
		return parentItem->childs.size();
	}
}

int ExecutionListModel::columnCount(const QModelIndex &) const {
	return 1;
}

QVariant ExecutionListModel::data(const QModelIndex &index, int role) const {
	if ( !index.isValid() ) return QVariant();
	if ( role != Qt::DisplayRole ) return QVariant();
	return static_cast<ExecutionInfo*>(index.internalPointer())->title;
}

QVariant ExecutionListModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole) return QVariant();

	if (orientation == Qt::Horizontal) {
		if (section==0) return i18n("Loaded application Profiles");
	}

	return QVariant();
}

QString ExecutionListModel::getApplicationProfileName(const QString& uid) {
	QString name;
	ApplicationProfileFile* ap = new ApplicationProfileFile(uid);
	/* valid application profile? */
	if (ap->getState() == DesktopFile::Valide) {
		name = ap->getName();
	} else {
		name = uid;
	}
	delete ap;
	return name;
}

void ExecutionListModel::deviceAddedExecution(const QString & rid) {
	// duplicate?
	for (int i=0;i<root->childs.size();++i)
		if (root->childs[i]->rid == rid) return;

	// get execution engine device
	OrgLiriExecutionReceiverInterface* dev = busconnection->getExecutionEngineReceiver(rid);
	OrgLiriDevManagerReceiverInterface* devman = busconnection->getDeviceManagerReceiver(rid);
	if (!dev || !devman) return;

	QStringList keys; keys << QLatin1String("remotereceiver.duid");
	keys = devman->getSettings(keys);
	Q_ASSERT(keys.size()>0);

	// create entry
	ExecutionInfo* info = new ExecutionInfo();
	info->rid = rid;
	info->title = keys[0];
	info->parent = root;
	info->row = root->childs.size();
	// create childs
	QStringList profileuids = dev->getProfileUids();
	foreach(QString profileuid, profileuids) {
		ExecutionInfo* child = new ExecutionInfo();
		child->rid = -1;
		child->title = getApplicationProfileName(profileuid);
		child->parent = info;
		child->row = info->childs.size();
		info->childs.append(child);
	}

	// add entry
	beginInsertRows(QModelIndex(), root->childs.size(), root->childs.size());
	root->childs.append(info);
	endInsertRows();
}

void ExecutionListModel::deviceRemovedExecution(const QString & rid) {
	int movePos = 0;
	for (int i=0;i<root->childs.size();++i) {
		root->childs[i]->row += movePos;
		if (root->childs[i]->rid == rid) {
			delete root->childs[i];
			root->childs.removeAt(i);
			movePos--;
		}
	}
	reset();
}

void ExecutionListModel::changed(const QString & rid) {
	deviceRemovedExecution(rid);
	deviceAddedExecution(rid);
}
