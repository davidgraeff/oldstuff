/****************************************************************************
** This file is part of the linux remotes project
**
** Use this file under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#ifndef DBUS_PROXY_EXECUTIONINSTANCE_H_1220722903
#define DBUS_PROXY_EXECUTIONINSTANCE_H_1220722903

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.liri.ExecutionInstance
 */
class OrgLiriExecutionReceiverInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.liri.Device"; }

public:
    OrgLiriExecutionReceiverInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~OrgLiriExecutionReceiverInterface();

public Q_SLOTS: // METHODS
    inline QDBusReply<QStringList> getProfileUids()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getProfileUids"), argumentList);
    }
    inline QDBusReply<QString> getMode()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getMode"), argumentList);
    }

    inline QDBusReply<void> reload()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("reload"), argumentList);
    }

    inline QDBusReply<void> setMode(const QString &mode)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(mode);
        return callWithArgumentList(QDBus::Block, QLatin1String("setMode"), argumentList);
    }

    inline QDBusReply<QStringList> getVariables()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getVariables"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

#endif
