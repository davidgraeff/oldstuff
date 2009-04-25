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
#include "driver.h"

Driver::Driver(DeviceSettings* settings) : liri::IDriverUSB(settings) {}
char buffer[LIRI_KEYCODE_LENGTH];

int Driver::init() {
	/* init base class */
	int res = usb_initdriver();
	if (res) return res;

	//(usb) input endpoint
	res = usb_getEndpoint(0, USB_ENDPOINT_TYPE_INTERRUPT, USB_ENDPOINT_IN);
	if (res) return res;

	//(usb) output endpoint
	res = usb_getEndpoint(1, USB_ENDPOINT_TYPE_INTERRUPT, USB_ENDPOINT_OUT);
	if (res) return res;

	//(usb) initiate the device
	static char init1[] = {0x80, 0x01, 0x00, 0x20, 0x14};
	static char init2[] = {0x80, 0x01, 0x00, 0x20, 0x14, 0x20, 0x20, 0x20};
	res = usb_write_interrupt(&init1[0], sizeof(init1));
	if (res) return res;
	res = usb_write_interrupt(&init2[0], sizeof(init2));
	if (res) return res;

	key.keycodeLen = 2;
	/* set keycode bytes to zeros */
	memset(key.keycode, 0, sizeof(key.keycode));

	return LIRI_DEVICE_RUNNING;
}

liri::KeyCode Driver::listen(int timeout) {
	/* read from the device */
	readed = usb_read_interrupt( &(char*)buffer, LIRI_KEYCODE_LENGTH, timeout );

	if (readed < 0) { //igore timeouts
		key.state = readed;
		if (key.state == LIRIERR_timeout) key.state = 0;
	} else if (readed != 4 || buffer[0] != 14) {
		/* ignore init bits and code without correct header */
		key.state = 0;
	} else {
		/* get channel */
		key.channel = (buffer[3] >> 4) & 0x0F;

		/* copy two data bytes and erase first bit (togglebit) for each of them */
		key.keycode[0] = buffer[1] & 0x7f;
		key.keycode[1] = buffer[2] & 0x7f;

		/* erase channel code that is also encoded into the first data byte */
		key.keycode[0] -= (key.channel<<4);

		/* this receiver can only send pressed events */
		key.pressed = 1;

		/* valid */
		key.state = 1;
	}
	return key;
}
