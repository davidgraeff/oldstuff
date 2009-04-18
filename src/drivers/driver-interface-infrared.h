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

#ifndef DRIVERHELPERINFRARED_H_
#define DRIVERHELPERINFRARED_H_

#include "driver-interface.h"

#define LIRI_INFRARED_HIGH 1
#define LIRI_INFRARED_LOW 0
#define LIRI_INFRARED_BURST 2

namespace liri {

	/**
	* \brief Convenience support interface for drivers that need to decode infrared signals
	*
	* A convenience class that decodes infrared code. You have to input an array of bytes
	* each representing a high(=1) or low(=0) bit. This class will try to guess which
	* protocoll might have been used and decode according to its decision.
	*
	* \sa driver-interface
	* \author David Gräff <david.graeff at cs.udo.edu>
	*/
class IDriverInfrared {
public:
	bool decode_infrared(liri::KeyCode& target, char* input_bits, int length);
};

}

#endif
