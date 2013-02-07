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
#include "liriexec/actions/ActionProgram.h"
#include <QProcess>
#include "config.h"
#include <QDebug>

ActionProgram::ActionProgram() : Action(Action::Program), timeout(3000), pipevar(0) {}
ActionProgram::~ActionProgram() {}

bool ActionProgram::work() {
	if (!workWithOutput || !pipevar) {
		if (!QProcess::startDetached(program)) {
			qDebug() << "ActionProgram::work(): Failed with " << program;
			//	<< QProcess().environment();
		} else {
			//qDebug() << "ActionProgram::work(): " << program;
		}
	} else {
		QProcess process;
		if (workingDirectory.size()) process.setWorkingDirectory(workingDirectory);
		process.start(program);
		process.waitForFinished(timeout);
		if (process.state() == QProcess::Running)
			process.terminate();
		QString src = QString::fromUtf8(process.readAllStandardOutput());
		if (pipevar->type() == QVariant::String)
			pipevar->setValue(src);
		else if (pipevar->type() == QVariant::Int)
			pipevar->setValue(src.toInt());
		else if (pipevar->type() == QVariant::UInt)
			pipevar->setValue(src.toUInt());
		else if (pipevar->type() == QVariant::Double)
			pipevar->setValue(src.toDouble());
		else if (pipevar->type() == QVariant::Bool)
			pipevar->setValue((bool)src.toInt());
	}
	emit executed(deviceinstance, LIRIOK_executed, program);
	return true;
}
