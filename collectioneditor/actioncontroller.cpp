#include "actioncontroller.h"
#include <QDebug>
#include "models/notificationmodel.h"

ActionController::ActionController(QObject *parent) :
    QObject(parent)
{
}

void ActionController::serverJSON(const QVariantMap &jsondata)
{
    if (jsondata.value(QLatin1String("member_")).toByteArray() == "changeProperty") {
		const QString pluginid = jsondata.value(QLatin1String("plugin_")).toString();
		const QString instanceid = jsondata.value(QLatin1String("instanceid_")).toString();
		const QByteArray type = jsondata.value(QLatin1String("__type")).toByteArray();
		if (type == "notification") {
			emit notification(jsondata.value(QLatin1String("plugin_")).toByteArray(), jsondata);
		} else if (type=="model") {
			const QByteArray event = jsondata.value(QLatin1String("__event")).toByteArray();
			const QByteArray modelid = jsondata.value(QLatin1String("id")).toByteArray();
			NotificationModel* model = m_models.value(modelid);
			if (!model) {
				model = new NotificationModel(pluginid, instanceid);
				m_models.insert(modelid, model);
			}
			if (event == "reset") {
				const QString key = jsondata.value(QLatin1String("__key")).toString();
				model->clear(key);
			} else if (event == "remove") {
				model->remove(jsondata);
			} else if (event == "change") {
				model->change(jsondata);
			} else {
				qWarning()<<"Notification model event not supported!" << event << jsondata;
			}
		}
	}
}

QObject* ActionController::getModel(const QString& modelid)
{
	if (m_models.value(modelid))
		qDebug() << m_models.value(modelid)->getPluginid() << m_models.value(modelid)->rowCount();
	return m_models.value(modelid);
}

#include "actioncontroller.moc"