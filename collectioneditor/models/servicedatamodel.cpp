#include "servicedatamodel.h"
#include <QDebug>
#include "libdatabase/json.h"
#include "libdatabase/database.h"

QDebug operator<<(QDebug dbg, ServiceDataModel::ModelDataItem & d) {
    dbg.nospace() << d.name;
    return dbg.space();
}

QDebug operator<<(QDebug dbg, QList<ServiceDataModel::ModelDataItem> & d) {
    foreach(const ServiceDataModel::ModelDataItem& dd, d)
    dbg.nospace() << dd.name << " ";
    return dbg.space();
}

ServiceDataModel::ModelDataItem::ModelDataItem(const QString& id)
        : id(id), selected(false)
{
}

ServiceDataModel::ServiceDataModel(QObject* parent)
        : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[DescriptionRole] = "description";
    roles[ValueRole] = "value";
    roles[TypeRole] = "type";
    roles[ServiceIdRole] = "serviceID";
    roles[SelectedRole] = "selected";
    setRoleNames(roles);
}

int ServiceDataModel::rowCount(const QModelIndex &) const {
    return m_data.count();
}

QVariant ServiceDataModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() > m_data.count())
        return QVariant();

    const ModelDataItem* d = m_data[index.row()];
    if (role == NameRole)
        return d->name;
    else if (role == DescriptionRole)
        return d->desc;
    else if (role == ValueRole)
        return d->data;
    else if (role == TypeRole)
        return d->type;
    else if (role == ServiceIdRole)
        return d->id;
    else if (role == SelectedRole)
        return d->selected;
    return QVariant();
}

void ServiceDataModel::clear() {
    qDeleteAll(m_data);
    m_data.clear();
    m_collectionid.clear();
    reset();
}

void ServiceDataModel::setValue(const QString& id, const QVariantMap& data) {
    int foundAtIndex = -1;
    for (int i = 0;i<m_data.size();++i) {
        if (m_data[i]->id == id) {
            foundAtIndex = i;
        }
    }
    if (foundAtIndex==-1)
        return;
    ModelDataItem* item = m_data[foundAtIndex];
    item->data = data;
    {
        QModelIndex modelindex = createIndex(foundAtIndex,0,0);
        emit dataChanged(modelindex, modelindex);
    }
    emit dataToSend(*item);
}

void ServiceDataModel::setSelected(int index, bool selected)
{
    m_data[index]->selected = selected;
    {
        QModelIndex modelindex = createIndex(index,0,0);
        emit dataChanged(modelindex, modelindex);
    }
}

const QVariantMap ServiceDataModel::getValue(int index) const {
    return m_data[index]->data;
}

const bool ServiceDataModel::getSelected(int index) const
{
    return m_data[index]->selected;
}

void ServiceDataModel::change(const QString& id, const QVariantMap& data) {
    // only accept database documents, if collection id is known
    if (m_collectionid.isEmpty() || id.isEmpty())
        return;

    QByteArray type = data.value(QLatin1String("type_")).toByteArray();
    if (type != "action" && type != "event" && type != "condition")
        return;

    int foundAtIndex = -1;
    for (int i = m_data.size() - 1;i>=0;--i) {
        if (m_data[i]->id == id)
            foundAtIndex = i;
    }

    QVariantMap reduceddata = data;
    reduceddata.remove(QLatin1String("_id"));
    reduceddata.remove(QLatin1String("_rev"));
    reduceddata.remove(QLatin1String("plugin_"));
	reduceddata.remove(QLatin1String("instanceid_"));
    reduceddata.remove(QLatin1String("type_"));
    reduceddata.remove(QLatin1String("member_"));
    reduceddata.remove(QLatin1String("collection_"));
    reduceddata.remove(QLatin1String("delay_"));

    ModelDataItem* item = m_data.value(foundAtIndex);
    if (!item)
        item = new ModelDataItem(id);
    item->data = data;
    item->type = type;
    item->desc = JSON::stringify(reduceddata);
    item->name = data.value(QLatin1String("plugin_")).toString() + QLatin1String(".") +
				 data.value(QLatin1String("instanceid_")).toString() + QLatin1String(".") +
                 data.value(QLatin1String("type_")).toString() + QLatin1String(".") +
                 data.value(QLatin1String("member_")).toString();


    // insert
    if (foundAtIndex == -1) {
        beginInsertRows(QModelIndex(), 0, 0);
        m_data.insert(0, item);
        endInsertRows();
    } else {
        QModelIndex modelindex = createIndex(foundAtIndex,0,0);
        emit dataChanged(modelindex, modelindex);
    }
}

void ServiceDataModel::remove(const QString& id) {
    for (int i = 0;i<m_data.size();++i) {
        if (m_data[i]->id == id) {
            beginRemoveRows(QModelIndex(), i, i);
            m_data.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}

void ServiceDataModel::dataOfCollection(const QString& collectionid, const QList< QVariantMap >& services) {
    if (m_collectionid != collectionid) {
        qWarning() << "Received data of not requested collection!";
        return;
    }
    for (int i=0;i<services.size();++i)
        change(services[i].value(QLatin1String("_id")).toString(), services[i]);
}
void ServiceDataModel::requestDataOfCollection(const QString& collection_id) {
    clear();
    m_collectionid = collection_id;
    Database::instance()->requestDataOfCollection(m_collectionid);
}

#include "servicedatamodel.moc"

