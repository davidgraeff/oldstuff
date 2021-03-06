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

#include "driver-common.h"

extern "C" {
	extern struct pollstr** open(const char* udi, const char* usbVendorID, const char* usbProductID, const char* usbSerialID, char* error_string);
	extern void init();
	extern KeyCode activity(const char* cmd, const char* value);
	extern void close();
};

#endif
