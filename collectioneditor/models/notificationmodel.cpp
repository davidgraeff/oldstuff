#include "notificationmodel.h"
#include <QDebug>
#include "libdatabase/json.h"
#include "libdatabase/database.h"

QDebug operator<<(QDebug dbg, NotificationModel::ModelDataItem & d) {
    dbg.nospace() << d.name;
    return dbg.space();
}

QDebug operator<<(QDebug dbg, QList<NotificationModel::ModelDataItem> & d) {
    foreach(const NotificationModel::ModelDataItem& dd, d)
    dbg.nospace() << dd.name << " ";
    return dbg.space();
}

NotificationModel::ModelDataItem::ModelDataItem(const QString& id)
        : id(id)
{
}

QString NotificationModel::getPluginid() const
{
    return m_pluginid;
}

NotificationModel::NotificationModel(const QString& pluginid, const QString& instanceid, QObject* parent)
        : m_pluginid(pluginid), m_instanceid(instanceid), QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[DataRole] = "data";
    roles[IdRole] = "id";
    setRoleNames(roles);
}

int NotificationModel::rowCount(const QModelIndex &) const {
    return m_data.count();
}

QVariant NotificationModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() > m_data.count())
        return QVariant();

    const ModelDataItem* d = m_data[index.row()];
    if (role == Qt::DisplayRole || role == TextRole)
        return d->name;
    else if (role == DataRole)
        return d->data;
    else if (role == IdRole)
        return d->id;
    return QVariant();
}

void NotificationModel::clear(const QString& key) {
    m_key = key;
    qDeleteAll(m_data);
    m_data.clear();
    reset();
	emit rowCountChanged();
}

void NotificationModel::setValue(const QString& id, const QVariantMap& data) {
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

const QVariantMap NotificationModel::getValue(const QString& id) const {
    int foundAtIndex = -1;
    for (int i = 0;i<m_data.size();++i) {
        if (m_data[i]->id == id) {
            foundAtIndex = i;
        }
    }
    if (foundAtIndex==-1)
        return QVariantMap();
    return m_data[foundAtIndex]->data;
}

const QVariantMap NotificationModel::get(int index) const
{
    if (index>=m_data.size())
        return QVariantMap();
    return m_data[index]->data;
}

const int NotificationModel::indexOf(const QString& id) const
{
    int foundAtIndex = -1;
    for (int i = 0;i<m_data.size();++i) {
        if (m_data[i]->id == id) {
            foundAtIndex = i;
        }
    }
    return foundAtIndex;
}

void NotificationModel::change(const QVariantMap& data) {
    if (m_key.isEmpty())
        return;
    const QString id = data.value(m_key).toString();
    if (id.isEmpty())
        return;

    int foundAtIndex = -1;
    for (int i = m_data.size() - 1;i>=0;--i) {
        if (m_data[i]->id == id)
            foundAtIndex = i;
    }

    ModelDataItem* item = m_data.value(foundAtIndex);
    if (!item)
        item = new ModelDataItem(id);
    item->data = data;
    item->name = data.value(m_key).toString();


    // insert
    if (foundAtIndex == -1) {
        beginInsertRows(QModelIndex(), 0, 0);
        m_data.insert(0, item);
        endInsertRows();
    } else {
        QModelIndex modelindex = createIndex(foundAtIndex,0,0);
        emit dataChanged(modelindex, modelindex);
    }
    emit rowCountChanged();
}

void NotificationModel::remove(const QVariantMap& data) {
    if (m_key.isEmpty())
        return;
    const QString id = data.value(m_key).toString();
    if (id.isEmpty())
        return;

    for (int i = 0;i<m_data.size();++i) {
        if (m_data[i]->id == id) {
            beginRemoveRows(QModelIndex(), i, i);
            m_data.removeAt(i);
            endRemoveRows();
			emit rowCountChanged();
            return;
        }
    }
}

#include "notificationmodel.moc"

