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
#include "driver-interface-alsa.h"
#include <sstream> //for converting strings to int
#include <iostream>

namespace liri {
	
template <typename T>
T IDriverALSA::stringTo( std::string str ) {
	if (str.empty()) str = "0";
	std::istringstream isst (str);
	T zahl;
	isst >> zahl;
	return zahl;
}

IDriverALSA::IDriverALSA(DeviceSettings* settings) : IDriver(settings) {
	hwdep = 0;
}

IDriverALSA::~IDriverALSA() {
	snd_hwdep_close(hwdep);
}

std::string IDriverALSA::getBlockingDriver() {
	return std::string();
}

} //end namespace

