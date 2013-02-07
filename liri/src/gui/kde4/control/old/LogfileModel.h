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
#ifndef LIRI_ACTIONSMODEL_H_
#define LIRI_ACTIONSMODEL_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QString>
#include "businterconnect/ExecutionConnection.h"
#include "businterconnect/Dbus_proxy_execution.h"
#include "businterconnect/Dbus_proxy_executioninstance.h"
#include "businterconnect/DaemonConnection.h"
#include "businterconnect/Dbus_proxy_receiverlist.h"

class ActionInfo {
	public:
		long long instance;
		QString instancename;
		QString name;
		QString tooltip;
};

class ActionsModel : public QAbstractListModel {
     Q_OBJECT
public:
	ActionsModel(DaemonConnection* dconnection, ExecutionConnection* econnection);
	~ActionsModel();
	void clear();
	void refresh();
	/* for model access */
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
			    int role = Qt::DisplayRole) const;
private:
	QList<ActionInfo*> actions;
	DaemonConnection* dconnection;
	ExecutionConnection* econnection;
};

#endif
