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

class BusConnection;
class DeviceListModel;
class TargetListModel;
class ExecutionListModel;
class MessagesWindow;
class DriverlistWindow;
class KAction;

class MainWindow : public KXmlGuiWindow
{
  Q_OBJECT
public:
    MainWindow(BusConnection* busconnection, QWidget *parent=0);
    ~MainWindow();

private:
	Ui::MainWindow ui;
	KAction* actionMessages;
	KAction* actionDriverlist;
	KAction* actionReloadRemote;
	KAction* actionReloadProfile;
	KAction* actionDevManShutdown;
	KAction* actionExecutionShutdown;
	KAction* actionExecutionStart;
	MessagesWindow* messageswindow;
	DriverlistWindow* driverlistwindow;
	BusConnection* busconnection;
	DeviceListModel* devicelistmodel;
	TargetListModel* targetlistmodel;
	ExecutionListModel* executionlistmodel;
	KIcon onlineIcon;
	KIcon offlineIcon;
	QString current_rid;
private Q_SLOTS:
	/* running detection: for dis/enabling menu buttons */
	void devicemanagerStateChanged(int state);
	void executionengineStateChanged(int state);

	// devicelist click/doubleclick
	void clicked ( const QModelIndex & index );
	void doubleClicked ( const QModelIndex & index );

	/* Menu */
	void btnReloadRemote();
	void btnReloadProfiles();
	void btnDeviceManagerShutdown();
	void btnExecutionEngineShutdown();
	void btnExecutionEngineStart();
	void btnMessages();
	void btnDriverlist();
	void closedMessages();
	void closedDriverlist();
};

#endif
