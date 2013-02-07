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
#include "liriexec/actions/Action.h"
#include "liriexec/actions/ActionGroup.h"
#include "config.h"
#include <QStringList>

ActionGroup::ActionGroup() {
	repeat = false;
	repeatlag = LIRI_DBUS_REPEAT_INIT;
	repeattick = LIRI_DBUS_REPEAT_TICK;
	keystate = 1;
}

ActionGroup::~ActionGroup() {
	while (size()) delete takeFirst();
}

void ActionGroup::setRepeat(const QString& value) {
	QStringList s = value.split(QLatin1Char(' '));
	if (s.size()) {
		repeat = true;
		repeatlag = s[0].toInt();
	}
	if (s.size() > 1)
		repeattick = s[1].toInt();
}

QString ActionGroup::getRepeat() {
	if (repeat) {
		QString tmp = QString::number(repeatlag);
		if (repeattick) tmp += QLatin1String(" ") + QString::number(repeattick);
		return tmp;
	} else return QString();
}

