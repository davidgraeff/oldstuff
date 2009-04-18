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
#include "DeviceSettings.h"

DeviceSettings& DeviceSettings::operator <<(std::string &os)
{
	debugText(os);
	return *this;
}

DeviceSettings::DeviceSettings() {
	pthread_mutex_init(&pending_mutex, NULL);
}

void DeviceSettings::debugText(const std::string& txt) {
	pthread_mutex_lock(&pending_mutex);
	debugs.push_back( txt );
	pthread_mutex_unlock(&pending_mutex);
}

std::string DeviceSettings::getNextDebugText() {
	pthread_mutex_lock(&pending_mutex);
	dbg = debugs.front();
	debugs.pop_front();
	pthread_mutex_unlock(&pending_mutex);
	return dbg;
}

bool DeviceSettings::hasDebugText() {
	return debugs.size();
}

DeviceSettings::~DeviceSettings() {
	pthread_mutex_destroy(&pending_mutex);
}

void DeviceSettings::set(const std::string& key, const std::string& value, DeviceSettingsPropagate dsp) {
	settings[key] = value;
	if (dsp == ForwardToDriver) {
		pthread_mutex_lock(&pending_mutex);
		pendingForDriver.insert( std::pair<std::string,std::string>(key,value) );
		pthread_mutex_unlock(&pending_mutex);
	} else if (dsp == ForwardToList) {
		pthread_mutex_lock(&pending_mutex);
		pendingForList.insert( std::pair<std::string,std::string>(key,value) );
		pthread_mutex_unlock(&pending_mutex);
	}
}

std::string DeviceSettings::get(const std::string& key) {
	std::map<std::string,std::string>::iterator it;
	it = settings.find(key);
	if (it != settings.end())
		return it->second;
	else
		return std::string();
}

bool DeviceSettings::has(const std::string& key) {
	std::map<std::string,std::string>::iterator it;
	it = settings.find(key);
	return (it != settings.end());
}

int DeviceSettings::pendingChangesForDriver() {
	pthread_mutex_lock(&pending_mutex);
	return pendingForDriver.size();
	pthread_mutex_unlock(&pending_mutex);
}

std::map<std::string, std::string> DeviceSettings::getPendingChangesForDriver() {
	pthread_mutex_lock(&pending_mutex);
	return pendingForDriver;
	pthread_mutex_unlock(&pending_mutex);
}

void DeviceSettings::releasePendingChangesForDriver() {
	pthread_mutex_lock(&pending_mutex);
	pendingForDriver.clear();
	pthread_mutex_unlock(&pending_mutex);
}

int DeviceSettings::pendingChangesForList() {
	pthread_mutex_lock(&pending_mutex);
	return pendingForList.size();
	pthread_mutex_unlock(&pending_mutex);
}

std::map<std::string, std::string> DeviceSettings::getPendingChangesForList() {
	pthread_mutex_lock(&pending_mutex);
	return pendingForList;
	pthread_mutex_unlock(&pending_mutex);
}

void DeviceSettings::releasePendingChangesForList() {
	pthread_mutex_lock(&pending_mutex);
	pendingForList.clear();
	pthread_mutex_unlock(&pending_mutex);
}

#ifdef QT_VERSION
bool DeviceSettings::has(const QString& key) {
	return has(key.toStdString());
}

QString DeviceSettings::get(const QString& key) {
	return QString::fromStdString(get(key.toStdString()));
}

void DeviceSettings::set(const QString& key, const QString& value, DeviceSettingsPropagate dsp) {
	set(key.toStdString(), value.toStdString(), dsp);
}

QMap<QString, QString> DeviceSettings::getSettings() {
	QMap<QString, QString> tmp;
	std::map<std::string, std::string>::iterator it;
	for ( it=settings.begin() ; it != settings.end(); it++ )
		tmp[ QString::fromStdString(it->first) ] = QString::fromStdString(it->second);
	return tmp;
}

#endif
