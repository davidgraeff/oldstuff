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
#include "libusb-1.0/libusb.h"
#include <cstring>
#include <stdio.h>

libusb_device_handle *m_usbdevicehandle;
libusb_device *m_usbdevice;
struct libusb_endpoint_descriptor inEP;
struct libusb_endpoint_descriptor outEP;
struct libusb_config_descriptor ** config;
struct libusb_transfer* transfer;
int ifno;

unsigned char buffer[LIRI_KEYCODE_LENGTH];
char m_udi[100];
char m_usbSerialID[100];
int m_usbVendorID;
int m_usbProductID;
KeyCode key;

struct pollstr** open(const char* udi, const char* usbVendorID, const char* usbProductID,
		 const char* usbSerialID, const char* error_string)
{
	strncpy(m_udi, udi, 100);
	strncpy(m_usbSerialID, usbSerialID, 100);
	sscanf (usbVendorID,"%x",&m_usbVendorID);
	sscanf (usbProductID,"%x",&m_usbProductID);
	ifno = 0;
	config = 0;
	transfer = 0;

	key.keycodeLen = 2;
	/* set keycode bytes to zeros */
	memset(key.keycode, 0, sizeof(key.keycode));
	
	// discover devices
	libusb_device **list;
	struct libusb_device_descriptor dsc;
	ssize_t cnt = libusb_get_device_list(NULL, &list);
	ssize_t i = 0;
	int err = 0;
	if (cnt < 0) {
		error_string = "libusb_get_device_list failed";
		return 0;
	}

	for (i = 0; i < cnt; i++) {
		libusb_device *device = list[i];
		const int ret = libusb_get_device_descriptor(device, &dsc);
		if (ret>=0 && dsc.idVendor == m_usbVendorID && dsc.idProduct == m_usbProductID) {
			err = libusb_open(device, &m_usbdevicehandle);
			m_usbdevice = device;
			break;
		}
	}

	libusb_free_device_list(list, 1);
	if (err) {
		error_string = "libusb_open failed";
		return 0;
	}

	// Claim interfaces and get endpoints
	if (libusb_get_active_config_descriptor(m_usbdevice, config)==0)
	{
		struct libusb_interface_descriptor iface = config[0]->interface[0].altsetting[0];
		ifno = iface.bInterfaceNumber;
		if (libusb_claim_interface(m_usbdevicehandle, ifno) != 0)
		{
			error_string = "claim interface failed";
			libusb_close(m_usbdevicehandle);
			return 0;
		}
		inEP = iface.endpoint[0];
		outEP = iface.endpoint[1];
		if ((inEP.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) != LIBUSB_TRANSFER_TYPE_INTERRUPT) {
			error_string = "endepoint in type wrong";
			return 0;
		}
		if ((inEP.bEndpointAddress & LIBUSB_ENDPOINT_IN) ) {
			error_string = "endepoint in not in";
			return 0;
		}
		if ((outEP.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) != LIBUSB_TRANSFER_TYPE_INTERRUPT) {
			error_string = "endepoint out type wrong";
			return 0;
		}
		if ((outEP.bEndpointAddress & LIBUSB_ENDPOINT_OUT) ) {
			error_string = "endepoint out not out";
			return 0;
		}
	}

	// File descriptors
	return (struct pollstr**)libusb_get_pollfds(0);
}

void dataIn(struct libusb_transfer *transfer)
{
		/* ignore init bits and code without correct header */
	if (transfer->actual_length < 2 || buffer[0] != 20) {
		key.state = -1;
	} else {
		/* get channel */
		key.channel = (buffer[3] >> 4) & 0x0F;

		/* copy the two data bytes and erase first bit (togglebit) for each of them
		 * and also erase channel code that is encoded into the first 4 bits of the data byte */
		key.keycode[0] = buffer[1] & 0x0f;
		key.keycode[1] = buffer[2] & 0x7f;

		/* this receiver can only send one type of events: "Pressed" */
		key.pressed = 1;

		/* valid */
		key.state = 1;
	}
}

void init()
{
	static unsigned char init1[] = {0x80, 0x01, 0x00, 0x20, 0x14};
	static unsigned char init2[] = {0x80, 0x01, 0x00, 0x20, 0x14, 0x20, 0x20, 0x20};
	int transferred = 0;
	libusb_interrupt_transfer(m_usbdevicehandle, outEP.bEndpointAddress,
				init1, sizeof(init1), &transferred, 1000);
	libusb_interrupt_transfer(m_usbdevicehandle, outEP.bEndpointAddress,
				init2, sizeof(init2), &transferred, 1000);

	transfer = libusb_alloc_transfer(0);
	libusb_fill_interrupt_transfer(transfer, m_usbdevicehandle,
								   inEP.bEndpointAddress, buffer, LIRI_KEYCODE_LENGTH,
								   dataIn, 0, 0);
}

KeyCode activity(const char* cmd, const char* value)
{
	// libusb check for events in non-blocking mode
	libusb_handle_events_timeout(0, 0);

	return key;
}

void close()
{
	if (m_usbdevicehandle)
	{
		libusb_free_transfer(transfer);
		libusb_free_config_descriptor(*config);
		libusb_release_interface(m_usbdevicehandle, ifno);
		libusb_close(m_usbdevicehandle);
	}
}

