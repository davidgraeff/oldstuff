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
#ifndef LIRI_ACTIONBUS_H_
#define LIRI_ACTIONBUS_H_

#include <QVariant>
#include <QString>
#include <QList>
#include "liriexec/actions/Action.h"

struct ActionArgument {
	QString name; // arg name
	char dbustype; // var type
	QString argtext; // text
	QVariant* argvalue;
};

class TargetList;

class ActionBus : public Action {
	Q_OBJECT
public:
	ActionBus();
	virtual ~ActionBus();

	bool work();
	void setBusText(const QString& txt);
	QString getBusText();

	QList<ActionArgument> argin; //EVAL
	QList<ActionArgument> argout; //EVAL

	QString interface;
	QString object;
	QString method;
	int timeout;
	QString busserviceid;

	TargetList* targetlist; //EVALonly
};

#endif
