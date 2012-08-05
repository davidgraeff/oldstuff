#pragma once
#include <QAbstractListModel>
#include <QDebug>

class ServiceDataModel : public QAbstractListModel
{
	Q_OBJECT
public:
	class ModelDataItem
	{
	public:
		ModelDataItem(const QString &id);
		QString id;
		QString name;
		QString desc;
		QByteArray type;
		QVariantMap data;
		bool selected;
	};
    enum ModelRoles {
		NameRole = Qt::UserRole + 1,
		DescriptionRole,
		ValueRole,
		TypeRole,
		ServiceIdRole,
		SelectedRole
	};

	ServiceDataModel(QObject* parent = 0);

	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

public Q_SLOTS:
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    void clear();
    void change(const QString& id, const QVariantMap& data);
    void remove(const QString& id);
    void setValue(const QString& id, const QVariantMap& data);
	void setSelected(int index, bool selected);
    const QVariantMap getValue(int index) const;
	const bool getSelected(int index) const;
    void requestDataOfCollection(const QString& collection_id);
    void dataOfCollection (const QString& collectionid, const QList<QVariantMap>& services);
Q_SIGNALS:
    void dataToSend(const ModelDataItem& data);
private:
    QList<ModelDataItem*> m_data;
    QString m_collectionid;
};
