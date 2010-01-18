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
#ifndef LIRI_ACTIONSOFINSTANCE_H_
#define LIRI_ACTIONSOFINSTANCE_H_

#include <QString>
#include <QObject>
#include <QMap>
#include <QList>
#include <QVariant>
#include <QStringList>
#include "ActionTree.h"

class QDBusConnection;
class Action;
class ActionGroup;
class DeviceList;
class BusConnection;
class OrgLiriDevManagerReceiverInterface;
class ApplicationProfileFile;
class ActionChangeMode;
class ActionChangeVariable;
class ActionChangeBusInstance;
class ActionProgram;
class ActionBus;

/* The shared values per remote and
a map with all actions of a remote key */
class DeviceInstance: public QObject {
	Q_OBJECT
	public:
		DeviceInstance(QDBusConnection* conn, DeviceList* devicelist, BusConnection* busconnection,
			int instance, QObject* parent = 0);
		~DeviceInstance();

		/* about actions */
		inline void addApplicationProfileFile(ApplicationProfileFile* ap);
		inline void evalChangeMode(ActionChangeMode* action);
		inline void evalChangeVariable(ActionChangeVariable* action);
		inline void evalChangeBusInstance(ActionChangeBusInstance* action);
		inline void evalProgram(ActionProgram* action);
		inline void evalBus(ActionBus* action);

		/* shared variables */
		QVariant* classifyVar(int varianttype, const QString& text);
		void addKnownVar(const QString& name, char dbustype, const QString& text);
		QStringList getVariables();
		QStringList getProfileUids();

		/* mode */
		QString getMode() const;
		void setMode(const QString &mode);

		/* instance */
		int getInstance();
		void clear();
		void reload();

	private Q_SLOTS:;
		void key(const QString &keycode, const QString &keyname, uint channel, int pressed);

	private:
		QDBusConnection* conn;
		QMap< QString, QVariant > vars;
		QList<QVariant> staticvars;
		ActionTree actions;
		QString currentmode;
		DeviceList* devicelist;
		BusConnection* busconnection;
		OrgLiriDevManagerReceiverInterface * receiver;
		QStringList appProUids;
		int instance;
};

#endif /*LIRI_ACTIONSOFINSTANCE_H_*/
