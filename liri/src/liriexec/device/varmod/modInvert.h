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
#ifndef LIRI_APPLICATION_MODIFIER_INVERT_H_
#define LIRI_APPLICATION_MODIFIER_INVERT_H_

#include "device/varmod/base.h"

//invert if bool
class modInvert : public VariableModifier {
public:
	virtual void evaluate(QVariant* value, QStringList& arguments) {
		if (value->type() == QVariant::Bool)
			value->setValue(!value->value<bool>());
	}
};

#endif
