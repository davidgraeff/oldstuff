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
#ifndef DRIVER_H_
#define DRIVER_H_

#define SET_INFRABUFFER_EMPTY   1
#define GET_INFRACODE	   2
#define DEVICE_BUFLEN	   36
/* Igor's firmware cannot record bursts longer than 36. */
/** Header at the beginning of the device's buffer:
	unsigned char data_length
	unsigned char data_start    (!=0 means ring-buffer overrun)
	unsigned char counter       (incremented by each burst)
**/
#define DEVICE_HEADERLEN	3
/* ignore the first 4 bytes, mostly a start bit and a toggle it */
#define IRCODE_HEADERLEN	4

#include "driver-interface-usb.h"
#include "driver-interface-infrared.h"

class Driver : public liri::IDriverUSB, public liri::IDriverInfrared {
public:
	Driver(DeviceSettings* settings);
	inline int init();
	inline liri::KeyCode listen(int timeout);
private:
	char buffer[DEVICE_BUFLEN];
	char buffer_dummy[DEVICE_BUFLEN];
};

LIRIDRIVER_OPENDL(Driver)

#endif
