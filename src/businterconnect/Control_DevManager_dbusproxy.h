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
#ifndef DBUS_PROXY_RECEIVERLIST_H_1220722836
#define DBUS_PROXY_RECEIVERLIST_H_1220722836

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

typedef QList<int> intList;
Q_DECLARE_METATYPE(intList);


/*
 * Proxy class for interface org.liri.Control
 */
class OrgLiriDevManagerControlInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.liri.Control"; }

public:
    OrgLiriDevManagerControlInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~OrgLiriDevManagerControlInterface();

public Q_SLOTS: // METHODS
    inline QDBusReply<QString> lastHalCallout()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("lastHalCallout"), argumentList);
    }

    inline QDBusReply<void> quit()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("quit"), argumentList);
    }

    inline QDBusReply<void> start()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("start"), argumentList);
    }

    inline QDBusReply<QString> started()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("started"), argumentList);
    }

    inline QDBusReply<QString> version()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("version"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void deviceAdded(int rid);
    void deviceRemoved(int rid);
};

#endif
