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
#include "liriexec/actions/ActionChangeVariable.h"
#include "liriexec/device/varmod/base.h"
#include "config.h"
#include <QDebug>

ActionChangeVariable::ActionChangeVariable() : Action(Action::ChangeVariable), var(0), modifier(0) {}
ActionChangeVariable::~ActionChangeVariable() {}

bool ActionChangeVariable::work() {
	if (!modifier || !var) return false;
	modifier->evaluate(var, args);
	return true;
}

void ActionChangeVariable::setModText(const QString& txt) {
	QStringList tmp = txt.split(QLatin1Char(' '));
	if (tmp.size() > 0) mod = tmp[0];
	if (tmp.size() > 1) {
		tmp.removeAt(0);
		args = tmp;
	}
}

QString ActionChangeVariable::getModText() {
	return mod + QLatin1Char(' ') + args.join(QLatin1String(" "));
}

