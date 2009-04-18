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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <KXmlGuiWindow>
#include <kicon.h>
#include "ui_mainwindow.h"
#include <kcategorizedsortfilterproxymodel.h>

#include <QFileSystemWatcher>

class DaemonConnection;
class ExecutionConnection;
class DBusServiceList;
class ReceiverList;
class ActionsModel;

class MainWindow : public KXmlGuiWindow
{
  Q_OBJECT
public:
    MainWindow(QWidget *parent=0);
    ~MainWindow();

private:
	QFileSystemWatcher fwatcher;
	Ui::MainWindow *ui;
	inline void setupActions();
	void updateActionView();
	DaemonConnection* dconnection;
	ExecutionConnection* econnection;
	DBusServiceList* dlist;
	ReceiverList* receiverlist;
	ActionsModel* actionsmodel;
	KCategorizedSortFilterProxyModel* actionsmodelproxy;
	QWidget* logfile;
	QString logfilename;
	KIcon onlineIcon;
	KIcon offlineIcon;
private Q_SLOTS:
	/* running detection */
	void daemonStateChanged(int state);
	void executionStateChanged(int state);
	void receiverStateChanged(int instance, int state);
	void remoteStateChanged(int instance, int state);
	
	/* bus Commands */
	void btnReloadRemotes();
	void btnLiridShutdown();
	void btnReloadProfiles();
	void btnExecutionShutdown();

	/* Commands */
	void btnExecutionLogfile();
	void btnLiridLogfile();
	void btnClearLogfile();

	void halhelperLogfileChanged();
	
};

#endif
