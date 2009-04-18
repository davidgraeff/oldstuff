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
#include "driver-interface-usb.h"
#include <sstream> //for converting strings to int
#include <iostream>

namespace liri {

template <typename T>
T IDriverUSB::stringTo( std::string str ) {
	if (str.empty()) str = "0";
	std::istringstream isst (str);
	T zahl;
	isst >> zahl;
	return zahl;
}

IDriverUSB::IDriverUSB(DeviceSettings* settings) : IDriver(settings) {
	usbdev_ep_in = 0;
	usbdev_ep_out = 0;
	usbdev_handle = 0;
	claimed = false;
}

IDriverUSB::~IDriverUSB() {
	if (usbdev_handle)
	{
		usb_close(usbdev_handle);
		usbdev_handle = NULL;
		claimed = false;
	}
}

int IDriverUSB::usb_initdriver() {
	/* check for necessary data */
	if (!settings->has("usb_device.vendor_id") ||
		!settings->has("usb_device.product_id"))
		return LIRIERR_notFound;

	/* open usb device */
	int res = usb_openDevice(settings->get("usb_device.vendor_id"), settings->get("usb_device.product_id"));
	if (res) return res;

	/* no errors */
	return 0;
}

std::string IDriverUSB::getBlockingDriver() {
	char blocking_driver[40];
	memset(&blocking_driver, 0, sizeof(blocking_driver));
	#ifdef LIBUSB_HAS_GET_DRIVER_NP
	usb_get_driver_np(usbdev_handle, 0, &blocking_driver[0], sizeof(blocking_driver));
	#endif
	return blocking_driver;
}

int IDriverUSB::usb_openDevice( const std::string& vendorid, const std::string& procuctid ) {
	return usb_openDevice( stringTo<unsigned int>(vendorid), stringTo<unsigned int>(procuctid) );
}

int IDriverUSB::usb_openDevice( unsigned int vendorid, unsigned int productid ) {
	struct usb_bus *usb_bus;
	struct usb_device *dev;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next)
		for (dev = usb_bus->devices; dev; dev = dev->next)
		{
			if ( (dev->descriptor.idVendor == vendorid) &&
				(dev->descriptor.idProduct == productid) )
			{
				usbdev = dev;
				usbdev_handle = usb_open(dev);
				return 0;
			}
		}

	return LIRIERR_notFound;
}

int IDriverUSB::usb_endpointType(usb_endpoint_descriptor *ep) {
	if ( ep == NULL ) return LIRIERR_endpoint_type;
	return ep->bEndpointAddress & USB_ENDPOINT_TYPE_MASK;
}

int IDriverUSB::usb_endpointDirection(usb_endpoint_descriptor *ep) {
	if ( ep == NULL ) return LIRIERR_endpoint_direction;
	return ep->bEndpointAddress & USB_ENDPOINT_DIR_MASK;
}

int IDriverUSB::usb_getEndpoint( unsigned int endpointnum, int type, int direction,
								 usb_endpoint_descriptor** target) {

	//device not valid
	if ( !usbdev || !usbdev_handle )
		return LIRIERR_notFound;

	//convert strings to integers
	unsigned int configurationInt = stringTo<unsigned int>(settings->get("usb_device.configuration"));
	unsigned int interfaceInt = stringTo<unsigned int>(settings->get("usb_device.interface"));
	int altsettingInt = stringTo<int>(settings->get("usb_device.altsetting"));

	//if the device interface is not yet claimed, claim it
	if (!claimed) {
		if (usb_claim_interface(usbdev_handle, interfaceInt) != 0) {
			settings->set("driver.blocking", getBlockingDriver(), ForwardToList);
			return LIRIERR_claiming;
		} else
			claimed = true;
	}

	//number of configurations
	if ( usbdev->descriptor.bNumConfigurations < configurationInt )
		return LIRIERR_settings;

	//number of interfaces
	if ( usbdev->config[configurationInt].bNumInterfaces < interfaceInt )
		return LIRIERR_settings;

	//number of altsettingInt
	if ( usbdev->config[configurationInt].interface[interfaceInt].num_altsetting
		< altsettingInt )
		return LIRIERR_settings;

	//number of endpoints
	if ( usbdev->config[configurationInt].interface[interfaceInt].
		 altsetting[altsettingInt].bNumEndpoints < endpointnum )
		return LIRIERR_settings;

	//check if the type and direction of this endpoint is correct
	int bEndpointAddress = usbdev->config[configurationInt].interface[interfaceInt].
		altsetting[altsettingInt].endpoint[endpointnum].bEndpointAddress;
	int bmAttributes = usbdev->config[configurationInt].interface[interfaceInt].
		altsetting[altsettingInt].endpoint[endpointnum].bmAttributes;

	if ((bmAttributes & USB_ENDPOINT_TYPE_MASK) != type)
		return LIRIERR_endpoint_type;
	if ((bEndpointAddress & USB_ENDPOINT_DIR_MASK) != direction)
		return LIRIERR_endpoint_direction;

	//return pointer to endpoint
	if (target == 0) {
		if (direction == USB_ENDPOINT_IN)
			target = &usbdev_ep_in;
		else if (direction == USB_ENDPOINT_OUT)
			target = &usbdev_ep_out;
		else
			return LIRIERR_settings;
	}

	*target = &(usbdev->config[configurationInt].interface[interfaceInt].
		altsetting[altsettingInt].endpoint[endpointnum]);

	return 0;
}

int IDriverUSB::usb_read_interrupt(char* msg, int len, int timeout, usb_endpoint_descriptor *endpoint) {
	if (!usbdev_handle)
		return LIRIERR_notFound;

	if (!endpoint)
		endpoint = usbdev_ep_in;

	if (!endpoint)
		return LIRIERR_endpoint_missing;

	int res = usb_interrupt_read(usbdev_handle, endpoint->bEndpointAddress,
				msg, len, timeout);

	if (res == -ETIMEDOUT)
		return LIRIERR_timeout;
	else if (res < 0)
		return LIRIERR_reading;
	else
		return res;
}

int IDriverUSB::usb_write_interrupt(char* msg, int len, usb_endpoint_descriptor *endpoint) {
	if (!usbdev_handle)
		return LIRIERR_notFound;

	if (!endpoint)
		endpoint = usbdev_ep_out;

	if (!endpoint)
		return LIRIERR_endpoint_missing;

	int res = usb_interrupt_write(usbdev_handle, endpoint->bEndpointAddress,
				msg, len, 100);

	if (res != len)
		return LIRIERR_writing;
	else
		return 0;
}

} //end namespace

