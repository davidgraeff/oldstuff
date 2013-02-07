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
#ifndef LIRI_DRIVERLISTMODEL_H_
#define LIRI_DRIVERLISTMODEL_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QFile>

class DriverTextLine {
	public:
		QString name;
		QString version;
		QString id;
		QString description;
};

class DriverListModel : public QAbstractListModel {
     Q_OBJECT
public:
	DriverListModel();
	~DriverListModel();
	void reload();
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
			    int role = Qt::DisplayRole) const;
private:
	QList<DriverTextLine> drivers;
};

#endif
