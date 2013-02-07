#ifndef LIRI_ATTRIBUTES_MODEL_H_
#define LIRI_ATTRIBUTES_MODEL_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>

class RemoteFile;

class Settings_Model : public QAbstractListModel
{
	Q_OBJECT
	public:
		Settings_Model(RemoteFile* re, QObject *parent = 0);
		~Settings_Model();
		int addSetting();
		
		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
		QVariant data(const QModelIndex &index, int role) const;
		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	Q_SIGNALS: // SIGNALS
		void keyfocus(const QModelIndex&);
										
	private:
		RemoteFile* remote;
};

#endif
