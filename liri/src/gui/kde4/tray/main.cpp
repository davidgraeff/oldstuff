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
#include <QDebug>

#include <KAboutData>
#include <KCmdLineArgs>
#include <kicon.h>
#include <kstartupinfo.h>
#include "tray.h"
#include "myKUniqueApplication.h"

#include "businterconnect/BusConnection.h"

#include "config.h"

// should this program should run only once?
#define unique_app

int main(int argc, char **argv)
{
	/* about */
	KAboutData aboutData("liri-client-tray-kde4",
		0,
		ki18n("Liri Tray Icon"), //programname
		ABOUT_VERSION, //version
		ki18n(ABOUT_SUMMARY), //short Desc
		KAboutData::License_LGPL_V3, //license
		ki18n(ABOUT_COPYRIGHT), //copywrite statement
		ki18n("For notifications of the liri framework"), //text
		"http://cerebro.webhop.net/liri", //homepage
		ABOUT_AUTHOR_EMAIL); //bugsemail address
	aboutData.addAuthor(ki18n(ABOUT_AUTHOR), ki18n("Lead developer"), ABOUT_AUTHOR_EMAIL);
	aboutData.setProgramIconName(QLatin1String("liri-tray"));

	/* unique instance? */
	KCmdLineArgs::init( argc, argv, &aboutData );
	#ifdef unique_app
	myKUniqueApplication::addCmdLineOptions();
	myKUniqueApplication app;
	if (!myKUniqueApplication::start()) {
		fprintf(stderr, "Application is already running!\n");
		return 0;
	}
	#else
	KApplication app;
	#endif

	/* some app stuff */
	const KIcon icon(QLatin1String("liri-tray"));
	const KIcon icon_event(QLatin1String("liri-tray-event"));
	app.setWindowIcon(icon);
	app.setQuitOnLastWindowClosed(false);

	/* framework bus connection */
	BusConnection* connection = new BusConnection();

	TrayIconClient* trayiconclient = new TrayIconClient(icon, icon_event, connection);

	/* connect signals */
	#ifdef unique_app
	trayiconclient->connect( &app, SIGNAL ( anotherInstance() ), SLOT( anotherInstance() ));
	#endif

	/* app start completed now
	   signal this to the system to stop startup notification animations etc */
	KStartupInfo::appStarted();

	/* mainloop */
	app.exec();

	/* cleanup */
	delete connection;
	delete trayiconclient;

	return 0;
}
