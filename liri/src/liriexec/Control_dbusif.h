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
#ifndef DBUS_INTERFACES_EXECUTION_H_1220651000
#define DBUS_INTERFACES_EXECUTION_H_1220651000

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
class Control;

/*
 * Adaptor class for interface org.liri.Execution
 */
class ControlAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.liri.Control")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.liri.Control\" >\n"
"    <method name=\"quit\" />\n"
"    <method name=\"started\" >\n"
"      <arg direction=\"out\" type=\"s\" name=\"datetime\" />\n"
"    </method>\n"
"    <method name=\"version\" >\n"
"      <arg direction=\"out\" type=\"s\" name=\"version\" />\n"
"    </method>\n"
"    <method name=\"getTargets\" >\n"
"      <arg direction=\"out\" type=\"as\" name=\"targets\" />\n"
"    </method>\n"
"    <method name=\"setTarget\" >\n"
"      <arg direction=\"in\" type=\"s\" name=\"targetid\" />\n"
"      <arg direction=\"in\" type=\"b\" name=\"active\" />\n"
"    </method>\n"
"    <signal name=\"modeChanged\" >\n"
"      <arg type=\"i\" name=\"rid\" />\n"
"      <arg type=\"s\" name=\"oldmode\" />\n"
"      <arg type=\"s\" name=\"newmode\" />\n"
"    </signal>\n"
"    <signal name=\"executed\" >\n"
"      <arg type=\"i\" name=\"rid\" />\n"
"      <arg type=\"i\" name=\"result\" />\n"
"      <arg type=\"s\" name=\"executed\" />\n"
"    </signal>\n"
"    <signal name=\"targetChanged\" >\n"
"      <arg type=\"i\" name=\"state\" />\n"
"      <arg type=\"s\" name=\"targetid\" />\n"
"    </signal>\n"
"    <signal name=\"deviceAddedExecution\" >\n"
"      <arg type=\"i\" name=\"rid\" />\n"
"    </signal>\n"
"    <signal name=\"deviceRemovedExecution\" >\n"
"      <arg type=\"i\" name=\"rid\" />\n"
"    </signal>\n"
"    <signal name=\"profilesLoaded\" >\n"
"      <arg type=\"i\" name=\"rid\" />\n"
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
	QString started();
	QString version();
	QStringList getTargets();
	void setTarget(const QString &targetid, bool active);
Q_SIGNALS: // SIGNALS
	void executed(int rid, int result, const QString &executed);
	void modeChanged(int rid, const QString &oldmode, const QString &newmode);
	void targetChanged(int state, const QString &targetid);
	void deviceAddedExecution(int rid);
	void deviceRemovedExecution(int rid);
	void profilesLoaded(int rid);
};

#endif
