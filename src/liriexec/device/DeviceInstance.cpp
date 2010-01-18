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
/* execution engine */
#include "device/DeviceInstance.h"
#include "device/DeviceInstance_dbusif.h"
#include "device/TargetList.h"
#include "DeviceList.h"
#include "liriexec/actions/ActionChangeMode.h"
#include "liriexec/actions/ActionChangeVariable.h"
#include "liriexec/actions/ActionChangeBusInstance.h"
#include "liriexec/actions/ActionProgram.h"
#include "liriexec/actions/ActionBus.h"
#include "liriexec/actions/Action.h"
#include "liriexec/actions/ActionGroup.h"
/* read desktop and application profiles */
#include "fileformats/RemoteLoadProfilesFile.h"
#include "fileformats/DesktopProfileFile.h"
#include "fileformats/ApplicationProfileFile.h"
/* variable manipulation classes */
#include "varmod/modSet.h"
#include "varmod/modInvert.h"
#include "varmod/modChangeVolume.h"
#include "varmod/modChange.h"
/* common liri stuff */
#include "config.h"
/* qt */
#include <QStringList>
#include <QVariant>
#include <QSet>
#include <QDebug>
/* dbus */
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include "businterconnect/BusConnection.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"

DeviceInstance::DeviceInstance(QDBusConnection* conn, DeviceList* devicelist, BusConnection* busconnection,
			int instance, QObject* parent)
			: QObject(parent), conn(conn), devicelist(devicelist), busconnection(busconnection), instance(instance) {
	Q_ASSERT(conn);
	Q_ASSERT(devicelist);
	Q_ASSERT(busconnection);
	/* propagate execution interface for this instance */
	new DeviceInstanceAdaptor(this);
	QString objectname;
	objectname.append(QLatin1String(LIRI_DBUS_OBJECT_RECEIVERS"/"));
	objectname.append(QString::number(instance));
	setObjectName(objectname);
	if ( !conn->registerObject(objectname, static_cast<QObject*>(this)) ) {
		qWarning() << "DeviceInstance: Couldn't register object:" << objectname;
	}

	receiver = busconnection->getDeviceManagerReceiver(instance);

	if (!receiver) {
		qWarning() << "DeviceInstance: Couldn't find device manager object";
	} else {
		connect((QObject*)receiver, SIGNAL( key(const QString &, const QString &, uint, int) ),
			SLOT( key(const QString &, const QString &, uint, int) ));
	}
}

DeviceInstance::~DeviceInstance() {
	clear();
	QString objectname;
	objectname.append(QLatin1String(LIRI_DBUS_OBJECT_RECEIVERS"/"));
	objectname.append(QString::number(instance));
	conn->unregisterObject(objectname);
}

void DeviceInstance::clear() {
	actions.clear();
	appProUids.clear();
}

void DeviceInstance::key(const QString &keycode, const QString &keyname, uint channel, int pressed) {
	Q_UNUSED(keycode);

	if (!keyname.size()) return;

	ActionGroup* actiongroup = actions.get(channel, keyname, currentmode);
	if (!actiongroup) return;

	/* only trigger if channel is fitting (if any) */
	if (actiongroup->channels.size() && !actiongroup->channels.contains(channel)) return;

	devicelist->stopJobs();
	if (actiongroup && actiongroup->keystate == pressed) {
		devicelist->startJob(actiongroup);
	}
}

void  DeviceInstance::reload() {
	//clear
	clear();
	// get remote-UID, for the remote filename
	QStringList settingkeys;
	settingkeys.append(QLatin1String("remote.uid"));
	QString remoteuid = receiver->getSettings(settingkeys).value()[0];

	// get controlled DesktopProfile-UIDs
	RemoteLoadProfilesFile* loaderfile = new RemoteLoadProfilesFile(remoteuid);
	if (loaderfile->state() < 1) {
		qDebug() << "RID:" << instance << "Invalide loader file:" << loaderfile->getFilename();
		delete loaderfile; loaderfile = 0;
		return;
	} else {
		qDebug() << "RID:" << instance << "Profiles controlled by remote" << remoteuid;
	}

	QSet<QString> DesktopProfileFileUIDs = loaderfile->controlled();
	delete loaderfile; loaderfile = 0;

	// get ApplicationProfile-UIDs
	QSet<QString> ApplicationProfileFileUIDs;
	foreach (QString uid, DesktopProfileFileUIDs) {
		DesktopProfileFile* dp = new DesktopProfileFile(uid);
		if (dp->getState() == DesktopFile::Valide) {
			ApplicationProfileFileUIDs.unite(*dp);
			qDebug() << "RID:" << instance << "Load desktop profile" << dp->getUid() << dp->getName();
		}
		delete dp;
	}

	// add ActionGroups/Actions to this instance
	bool added = false;
	foreach (QString uid, ApplicationProfileFileUIDs) {
		ApplicationProfileFile* ap = new ApplicationProfileFile(uid);
		/* valid application profile? */
		if (ap->getState() == DesktopFile::Valide) {
			qDebug() << "RID:" << instance << "Load app profile" << ap->getUid() << ap->getName();
			/* app profile must not delete actions, we take care of them now */
			ap->setNoClear();
			appProUids.append(uid);
			addApplicationProfileFile(ap);
			added = true;
		} else {
			qDebug() << "RID:" << instance << "Load app profile failed" << ap->getUid() << ap->getName() << ap->getState();
		}
		delete ap;
	}

	if (!added)
		qDebug() << "RID:" << instance << "No actions found";
	else
		devicelist->instanceLoadedProfiles(this);
}

QStringList DeviceInstance::getProfileUids() {
	return appProUids;
}

int DeviceInstance::getInstance() {
	return instance;
}

QString DeviceInstance::getMode() const {
	return currentmode;
}

void DeviceInstance::setMode(const QString &mode) {
	currentmode = mode;
}

inline void DeviceInstance::addApplicationProfileFile(ApplicationProfileFile* ap) {
	/* add known variables */
	foreach (VariablesInAppProfile va, ap->getVariables()) {
		addKnownVar(va.name, va.dbustype, va.init);
	}

	/* add bus services */
	foreach (BusServiceWithOptions busservice, ap->getBusserviceNames()) {
		devicelist->addTargetList(busservice);
	}

	/* get all actions from the application profile */
	for (int i=0; i < ap->size(); ++i) {
		ActionGroup* actiongroup = ap->at(i);
		// evaluate actions of this actiongroup
		for (int j=0; j < actiongroup->size(); ++j) {
			Action* action = actiongroup->at(j);
			action->deviceinstance = instance;

			/* conditions */
			QStringList cond = action->conditiontext.split(QLatin1Char(' '));
			if (cond.size() == 3) {
				action->conditionsrc1 = classifyVar('s', cond[0]);
				action->conditionsrc2 = classifyVar('s', cond[2]);
				if (cond[1] == QLatin1String("="))
					action->conditioncmp = Action::cmpEqual;
				else if (cond[1] == QLatin1String(">"))
					action->conditioncmp = Action::cmpGreater;
				else if (cond[1] == QLatin1String("<"))
					action->conditioncmp = Action::cmpSmaller;
				else
					action->conditioncmp = Action::cmpUndefined;
			}

			/* evaluate variables / conditions */
			if (action->type == Action::ChangeMode)
				evalChangeMode(dynamic_cast<ActionChangeMode*>(action));
			else if (action->type == Action::ChangeVariable)
				evalChangeVariable(dynamic_cast<ActionChangeVariable*>(action));
			else if (action->type == Action::ChangeServiceInstance)
				evalChangeBusInstance(dynamic_cast<ActionChangeBusInstance*>(action));
			else if (action->type == Action::Program)
				evalProgram(dynamic_cast<ActionProgram*>(action));
			else if (action->type == Action::Bus)
				evalBus(dynamic_cast<ActionBus*>(action));

			/* connect signal */
			connect(action, SIGNAL(executed(int, int, const QString&)),
				devicelist, SIGNAL(executed(int, int, const QString&)));

		}

		// add actiongroup
		actions.add(actiongroup);
	} // for (int i=0; i < ap->size(); ++i)
}

inline void DeviceInstance::evalChangeMode(ActionChangeMode* action) {
	action->currentmode = &currentmode;
	connect(action, SIGNAL(modeChanged(int, const QString&, const QString&)),
		devicelist, SIGNAL(modeChanged(int, const QString&, const QString&)));
}

inline void DeviceInstance::evalChangeVariable(ActionChangeVariable* action) {
	action->var = classifyVar(0, action->name);
	QStringList s = action->mod.split(QChar::fromAscii(' '));
	if (s.size()) {
		for (int i = 1; i < s.size(); ++i) {
			action->args.push_back(s[i]);
		}
		if (s[0] == QLatin1String("@INVERT"))
			action->modifier = new modInvert();
		else if (s[0] == QLatin1String("@CHANGE"))
			action->modifier = new modChange();
		else if (s[0] == QLatin1String("@CHANGEVOLUME"))
			action->modifier = new modChangeVolume();
		else if (s[0] == QLatin1String("@SET") && s.size()>1)
			action->modifier = new modSet(classifyVar(0, s[1]));
		else { // Zuweisung
			action->modifier = new modSet(classifyVar(0, s[0]));
		}
	}
}

inline void DeviceInstance::evalChangeBusInstance(ActionChangeBusInstance* action) {
	action->targetlist = devicelist->getTargetList(action->busserviceid);
}

inline void DeviceInstance::evalProgram(ActionProgram* action) {
	if (action->pipeoutput.size()) {
		action->workWithOutput = true;
		action->pipevar = classifyVar(0, action->pipeoutput);
	} else {
		action->workWithOutput = false;
	}
}

inline void DeviceInstance::evalBus(ActionBus* action) {
	if (!action->method.size() || !action->object.size() || !action->interface.size()) return;
	action->targetlist = devicelist->getTargetList(action->busserviceid);

	for (int i=0;i< action->argin.size(); ++i)
		action->argin[i].argvalue = classifyVar(
			Action::getVariantType(action->argin[i].dbustype), action->argin[i].argtext);
	for (int i=0;i< action->argout.size(); ++i)
		action->argout[i].argvalue = classifyVar(
			Action::getVariantType(action->argout[i].dbustype), action->argout[i].argtext);
}

void DeviceInstance::addKnownVar(const QString& name, char dbustype, const QString& text) {
	if (!name.size() || dbustype == 0) return;
	vars[name] = Action::convert(Action::getVariantType(dbustype), text);
}

QVariant* DeviceInstance::classifyVar(int varianttype, const QString& text) {
	QMap< QString, QVariant >::iterator it = vars.find(text);
	if (it != vars.end())
	// it is a known variable. Ignore the type and return a pointer
		return &(it.value());
	else if (!varianttype) {
	//not known but should be known due to the ommited type: Error
		return 0;
	} else
	// not known: it is a static value with the type of varianttype
	{
		staticvars.append(Action::convert(varianttype, text));
		return &(staticvars[staticvars.size()-1]);
	}
}

QStringList DeviceInstance::getVariables() {
	QStringList map;
	QMap< QString, QVariant >::iterator it;

	for (it = vars.begin(); it != vars.end(); ++it) {
		map << it.key() + QLatin1Char('@') + it.value().toString();
	}

	return map;
}
