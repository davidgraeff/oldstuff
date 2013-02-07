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
#include "fileformats/RemoteLoadProfilesFile.h"
#include "fileformats/DesktopFile.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include "config.h"

RemoteLoadProfilesFile::RemoteLoadProfilesFile(const QString& uid) {
	filename = getFilenameFromRemoteUid(uid);
	reload();
}

int RemoteLoadProfilesFile::state() {
	return pstate;
}

QString& RemoteLoadProfilesFile::getFilename() {
	return filename;
}

void RemoteLoadProfilesFile::clear() {
	QSet<QString>::clear();
	all = false;
	pstate = 0;
}

bool RemoteLoadProfilesFile::isAll() {
	return all;
}

void RemoteLoadProfilesFile::setAll(bool b) {
	all = b;
}

QSet<QString> RemoteLoadProfilesFile::controlled() {
	if (all) {
		QDir dphome = QDir::home(); dphome.cd(QLatin1String(LIRI_HOME_DESKTOPPROFILES_DIR));
		QStringList filter; filter << QLatin1String("*.desktop");
		QList<QString> t = dphome.entryList(filter, QDir::Readable | QDir::NoDotAndDotDot | QDir::Files);
		QSet<QString> uids;
		foreach (QString file, t)
			uids.insert(DesktopFile::getUidOfFilename(file));
		return uids;
	} else {
		return (*this);
	}
}

QString RemoteLoadProfilesFile::getFilenameFromRemoteUid(const QString& uid) {
	return (QString::fromLatin1("%1/"LIRI_HOME_REMOTE_LOAD_PROFILES_DIR"/%2.loader")).arg(QDir::homePath()).arg(uid);
}

void RemoteLoadProfilesFile::reload() {
	clear();
	
	/* don't do anything if remote.file does not exist */
	if (!QFile::exists(filename)) {
		qDebug() << filename << "invalid! Cannot determine used desktop profiles!";
		pstate = -1;
		return;
	}

	QFile loader(filename);
	loader.open(QIODevice::ReadOnly);

	while (!loader.atEnd()) {
		QString line = QString::fromLatin1(loader.readLine()).replace(QLatin1Char('\n'),QString());
		if (line == QLatin1String("ALL")) {
			all = true;
			QSet<QString>::clear();
			break;
		}
		insert(line);
	}

	loader.close();
	pstate = 1;
}

bool RemoteLoadProfilesFile::save() {
	if (!filename.size())
		return false;

	QFile loader(filename);
	loader.open(QIODevice::WriteOnly | QIODevice::Truncate);

	if (all) {
		loader.write("ALL\n");
	} else {
		foreach (QString line, (*this))
			loader.write(line.toUtf8() + "\n");
	}

	loader.close();
	return true;
}

