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

#define LIRI_KEYCODE_LENGTH 50
#include <cstring>

struct pollstr
{
	int fd;
	short events; // from <poll.h>; POLLIN, POLLOUT
};

struct KeyCode
{
	//unique string for device
	char receiver[250];
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

#endif