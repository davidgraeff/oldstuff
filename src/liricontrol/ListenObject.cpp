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
#include "ListenObject.h"
#include "businterconnect/BusConnection.h"
#include "businterconnect/Control_DevManager_dbusproxy.h"
#include "businterconnect/Control_Execution_dbusproxy.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"
#include "businterconnect/Receiver_Execution_dbusproxy.h"

ListenObject::ListenObject(BusConnection* busconnection) : busconnection(busconnection) {
	connect(busconnection, SIGNAL( devicemanagerStateChanged(int) ), SLOT( devicemanagerStateChanged(int) ));
	connect(busconnection, SIGNAL( executionengineStateChanged(int) ), SLOT( executionengineStateChanged(int) ));
	connect(busconnection, SIGNAL( deviceAdded(int) ), SLOT( deviceAdded(int) ));
	connect(busconnection, SIGNAL( deviceRemoved(int) ), SLOT( deviceRemoved(int) ));
	connect(busconnection, SIGNAL( deviceAddedExecution(int) ), SLOT( deviceAddedExecution(int) ));
	connect(busconnection, SIGNAL( deviceRemovedExecution(int) ), SLOT( deviceRemovedExecution(int) ));
	connect(busconnection, SIGNAL( profilesLoaded(int) ), SLOT( profilesLoaded(int) ));
	connect(busconnection, SIGNAL( executed(int, int, const QString &) ), SLOT( executed(int, int, const QString &) ));
	connect(busconnection, SIGNAL( modeChanged(int, const QString &, const QString &) ), SLOT( modeChanged(int, const QString &, const QString &) ));
	connect(busconnection, SIGNAL( targetChanged(int, const QString &) ), SLOT( targetChanged(int, const QString &) ));
	busconnection->reemitAddedSignals(reemitDeviceManager);
	busconnection->reemitAddedSignals(reemitExecutionEngine);
}

void ListenObject::devicemanagerStateChanged(int state) {
	qDebug() << "devicemanagerStateChanged" << state;
}

void ListenObject::executionengineStateChanged(int state) {
	qDebug() << "executionengineStateChanged" << state;
}

void ListenObject::deviceAddedExecution(int rid) {
	qDebug() << "deviceAddedExecution" << rid;
}

void ListenObject::deviceRemovedExecution(int rid) {
	qDebug() << "deviceRemovedExecution" << rid;
}

void ListenObject::profilesLoaded(int rid) {
	qDebug() << "profilesLoaded" << rid;
}

void ListenObject::deviceAdded(int rid) {
	qDebug() << "deviceAdded" << rid;
	OrgLiriDevManagerReceiverInterface * const ri = busconnection->getDeviceManagerReceiver(rid);
	if (!ri) return;
	connect(ri, SIGNAL( driverChangedSettings(const QMap<QString,QString> &) ),
		SLOT( driverChangedSettings(const QMap<QString,QString> &) ));
	connect(ri, SIGNAL( key(const QString &, const QString &, uint, int) ),
		SLOT( key(const QString &, const QString &, uint, int) ));
	connect(ri, SIGNAL( receiverStateChanged(int) ),
		SLOT( receiverStateChanged(int) ));
	connect(ri, SIGNAL( remoteStateChanged(int) ),
		SLOT( remoteStateChanged(int) ));
}

void ListenObject::deviceRemoved(int rid) {
	qDebug() << "deviceRemoved" << rid;
}

void ListenObject::executed(int receiverinstance, int result, const QString &executed) {
	qDebug() << "executed" << receiverinstance << result << executed;
}

void ListenObject::modeChanged(int receiverinstance, const QString &oldmode, const QString &newmode) {
	qDebug() << "modeChanged" << receiverinstance << oldmode << newmode;
}

void ListenObject::targetChanged(int state, const QString &targetname) {
	qDebug() << "targetChanged" << state << targetname;
}

void ListenObject::driverChangedSettings(const QMap<QString,QString> &changedsettings) {
	qDebug() << "driverChangedSettings" << changedsettings;
}

void ListenObject::key(const QString &keycode, const QString &keyname, uint channel, int pressed) {
	qDebug() << "key" << keycode << keyname << channel << pressed;
}

void ListenObject::receiverStateChanged(int state) {
	qDebug() << "receiverStateChanged" << state;
}

void ListenObject::remoteStateChanged(int state) {
	qDebug() << "remoteStateChanged" << state;
}
