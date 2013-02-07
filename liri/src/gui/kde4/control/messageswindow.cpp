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
#include "messageswindow.h"

#include <kstatusbar.h>
#include <kxmlguifactory.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>
#include <ktoolbar.h>
#include <ktoolinvocation.h>
#include <KDE/KLocale>

#include <QStringList>
#include <QByteArray>
#include <QHeaderView>
#include <QDir>

#include "config.h"
#include "businterconnect/BusConnection.h"

#include "MessagesModel.h"
#include "LogfileModel.h"


MessagesWindow::MessagesWindow(BusConnection* busconnection, QWidget *parent) : QMainWindow(parent) {
	// ui
	setupUi(this);
	listLogDevMan->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	listLogExecution->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	// watch files
	filenameDevMan = QLatin1String(LIRI_LOGFILE_DAEMON);
	filenameExecution = QDir::home().absoluteFilePath(QLatin1String(LIRI_LOGFILE_EXECUTION));
	fwatcher.addPath(filenameDevMan);
	fwatcher.addPath(filenameExecution);
	connect(&fwatcher, SIGNAL( fileChanged(const QString &) ), SLOT( logfileChanged(const QString &) ));

	// set labels
	lblLogfileDevMan->setText(i18n("Logfile: %1", filenameDevMan));
	lblLogfileExecution->setText(i18n("Logfile: %1", filenameExecution));

	//create models
	messagesmodel = new MessagesModel(busconnection);
	devman_logfilemodel = new LogfileModel(filenameDevMan);
	execution_logfilemodel = new LogfileModel(filenameExecution);
	connect(devman_logfilemodel, SIGNAL(saveFeedback(QFile::FileError)), SLOT(saveFeedbackDevMan(QFile::FileError)));
	connect(execution_logfilemodel, SIGNAL(saveFeedback(QFile::FileError)), SLOT(saveFeedbackExecution(QFile::FileError)));

	// connect button signals
	connect(btnClearMessages, SIGNAL(clicked(bool)), SLOT(clearMessages()));
	connect(btnClearDevMan, SIGNAL(clicked(bool)), SLOT(clearDeviceManagerLog()));
	connect(btnClearExecution, SIGNAL(clicked(bool)), SLOT(clearExecutionEngineLog()));
	connect(chkKeyPresses, SIGNAL(stateChanged(int)), SLOT(chkKeyListen(int)));

	/* models */
	messagesmodel->setListenKeys( ((chkKeyPresses->checkState()==Qt::Checked)?true:false) );
	listMessages->setModel(messagesmodel);
	listLogDevMan->setModel(devman_logfilemodel);
	listLogExecution->setModel(execution_logfilemodel);
}

MessagesWindow::~MessagesWindow() {
	delete messagesmodel;
	delete devman_logfilemodel;
	delete execution_logfilemodel;
	emit closednow();
}

void MessagesWindow::chkKeyListen(int state) {
	if (state == Qt::Checked) messagesmodel->setListenKeys(true);
	else messagesmodel->setListenKeys(false);
}

void MessagesWindow::clearDeviceManagerLog() {
	devman_logfilemodel->clearfile();
}

void MessagesWindow::clearExecutionEngineLog() {
	execution_logfilemodel->clearfile();
}

void MessagesWindow::clearMessages() {
	messagesmodel->clear();
}

void MessagesWindow::logfileChanged(const QString & path) {
	if (path == filenameDevMan)
		devman_logfilemodel->reload();
	else if (path == filenameExecution)
		execution_logfilemodel->reload();
}

void MessagesWindow::saveFeedbackDevMan(QFile::FileError err) {
	if (err == QFile::NoError)
		statusBar()->showMessage(i18n("Cleared device manager logfile"));
	else if ( err == QFile::PermissionsError )
		statusBar()->showMessage(i18n("You have no write permission for the device manager logfile"));
	else
		statusBar()->showMessage(i18n("Failed to clear device manager logfile"));
}

void MessagesWindow::saveFeedbackExecution(QFile::FileError err) {
	if (err == QFile::NoError)
		statusBar()->showMessage(i18n("Cleared execution engine logfile"));
	else
		statusBar()->showMessage(i18n("Failed to clear execution engine logfile"));
}
