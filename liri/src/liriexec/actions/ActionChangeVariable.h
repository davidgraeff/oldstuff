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
#ifndef LIRI_ACTIONCHANGEVARIABLE_H_
#define LIRI_ACTIONCHANGEVARIABLE_H_

#include <QVariant>
#include <QString>
#include <QStringList>
#include "liriexec/actions/Action.h"

class VariableModifier;

class ActionChangeVariable : public Action {
	Q_OBJECT
public:
	ActionChangeVariable();
	virtual ~ActionChangeVariable();

	bool work();
	void setModText(const QString& txt);
	QString getModText();
	
	QString name;
	QString mod;
	QStringList args;

	QVariant* var; //EVALonly
	VariableModifier* modifier; //EVALonly
};
#endif

