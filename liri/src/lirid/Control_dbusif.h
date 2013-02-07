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
#ifndef DBUS_INTERFACES_DEVICELIST_H_1220516561
#define DBUS_INTERFACES_DEVICELIST_H_1220516561

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
class Control;

typedef QList<int> intList;
Q_DECLARE_METATYPE(intList);

/*
 * Adaptor class for interface org.liri.Control
 */
class ControlAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.liri.Control")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.liri.Control\" >\n"
"    <method name=\"quit\" />\n"
"    <method name=\"start\" />\n"
"    <method name=\"version\" >\n"
"      <arg direction=\"out\" type=\"s\" name=\"version\" />\n"
"    </method>\n"
"    <method name=\"lastHalCallout\" >\n"
"      <arg direction=\"out\" type=\"s\" name=\"datetime\" />\n"
"    </method>\n"
"    <method name=\"started\" >\n"
"      <arg direction=\"out\" type=\"s\" name=\"datetime\" />\n"
"    </method>\n"
"    <signal name=\"deviceAdded\" >\n"
"      <arg type=\"s\" name=\"rid\" />\n"
"    </signal>\n"
"    <signal name=\"deviceRemoved\" >\n"
"      <arg type=\"s\" name=\"rid\" />\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
	ControlAdaptor(Control *parent);
	virtual ~ControlAdaptor();
	inline Control *parent() const;

public: // PROPERTIES
public Q_SLOTS: // METHODS
	void quit();
	void start();
	QString version();
	QString lastHalCallout();
	QString started();
Q_SIGNALS: // SIGNALS
	void deviceAdded(const QString& rid);
	void deviceRemoved(const QString& rid);
};

#endif
