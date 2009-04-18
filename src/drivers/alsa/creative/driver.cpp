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
#include <stdio.h>
#include <iostream>
#include <unistd.h> //usleep

Driver::Driver(DeviceSettings* settings) : liri::IDriverALSA(settings) {}

int Driver::init() {
	// search soundcards for supported remote device
	char device[36];
	snd_hwdep_info_t *info;
	int card, err;

	snd_hwdep_info_alloca(&info);
	card = -1;
	bool found = false;
	while (!found && snd_card_next(&card) >= 0 && card >= 0) {
		char ctl_name[20];
		snd_ctl_t *ctl;
		int deviceno;

		sprintf(ctl_name, "hw:CARD=%d", card);
		err = snd_ctl_open(&ctl, ctl_name, SND_CTL_NONBLOCK);
		if (err < 0)
			continue;
		deviceno = -1;
		while (snd_ctl_hwdep_next_device(ctl, &deviceno) >= 0 && deviceno >= 0) {
			snd_hwdep_info_set_device(info, deviceno);
			err = snd_ctl_hwdep_info(ctl, info);
			if (err >= 0 &&
			    snd_hwdep_info_get_iface(info) == SND_HWDEP_IFACE_SB_RC) {
				sprintf(device, "hw:CARD=%d,DEV=%d", card, deviceno);
				found = true;
				break;
			}
		}
		snd_ctl_close(ctl);
	}

	/* not found: abort */
	if (!found) {
		return LIRIERR_notFound;
	}

	/* open device */
	err = snd_hwdep_open(&hwdep, device, SND_HWDEP_OPEN_READ);
	if (err < 0) {
		std::cerr << "cannot open: " << device << " " << snd_strerror(err) << std::endl;
		return LIRIERR_opendevice;
	}
	snd_hwdep_info_alloca(&info);
	err = snd_hwdep_info(hwdep, info);
	if (err < 0) {
		snd_hwdep_close(hwdep);
		std::cerr << "cannot get hwdep info: " << device << " " << snd_strerror(err) << std::endl;
		return LIRIERR_endpoint_missing;
	}
	if (snd_hwdep_info_get_iface(info) != SND_HWDEP_IFACE_SB_RC) {
		std::cerr << "not a Sound Blaster remote control device: " << device << std::endl;
		snd_hwdep_close(hwdep);
		return LIRIERR_initdevice;
	}
	err = snd_hwdep_poll_descriptors(hwdep, &pollfd, 1);
	if (err < 0) {
		std::cerr << "cannot get file descriptor: " << snd_strerror(err) << std::endl;
		snd_hwdep_close(hwdep);
		return LIRIERR_endpoint_type;
	}
	if (err != 1) {
		std::cerr << "invalid number of file descriptors: " << err << snd_strerror(err) << std::endl;
		snd_hwdep_close(hwdep);
		return LIRIERR_initdevice;
	}

	/* all went well */
	return LIRI_DEVICE_RUNNING;
}

liri::KeyCode Driver::listen(int timeout) {
	/* read from the device */

	//unsigned short revents;
	//snd_hwdep_poll_descriptors_revents(hwdep, &pollfd, 1, &revents);

	if (poll(&pollfd, 1, timeout)) {
		readed = snd_hwdep_read(hwdep, &(key.keycode)[0], 1);
	} else {
		readed = LIRIERR_timeout;
	}

	if (readed < 0) { //igore timeouts
		key.state = readed;
		if (key.state == LIRIERR_timeout) key.state = 0;
	} else {
		/* length */
		key.keycodeLen = readed;

		/* get channel */
		key.channel = 0;

		/* pad the code with zeros (if shorter than sizeof(key.keycode)) */
		memset(key.keycode + key.keycodeLen, 0, sizeof(key.keycode) - key.keycodeLen);

		/* this receiver can only send pressed events */
		key.pressed = 1;

		/* valid */
		key.state = 1;
	}
	return key;
}
