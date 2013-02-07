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
#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <kicon.h>

#include "config.h"

#include "mainwindow.h"

int main(int argc, char **argv)
{
	KAboutData aboutData("liri-editor-kde4",
		0,
		ki18n("Liri Editor"), //programname
		ABOUT_VERSION, //version
		ki18n(ABOUT_SUMMARY), //short Desc
		KAboutData::License_LGPL_V3, //license
		ki18n(ABOUT_COPYRIGHT), //copywrite statement
		ki18n("Add and modify remotes and application/desktop profiles"), //text
		"http://cerebro.webhop.net/liri", //homepage
		ABOUT_AUTHOR_EMAIL); //bugsemail address
	aboutData.addAuthor(ki18n(ABOUT_AUTHOR), ki18n("Lead developer"), ABOUT_AUTHOR_EMAIL);
	aboutData.setProgramIconName(QLatin1String("liri-controlcenter"));

	KCmdLineArgs::init( argc, argv, &aboutData );
	KApplication app;

	app.setWindowIcon(KIcon(QLatin1String("liri-controlcenter")));
	app.setQuitOnLastWindowClosed(true);

	/* self freeing object! */
	MainWindow* window = new MainWindow();
	window->show();

	return app.exec();
}
