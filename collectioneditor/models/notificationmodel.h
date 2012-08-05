#pragma once
#include <QAbstractListModel>

class NotificationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY rowCountChanged)
public:
	class ModelDataItem
		{
		public:
			ModelDataItem(const QString &id);
			QString id;
			QString name;
			QVariantMap data;
		};
	enum ModelRoles {
		TextRole = Qt::UserRole + 1,
		DataRole,
		IdRole
	};

	NotificationModel(const QString& pluginid, const QString& instanceid, QObject* parent = 0);

	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

public Q_SLOTS:
	int rowCount(const QModelIndex & parent = QModelIndex()) const;
	void clear(const QString& key);
	void change(const QVariantMap& data);
	void remove(const QVariantMap& data);
	void setValue(const QString& id, const QVariantMap& data);
	const QVariantMap getValue(const QString& id) const;
	const int indexOf(const QString& id) const;
	const QVariantMap get(int index) const;
	QString getPluginid() const;
Q_SIGNALS:
	void dataToSend(const ModelDataItem& data);
	void rowCountChanged();
private:
	QList<ModelDataItem*> m_data;
	QString m_key;
	QString m_pluginid;
	QString m_instanceid;
};
