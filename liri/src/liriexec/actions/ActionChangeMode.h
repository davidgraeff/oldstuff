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
#ifndef LIRI_ACTIONCHANGEMODE_H_
#define LIRI_ACTIONCHANGEMODE_H_

#include <QVariant>
#include <QString>
#include <QList>
#include "liriexec/actions/Action.h"

class ActionChangeMode : public Action {
	Q_OBJECT
public:
	ActionChangeMode();
	virtual ~ActionChangeMode();

	bool work();

	QString newmode;

	QString* currentmode; //EVALonly
Q_SIGNALS:
	void modeChanged(QString receiverinstance, const QString &oldmode, const QString &newmode);
};

#endif
