#pragma once
#include <QAbstractListModel>
#include <QDebug>

class DataModel : public QAbstractListModel
{
    Q_OBJECT
public:
    class ModelDataItem
     {
     public:
         ModelDataItem(const QString &id, const QString &name, const QVariantMap& data);

         QString id;
         QString name;
         QVariantMap data;
     };
    enum ModelRoles {
        NameRole = Qt::UserRole + 1,
        ValueRole,
        IdRole,
        isChangingRole
    };

    /**
     * To parse data in parseData correctly you have to provide the type key and the 
     * name key. E.g. incoming data is "{"id":"0", "text":"abc", "type_":"abc"}" then dataid=type_, dataname=text.
     * Also you have to provide the id that this model is connected to.
     */
    DataModel(const QString& modeltype, const QString& key_type, const QString& dataname, QObject* parent = 0);
    QString modelKey();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

public Q_SLOTS:
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    void clear();
    void change(const QString& id, const QVariantMap& data);
    void remove(const QString& id);
    void setValue(const QString& id, const QVariantMap& data);
    const QVariantMap getValue(const QString& id) const;
	const QVariantMap getValueByIndex(int i) const;
Q_SIGNALS:
    void dataToSend(const ModelDataItem& data);
private:
    QList<ModelDataItem> m_data;
    QString m_modeltype;
    QString m_key_type;
    QString m_dataname;
};
