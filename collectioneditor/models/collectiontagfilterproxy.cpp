#include "collectiontagfilterproxy.h"
#include "collectionsmodel.h"

collectionTagFilterProxy::collectionTagFilterProxy(CollectionsModel* model, QObject* parent):
        QSortFilterProxyModel(parent), m_model(model)
{
    setSourceModel(model);
}

collectionTagFilterProxy::~collectionTagFilterProxy()
{}

bool collectionTagFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (m_filterTags.size()) {
        return (m_filterTags & m_model->getCategories(sourceRow)).size();
	}

    return true;
}

void collectionTagFilterProxy::addTag(const QString& tag)
{
    m_filterTags.insert(tag);
    invalidateFilter();
}

void collectionTagFilterProxy::removeTag(const QString& tag)
{
    m_filterTags.remove(tag);
    invalidateFilter();
}

void collectionTagFilterProxy::resetTags()
{
    m_filterTags.clear();
	invalidateFilter();
}

#include "collectiontagfilterproxy.moc"
