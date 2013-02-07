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
#include "fileformats/RemoteFile.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include "config.h"

RemoteFile::RemoteFile(const QString& uid) : DesktopFile(uid) {
	reload();
}

const QPair<QString, QString> RemoteFile::getSetting(int index) const {
	if (settings.size() <= index) return QPair<QString, QString>();
	return settings[index];
}

void RemoteFile::setSettingKey(int index, const QString& key) {
	if (settings.size() <= index) return;
	if (!key.size())
		settings.removeAt(index);
	else
		settings[index].first = key;
}

void RemoteFile::setSettingValue(int index, const QString& value) {
	if (settings.size() <= index) return;
	settings[index].second = value;
}

int RemoteFile::addSetting(const QString& key, const QString& value) {
	for (int i=0; i<settings.size(); i++) {
		if (settings[i].first == key) {
			settings[i].second = value;
			return i;
		}
	}

	settings.push_back(QPair<QString, QString>(key, value));
	return -1;
}

int RemoteFile::countSettings() const {
	return settings.size();
}

int RemoteFile::countKeys() const {
	return keylist.size();
}

QPair<QString, QString> RemoteFile::getKey(int index) {
	if (keylist.size() <= index) return QPair<QString, QString>(QString(),QString());
	return keylist[index];
}

int RemoteFile::addKey(const QString& keycode, const QString& keyname) {
	keylist.append(QPair<QString, QString>(keycode, keyname));
	return keylist.size()-1;
}

void RemoteFile::setKeyCode(int index, const QString& keycode){
	if (keylist.size() <= index) return;
	if (!keycode.size()) {
		keylist.removeAt(index);
	} else {
		keylist[index].first = keycode;
	}
}

void RemoteFile::setKeyName(int index, const QString& keyname){
	if (keylist.size() <= index) return;
	if (!keyname.size()) {
		keylist.removeAt(index);
	} else {
		keylist[index].second = keyname;
	}
}

int RemoteFile::posKeyCode(const QString& keycode) {
	for (int i=0; i<keylist.size(); i++) {
		if (keylist[i].first == keycode) return i;
	}
	return -1;
}

bool RemoteFile::containsKeyname(const QString& keyname) {
	for (int i=0; i<keylist.size(); i++) {
		if (keylist[i].second == keyname) return true;
	}
	return false;
}

int RemoteFile::getKeySubset() const {
	return keysubsetCache;
}

void RemoteFile::clear() {
	keylist.clear();
	settings.clear();
	keysubsetCache = 0;
	DesktopFile::clear();

}

const QString RemoteFile::path() const {
	return QLatin1String(LIRI_SYSTEM_REMOTES_DIR);
}

const QString RemoteFile::type() const {
	return QLatin1String(LIRI_FILEFORMAT_REMOTES);
}

double RemoteFile::minversion() const {
	return LIRI_FILEFORMAT_MIN;
}

double RemoteFile::maxversion() const {
	return LIRI_FILEFORMAT_MAX;
}


void RemoteFile::updateKeySubsetCache() {
	/* calculate subset cache */
	//desktop
	if (containsKeyname(QLatin1String("#SHOW_DESKTOP")) &&
		containsKeyname(QLatin1String("#FULLSCREEN")) &&
		containsKeyname(QLatin1String("#OK")))
		keysubsetCache |= RemoteFile::DesktopControl;

	//Cursor
	if (containsKeyname(QLatin1String("#UP")) && containsKeyname(QLatin1String("#LEFT"))
		&& containsKeyname(QLatin1String("#RIGHT")) && containsKeyname(QLatin1String("#DOWN")))
		keysubsetCache |= RemoteFile::Cursor;

	//MultimediaModeKeys
	if (containsKeyname(QLatin1String("@MUSIC")) && containsKeyname(QLatin1String("@DISC")))
		keysubsetCache |= RemoteFile::MultimediaModeKeys;

	//Multimedia
	if (containsKeyname(QLatin1String("#STOP")) && containsKeyname(QLatin1String("#PLAY"))
		&& containsKeyname(QLatin1String("#PAUSE")) && containsKeyname(QLatin1String("#NEXT"))
		&& containsKeyname(QLatin1String("#PREVIOUS")) && containsKeyname(QLatin1String("#VOL+"))
		&& containsKeyname(QLatin1String("#VOL-")))
		keysubsetCache |= RemoteFile::Multimedia;
}

void RemoteFile::readgroup(const QString&) {}

bool RemoteFile::readline(const QString& group, const QString& key, const QString&, const QString& value) {
	if (group == QLatin1String("Settings")) {
		settings.append(QPair<QString, QString>(key,value));
	} else if (group == QLatin1String("CompatibleReceivers")) {
		compatibleReceivers.append(QPair<QString, QString>(key,value));
	} else if (group == QLatin1String("Keys")) {
		addKey(key, value);
	}
	return true;
}

bool RemoteFile::save_private(QFile& inifile) {
	inifile.write("[Settings]\n");
	QList< QPair<QString, QString> >::iterator settingsIT;
	for ( settingsIT=settings.begin() ; settingsIT != settings.end(); settingsIT++ )
		inifile.write(settingsIT->first.toUtf8() + "=" + settingsIT->second.toUtf8() + "\n");

	inifile.write("\n");
	inifile.write("[CompatibleReceivers]\n");
	QList< QPair<QString, QString> >::iterator receiversIT;
	for ( receiversIT=compatibleReceivers.begin() ; receiversIT != compatibleReceivers.end(); receiversIT++ )
		inifile.write(receiversIT->first.toUtf8() + "=" + receiversIT->second.toUtf8() + "\n");

	inifile.write("\n");
	inifile.write("[Keys]\n");
	QList< QPair<QString, QString> >::iterator keysIT;
	for ( keysIT=keylist.begin() ; keysIT != keylist.end(); keysIT++ )
		inifile.write(keysIT->first.toAscii() + "=" + keysIT->second.toUtf8() + "\n");

	return true;
}

