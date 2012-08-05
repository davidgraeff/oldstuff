#pragma once
#include <QAbstractListModel>
#include <QDebug>
#include <QStringList>

class CollectionsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    class ModelDataItem
     {
     public:
         ModelDataItem(const QString &id, const QVariantMap& data);
		void setData(const QVariantMap& data);
         QString id;
         QString name;
		 QSet< QString > categories;
		 int countServices;
		 bool enabled;
         QVariantMap data;
     };
    enum ModelRoles {
        NameRole = Qt::UserRole + 1,
        ValueRole,
        IdRole,
        isChangingRole,
		CategoriesRole,
		EnabledRole,
		CountServicesRole
    };

    /**
     * To parse data in parseData correctly you have to provide the type key and the 
     * name key. E.g. incoming data is "{"id":"0", "text":"abc", "type_":"abc"}" then dataid=type_, dataname=text.
     * Also you have to provide the id that this model is connected to.
     */
    CollectionsModel(QObject* parent = 0);

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

public Q_SLOTS:
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    void clear();
    void change(const QString& id, const QVariantMap& data);
    void remove(const QString& id);
    void setValue(const QString& id, const QVariantMap& data);
    const QVariantMap getValue(const QString& id) const;
	const QVariantMap getValueByIndex(int i) const;
	const QSet<QString>& getCategories(int index) const;
Q_SIGNALS:
    void dataToSend(const ModelDataItem& data);
private:
    QList<ModelDataItem> m_data;
};
