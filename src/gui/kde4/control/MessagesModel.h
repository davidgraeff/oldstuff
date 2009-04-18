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
#ifndef LIRI_MESSAGESMODEL_H_
#define LIRI_MESSAGESMODEL_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QString>

class BusConnection;
class trLiriClass;

class MessagesModel : public QAbstractListModel {
     Q_OBJECT
public:
	MessagesModel(BusConnection* busconnection);
	~MessagesModel();
	void setListenKeys(bool);
	void clear();
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
private Q_SLOTS:
	void devicemanagerStateChanged(int state);
	void executionengineStateChanged(int state);
	void deviceAdded(int rid);
	void deviceRemoved(int rid);
	void deviceAddedExecution(int rid);
	void deviceRemovedExecution(int rid);
	void profilesLoaded(int rid);
	void executed(int rid, int result, const QString &executed);
	void modeChanged(int rid, const QString &oldmode, const QString &newmode);
	void targetChanged(int state, const QString &targetname);
	void driverChangedSettings(const QMap<QString,QString> &changedsettings);
	void key(const QString &keycode, const QString &keyname, uint channel, int pressed);
	void receiverStateChanged(int state);
	void remoteStateChanged(int state);
private:
	QList<QString> messagelist;
	BusConnection* busconnection;
	void addMessage(const QString& text);
	trLiriClass* trLiriMessages;
	bool listenkeys;
};

#endif
