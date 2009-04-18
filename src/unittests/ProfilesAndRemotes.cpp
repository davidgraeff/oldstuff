#include "profiles_remotes/RemoteFile.h"
#include "profiles_remotes/DesktopProfile.h"
#include "profiles_remotes/ApplicationProfile.h"

#include <QString>
#include <QDir>

using liri;

int main(int argc, char* argv[]) {
	/* open and save */
	QDir path = QDir::current ();
	RemoteFile* re = new RemoteFile(path.absoluteFilePath(QLatin1String("re.desktop")));
	DesktopProfile* dp = new DesktopProfile(path.absoluteFilePath(QLatin1String("re.desktop")));
	ApplicationProfile* ap = new ApplicationProfile(path.absoluteFilePath(QLatin1String("re.desktop")));

	re->setFilename(path.absoluteFilePath(QLatin1String("re2.desktop")));
	dp->setFilename(path.absoluteFilePath(QLatin1String("dp2.desktop")));
	ap->setFilename(path.absoluteFilePath(QLatin1String("ap2.desktop")));

	re->saveDesktopFile();
	dp->saveDesktopFile();
	ap->saveDesktopFile();

	delete re;
	delete ap;
	delete dp;
	
	/* compare */

	return 0;
}