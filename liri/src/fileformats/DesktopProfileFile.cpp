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
#include "fileformats/DesktopProfileFile.h"
#include <QDebug>
#include <QDir>
#include "config.h"

DesktopProfileFile::DesktopProfileFile(const QString& uid) : DesktopFile(uid) {
	reload();
}

const QString DesktopProfileFile::path() const {
	return QDir::homePath() + QLatin1String("/") + QLatin1String(LIRI_HOME_DESKTOPPROFILES_DIR);
}

const QString DesktopProfileFile::type() const {
	return QLatin1String(LIRI_FILEFORMAT_DESKTOPPROFILES);
}

double DesktopProfileFile::minversion() const {
	return LIRI_FILEFORMAT_MIN;
}

double DesktopProfileFile::maxversion() const {
	return LIRI_FILEFORMAT_MAX;
}

void DesktopProfileFile::clear() {
	DesktopFile::clear();
	QSet<QString>::clear();
}

bool DesktopProfileFile::save_private(QFile& inifile) {
	inifile.write("[Uses]\n");
	foreach (const QString str, *(this))
		inifile.write(str.toUtf8() + "\n");
	return true;
}

void DesktopProfileFile::readgroup(const QString&) {

}

bool DesktopProfileFile::readline(const QString& group, const QString& key, const QString& keylocale, const QString& value) {
	Q_UNUSED(value);
	Q_UNUSED(keylocale);
	if (group == QLatin1String("Uses")) {
		insert(key);
	}
	return true;
}
