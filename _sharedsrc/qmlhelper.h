#pragma once
#include <QObject>
#include <QVariantMap>

class QMLHelper : public QObject
{
	Q_OBJECT
public Q_SLOTS:
	/**
	 * Create a deep copy for qml
	 */
	static QVariantMap deepcopy(const QVariantMap& orig);

};
