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
#ifndef LIRI_DRIVERLIST_H_
#define LIRI_DRIVERLIST_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>

class DaemonConnection;

class DriverInfo {
	public:
		int instance;
		QString id;
		QString remote;
};

class DriverList : public QAbstractListModel {
     Q_OBJECT
public:
	DriverList(DaemonConnection* dconnection);
	~DriverList();

	/* for model access */
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
			    int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
private Q_SLOTS:
	void receiverStateChanged(int instance, int state);
private:
	QList<DriverInfo> drivers;
	DaemonConnection* dconnection;
};

#endif
