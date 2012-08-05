#include "collectionsmodel.h"
#include <QDebug>

QDebug operator<<(QDebug dbg, CollectionsModel::ModelDataItem & d) {
    dbg.nospace() << d.name;
    return dbg.space();
}

QDebug operator<<(QDebug dbg, QList<CollectionsModel::ModelDataItem> & d) {
    foreach(const CollectionsModel::ModelDataItem& dd, d)
    dbg.nospace() << dd.name << " ";
    return dbg.space();
}

QSet< QString > convertToStringList(const QList<QVariant>& v)  {
	QSet< QString > l;
	for (int i=0;i<v.size();++i)
		l.insert(v[i].toString());
	return l;
}

CollectionsModel::ModelDataItem::ModelDataItem(const QString& id, const QVariantMap& data)
        : id(id)
{
	setData(data);
}

void CollectionsModel::ModelDataItem::setData(const QVariantMap& data)
{
	name = data.value(QLatin1String("name")).toString();
	enabled = data.value(QLatin1String("enabled")).toBool();
	categories = convertToStringList(data.value(QLatin1String("categories")).toList());
	this->data = data;
}

CollectionsModel::CollectionsModel(QObject* parent)
        : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ValueRole] = "value";
    roles[IdRole] = "collectionid";
    roles[isChangingRole] = "isChanging";
	roles[CategoriesRole] = "categories";
    setRoleNames(roles);
}

int CollectionsModel::rowCount(const QModelIndex &) const {
    return m_data.count();
}

QVariant CollectionsModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() > m_data.count())
        return QVariant();

    const ModelDataItem &d = m_data[index.row()];
    if (role == NameRole || role == Qt::DisplayRole)
        return d.name;
    else if (role == ValueRole)
        return d.data;
    else if (role == IdRole)
        return d.id;
    else if (role == EnabledRole)
        return d.enabled;
    else if (role == CountServicesRole)
        return d.countServices;
    return QVariant();
}

void CollectionsModel::clear() {
    m_data.clear();
    reset();
}

void CollectionsModel::setValue(const QString& id, const QVariantMap& data) {
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

const QVariantMap CollectionsModel::getValue(const QString& id) const {
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

const QVariantMap CollectionsModel::getValueByIndex(int i) const
{
    if (i>=m_data.size())
        return QVariantMap();
    return m_data[i].data;
}

const QSet< QString >& CollectionsModel::getCategories(int index) const
{
	Q_ASSERT (index<m_data.size());
    return m_data[index].categories;
}

void CollectionsModel::change(const QString& id, const QVariantMap& data) {
    if (data.value(QLatin1String("type_")).toString() != QLatin1String("collection"))
        return;

	if (id.isEmpty()) {
        qWarning()<<"Data with empty id found!" << data;
        return;
	}
	
    const QString name = data.value(QLatin1String("name")).toString();
    if (name.isEmpty()) {
        qWarning()<<"Data with empty name found!" << data;
        return;
    }

    int foundAtIndex = -1, sortedIndex = m_data.size(), sortedIndexOk = 0;
    for (int i = m_data.size() - 1;i>=0;--i) {
        if (m_data[i].id == id)
            foundAtIndex = i;
        if (m_data[i].name.toLower() >= name.toLower()) {
            sortedIndex = i;
            sortedIndexOk = 1;
        }
    }

    // insert
    if (foundAtIndex == -1) {
        beginInsertRows(QModelIndex(), sortedIndex, sortedIndex);
        m_data.insert(sortedIndex, ModelDataItem(id, data));
        endInsertRows();
    } else {
        m_data[foundAtIndex].setData(data);
		
		// The new item name compared itself to the old item name.
		// Because the old one will be removed, subtract 1 from sortedIndex
		if (foundAtIndex<sortedIndex)
			--sortedIndex;
		
        if (sortedIndex == foundAtIndex) {
            QModelIndex modelindex = createIndex(foundAtIndex,0,0);
            emit dataChanged(modelindex, modelindex);
        } else {
            m_data.move(foundAtIndex, sortedIndex);
			beginRemoveRows(QModelIndex(), foundAtIndex, foundAtIndex);
			endRemoveRows();
			// if we remove the old entry and it is before sortedIndex => sortedIndex increment
			if (sortedIndex>foundAtIndex)
				--sortedIndex;
            beginInsertRows(QModelIndex(), sortedIndex, sortedIndex);
            endInsertRows();
        }
    }
}

void CollectionsModel::remove(const QString& id) {
    for (int i = 0;i<m_data.size();++i) {
        if (m_data[i].id == id) {
            beginRemoveRows(QModelIndex(), i, i);
            m_data.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}

#include "collectionsmodel.moc"

