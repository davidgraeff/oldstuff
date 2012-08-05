#pragma once

#include <QObject>
#include <QVariantMap>

class NotificationModel;
class ActionController : public QObject
{
    Q_OBJECT
public:
    explicit ActionController(QObject *parent = 0);
private:
	QMap<QString, NotificationModel*> m_models;
signals:
    void notification(const QString& plugin, const QVariant& value);
public slots:
    void serverJSON(const QVariantMap& data);
	QObject* getModel(const QString& modelid);
};
