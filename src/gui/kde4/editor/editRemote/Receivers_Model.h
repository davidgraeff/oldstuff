#ifndef LIRI_RECEIVERS_MODEL_H_
#define LIRI_RECEIVERS_MODEL_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>

class OrgLiriDevManagerReceiverInterface;
class BusConnection;

//////////////// Receiver (item) class /////////////////
class Receiver : public QObject {
	Q_OBJECT
	public:
		Receiver(BusConnection* connection, const QString & rid);
		~Receiver();
		QString rid;
		QString name;
		bool checked;
		OrgLiriDevManagerReceiverInterface* iface;

	private Q_SLOTS:
		void key(const QString &keycode, const QString &keyname, uint channel, int pressed);

	Q_SIGNALS:
		void keyevent(const QString &keycode);
};

//////////////// Receiver model (itemlist) class /////////////////////
class Receivers_Model : public QAbstractListModel {
	Q_OBJECT

	public:
		Receivers_Model(BusConnection* connection, QObject *parent = 0);
		~Receivers_Model();

		Qt::ItemFlags flags(const QModelIndex &index) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
		QVariant data(const QModelIndex &index, int role) const;
		bool setData ( const QModelIndex & index, const QVariant & value, int role);
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	Q_SIGNALS:
		void keyevent(const QString& keycode);
	private Q_SLOTS:
		void deviceAdded(const QString & rid);
		void deviceRemoved(const QString & rid);
	private:
		BusConnection* connection;
		QList<Receiver*> receivers;
};

#endif
