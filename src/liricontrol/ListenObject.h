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
#ifndef LIRI_LISTENOBJECT_H_
#define LIRI_LISTENOBJECT_H_

#include <QString>
#include <QObject>
#include <QMap>

class BusConnection;

class ListenObject : QObject {
	Q_OBJECT
public:
	ListenObject(BusConnection* busconnection);
private Q_SLOTS:
	void devicemanagerStateChanged(int state);
	void executionengineStateChanged(int state);
	void deviceAdded(int rid);
	void deviceRemoved(int rid);
	void deviceAddedExecution(int rid);
	void deviceRemovedExecution(int rid);
	void profilesLoaded(int rid);
	void executed(int receiverinstance, int result, const QString &executed);
	void modeChanged(int receiverinstance, const QString &oldmode, const QString &newmode);
	void targetChanged(int state, const QString &targetname);
	void driverChangedSettings(const QMap<QString,QString> &changedsettings);
	void key(const QString &keycode, const QString &keyname, uint channel, int pressed);
	void receiverStateChanged(int state);
	void remoteStateChanged(int state);
private:
	BusConnection* busconnection;
};
#endif
