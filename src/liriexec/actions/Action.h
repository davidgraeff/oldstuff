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
#ifndef LIRI_APPLICATION_ACTION_H_
#define LIRI_APPLICATION_ACTION_H_

#include <dbus/dbus-protocol.h>
#include <QObject>
#include <QVariant>
#include <QString>

class Action : public QObject {
	Q_OBJECT
public:
	/* type */
	enum ActionType {
		Undefined,
		ChangeMode,
		ChangeVariable,
		ChangeServiceInstance,
		Program,
		Bus
	};

	Action(Action::ActionType type);
	virtual bool work();

	enum condition_cmp { cmpUndefined, cmpGreater, cmpSmaller, cmpEqual };

	/* condition */
	bool isConditionOK() const;

	void setDeviceInstance(QString);

	static int getVariantType(char dbustype);
	static QVariant convert(int varianttype, const QString& text);

	/* type */
	ActionType type;

	QString deviceinstance; //EVALonly

	/* condition */
	QString conditiontext;
	QVariant* conditionsrc1;
	QVariant* conditionsrc2;
	condition_cmp conditioncmp;

	/* execution engine */
	bool pressed;
Q_SIGNALS:
	void executed(QString receiverinstance, int result, const QString &executedstring);

};

#endif
