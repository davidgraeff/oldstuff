#ifndef LIRI_VARIABLES_MODEL_H_
#define LIRI_VARIABLES_MODEL_H_

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>
#include <QObject>

class ApplicationProfileFile;

class Variables_Model : public QAbstractTableModel {
	Q_OBJECT

	public:
		Variables_Model(ApplicationProfileFile* app, QObject *parent = 0);
		~Variables_Model();

		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
		bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
		void add();

		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

		QVariant data(const QModelIndex &index, int role) const;

	Q_SIGNALS: // SIGNALS
		void keyfocus(const QModelIndex&);

	private:
		ApplicationProfileFile* app;
};

#endif
