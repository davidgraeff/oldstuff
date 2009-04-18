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
#ifndef DEVICESETTINGS_H_
#define DEVICESETTINGS_H_

#include <string>
#include <map>
#include <pthread.h>
#include <deque>
#include <sstream>

#ifdef SETTINGS_WITH_QT
#include <QString>
#include <QMap>
#endif

enum DeviceSettingsPropagate {
	NoForward,
	ForwardToDriver,
	ForwardToList
};

class DeviceSettings : public std::stringstream {
	public:
		DeviceSettings& operator <<(std::string &os);
		
		DeviceSettings();
		~DeviceSettings();
		void set( const std::string&, const std::string&, DeviceSettingsPropagate = NoForward );
		std::string get(const std::string&);
		bool has(const std::string&);

		/* debug texts from driver->liri framework: thread safe methods */
		void debugText(const std::string& txt);
		std::string getNextDebugText();
		bool hasDebugText();

		/* pendingChangesForDriver: thread safe methods */
		int pendingChangesForDriver();
		std::map<std::string, std::string> getPendingChangesForDriver();
		void releasePendingChangesForDriver();

		/* pendingChangesForList: thread safe methods */
		int pendingChangesForList();
		std::map<std::string, std::string> getPendingChangesForList();
		void releasePendingChangesForList();
		
		/* QT convenience functions for lirid - not compiled into pure c++ drivers */
		#ifdef SETTINGS_WITH_QT
		bool has(const QString&);
		QString get(const QString&);
		void set(const QString&, const QString&, DeviceSettingsPropagate = NoForward );
		QMap<QString, QString> getSettings(); //expensive conversion!
		#endif
	private:
		std::deque<std::string> debugs;
		std::string dbg;
		std::map<std::string, std::string> settings;
		std::map<std::string, std::string> pendingForDriver;
		std::map<std::string, std::string> pendingForList;
		pthread_mutex_t pending_mutex;
};
#endif