#pragma once
#include <QSortFilterProxyModel>
#include <QSet>

class CollectionsModel;
class collectionTagFilterProxy: public QSortFilterProxyModel
{
	Q_OBJECT
public:
    collectionTagFilterProxy(CollectionsModel* model, QObject* parent = 0);
    virtual ~collectionTagFilterProxy();
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
public Q_SLOTS:
	// Remember to call invalidateFilter() after all of the following methods
	void addTag(const QString& tag);
	void removeTag(const QString& tag);
	void resetTags();
private:
	CollectionsModel* m_model;
	QSet<QString> m_filterTags;
};