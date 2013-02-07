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
#ifndef MESSAGESWINDOW_H
#define MESSAGESWINDOW_H
#include <QDialog>
#include "ui_messageswindow.h"
#include <QFileSystemWatcher>
#include <QFile>
#include <QMainWindow>

class BusConnection;
class MessagesModel;
class LogfileModel;

class MessagesWindow : public QMainWindow, public Ui::MessagesWindow
{
  Q_OBJECT
public:
    MessagesWindow(BusConnection* busconnection, QWidget *parent=0);
    ~MessagesWindow();

private:
	QFileSystemWatcher fwatcher;
	QString filenameDevMan;
	QString filenameExecution;
	MessagesModel* messagesmodel;
	LogfileModel* devman_logfilemodel;
	LogfileModel* execution_logfilemodel;
private Q_SLOTS:
	/* Commands */
	void clearDeviceManagerLog();
	void clearExecutionEngineLog();
	void clearMessages();

	// to react on signals from QFileSystemWatcher
	void logfileChanged(const QString & path);

	// logfileModel signals
	void saveFeedbackDevMan(QFile::FileError);
	void saveFeedbackExecution(QFile::FileError);

	// gui
	void chkKeyListen(int state);

Q_SIGNALS:
	void closednow();
};

#endif
