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
#include "config.h"
#include <QDebug>

Action::Action(Action::ActionType type) : type(type), conditioncmp(cmpUndefined) {

}

bool Action::work() {
	return false;
}

void Action::setDeviceInstance(QString ri) {
	deviceinstance = ri;
}

bool Action::isConditionOK() const {
	if (conditioncmp==cmpUndefined || !conditionsrc1 || !conditionsrc2) return true;
	if (conditionsrc1->type() != conditionsrc2->type()) return true;

	if (conditionsrc1->type() == QVariant::String) {
		if (conditioncmp==cmpGreater)
			return (conditionsrc1->toString() > conditionsrc2->toString());
		else if (conditioncmp==cmpSmaller)
			return (conditionsrc1->toString() < conditionsrc2->toString());
		else if (conditioncmp==cmpEqual)
			return (conditionsrc1->toString() == conditionsrc2->toString());
	} else
	if (conditionsrc1->type() == QVariant::Int) {
		if (conditioncmp==cmpGreater)
			return (conditionsrc1->toInt() > conditionsrc2->toInt());
		else if (conditioncmp==cmpSmaller)
			return (conditionsrc1->toInt() < conditionsrc2->toInt());
		else if (conditioncmp==cmpEqual)
			return (conditionsrc1->toInt() == conditionsrc2->toInt());
	} else
	if (conditionsrc1->type() == QVariant::UInt) {
		if (conditioncmp==cmpGreater)
			return (conditionsrc1->toUInt() > conditionsrc2->toUInt());
		else if (conditioncmp==cmpSmaller)
			return (conditionsrc1->toUInt() < conditionsrc2->toUInt());
		else if (conditioncmp==cmpEqual)
			return (conditionsrc1->toUInt() == conditionsrc2->toUInt());
	} else
	if (conditionsrc1->type() == QVariant::Double) {
		if (conditioncmp==cmpGreater)
			return (conditionsrc1->toDouble() > conditionsrc2->toDouble());
		else if (conditioncmp==cmpSmaller)
			return (conditionsrc1->toDouble() < conditionsrc2->toDouble());
		else if (conditioncmp==cmpEqual)
			return (conditionsrc1->toDouble() == conditionsrc2->toDouble());
	} else
	if (conditionsrc1->type() == QVariant::Bool) {
		if (conditioncmp==cmpEqual)
			return (conditionsrc1->toBool() == conditionsrc2->toBool());
	}
	return false;
}

QVariant Action::convert(int varianttype, const QString& text) {
	switch (varianttype) {
		case 1: return (text==QLatin1String("true") || text.toInt()==1);
		case 2: return text.toInt();
		case 3: return text.toUInt();
		case 6: return text.toDouble();
		case 10: return text;
		default:
		qWarning() << "Text->Variant conversion failed: Unknown type! Text:" << text << "Type:"<<varianttype;
		return QVariant();
	}
}

int Action::getVariantType(char dbustype) {
	switch (dbustype) {
		case 'b': return 1;
		case 'i': return 2;
		case 'u': return 3;
		case 'd': return 6;
		case 's': return 10;
		default:
		return 0;
	}
}

