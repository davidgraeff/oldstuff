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

#ifndef DRIVERHELPERALSA_H_
#define DRIVERHELPERALSA_H_
#include <alsa/asoundlib.h>
#include <alsa/hwdep.h>
#include <poll.h>

#include "driver-interface.h"

namespace liri {

	/**
	* \brief Convenience interface for liri alsa drivers
	*
	* A convenience class for implementing alsa soundcard drivers via alsa lib.
	* Please use the lirilog singleton object for logging errors or debugging output.
	* This class will open the device for you on init and close it again if destructed.
	*
	* \sa driver-interface, driver-interface-usb
	* \author David Gräff <david.graeff at cs.udo.edu>
	*/
class IDriverALSA : public IDriver {
public:
	IDriverALSA(DeviceSettings* settings);
	virtual ~IDriverALSA();
	virtual std::string getBlockingDriver();
private:
	template <typename T> T stringTo( std::string str );	
protected:
	snd_hwdep_t *hwdep;
	struct pollfd pollfd;

};

}

#endif
