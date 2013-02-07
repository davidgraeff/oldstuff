#include <KDE/KLocale>

#include <QDebug>
#include <QSet>
#include <QByteArray>

#include "Keys_Model.h"
#include "fileformats/RemoteFile.h"

Keys_Model::Keys_Model(RemoteFile* re, QObject *parent)
: QAbstractListModel(parent), remote(re) {
}

Keys_Model::~Keys_Model() {

}

int Keys_Model::addKey(const QString& keycode) {
	int row = remote->countKeys();
	beginInsertRows(QModelIndex(), row, row);
	remote->addKey(keycode, QString());
	endInsertRows();
	if (keycode.size())
		// focus on the second column, the first is filled already
		emit keyfocus(createIndex(row, 1, 0));
	else
		// focus on the first column
		emit keyfocus(createIndex(row, 0, 0));
	return row;
}

void Keys_Model::keyevent(const QString& keycode) {
	if (!keycode.size())
		return;

	int row = remote->posKeyCode(keycode);
	if (row == -1) {
		row = addKey(keycode);
	} else
		// focus on the second column, the first is filled already
		emit keyfocus(createIndex(row, 1, 0));
}

int Keys_Model::rowCount(const QModelIndex &) const {
	return remote->countKeys();
}

int Keys_Model::columnCount ( const QModelIndex & ) const {
	return 2;
}

Qt::ItemFlags Keys_Model::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool Keys_Model::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (!index.isValid() || role != Qt::EditRole) return false;

	QString ndata = value.toString();

	if (ndata.isEmpty()) { //delete
		return removeRows(index.row(), 1, QModelIndex());
	} else {
		if (index.column() == 0)
			remote->setKeyCode(index.row(), ndata);
		else
			remote->setKeyName(index.row(), ndata);
		emit dataChanged(index, index);
		return true;
	}

	return false;
}

bool Keys_Model::removeRows ( int row, int count, const QModelIndex & ) {
	beginRemoveRows(QModelIndex(), row, row+count-1);

	for (int i=row+count-1;i>=row;--i)
		remote->setKeyCode(i, QString());

	endRemoveRows();
	return true;
}

QVariant Keys_Model::data(const QModelIndex &index, int role) const {
	QVariant theData;

	if ( !index.isValid() ) {
		return QVariant();
	}

	QPair<QString, QString> key = remote->getKey(index.row());
	switch ( role ) {
		case Qt::EditRole:
		case Qt::ToolTipRole:
		case Qt::DisplayRole:
			if (index.column() == 0)
				theData.setValue( key.first );
			else
				theData.setValue( key.second );
			break;
		default:
			break;
	}

	return theData;
}


QVariant Keys_Model::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal && section==0)
		return i18n("Key ID");

	if (orientation == Qt::Horizontal && section==1)
		return i18n("Mapping");

	return QVariant();
}

