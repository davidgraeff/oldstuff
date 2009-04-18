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
#include <KDE/KLocale>
#include "config.h"
#include "messages.h"

trLiriClass::trLiriClass() {
	msgs[LIRI_UNKNOWN] = i18n("Unknown");
	msgs[LIRI_RUNNING] = i18n("Running");
	msgs[LIRI_DEVICE_OFFLINE] = i18n("Offline/Disconnected");
	msgs[LIRI_DEVICE_INIT] = i18n("Opening...");
	msgs[LIRI_DEVICE_RUNNING] = i18n("Running");
	msgs[LIRI_DEVICE_CANCEL] = i18n("Closing...");
	msgs[LIRI_REMOTE_LOADED] = i18n("Remote loaded");
	msgs[LIRI_REMOTE_NO] = i18n("No remote");
	msgs[LIRI_REMOTE_RELOAD] = i18n("Reload remote");
	msgs[LIRI_REMOTE_RELOADED] = i18n("Remote reloaded");
	msgs[LIRI_REMOTE_UNLOADED] = i18n("Remote unloaded");
	msgs[LIRIOK_executed] = i18n("Executed successfully");
	msgs[LIRIERR_permission] = i18n("Permission denied");
	msgs[LIRIERR_filename] = i18n("Invalid filename");
	msgs[LIRIERR_opendriver] = i18n("Open driver failed");
	msgs[LIRIERR_symbolCreate] = i18n("Dlopen Symbol Create invalid");
	msgs[LIRIERR_symbolDestroy] = i18n("Dlopen Symbol Destroy invalid");
	msgs[LIRIERR_notFound] = i18n("Not found");
	msgs[LIRIERR_opendevice] = i18n("Open device failed");
	msgs[LIRIERR_endpoint_type] = i18n("Endpoint type invalid");
	msgs[LIRIERR_endpoint_direction] = i18n("Endpoint direction invalid");
	msgs[LIRIERR_endpoint_missing] = i18n("Endpoint invalid");
	msgs[LIRIERR_resetendpoint] = i18n("Endpoint reset failed");
	msgs[LIRIERR_initdevice] = i18n("Initialise failure");
	msgs[LIRIERR_claiming] = i18n("Device claiming failed");
	msgs[LIRIERR_writing] = i18n("Device writing failed");
	msgs[LIRIERR_reading] = i18n("Device reading failed");
	msgs[LIRIERR_timeout] = i18n("Device timeout");
	msgs[LIRIERR_settings] = i18n("Illegal settings");
	msgs[LIRIERR_noservices] = i18n("No services found");
	msgs[LIRIERR_autostart] = i18n("Service autostart failed");
	msgs[LIRIERR_changebusinstance] = i18n("Failed to change service instance");
	msgs[LIRIERR_noservicewithnumber] = i18n("Failed to change to service instance with that number");
}

QString trLiriClass::msg(int state) {
	if (msgs.contains(state))
		return msgs[state];
	else
		return QString();
}