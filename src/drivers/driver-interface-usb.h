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
#ifndef DRIVERHELPERUSB_H_
#define DRIVERHELPERUSB_H_
#define USE_UNTESTED_LIBUSB_METHODS
#include <usb.h>

#include "driver-interface.h"

namespace liri {

	/**
	* \brief Convenience interface for liri usb drivers
	*
	* A convenience class for implementing usb drivers via libusb.
	* Please use the lirilog singleton object for logging errors or debugging output.
	* This class will open the device for you on init and close it again if destructed.
	*
	* \sa driver-interface, driver-interface-ieee1394
	* \author David Gräff <david.graeff at cs.udo.edu>
	*/
class IDriverUSB : public IDriver {
public:
	IDriverUSB(DeviceSettings* settings);
	virtual ~IDriverUSB();
	int usb_openDevice( const std::string& vendorid, const std::string& procuctid );
	int usb_openDevice( unsigned int vendorid, unsigned int productid );
	int usb_getEndpoint( unsigned int endpointnum, int type, int direction, usb_endpoint_descriptor** target=0);
	int usb_read_interrupt(char* msg, int len, int timeout, usb_endpoint_descriptor *endpoint=0);
	int usb_write_interrupt(char* msg, int len, usb_endpoint_descriptor *endpoint=0);
	int usb_endpointType(usb_endpoint_descriptor *ep);
	int usb_endpointDirection(usb_endpoint_descriptor *ep);
	/* implement */
	virtual std::string getBlockingDriver();
	int usb_initdriver();
	
private:
	bool claimed;
	template <typename T> T stringTo( std::string str );
protected:
	struct usb_device *usbdev;
	struct usb_dev_handle *usbdev_handle;
	struct usb_endpoint_descriptor *usbdev_ep_in;
	struct usb_endpoint_descriptor *usbdev_ep_out;
};

}

#endif

