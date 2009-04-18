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
#ifndef DBUS_INTERFACES_RECEIVER_H_1220516577
#define DBUS_INTERFACES_RECEIVER_H_1220516577

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
class DeviceInstance;

/*
 * Adaptor class for interface org.liri.Receiver
 */
class ReceiverAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.liri.Device")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.liri.Device\" >\n"
"    <method name=\"setAssociatedRemote\" >\n"
"      <arg direction=\"in\" type=\"s\" name=\"remoteid\" />\n"
"    </method>\n"
"    <method name=\"reloadAssociatedRemote\" />\n"
"    <method name=\"RemoteState\" >\n"
"      <arg direction=\"out\" type=\"i\" name=\"state\" />\n"
"    </method>\n"
"    <method name=\"ReceiverState\" >\n"
"      <arg direction=\"out\" type=\"i\" name=\"state\" />\n"
"    </method>\n"
"    <method name=\"getSettings\" >\n"
"      <arg direction=\"in\" type=\"as\" name=\"keys\" />\n"
"      <arg direction=\"out\" type=\"as\" name=\"values\" />\n"
"    </method>\n"
"    <method name=\"getAllSettings\" >\n"
"      <arg direction=\"out\" type=\"a{ss}\" name=\"settings\" />\n"
"      <annotation value=\"QMap&lt;QString,QString>\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
"    </method>\n"
"    <method name=\"setSettings\" >\n"
"      <arg direction=\"in\" type=\"a{ss}\" name=\"settings\" />\n"
"      <annotation value=\"QMap&lt;QString,QString>\" name=\"com.trolltech.QtDBus.QtTypeName.In0\" />\n"
"    </method>\n"
"    <signal name=\"receiverStateChanged\" >\n"
"      <arg type=\"i\" name=\"state\" />\n"
"    </signal>\n"
"    <signal name=\"remoteStateChanged\" >\n"
"      <arg type=\"i\" name=\"state\" />\n"
"    </signal>\n"
"    <signal name=\"key\" >\n"
"      <arg type=\"s\" name=\"keycode\" />\n"
"      <arg type=\"s\" name=\"keyname\" />\n"
"      <arg type=\"u\" name=\"channel\" />\n"
"      <arg type=\"i\" name=\"pressed\" />\n"
"    </signal>\n"
"    <signal name=\"driverChangedSettings\" >\n"
"      <arg type=\"a{ss}\" name=\"changedsettings\" />\n"
"      <annotation value=\"QMap&lt;QString,QString>\" name=\"com.trolltech.QtDBus.QtTypeName.In0\" />\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
	ReceiverAdaptor(DeviceInstance *parent);
    virtual ~ReceiverAdaptor();
	inline DeviceInstance *parent() const;

public: // PROPERTIES
public Q_SLOTS: // METHODS
    int ReceiverState();
    int RemoteState();
    QMap<QString,QString> getAllSettings();
    QStringList getSettings(const QStringList &keys);
    void reloadAssociatedRemote();
    void setAssociatedRemote(const QString &remoteid);
    void setSettings(const QMap<QString,QString> &settings);
Q_SIGNALS: // SIGNALS
    void driverChangedSettings(const QMap<QString,QString> &changedsettings);
    void key(const QString &keycode, const QString &keyname, uint channel, int pressed);
    void receiverStateChanged(int state);
    void remoteStateChanged(int state);
};

#endif
