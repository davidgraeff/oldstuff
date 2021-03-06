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
#ifndef LIRI_ACTIONCHANGEBUSINSTANCE_H_
#define LIRI_ACTIONCHANGEBUSINSTANCE_H_

#include <QVariant>
#include <QString>
#include <QList>
#include "liriexec/actions/Action.h"

class TargetList;

class ActionChangeBusInstance : public Action {
	Q_OBJECT
public:
	ActionChangeBusInstance();
	virtual ~ActionChangeBusInstance();

	bool work();
	void setWhatText(const QString& txt);
	QString getWhatText();

	enum {all, none, increase, decrease, specific, multiple} what;
	QList<int> multiplenr;
	int specificnr;
	QString busserviceid;

	TargetList* targetlist; //EVALonly
};
#endif
