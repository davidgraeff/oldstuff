#include "collectiontagsmodel.h"
#include <QDebug>
#include <qsortfilterproxymodel.h>

QDebug operator<<(QDebug dbg, CollectiontagsModel::ModelDataItem & d) {
    dbg.nospace() << d.name;
    return dbg.space();
}

QDebug operator<<(QDebug dbg, QList<CollectiontagsModel::ModelDataItem> & d) {
    foreach(const CollectiontagsModel::ModelDataItem& dd, d)
    dbg.nospace() << dd.name << " ";
    return dbg.space();
}

CollectiontagsModel::ModelDataItem::ModelDataItem(const QString& name, const QString& firstCollectionid)
        : name(name)
{
    checked = false;
    collectionids.insert(firstCollectionid);
}

CollectiontagsModel::CollectiontagsModel(QObject* parent)
        : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[CollectionCountRole] = "collectioncount";
    roles[CheckRole] = "checked";
    setRoleNames(roles);
}

int CollectiontagsModel::rowCount(const QModelIndex &) const {
    return m_data.count();
}

QVariant CollectiontagsModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() > m_data.count())
        return QVariant();

    const ModelDataItem &d = m_data[index.row()];
    if (role == NameRole || role == Qt::DisplayRole)
        return d.name;
    else if (role == CollectionCountRole)
        return d.collectionids.size();
    else if (role == CheckRole|| role == Qt::CheckStateRole)
        return d.checked;
    return QVariant();
}

void CollectiontagsModel::clear() {
    m_data.clear();
    reset();
}

void CollectiontagsModel::change(const QString& id, const QVariantMap& data) {
    if (data.value(QLatin1String("type_")).toString() != QLatin1String("collection"))
        return;

    const QVariantList variantList = data.value(QLatin1String("categories")).toList();
    QList<QString> tags;
    QListIterator<QVariant> i(variantList);
    while (i.hasNext()) {
        tags << i.next().toString();
    }
    checkAndRemoveTags(id, tags);
    addTags(id, tags);
}

void CollectiontagsModel::remove(const QString& id) {
    for (int i = 0;i<m_data.size();++i) {
        if (m_data[i].collectionids.contains(id)) {
            beginRemoveRows(QModelIndex(), i, i);
            m_data.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}

void CollectiontagsModel::toggleChecked(int index)
{
    bool newvalue = !m_data[index].checked;
    if (newvalue)
        emit tagChecked(m_data[index].name);
    else
        emit tagUnchecked(m_data[index].name);
    m_data[index].checked = newvalue;
    QModelIndex modelindex = createIndex(index,0,0);
    emit dataChanged(modelindex, modelindex);
}

void CollectiontagsModel::checkAndRemoveTags(const QString& collectionid, const QList< QString >& tags) {
    // go through all tags and remove this collectionid. if any tag does not have collections remove it
    // if the new tags contain a tag with the same name, do not remove it
    for (int i = m_data.size() - 1;i>=0;--i) {
		ModelDataItem& item = m_data[i];
        if (item.collectionids.remove(collectionid) && item.collectionids.isEmpty() &&
                !tags.contains(item.name)
           ) {
            if (item.checked)
                emit tagUnchecked(item.name);
            beginRemoveRows(QModelIndex(), i, i);
            m_data.removeAt(i);
            endRemoveRows();
        }
    }
}

void CollectiontagsModel::addTags(const QString& collectionid, const QList< QString >& tags) {
    for (int indexTag=0;indexTag<tags.size();++indexTag) {
        int foundAtIndex = -1, sortedIndex = m_data.size(), sortedIndexOk = 0;
        const QString& tagname = tags[indexTag];
        for (int i = m_data.size() - 1;i>=0;--i) {
            if (m_data[i].name == tagname)
                foundAtIndex = i;
            if (m_data[i].name.toLower() > tagname.toLower()) {
                sortedIndex = i;
                sortedIndexOk = 1;
            }
        }

        // no tag with this name found, add a new one
        if (foundAtIndex == -1) {
            beginInsertRows(QModelIndex(), sortedIndex, sortedIndex);
            m_data.insert(sortedIndex, ModelDataItem(tagname, collectionid));
            endInsertRows();
        } else {
            // a tag was found: add collectionid
            m_data[foundAtIndex].collectionids.insert(collectionid);
            QModelIndex modelindex = createIndex(foundAtIndex,0,0);
            emit dataChanged(modelindex, modelindex);
        }
    }
}

#include "collectiontagsmodel.moc"

