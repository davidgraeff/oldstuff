#include <KDE/KLocale>
#include "config.h"

#include <QDebug>
#include <QSet>
#include <QByteArray>

#include "Receivers_Model.h"
#include "businterconnect/BusConnection.h"
#include "businterconnect/Control_DevManager_dbusproxy.h"
#include "businterconnect/Control_Execution_dbusproxy.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"
#include "businterconnect/Receiver_Execution_dbusproxy.h"

//////////////// Receiver (item) class /////////////////
Receiver::Receiver(BusConnection* connection, int rid) {
	Q_ASSERT(connection);
	iface = connection->getDeviceManagerReceiver(rid);
	if (iface) {
		QStringList settingkeys;
		settingkeys.append(QLatin1String("remotereceiver.duid"));
		name = iface->getSettings(settingkeys).value()[0];
		connect(iface, SIGNAL( key(const QString &, const QString &, uint, int)),
			SLOT( key(const QString &, const QString &, uint, int)));
	} else {
		name = QLatin1String("unknown error");
	}
}

Receiver::~Receiver() {}

void Receiver::key(const QString &keycode, const QString &keyname, uint channel, int pressed) {
	Q_UNUSED(keyname);
	Q_UNUSED(channel);
	if (pressed && checked) emit keyevent(keycode);
}

//////////////// Receiver model (itemlist) class /////////////////////

Receivers_Model::Receivers_Model(BusConnection* connection, QObject *parent)
	: QAbstractListModel(parent), connection(connection) {
	QList<int> receivers = connection->receivers();
	foreach(int rid, receivers) deviceAdded(rid);
}

Receivers_Model::~Receivers_Model() {

}

void Receivers_Model::deviceAdded(int rid) {
	int position = -1;
	for (int i=0; i < receivers.size(); ++i)
		if (receivers.at(i)->rid == rid) { position = i; return; }

	Receiver * receiver = new Receiver(connection, rid);
	connect(receiver, SIGNAL( keyevent(const QString &) ), SIGNAL( keyevent(const QString &) ));
	beginInsertRows(QModelIndex(), receivers.size(), receivers.size());
	receivers.append(receiver);
	endInsertRows();
}

void Receivers_Model::deviceRemoved(int rid) {
	int position = -1;
	for (int i=0; i < receivers.size(); ++i)
		if (receivers.at(i)->rid == rid) { position = i; break; }

	if (position >= 0 && position < receivers.size()) {
		beginRemoveRows(QModelIndex(), position, position);
		receivers.removeAt(position);
		endRemoveRows();
	}
}

int Receivers_Model::rowCount(const QModelIndex &) const {
	return receivers.size();
}

int Receivers_Model::columnCount ( const QModelIndex & ) const {
	return 1;
}

Qt::ItemFlags Receivers_Model::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant Receivers_Model::data(const QModelIndex &index, int role) const {
	if ( !index.isValid() ) {
		return QVariant();
	}

	switch ( role ) {
		case Qt::EditRole:
		case Qt::ToolTipRole:
		case Qt::DisplayRole:
			return receivers[index.row()]->name;
			break;
		case Qt::CheckStateRole:
			return ((receivers[index.row()]->checked)?2:0);
			break;
		default:
			break;
	}

	return QVariant();
}

bool Receivers_Model::setData ( const QModelIndex & index, const QVariant & value, int role) {
	if (index.isValid() && role == Qt::CheckStateRole) {
		receivers[index.row()]->checked = value.value<bool>();
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

QVariant Receivers_Model::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal && section==0)
		return i18n("Devices");

	return QVariant();
}

