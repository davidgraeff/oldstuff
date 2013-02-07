#include "Settings_Model.h"
#include <KDE/KLocale>

#include <QDebug>
#include <QSet>

#include "fileformats/RemoteFile.h"

Settings_Model::Settings_Model(RemoteFile* re, QObject *parent)
: QAbstractListModel(parent), remote(re) {
}

Settings_Model::~Settings_Model() {

}

int Settings_Model::addSetting() {
	int row = remote->countSettings();
	beginInsertRows(QModelIndex(), row, row);
	remote->addSetting(QString(), QString());
	endInsertRows();
	emit keyfocus(createIndex(row, 0, 0));
	return row;
}

int Settings_Model::rowCount(const QModelIndex &) const {
	return remote->countSettings();
}

int Settings_Model::columnCount ( const QModelIndex & ) const {
	return 2;
}

Qt::ItemFlags Settings_Model::flags(const QModelIndex &index) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;
	
	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool Settings_Model::setData(const QModelIndex &index, const QVariant &value, int role) {
	if (!index.isValid() || role != Qt::EditRole) return false;
	
	QString ndata = value.toString();
	
	if (ndata.isEmpty()) { //delete
		return removeRows(index.row(), 1, QModelIndex());
	} else {
		if (index.column() == 0)
			remote->setSettingKey(index.row(), ndata);
		else
			remote->setSettingValue(index.row(), ndata);
		emit dataChanged(index, index);
		return true;
	}

	return false;
}

bool Settings_Model::removeRows ( int row, int count, const QModelIndex & ) {
	beginRemoveRows(QModelIndex(), row, row+count-1);

	for (int i=row+count-1;i>=row;--i)
		remote->setSettingKey(i, QString());

	endRemoveRows();
	return true;
}

QVariant Settings_Model::data(const QModelIndex &index, int role) const {
	QVariant theData;
	
	if ( !index.isValid() ) {
		return QVariant();
	}

	QPair<QString, QString> key = remote->getSetting(index.row());
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


QVariant Settings_Model::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();
	
	if (orientation == Qt::Horizontal && section==0)
		return i18n("Key");
	
	if (orientation == Qt::Horizontal && section==1)
		return i18n("Value");
	
	return QVariant();
}

