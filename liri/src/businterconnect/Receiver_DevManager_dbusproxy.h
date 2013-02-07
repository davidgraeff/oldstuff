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
#ifndef DBUS_PROXY_RECEIVER_H_1220722828
#define DBUS_PROXY_RECEIVER_H_1220722828

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#ifndef LIRI_QT_STRINGMAP
#define LIRI_QT_STRINGMAP
typedef QMap<QString, QString> StringMap;
Q_DECLARE_METATYPE(StringMap);
#endif
#ifndef LIRI_QT_STRINGBOOLMAP
#define LIRI_QT_STRINGBOOLMAP
typedef QMap<QString, bool> StringBoolMap;
Q_DECLARE_METATYPE(StringBoolMap);
#endif

/*
 * Proxy class for interface org.liri.Receiver
 */
class OrgLiriDevManagerReceiverInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.liri.Device"; }

public:
    OrgLiriDevManagerReceiverInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~OrgLiriDevManagerReceiverInterface();
	 int position;

public Q_SLOTS: // METHODS
    inline QDBusReply<int> ReceiverState()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("ReceiverState"), argumentList);
    }

    inline QDBusReply<int> RemoteState()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("RemoteState"), argumentList);
    }

    inline QDBusReply<QMap<QString,QString> > getAllSettings()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getAllSettings"), argumentList);
    }

    inline QDBusReply<QStringList> getSettings(const QStringList &keys)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(keys);
        return callWithArgumentList(QDBus::Block, QLatin1String("getSettings"), argumentList);
    }

    inline QDBusReply<void> reloadAssociatedRemote()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("reloadAssociatedRemote"), argumentList);
    }

    inline QDBusReply<void> setAssociatedRemote(const QString &remoteid)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(remoteid);
        return callWithArgumentList(QDBus::Block, QLatin1String("setAssociatedRemote"), argumentList);
    }

    inline QDBusReply<void> setSettings(const QMap<QString,QString> &settings)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(settings);
        return callWithArgumentList(QDBus::Block, QLatin1String("setSettings"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void driverChangedSettings(const QMap<QString,QString> &changedsettings);
    void key(const QString &keycode, const QString &keyname, uint channel, int pressed);
    void receiverStateChanged(int state);
    void remoteStateChanged(int state);
};

#endif
