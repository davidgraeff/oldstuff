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
#include "liriexec/actions/ActionChangeBusInstance.h"
#include "liriexec/device/TargetList.h"

#include <QDebug>
#include <QStringList>
#include "config.h"

ActionChangeBusInstance::ActionChangeBusInstance() : Action(Action::ChangeServiceInstance), what(all), specificnr(0), targetlist(0) {}
ActionChangeBusInstance::~ActionChangeBusInstance() {}

void ActionChangeBusInstance::setWhatText(const QString& txt) {
	QStringList arg = txt.split(QLatin1Char(' '));
	if (!arg.size()) what = all;

	if (arg[0] == QLatin1String("+")) {
		what = increase;
	} else if (arg[0] == QLatin1String("-")) {
		what = decrease;
	} else if (arg[0] == QLatin1String("*")) {
		what = all;
	} else if (arg[0] == QLatin1String("#")) {
		what = none;
	} else if (arg[0] == QLatin1String("=")) {
		what = specific;
		specificnr = txt.toInt();
	} else if (arg[0] == QLatin1String("~")) {
		what = multiple;
		for (int i=1; i<arg.size();++i)
			multiplenr.append(txt.toInt());
	}
}

QString ActionChangeBusInstance::getWhatText() {
	QString res;
	switch (what) {
		case all:
			res = QLatin1String("*");
			break;
		case none:
			res = QLatin1String("#");
			break;
		case increase:
			res = QLatin1String("+");
			break;
		case decrease:
			res = QLatin1String("-");
			break;
		case specific:
			res = QLatin1String("= ") + QString::number(specificnr);
			break;
		case multiple:
			res = QLatin1String("~ ");
			foreach(int i, multiplenr) {
				res += QString::number(i) + QLatin1String(" ");
			}
			break;
		default:
			break;
	}
	return res;
}

bool ActionChangeBusInstance::work() {
	if (!targetlist) return false;

	int res = 0;

	switch (what) {
		case all:
			res = targetlist->activateAll();
			break;
		case none:
			res = targetlist->deactivateAll();
			break;
		case increase:
			res = targetlist->activateNext();
			break;
		case decrease:
			res = targetlist->activatePrevious();
			break;
		case specific:
			res = targetlist->activate(specificnr, true);
			break;
		case multiple:
			res = targetlist->activateMultiple(multiplenr);
			break;
		default:
			qWarning() << "ActionChangeBusInstance: Enumerator unknown";
			res = LIRIERR_changebusinstance;
	}

	emit executed(deviceinstance, res, QString());
	return (res > 0);
}
