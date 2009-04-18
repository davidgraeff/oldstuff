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
#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

Driver::Driver(DeviceSettings* settings) : liri::IDriverUSB(settings) {}

int Driver::init() {
	/* init base class */
	int res = usb_initdriver();
	if (res) return res;

	//(usb) input endpoint
	res = usb_getEndpoint(0, USB_ENDPOINT_TYPE_CONTROL, USB_ENDPOINT_IN);
	if (res) return res;

	memset(&buffer_dummy, 0, sizeof(buffer_dummy));

	/* three attemps to clear the device buffer */
	for (int i = 0; i < 3; ++i) {
		/* clear device buffer */
		res = usb_control_msg(usbdev_handle,
			USB_TYPE_VENDOR|USB_ENDPOINT_IN, SET_INFRABUFFER_EMPTY,
			0, 0, buffer_dummy, sizeof(buffer_dummy), 1000);

		if (res == -ETIMEDOUT) /* timeout */ {
			return LIRIERR_timeout;
		} else if (res == -EPIPE || res == -EIO) /* stall */ {
			if (usb_clear_halt(usbdev_handle, 0) < 0)
				return LIRIERR_resetendpoint;
		} else if (res == -EILSEQ) /* device usb protocoll violation, reset */ {
			if (usb_reset(usbdev_handle) < 0) {
				debug() << "igorplug driver: device reset failed";
				return LIRIERR_initdevice;
			} else {
				int res = usb_initdriver();
				if (res) return res;
			}
		} else if (res < 0) {
			debug() << "unhandled igorplug driver error: " << res;
			return LIRIERR_initdevice;
		} else break;
	}

	return LIRI_DEVICE_RUNNING;
}

liri::KeyCode Driver::listen(int timeout) {
	int readed;
	bool found = false;
	bool recovered = false;

	/* we have at least as many time as timeout specifies,
	we should use this to listen. usb_control_msg will
	return immediately even if no data is available.
	Therefore try it again until timeout */
	struct timeval curtime, endtime;
	gettimeofday(&endtime,0);
	curtime.tv_sec = timeout / 1000;
	curtime.tv_usec = 1000*(timeout % 1000);
	timeradd(&endtime, &curtime, &endtime);
	gettimeofday(&curtime,0);

	while (timercmp(&endtime, &curtime, >)) {
		gettimeofday(&curtime,0);

		/* sleep a little bit and read from the device */
		usleep(10 * 1000);
		memset(&buffer, 0, sizeof(buffer));
		readed = usb_control_msg(usbdev_handle,
			USB_TYPE_VENDOR|USB_ENDPOINT_IN, GET_INFRACODE,
			0, 0, buffer, sizeof(buffer), 1000);

		if (readed < 0) {
			/* timeouts are allowed, just try again */
			if (readed == -ETIMEDOUT) {
				continue;
			/* stall: try to recover before giving up */
			} else if ((readed == -EPIPE || readed == -EIO) && !recovered) {
				if (!usb_clear_halt(usbdev_handle, 0)) {
					debug() << "igorplug recovering from EPIPE/EIO";
					recovered = true;
					continue;
				} else {
					debug() << "igorplug recovering from EPIPE/EIO failed!";
				}
			/* protocoll violation, reset device and try again */
			} else if (readed == -EILSEQ && !recovered) {
				if (!usb_reset(usbdev_handle) && !usb_initdriver()) {
					debug() << "igorplug recovering from EILSEQ";
					recovered = true;
					continue;
				} else {
					debug() << "igorplug recovering from EILSEQ failed!";
				}
			/* other error or recover failed: return with the error no */
			} else {
				key.state = readed;
				return key;
			}
		} else {
			/* clear device buffer */
			memset(&buffer_dummy, 0, sizeof(buffer_dummy));
			usb_control_msg(usbdev_handle,
				USB_TYPE_VENDOR|USB_ENDPOINT_IN, SET_INFRABUFFER_EMPTY,
				0, 0, buffer_dummy, sizeof(buffer_dummy), 1000);
			/* just an ACK byte: ignore */
			if (readed < DEVICE_HEADERLEN ) continue;
			/* found something */
			found = true;
			break;
		}
	}

	if (!found) {
		key.state = LIRI_STATE_INVALID;
		return key;
	}

	/* determine the length of the data. The first received byte of an igorplug
	   device also propagate the size. Take the smaller one to be on the safe side */
	readed = std::min(readed, ((int)(unsigned char)buffer[0])+DEVICE_HEADERLEN);

	/* convert the buffer array to only contain 1=high, 0=low and 2=burst */
	//std::cout << "raw data: ";
	for (int c=DEVICE_HEADERLEN; c<readed; ++c) {
		if (((unsigned char)buffer[c]) > 100) buffer[c] = 2;
		else if (((unsigned char)buffer[c]) > 12) buffer[c] = 1;
		else buffer[c] = 0;
		//std::cout << (int)(unsigned char)buffer[c] << " ";
	}
	//std::cout << std::endl;

	// decode. Don't take the igorplug-device header consisting of three bytes into consideration
	if (!decode_infrared(key, buffer+(DEVICE_HEADERLEN*sizeof(char)), readed-DEVICE_HEADERLEN)) {
		key.state = LIRI_STATE_INVALID;
		return key;
	}

	// Infrared remotes aren't able to transmit in parallel: only one possible channel
	key.channel = 0;

	/* this receiver can only send pressed events */
	key.pressed = 1;

	/* valid */
	key.state = 1;

	return key;
}

