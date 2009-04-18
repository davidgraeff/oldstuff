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
#include <fstream> //for logging
#include <string> //strings are more convenient that char arrays
#include <stdlib.h>
#include <dbus/dbus.h> //dbus
#include "config.h"
#include <unistd.h> // sleep

DBusConnection *busconn;
std::ofstream filelogstream;
std::string currenttime;
#define logstream filelogstream << currenttime << " "

bool initdbus();
void startservice();
void initlog();

//! The main function
/*!
  This program starts a lirid process if there is not already one running
*/
int main( int argc, char * argv[]) {
	/* init log */
	initlog();

	if (!initdbus())
		return EXIT_FAILURE;

	/* try to connect to the lirid process (may start the lirid process) */
	startservice();

	/* free system bus resources */
	dbus_connection_unref(busconn);

	/* close log */
	filelogstream.close();

	return EXIT_SUCCESS;
}

inline void initlog() {
	/* get current time */
	char buffer [80];
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strftime (buffer,80,"%Y.%m.%d %H:%M:%S",timeinfo);
	currenttime = buffer;

	filelogstream.open( LIRI_LOGFILE_CALLOUT, std::ios::trunc );
}

inline void startservice () {
	DBusError err;
	dbus_error_init(&err);

	// create a new method call and check for errors
	DBusMessage* msg = dbus_message_new_method_call(
	LIRI_DBUS_SERVICE_DEVMAN, LIRI_DBUS_OBJECT_RECEIVERS, LIRI_DBUS_INTERFACE_CONTROL, "start");

	if ( msg == 0 ) {
		logstream << "dbus_message_new_method_call failed" << std::endl;
		if (dbus_error_is_set(&err)) {
			logstream << err.message << std::endl;
			dbus_error_free(&err);
		}
	    return;
	}

	// send message and get a handle for a reply
	if (!dbus_connection_send_with_reply (busconn, msg, 0, -1)) {
		logstream << "dbus_connection_send_with_reply failed" << std::endl;
	}

	dbus_connection_flush (busconn);

	// wait for a response of the bus
	sleep(1);

	// free message
	dbus_message_unref(msg);
}

inline bool initdbus () {
	// initialise the error value
	DBusError err;
	dbus_error_init(&err);

	// connect to the DBUS system bus, and check for errors
	busconn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
	if (!busconn | dbus_error_is_set(&err)) {
		logstream << "Bus connection error: " << err.message << std::endl;
		dbus_error_free(&err);
		busconn = 0;
		return false;
	}

	// request name; only one instance of this program can run in parallel
	// DBUS_NAME_FLAG_DO_NOT_QUEUE
// 	if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER !=
// 		dbus_bus_request_name(busconn, LIRI_DBUS_SERVICE_CALLOUT, 0 , &err))
// 	{
// 		logstream << "Could not connect:" << std::endl;
// 		if (dbus_error_is_set(&err))
// 			logstream << err.message << std::endl;
// 		dbus_error_free(&err);
// 		dbus_connection_unref(busconn);
// 		busconn = 0;
// 		return false;
// 	}

	//bus connection ok
	return true;
}
