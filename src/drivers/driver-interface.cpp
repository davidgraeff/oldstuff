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
#include "driver-interface.h"

namespace liri
{
	
bool operator==(KeyCode const& a, KeyCode const& b)
{
	bool equal = (strcmp(a.receiver, b.receiver)==0);
	equal = equal && (a.keycodeLen == b.keycodeLen) && (memcmp(a.keycode, b.keycode, a.keycodeLen)==0);
	equal = equal && (a.channel == b.channel) && (a.pressed == b.pressed) && (a.state == b.state);
	return equal;
}

bool operator!=(KeyCode const& a, KeyCode const& b)
{
	return !(a==b);
}

IDriver::IDriver(DeviceSettings* settings) {
	internal_state = 0;
	this->settings = settings;
	
	//* set udii */
	memset(&key.receiver[0], 0, sizeof(key.receiver));
	strncpy(&key.receiver[0], settings->get(std::string("udi")).c_str(), sizeof(key.receiver)-1);
}


IDriver::~IDriver() {
	
}

bool IDriver::settingsChanged() {
	return true;
}

DeviceSettings& IDriver::debug() const {
	return *settings;
}

} //end namespace
