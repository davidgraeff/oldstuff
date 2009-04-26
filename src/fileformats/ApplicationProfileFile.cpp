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
#include "fileformats/ApplicationProfileFile.h"
#include "liriexec/actions/Action.h"
#include "liriexec/actions/ActionChangeMode.h"
#include "liriexec/actions/ActionChangeVariable.h"
#include "liriexec/actions/ActionChangeBusInstance.h"
#include "liriexec/actions/ActionProgram.h"
#include "liriexec/actions/ActionBus.h"
#include "liriexec/actions/ActionGroup.h"
#include <QStringList>
#include <QDebug>
#include <QDir>
#include "config.h"

ApplicationProfileFile::ApplicationProfileFile(const QString& uid) : DesktopFile(uid), noclear(false) {
	reload();
}

ApplicationProfileFile::~ApplicationProfileFile() {
	if (!noclear) clear();
}

QList< VariablesInAppProfile >& ApplicationProfileFile::getVariables() {
	return variables;
}

QList< BusServiceWithOptions >& ApplicationProfileFile::getBusserviceNames() {
	return busservices;
}

void ApplicationProfileFile::setNoClear() {
	noclear = true;
}

double ApplicationProfileFile::minversion() const {
	return LIRI_FILEFORMAT_MIN;
}

double ApplicationProfileFile::maxversion() const {
	return LIRI_FILEFORMAT_MAX;
}

const QString ApplicationProfileFile::path() const {
	return QDir::homePath() + QLatin1String("/") + QLatin1String(LIRI_HOME_APPPROFILES_DIR);
}

const QString ApplicationProfileFile::type() const {
	return QLatin1String(LIRI_FILEFORMAT_APPPROFILES);
}

void ApplicationProfileFile::clear() {
	lastAction = 0;
	lastActionGroup = 0;
	while (size())
		delete takeFirst();
	DesktopFile::clear();
	QList<ActionGroup*>::clear();
}

bool ApplicationProfileFile::save_private(QFile& inifile) {
	for (int i=0; i<size(); ++i) {
		// bus services
		inifile.write("[busservices]\n");
		for (int i=0; i < busservices.size(); ++i) {
			//Format of one busservice line: bus://servicename=option1 option2
			inifile.write(busservices[i].serviceid.toUtf8() + "=");
			for (int j=0; j < busservices[i].options.size(); ++j) {
				inifile.write(busservices[i].options[j].toUtf8() + " ");
			}
			inifile.write("\n");
		}
		inifile.write("\n");

		// variables
		inifile.write("[variables]\n");
		for (int i=0; i < variables.size(); ++i) {
			inifile.write(variables[i].name.toUtf8() + "=" + variables[i].dbustype +
				" " + variables[i].init.toUtf8() + "\n");
		}
		inifile.write("\n");

		// actiongroups
		for (int i=0; i < size(); ++i) {
			inifile.write("[group]\n");
			//FIXME channels
			if (at(i)->channels.size()) {
				QString a;
				foreach(unsigned int i, at(i)->channels) {
					a.append(QString::number(i));
					a.append(QLatin1Char(' '));
				}
				inifile.write("Channel=" + a.toUtf8() + "\n");
			}
			if (at(i)->mode.size())
				inifile.write("Mode=" + at(i)->mode.toUtf8() + "\n");
			if (at(i)->key.size())
				inifile.write("Key=" + at(i)->key.toUtf8() + "\n");
			if (at(i)->keystate != 1)
				inifile.write("KeyState=" + QString::number(at(i)->keystate).toAscii() + "\n");
			if (at(i)->getRepeat().size())
				inifile.write("Repeat=" + at(i)->getRepeat().toUtf8() + "\n");
			inifile.write("\n");

			// actions
			for (int j=0; j < at(i)->size(); ++j) {
				saveAction(inifile, at(i)->at(j));
			}
		}
	}

	return true;
}

void ApplicationProfileFile::saveAction(QFile& inifile, Action* action) {
	if (action->type == Action::ChangeMode) {
		ActionChangeMode* act = dynamic_cast<ActionChangeMode*>(action);
		inifile.write("[changemode]\n");
		if (act->conditiontext.size())
			inifile.write("Condition=" + act->conditiontext.toUtf8() + "\n");
		inifile.write("NewMode=" + act->newmode.toUtf8() + "\n");

	} else if (action->type == Action::ChangeVariable) {
		ActionChangeVariable* act = dynamic_cast<ActionChangeVariable*>(action);
		inifile.write("[changevariable]\n");
		if (act->conditiontext.size())
			inifile.write("Condition=" + act->conditiontext.toUtf8() + "\n");
		inifile.write("Name=" + act->name.toUtf8() + "\n");
		inifile.write("Mod=" + act->getModText().toUtf8() + "\n");

	} else if (action->type == Action::ChangeServiceInstance) {
		ActionChangeBusInstance* act = dynamic_cast<ActionChangeBusInstance*>(action);
		inifile.write("[changebusinstance]\n");
		if (act->conditiontext.size())
			inifile.write("Condition=" + act->conditiontext.toUtf8() + "\n");
		inifile.write("Serviceid=" + act->busserviceid.toUtf8() + "\n");
		inifile.write("Mod=" + act->getWhatText().toUtf8() + "\n");

	} else if (action->type == Action::Program) {
		ActionProgram* act = dynamic_cast<ActionProgram*>(action);
		inifile.write("[program]\n");
		if (act->conditiontext.size())
			inifile.write("Condition=" + act->conditiontext.toUtf8() + "\n");
		inifile.write("Program=" + act->program.toUtf8() + "\n");
		if (act->workingDirectory.size())
			inifile.write("WorkingDirectory=" + act->workingDirectory.toUtf8() + "\n");
		if (act->pipeoutput.size())
			inifile.write("PipeVar=" + act->pipeoutput.toUtf8() + "\n");
		if (act->timeout != 0)
			inifile.write("Timeout=" + QString::number(act->timeout).toUtf8() + "\n");

	} else if (action->type == Action::Bus) {
		ActionBus* act = dynamic_cast<ActionBus*>(action);
		inifile.write("[bus]\n");
		if (act->conditiontext.size())
			inifile.write("Condition=" + act->conditiontext.toUtf8() + "\n");
		inifile.write("Bus=" + act->getBusText().toUtf8() + "\n");
		if (act->timeout != 0)
			inifile.write("Timeout=" + QString::number(act->timeout).toUtf8() + "\n");
		for (int i = 0; i < act->argin.size(); ++i) {
			inifile.write("Arg=" + act->argin[i].name.toUtf8() + " in " +
				act->argin[i].dbustype + " " + act->argin[i].argtext.toUtf8() + "\n");
		}
		for (int i = 0; i < act->argout.size(); ++i) {
			inifile.write("Arg=" + act->argin[i].name.toUtf8() + " out " +
				act->argin[i].dbustype + " " + act->argin[i].argtext.toUtf8() + "\n");
		}
	}
	inifile.write("\n");
}

void ApplicationProfileFile::readgroup(const QString& group) {
	if (group == QLatin1String("bus") && lastActionGroup) {
		lastAction = new ActionBus();
		lastActionGroup->append(lastAction);
	} else if (group == QLatin1String("program") && lastActionGroup) {
		lastAction = new ActionProgram();
		lastActionGroup->append(lastAction);
	} else if (group == QLatin1String("changevariable") && lastActionGroup) {
		lastAction = new ActionChangeVariable();
		lastActionGroup->append(lastAction);
	} else if (group == QLatin1String("changemode") && lastActionGroup) {
		lastAction = new ActionChangeMode();
		lastActionGroup->append(lastAction);
	} else if (group == QLatin1String("changebusinstance") && lastActionGroup) {
		lastAction = new ActionChangeBusInstance();
		lastActionGroup->append(lastAction);
	} else if (group == QLatin1String("group")) {
		lastActionGroup = new ActionGroup();
		append(lastActionGroup);
	}
}

bool ApplicationProfileFile::readline(const QString& group, const QString& key, const QString&, const QString& value) {
	QByteArray k = key.toLatin1();
	if (group == QLatin1String("busservices")) {
		//format: bus://servicename=option1 option2
		QStringList options = value.split(QLatin1Char(' '));
		busservices.append(BusServiceWithOptions(key,options));
	}
	else if (group == QLatin1String("variables")) {
		VariablesInAppProfile tmpvar;
		QStringList tmp = value.split(QChar::fromAscii(' '));
		if (tmp.size() > 0) {
			tmpvar.name = key;
			tmpvar.dbustype = tmp[0][0].toAscii();
		}
		if (tmp.size() > 1) {
			tmpvar.init = tmp[1];
		}
		if (tmp.size() > 0) variables.append(tmpvar);
	}
	else if (group == QLatin1String("group") && lastActionGroup) {
		if (k == "Channel") {
			lastActionGroup->channels.clear();
			QList<QString> l = value.trimmed().split(QLatin1Char(' '));
			foreach (QString s, l) lastActionGroup->channels.insert(s.toUInt());
			return true; }
		if (k == "Mode") { lastActionGroup->mode = value; return true; }
		if (k == "Key") { lastActionGroup->key = value; return true; }
		if (k == "KeyState") { lastActionGroup->keystate = value.toInt(); return true; }
		if (k == "Repeat") { lastActionGroup->setRepeat(value); return true; }
	}
	else if (group == QLatin1String("program") && lastAction) {
		if (k == "Condition") { lastAction->conditiontext = value; return true; }
		ActionProgram* tmp = (ActionProgram*)lastAction;
		if (k == "Program") { tmp->program = value; return true; }
		if (k == "WorkingDirectory") { tmp->workingDirectory = value; return true; }
		if (k == "PipeVar") { tmp->pipeoutput = value; return true; }
		if (k == "Timeout") { tmp->timeout = value.toInt(); return true; }
	}
	else if (group == QLatin1String("bus") && lastAction) {
		if (k == "Condition") { lastAction->conditiontext = value; return true; }
		ActionBus* tmp = (ActionBus*)lastAction;
		if (k == "Timeout") { tmp->timeout = value.toInt(); return true; }
		if (k == "Bus") { tmp->setBusText(value); return true; }
		if (k == "Arg") {
			QStringList s = value.split(QLatin1Char(' '));
			if (s.size() != 4 || (s[1]!=QLatin1String("out") && s[1]!=QLatin1String("in"))) {
				qDebug() << "ApplicationProfileFile::readline: dbus arg invalid!" << s;
				return false;
			}
			ActionArgument tmparg;
			tmparg.name = s[0];
			tmparg.dbustype = s[2][0].toAscii();
			tmparg.argtext = s[3];

			if (s[1] == QLatin1String("out"))
				tmp->argout.append(tmparg);
			else if (s[1] == QLatin1String("in"))
				tmp->argin.append(tmparg);
			return true;
		}

	}
	else if (group == QLatin1String("changevariable") && lastAction) {
		if (k == "Condition") { lastAction->conditiontext = value; return true; }
		ActionChangeVariable* tmp = (ActionChangeVariable*)lastAction;
		if (k == "Name") { tmp->name = value; return true; }
		if (k == "Mod") { tmp->setModText(value); return true; }

	}
	else if (group == QLatin1String("changemode") && lastAction) {
		if (k == "Condition") { lastAction->conditiontext = value; return true; }
		ActionChangeMode* tmp = (ActionChangeMode*)lastAction;
		if (k == "NewMode") { tmp->newmode = value; return true; }

	}
	else if (group == QLatin1String("changebusinstance") && lastAction) {
		if (k == "Condition") { lastAction->conditiontext = value; return true; }
		ActionChangeBusInstance* tmp = (ActionChangeBusInstance*)lastAction;
		if (k == "Serviceid") { tmp->busserviceid = value; return true; }
		if (k == "Mod") { tmp->setWhatText(value); return true; }

	}

	return true;
}
