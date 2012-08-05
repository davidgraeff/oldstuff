#pragma once
#include <QAbstractListModel>
#include <QDebug>

class CollectiontagsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    class ModelDataItem
     {
     public:
         ModelDataItem(const QString &name, const QString& firstCollectionid);
         QString name;
         QSet<QString> collectionids;
		 bool checked;
     };
    enum ModelRoles {
        NameRole = Qt::UserRole + 1,
        CollectionCountRole,
		CheckRole
    };

    CollectiontagsModel(QObject* parent = 0);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

public Q_SLOTS:
	// Signals from database
    void clear();
    void change(const QString& id, const QVariantMap& data);
    void remove(const QString& id);
	// toggle check
	void toggleChecked(int index);
Q_SIGNALS:
	void tagChecked(const QString& tag);
	void tagUnchecked(const QString& tag);
private:
    QList<ModelDataItem> m_data;
    void checkAndRemoveTags(const QString& collectionid, const QList< QString >& tags);
    void addTags(const QString& collectionid, const QList< QString >& tags);
};
