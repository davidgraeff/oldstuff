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
#ifndef INTERFACEDRIVER_H_
#define INTERFACEDRIVER_H_

#include "config.h"
#include "lirid/device/DeviceSettings.h" //access to settings (eg product_id, vendor_id etc)
#include <errno.h> //ETIMEDOUT
#include <cstring> //memset
#include <string>
#include <map>

#define LIRI_KEYCODE_LENGTH 16
#define LIRI_STATE_INVALID 0

/*! \file driver-interface.h
Interface for liri drivers.
*/

namespace liri
{

/**
* \brief Represence a raw key code
*
* Contains information about the sending receiver, the raw key code,
* the key state (pressed=1/released=0) and the remote channel.
* Only valid if "valid" is 1.
*
* Additional information:
* SSE2 instructions get performance advantages for 16-Byte aligned data,
* therefore this structs size is a divider of 16.
* receiver (4*16) + keycode (1*16) + channel+pressed+valid+keycodeLen (4*4=16)
*
*/
struct KeyCode
{
	//unique string for device
	char receiver[64];
	//unique key code
	char keycode[LIRI_KEYCODE_LENGTH];
	//received bytes for keycode
	unsigned int keycodeLen;
	//remote channel. if one receiver can handle several remotes
	unsigned int channel;
	//keyDown oder keyUp. Most Receivers will only emit repeated "KeyDown" events.
	//the main process will in that case emit a "KeyUp" event after a time without
	//any key event. Might be other values then 1 and 0 if the device supports that.
	int pressed;
	//valid event = 1, invalid <= 0
	int state;
};

bool operator==(KeyCode const& a, KeyCode const& b);
bool operator!=(KeyCode const& a, KeyCode const& b);

/**
 * \brief Interface for liri drivers
 *
 * You may implement this interface directly, but it is recommened
 * to use one of the convenience classes "driver-interface-usb" (usb) or
 * "driver-interface-ieee1394" (firewire) to write a driver for liri.
 * Please use the lirilog singleton object for logging errors or debugging output.
 *
 * \sa driver-interface-usb, driver-interface-ieee1394
 * \author David Gräff <david.graeff at cs.udo.edu>
 */
class IDriver
{
public:
	/**
	* \brief Debug messages go here
	*
	* Just stream your debug messages to this method.
	*
	* Example: debug() << "An important message" << "another one";
	*
	* \return The devicesettings object
	*/
	DeviceSettings& debug() const;
	/**
	* \brief Initialise a driver instance
	*
	* Because this function is called from another lirid thread you may block the process.
	* If the device is not intialized within a given time frame (~30sec), lirid will terminate
	* the initializing thread and unload the driver.
	*
	* This function is implemented in all convenience classes.
	* If you reimplement init() call the function for your convenience class, too.
	*
	* \return A negative number indicating an error or 0 if all went well
	*/
	virtual int init() = 0;
	/**
	 * \brief apply a setting
	 *
	 * Settings like "drivername", "driverversion" or "driverauthors" are readonly
	 * and therefore are not propagated to this function.
	 * The purpose of this function is to allow to adjust settings necessary for communicating with the
	 * device. If you want to reinitalise your device after this method just set internal_state to zero.
	 *
	 * Well known keys are: "drivername", "driverversion", "devicename", "productid",
	 * "vendorid", "driverauthors", "protocoll"
	 * 
	 * \param key settings key
	 * \param value new value for the setting referenced by key
	 * \return True if the device acknowledges the setting otherwise
	 * \return those are still marked as changed when this function is called again.
	 */
	virtual bool settingsChanged();
	/**
	 * \brief Listening for incoming data
	 *
	 * Listen to the device. Because this function is called from another lirid thread
	 * (in a loop) you may block until you have received input from your device or a timeout occured.
	 * You must not exceed 5 seconds, due to the cancel flag in the lirid process that has
	 * to be evaluated regulary. You may use the timeout proposal, you even have to use that
	 * if the device does not support key release events.
	 *
	 * \param timeout A proposed timeout to wait for input from the device. To optimize the
	 * process execution, take that value.
	 */
	virtual KeyCode listen(int timeout) = 0;

	/**
	 * \brief Error result of device claiming
	 *
	 * If the device claiming failed this function should return the driver that is
	 * blocking. If you use the convenience classes "driver-interface-usb" (usb) or
	 * "driver-interface-ieee1394" (firewire) this will already be implemented for you.
	 *
	 * \sa driver-interface-usb, driver-interface-ieee1394
	 */
	virtual std::string getBlockingDriver() = 0;
	/**
	* \brief get internal state integer
	*
	* For internal use only.
	*
	* \return internal state integer
	*/
	int getInternalState();
	IDriver(DeviceSettings* settings);
	virtual ~IDriver();
protected:
	/**
	* \brief "Decoded key" buffer
	*
	* To not always reallocate the key buffer in every call to listen, you may use
	* this one. 
	*
	* \sa driver-interface-usb, driver-interface-ieee1394, KeyCode
	*/
	liri::KeyCode key;
	/**
	* \brief Settings object
	*
	* Contains all settings and paramters for this device. You do not have to poll
	* this object if you want to know about changed settings. The lirid thread that
	* is responsible for the listing to the device (calls listen) will call settingsChanged
	* on changed settings. Latency therefore depends on the timeout that your listen
	* implementation uses.
	*
	* \sa settingsChanged
	*/
	DeviceSettings* settings;
	/**
	* \brief Internal state
	*
	* You may use this integer for what ever you want as long as you do not set it to one of the
	* special values listed below.
	*
	* Special values:
	* If 0 (or set to 0 within the listen thread) lirid will call your init() implementation.
	* if 1-100 lirid will interpret that value as status update of your initialisation.
	*
	* \sa settingsChanged
	*/
	int internal_state;
};

} //end namespace liri

/**
 * Include this macro in your driver like this: LIRIDRIVER_OPENDL(mydrivername).
 * This is necessary to use your driver with dlopen.
 *
 */
#define LIRIDRIVER_OPENDL(CLASSNAME) \
	extern "C" liri::IDriver* create(DeviceSettings* settings) { return new CLASSNAME(settings); } \
	extern "C" void destroy(liri::IDriver* p) { delete p; }

#endif /*INTERFACEDRIVER_H_*/
