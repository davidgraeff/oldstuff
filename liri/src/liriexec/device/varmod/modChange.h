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
#ifndef LIRI_APPLICATION_MODIFIER_CHANGE_H_
#define LIRI_APPLICATION_MODIFIER_CHANGE_H_

#include "device/varmod/base.h"

class modChange : public VariableModifier {
	public:
		virtual void evaluate(QVariant* value, QStringList& arguments) {
			if (arguments.size() != 1) return;

			if (value->type() == QVariant::Int)
				value->setValue( value->value<int>() + arguments[0].toInt());
			else if (value->type() == QVariant::UInt) {
				int tmp = arguments[0].toInt();
				if (value->value<int>() + tmp < 0) {
					value->setValue((unsigned int)0);
				} else {
					value->setValue( value->value<unsigned int>() + tmp);
				}
			} else if (value->type() == QVariant::Double)
				value->setValue( value->value<double>() + arguments[0].toDouble());
		}
};

#endif
