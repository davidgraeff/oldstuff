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
#ifndef LIRI_EXECUTIONLISTMODEL_H_
#define LIRI_EXECUTIONLISTMODEL_H_

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class BusConnection;

class ExecutionInfo {
	public:
		int rid;
		QString title;
		ExecutionInfo* parent;
		int row;
		QList<ExecutionInfo*> childs;
		ExecutionInfo() { parent = 0; row = 0; }
		~ExecutionInfo() { qDeleteAll(childs); childs.clear(); }
};

class ExecutionListModel : public QAbstractItemModel {
     Q_OBJECT
public:
	ExecutionListModel(BusConnection* busconnection);
	~ExecutionListModel();
	QString getApplicationProfileName(const QString& uid);
	QModelIndex parent ( const QModelIndex & index ) const;
	QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
			    int role = Qt::DisplayRole) const;
private Q_SLOTS:
	void deviceAddedExecution(int rid);
	void deviceRemovedExecution(int rid);
	void changed(int rid);
private:
	ExecutionInfo* root;
	BusConnection* busconnection;
};
#endif
