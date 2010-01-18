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

#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <kdesktopfile.h>
#include <KDE/KLocale>

#include "config.h"
#include "messages.h"

#include "DeviceListModel.h"
#include "businterconnect/BusConnection.h"
#include "businterconnect/Control_DevManager_dbusproxy.h"
#include "businterconnect/Control_Execution_dbusproxy.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"
#include "businterconnect/Receiver_Execution_dbusproxy.h"
#include "fileformats/RemoteFile.h"

DeviceInfo::DeviceInfo(DeviceListModel* model, BusConnection* busconnection, int rid) :
	model(model), busconnection(busconnection) {

	this->rid = rid;
	icon = 0;
	updatetextflag = false;

	// get device-manager device bus interface
	OrgLiriDevManagerReceiverInterface* iface = busconnection->getDeviceManagerReceiver(rid);
	if (!iface) {
		qWarning() << "DeviceListModel::deviceAdded: interface invalid";
		text_ = i18n("Interface invalid");
		caption = text_;
	} else {
		// signals
		connect(iface, SIGNAL(key(const QString&, const QString&, uint, int)),
			SLOT(key(const QString&, const QString&, uint, int)));
		connect(iface, SIGNAL(receiverStateChanged(int)),
			SLOT(receiverStateChanged(int)));

		receiverStateChanged(iface->ReceiverState());

		QStringList keys;
		keys << QLatin1String("remotereceiver.haludi") << QLatin1String("remotereceiver.duid");
		keys << QLatin1String("remotereceiver.connected") << QLatin1String("remotereceiver.driver");
		keys << QLatin1String("remotereceiver.version") << QLatin1String("remote.uid");
		keys = iface->getSettings(keys);
		if (keys.size() != 6) return;
		remoteuid = keys[5];
		caption = i18n("Device: %1 (Driver: %2)", keys[1], keys[3]);
		remote = i18n("Remote: %1", RemoteFile(keys[5]).getName());
		tooltip = i18n("Hal udi: %1\nConnected: %2", keys[0], keys[2]);

		updatetextflag = true;
		updatetext();
	}
}

DeviceInfo::~DeviceInfo() {

}

void DeviceInfo::key(const QString &, const QString &, uint, int pressed) {
	if (pressed)
		icon = &(model->iconKey);
	else
		icon = &(model->iconRunning);

	model->changed(this);
}

void DeviceInfo::updatetext() {
	if (!updatetextflag) return;

	if (receiverstate==LIRI_DEVICE_INIT)
		icon = &(model->iconInit);
	else if (receiverstate==LIRI_DEVICE_RUNNING_WITH_LAYOUT)
	{
		OrgLiriDevManagerReceiverInterface* iface = busconnection->getDeviceManagerReceiver(rid);
		QStringList keys;
		keys << QLatin1String("remote.uid");
		keys = iface->getSettings(keys);
		if (keys.size() == 1) {
			remoteuid = keys[0];
			remote = i18n("Remote: %1", RemoteFile(keys[0]).getName());
		}
	  updatetext(); 
	  icon = &(model->iconRunning);
	} else if (receiverstate==LIRI_DEVICE_RUNNING_WITHOUT_LAYOUT)
		icon = &(model->iconRunning);
	else
		icon = &(model->iconUnknown);

	status = i18n("Status: %1", model->trLiriMessages->msg(receiverstate));
	text_ = caption + QLatin1Char('\n') + status + QLatin1Char('\n') + remote;

	model->changed(this);
}

void DeviceInfo::receiverStateChanged(int state) {
	receiverstate = state;
	updatetext();
}

DeviceListModel::DeviceListModel(BusConnection* busconnection) : busconnection(busconnection) {
	Q_ASSERT(busconnection);

	// an object for translating int codes to readable messages
	trLiriMessages = new trLiriClass();

	//TODO icons
	iconRunning = KIcon(QLatin1String("user-online"));
	iconUnknown = KIcon(QLatin1String("user-offline"));
	iconInit = KIcon(QLatin1String("user-invisible"));
	iconKey = KIcon(QLatin1String("games-hint"));

	connect(busconnection, SIGNAL( deviceAdded(int) ), SLOT( deviceAdded(int) ));
	connect(busconnection, SIGNAL( deviceRemoved(int) ), SLOT( deviceRemoved(int) ));

	QList<int> receivers = busconnection->receivers();
	foreach(int rid, receivers) deviceAdded(rid);

}

DeviceListModel::~DeviceListModel() {
	qDeleteAll(list);
	delete trLiriMessages;
}


int DeviceListModel::rowCount(const QModelIndex &) const {
	return list.size();
}

QVariant DeviceListModel::data(const QModelIndex &index, int role) const {
	if ( !index.isValid() ) {
		return QVariant();
	}

	DeviceInfo* deviceinfo = static_cast<DeviceInfo*>(list[index.row()]);

	switch ( role ) {
		case Qt::DecorationRole:
			return *(static_cast<QIcon*>(deviceinfo->icon));
		case Qt::DisplayRole:
			return deviceinfo->text_;
		case CaptionRole:
			return deviceinfo->caption;
		case StatusRole:
			return deviceinfo->status;
		case RemoteRole:
			return deviceinfo->remote;
		case RemoteUIDRole:
			return deviceinfo->remoteuid;
		case RIDRole:
			return deviceinfo->rid;
		case Qt::ToolTipRole:
			return deviceinfo->tooltip;
		default:
			break;
	}

	return QVariant();
}

void DeviceListModel::deviceAdded(int rid) {
	// duplicate ?
	for (int i=0; i < list.size(); ++i) if ( list[i]->rid == rid ) { return; }

	//add entry
	beginInsertRows(QModelIndex(), list.size(), list.size());
	list.append (new DeviceInfo(this, busconnection, rid) );
	endInsertRows();
}

void DeviceListModel::deviceRemoved(int rid) {
	// find device
	int pos = -1;
	for (int i=0; i < list.size(); ++i) if ( list[i]->rid == rid ) { pos = i; break; }
	if (pos == -1) return;

	//remove it
	DeviceInfo* di = list[pos];
	//list[pos]->abort();
	beginRemoveRows(QModelIndex(), pos, pos);
	list.removeAt(pos);
	endRemoveRows();
	delete di;
}

void DeviceListModel::changed(DeviceInfo* di) {
	int pos = list.indexOf(di);
	if (pos == -1) return;
	QModelIndex i = index(pos, 0);
	emit dataChanged(i,i);
}
