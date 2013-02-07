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
#ifndef LIRI_TARGETLISTMODEL_H_
#define LIRI_TARGETLISTMODEL_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QString>

class BusConnection;
class OrgLiriExecutionControlInterface;

class TargetInfo {
	public:
		int checked;
		QString name;
		QString abstract;
		QString concrete;
};

class TargetListModel : public QAbstractListModel {
     Q_OBJECT
public:
	TargetListModel(BusConnection* busconnection);
	~TargetListModel();
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
private Q_SLOTS:
	void targetChanged(int state, const QString &targetid);
	void executionengineStateChanged(int state);
private:
	QList<TargetInfo> targetlist;
	BusConnection* busconnection;
	OrgLiriExecutionControlInterface* ci;
};

#endif
