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
#ifndef DBUS_INTERFACES_EXECUTION_INSTANCE_H_1220651014
#define DBUS_INTERFACES_EXECUTION_INSTANCE_H_1220651014

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
 * Adaptor class for interface org.liri.ExecutionInstance
 */
class DeviceInstanceAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.liri.Device")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.liri.Device\" >\n"
"    <method name=\"getVariables\" >\n"
"      <arg direction=\"out\" type=\"as\" name=\"vars\" />\n"
"    </method>\n"
"    <method name=\"getProfileUids\" >\n"
"      <arg direction=\"out\" type=\"as\" name=\"vars\" />\n"
"    </method>\n"
"    <method name=\"getMode\" >\n"
"      <arg direction=\"out\" type=\"s\" name=\"mode\" />\n"
"    </method>\n"
"    <method name=\"setMode\" >\n"
"      <arg direction=\"in\" type=\"s\" name=\"mode\" />\n"
"    </method>\n"
"    <method name=\"reload\" />\n"
"  </interface>\n"
        "")
public:
    DeviceInstanceAdaptor(DeviceInstance *parent);
    virtual ~DeviceInstanceAdaptor();

    inline DeviceInstance *parent() const;
public: // PROPERTIES
public Q_SLOTS: // METHODS
	QStringList getProfileUids();
	QString getMode();
	void reload();
	void setMode(const QString &mode);
	QStringList getVariables();
};

#endif
