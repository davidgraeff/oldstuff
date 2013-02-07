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
#ifndef LIRI_EXECUTEJOB_H_
#define LIRI_EXECUTEJOB_H_

#include <QThread>
#include <QObject>
#include <QMutex>
#include <QRunnable>

class ActionGroup;

class QThreadSleep : public QThread {
	public:
		static void msleep(unsigned long msecs) {
			QThread::msleep(msecs);
		}
};

class ExecutionJob: public QObject, public QRunnable {
	Q_OBJECT
	public:
		ExecutionJob(ActionGroup* actiongroup);
		void run();
	public Q_SLOTS:
		void abort();
	private:
		ActionGroup* actiongroup;
		bool _abort;
		QMutex mutexAbort;
};

#endif
