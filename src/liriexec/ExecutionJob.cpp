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
#include "ExecutionJob.h"
#include "liriexec/actions/Action.h"
#include "liriexec/actions/ActionGroup.h"
#include <QDebug>
#include "config.h"

ExecutionJob::ExecutionJob(ActionGroup* actiongroup) : actiongroup(actiongroup), _abort(false) {}

void ExecutionJob::abort() {
	mutexAbort.lock();
	_abort = true;
	mutexAbort.unlock();
}

void ExecutionJob::run() {
	bool repeatTick = false;
	while (1) {
		/* execute all actions of this actiongroup */
		for (int i=0; i < actiongroup->size(); ++i) {
			Action* action = actiongroup->at(i);
			if (!action->isConditionOK()) return;
			if (!action->work()) return;
		}
		/* if all went well, check if the actiongroup want to be repeated */
		if (actiongroup->repeat) {
			if (!repeatTick)
			// repeat event with an interval of the initial repeat tick
			{
				repeatTick = true;
				QThreadSleep::msleep(actiongroup->repeatlag);
			} else
			// repeat event with an interval of the normal repeat tick
			{
				QThreadSleep::msleep(actiongroup->repeattick);
			}
		} else return;
		mutexAbort.lock();
		if (_abort) { mutexAbort.unlock(); return; }
		mutexAbort.unlock();
	}
}

