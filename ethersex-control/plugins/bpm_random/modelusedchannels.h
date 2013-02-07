#ifndef MODELUSEDCHANNELS_H
#define MODELUSEDCHANNELS_H

#include <QObject>
#include <QSet>
#include <QString>
#include <QVector>
#include <QList>
#include <QPair>
#include <QAbstractListModel>

class stellaConnection;

class modelUsedChannels : public QAbstractListModel
{
	Q_OBJECT
public:
		modelUsedChannels(stellaConnection* connection);
		QVariant data ( const QModelIndex & index, int role ) const;
		Qt::ItemFlags flags ( const QModelIndex & index ) const;
		int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
		bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
		QVector<int> usedchannels;
		QList< QPair<QString, int> > channels;
};

#endif // MODELUSEDCHANNELS_H
