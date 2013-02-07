#ifndef LIRI_ApplicationProfileActionModel_H_
#define LIRI_ApplicationProfileActionModel_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <kcategorizedsortfilterproxymodel.h>

#include <KDE/KLocale>

/*
Anzeigen von Run/IPC Operationen im Action Tab
*/
class Action;

class ApplicationProfileActionModel : public QAbstractListModel {
     Q_OBJECT

public:
	ApplicationProfileActionModel(Action* action, QObject *parent = 0);
	~ApplicationProfileActionModel();

	bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
			int role = Qt::DisplayRole) const;
	void updateIndex(int);
private:
	liri::Action* action;
};
 
#endif /*LIRI_ApplicationProfileActionModel_H_*/
