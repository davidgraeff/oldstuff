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
#ifndef LIRI_APPLICATION_MODIFIER_CHANGEVOLUME_H_
#define LIRI_APPLICATION_MODIFIER_CHANGEVOLUME_H_

#include "device/varmod/base.h"

class modChangeVolume : public VariableModifier {
public:
	template <class T>
	T setvar(T value, T threshold, T modvalue_under, T modevalue_upper, T min, T max) {
		value += ((value > threshold) ? modevalue_upper : modvalue_under);
		if (value < min) value = min;
		if (value > max) value = max;
		return value;
	}

	virtual void evaluate(QVariant* value, QStringList& arguments) {
		if (arguments.size() != 1) return;
		if (value->type() == QVariant::Double) {
			double v = value->value<double>();
			if (arguments[0] == QString::fromLatin1("+")) {
				value->setValue(setvar<double>(v, 0.2, 0.02, 0.1, 0.0, 1.0));
			} else if (arguments[0] == QString::fromLatin1("-")) {
				value->setValue(setvar<double>(v, 0.2, -0.02, -0.1, 0.0, 1.0));
			}
		} else
		if (value->type() == QVariant::Int) {
			int v = value->value<int>();
			if (arguments[0] == QString::fromLatin1("+")) {
				value->setValue(setvar<int>(v, 20, 2, 10, 0, 100));
			} else if (arguments[0] == QString::fromLatin1("-")) {
				value->setValue(setvar<int>(v, 20, -2, -10, 0, 100));
			}
		} else
		if (value->type() == QVariant::UInt) {
			int v = (int)value->value<unsigned int>();
			if (arguments[0] == QString::fromLatin1("+")) {
				value->setValue((unsigned int)setvar<int>(v, 20, 2, 10, 0, 100));
			} else if (arguments[0] == QString::fromLatin1("-")) {
				value->setValue((unsigned int)setvar<int>(v, 20, -2, -10, 0, 100));
			}
		}
	}
};

#endif
