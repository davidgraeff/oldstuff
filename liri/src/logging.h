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
#ifndef LIRILOGGING_H_
#define LIRILOGGING_H_

#include <QApplication>
#include <QTime>
#include <QDate>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "config.h"

/* The log class. Singleton to open only one log stream.
   Special logic if the application is running in a terminal.
   */
static class singletonlog {
	public:
		singletonlog() {
			debugout = false;
			if (getenv("DEBUG")) {
				debugout = true;
			}
			if (getenv("TERM")) {
				debugout = true;
				terminalout = true;
				targetstream = &(std::cout);
			} else {
				terminalout = false;
				targetstream = new std::ofstream( LIRI_LOGFILE, std::ios::app );
			}
		}
		~singletonlog() {
			if (!terminalout) {
				delete targetstream;
				targetstream = 0;
			}
		}
		std::ostream* targetstream;
		bool terminalout;
		bool debugout;
} singletonlogInstance;

static std::string currentdate;

static void liriMessageOutput(QtMsgType type, const char *msg)
{
	if (!currentdate.size()) {
		currentdate = QDate::currentDate().toString(Qt::ISODate).toStdString();
	}
	
	switch (type) {
		case QtDebugMsg:
			if (singletonlogInstance.debugout)
				*(singletonlogInstance.targetstream) << "Debug (" << currentdate << " " << QTime::currentTime().toString(Qt::ISODate).toStdString() << "): " << msg << std::endl;
			break;
		case QtWarningMsg:
			*(singletonlogInstance.targetstream) << "Warning (" << currentdate << " " << QTime::currentTime().toString(Qt::ISODate).toStdString() << "): " << msg << std::endl;
			break;
		case QtCriticalMsg:
			*(singletonlogInstance.targetstream) << "Critical (" << currentdate << " " << QTime::currentTime().toString(Qt::ISODate).toStdString() << "): " << msg << std::endl;
			break;
		case QtFatalMsg:
			*(singletonlogInstance.targetstream) << "Fatal (" << currentdate << " " << QTime::currentTime().toString(Qt::ISODate).toStdString() << "): " << msg << std::endl;
			abort();
	}
}

#endif