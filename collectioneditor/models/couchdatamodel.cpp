#include "couchdatamodel.h"
#include <QDebug>

QDebug operator<<(QDebug dbg, DataModel::ModelDataItem & d) {
    dbg.nospace() << d.name;
    return dbg.space();
}

QDebug operator<<(QDebug dbg, QList<DataModel::ModelDataItem> & d) {
    foreach(const DataModel::ModelDataItem& dd, d)
    dbg.nospace() << dd.name << " ";
    return dbg.space();
}

DataModel::ModelDataItem::ModelDataItem(const QString& id, const QString& name, const QVariantMap& data)
        : id(id), name(name), data(data)
{
}

DataModel::DataModel(const QString& modeltype, const QString& key_type, const QString& dataname, QObject* parent)
        : QAbstractListModel(parent), m_modeltype(modeltype), m_key_type(key_type), m_dataname(dataname)
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ValueRole] = "value";
    roles[IdRole] = "index";
    roles[isChangingRole] = "isChanging";
    setRoleNames(roles);
}

int DataModel::rowCount(const QModelIndex &) const {
    return m_data.count();
}

QVariant DataModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() > m_data.count())
        return QVariant();

    const ModelDataItem &d = m_data[index.row()];
    if (role == NameRole)
        return d.name;
    else if (role == ValueRole)
        return d.data;
    else if (role == IdRole)
        return d.id;
    return QVariant();
}

QString DataModel::modelKey() {
    return m_key_type;;
}

void DataModel::clear() {
    m_data.clear();
    reset();
}

void DataModel::setValue(const QString& id, const QVariantMap& data) {
    int foundAtIndex = -1;
    for (int i = 0;i<m_data.size();++i) {
        if (m_data[i].id == id) {
            foundAtIndex = i;
        }
    }
    if (foundAtIndex==-1)
        return;
    ModelDataItem& item = m_data[foundAtIndex];
    item.data = data;
    {
        QModelIndex modelindex = createIndex(foundAtIndex,0,0);
        emit dataChanged(modelindex, modelindex);
    }
    emit dataToSend(item);
}

const QVariantMap DataModel::getValue(const QString& id) const {
    int foundAtIndex = -1;
    for (int i = 0;i<m_data.size();++i) {
        if (m_data[i].id == id) {
            foundAtIndex = i;
        }
    }
    if (foundAtIndex==-1)
        return QVariantMap();
    return m_data[foundAtIndex].data;
}

const QVariantMap DataModel::getValueByIndex(int i) const
{
    if (i>=m_data.size())
        return QVariantMap();
    return m_data[i].data;
}

void DataModel::change(const QString& id, const QVariantMap& data) {
    if (data.value(m_key_type).toString() != m_modeltype)
        return;

	if (id.isEmpty()) {
        qWarning()<<"Data with empty id found!" << data;
        return;
	}
	
    const QString name = data.value(m_dataname).toString();
    if (name.isEmpty()) {
        qWarning()<<"Data with empty name found!" << data;
        return;
    }

    int foundAtIndex = -1, sortedIndex = m_data.size(), sortedIndexOk = 0;
    for (int i = m_data.size() - 1;i>=0;--i) {
        if (m_data[i].id == id)
            foundAtIndex = i;
        if (m_data[i].name.toLower() > name.toLower()) {
            sortedIndex = i;
            sortedIndexOk = 1;
        }
    }

    // insert
    if (foundAtIndex == -1) {
        beginInsertRows(QModelIndex(), sortedIndex, sortedIndex);
        m_data.insert(sortedIndex, ModelDataItem(id, name, data));
        endInsertRows();
    } else {
        m_data[foundAtIndex].data = data;
        if (name.size())
            m_data[foundAtIndex].name = name;
		
		// The new item name compared itself to the old item name.
		// Because the old one will be removed, subtract 1 from sortedIndex
		if (foundAtIndex<sortedIndex)
			--sortedIndex;
		
        if (sortedIndex == foundAtIndex) {
            QModelIndex modelindex = createIndex(foundAtIndex,0,0);
            emit dataChanged(modelindex, modelindex);
        } else {
            beginMoveRows(QModelIndex(), foundAtIndex, foundAtIndex, QModelIndex(), sortedIndex);
            m_data.move(foundAtIndex, sortedIndex);
            endMoveRows();
        }
    }
}

void DataModel::remove(const QString& id) {
    for (int i = 0;i<m_data.size();++i) {
        if (m_data[i].id == id) {
            beginRemoveRows(QModelIndex(), i, i);
            m_data.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}

#include "couchdatamodel.moc"

