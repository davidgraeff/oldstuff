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
#include "MessagesModel.h"
#include "messages.h"

#include "businterconnect/BusConnection.h"
#include "businterconnect/Control_DevManager_dbusproxy.h"
#include "businterconnect/Control_Execution_dbusproxy.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"
#include "businterconnect/Receiver_Execution_dbusproxy.h"

MessagesModel::MessagesModel(BusConnection* busconnection) :
	QAbstractListModel(0), busconnection(busconnection) {

	// an object for translating int codes to readable messages
	trLiriMessages = new trLiriClass();

	// important: busconnection != 0
	Q_ASSERT(busconnection);

	connect(busconnection, SIGNAL( devicemanagerStateChanged(int) ), SLOT( devicemanagerStateChanged(int) ));
	connect(busconnection, SIGNAL( executionengineStateChanged(int) ), SLOT( executionengineStateChanged(int) ));
	connect(busconnection, SIGNAL( deviceAdded(int) ), SLOT( deviceAdded(int) ));
	connect(busconnection, SIGNAL( deviceRemoved(int) ), SLOT( deviceRemoved(int) ));
	connect(busconnection, SIGNAL( executed(int, int, const QString &) ), SLOT( executed(int, int, const QString &) ));
	connect(busconnection, SIGNAL( modeChanged(int, const QString &, const QString &) ), SLOT( modeChanged(int, const QString &, const QString &) ));
	connect(busconnection, SIGNAL( targetChanged(int, const QString &) ), SLOT( targetChanged(int, const QString &) ));
	connect(busconnection, SIGNAL( deviceAddedExecution(int) ), SLOT( deviceAddedExecution(int) ));
	connect(busconnection, SIGNAL( deviceRemovedExecution(int) ), SLOT( deviceRemovedExecution(int) ));
	connect(busconnection, SIGNAL( profilesLoaded(int) ), SLOT( profilesLoaded(int) ));

	// inital device adding
	QList<int> receivers = busconnection->receivers();
	foreach(int rid, receivers) deviceAdded(rid);
	foreach(int rid, receivers) deviceAddedExecution(rid);
}

MessagesModel::~MessagesModel() {
	delete trLiriMessages;
	clear();
}

void MessagesModel::setListenKeys(bool b) {
	this->listenkeys = b;
}

void MessagesModel::clear() {
	messagelist.clear();
	reset();
}

int MessagesModel::rowCount(const QModelIndex &) const {
	return messagelist.size();
}

QVariant MessagesModel::data(const QModelIndex &index, int role) const {
	if ( !index.isValid() ) {
		return QVariant();
	}

	switch ( role ) {
		case Qt::DisplayRole:
		case Qt::ToolTipRole:
 			return messagelist[index.row()];
			break;
		default:
			break;
	}
	return QVariant();
}

void MessagesModel::addMessage(const QString& text) {
	messagelist.prepend(text);
	reset();
}

void MessagesModel::devicemanagerStateChanged(int state) {
	addMessage(QLatin1String("Device manager state changed: ") +trLiriMessages->msg(state));
}

void MessagesModel::executionengineStateChanged(int state) {
	addMessage(QLatin1String("Execution engine state changed: ") +trLiriMessages->msg(state));
}

void MessagesModel::deviceAddedExecution(int rid) {
	addMessage(QLatin1String("Execution Device added. UID: ") + QString::number(rid));
}

void MessagesModel::deviceRemovedExecution(int rid) {
	addMessage(QLatin1String("Execution Device removed. UID: ") + QString::number(rid));
}

void MessagesModel::profilesLoaded(int rid) {
	addMessage(QLatin1String("Profiles loaded. UID: ") + QString::number(rid));
}

void MessagesModel::deviceAdded(int rid) {
	addMessage(QLatin1String("Device added. UID: ") + QString::number(rid));
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

void MessagesModel::deviceRemoved(int rid) {
	addMessage(QLatin1String("Device removed. UID: ") + QString::number(rid));
}

void MessagesModel::executed(int rid, int /*result*/, const QString &executed) {
	addMessage(QLatin1String("Command executed: ") + executed + QLatin1String(", UID: ") + QString::number(rid));
}

void MessagesModel::modeChanged(int rid, const QString &oldmode, const QString &newmode) {
	addMessage(QLatin1String("Mode changed from ") + oldmode +
		QLatin1String(" to ") + newmode + QLatin1String(", UID: ") + QString::number(rid));
}

void MessagesModel::targetChanged(int state, const QString &targetname) {
	if (state == 0) {
		addMessage(QLatin1String("Lost target application: ") + targetname);
	} else {
		addMessage(QLatin1String("Add target application: ") + targetname);
	}
}

void MessagesModel::driverChangedSettings(const QMap<QString,QString> &changedsettings) {
	QMap<QString,QString>::const_iterator it = changedsettings.begin();
	for (;it != changedsettings.end(); ++it)
		addMessage(QLatin1String("Driver changed a setting. Key:") + it.key() + QLatin1String("Value: ") + it.value());
}

void MessagesModel::key(const QString &keycode, const QString &keyname, uint channel, int pressed) {
	if (!listenkeys) return;
	Q_UNUSED(channel);
	if (pressed) {
		addMessage(QLatin1String("Key pressed. Code: ") + keycode + QLatin1String("Name: ") + keyname);
	} else {
		addMessage(QLatin1String("Key pressed. Code: ") + keycode + QLatin1String("Name: ") + keyname);
	}
}

void MessagesModel::receiverStateChanged(int state) {
	addMessage(QLatin1String("Receiver state changed: ") +trLiriMessages->msg(state));
}

void MessagesModel::remoteStateChanged(int state) {
	addMessage(QLatin1String("Remote state changed: ") +trLiriMessages->msg(state));
}
