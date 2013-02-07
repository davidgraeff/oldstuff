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
#ifndef DBUS_PROXY_EXECUTION_H_1220722893
#define DBUS_PROXY_EXECUTION_H_1220722893

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#ifndef LIRI_QT_STRINGINTMAP
#define LIRI_QT_STRINGINTMAP
typedef QMap<QString, int> StringIntMap;
Q_DECLARE_METATYPE(StringIntMap);
#endif

/*
 * Proxy class for interface org.liri.Execution
 */
class OrgLiriExecutionControlInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.liri.Control"; }

public:
    OrgLiriExecutionControlInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~OrgLiriExecutionControlInterface();

public Q_SLOTS: // METHODS
    inline QDBusReply<void> quit()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("quit"), argumentList);
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

    inline QDBusReply<QStringList> getTargets()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getTargets"), argumentList);
    }

    inline QDBusReply<void> setTarget(const QString &targetid, bool active)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(targetid) << qVariantFromValue(active);
        return callWithArgumentList(QDBus::Block, QLatin1String("setTarget"), argumentList);
    }

Q_SIGNALS: // SIGNALS
	void executed(const QString & rid, int result, const QString &executed);
	void modeChanged(const QString & rid, const QString &oldmode, const QString &newmode);
	void targetChanged(int state, const QString &targetid);
	void deviceAddedExecution(const QString & rid);
	void deviceRemovedExecution(const QString & rid);
	void profilesLoaded(const QString & rid);
};

#endif
