#ifndef LIRI_KEYS_MODEL_H_
#define LIRI_KEYS_MODEL_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>

class RemoteFile;

class Keys_Model : public QAbstractListModel {
	Q_OBJECT

	public:
		Keys_Model(RemoteFile* re, QObject *parent = 0);
		~Keys_Model();

		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
		bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
		QVariant data(const QModelIndex &index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation,
									int role = Qt::DisplayRole) const;
		int addKey(const QString& keycode = QString());

	public Q_SLOTS:
		void keyevent(const QString& keycode);

	Q_SIGNALS: // SIGNALS
		void keyfocus(const QModelIndex&);

	private:
		RemoteFile* remote;
};

#endif
