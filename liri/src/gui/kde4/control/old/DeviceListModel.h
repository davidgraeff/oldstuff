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
#ifndef LIRI_RECEIVERLIST_H_
#define LIRI_RECEIVERLIST_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QByteArray>
#include <time.h> //time_t, time

class ReceiverInfo {
	public:
		QString name;
		QString description;
};

class Receiver {
	public:
		QString id;
		QString name;
		QString description;
		QString udi;
		QString remoteuid;
		QString remotename;
		QString connectedtime;
		QString disconnectedtime;
		int instance;
		int charsinstance;
		int state;
		int remotestate;
		bool installed;
};

class ReceiverList : public QAbstractListModel {
     Q_OBJECT
public:
	ReceiverList();
	~ReceiverList();
	void clear();
	void changed(int instance, int state, const QString& timeConnected, const QString& timeDisconnected, const QString& udi, const QString& driver, const QString& id);
	void changedRemote(int instance, int state, const QString& remoteUid, const QString& remoteName);
	/* for model access */
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
			    int role = Qt::DisplayRole) const;
private:
	//receiverID, Receiver*
	QMap<QString, ReceiverInfo*> receiverinfos;
	QList<Receiver*> receivers;
};

#endif
