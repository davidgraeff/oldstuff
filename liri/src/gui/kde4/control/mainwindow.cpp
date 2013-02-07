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
#include "mainwindow.h"

#include <kstatusbar.h>
#include <kxmlguifactory.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>
#include <ktoolbar.h>
#include <kcategorydrawer.h>

#include <QProcess>
#include <QStringList>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPointer>

#include <KDE/KLocale>
#include <ktoolinvocation.h>

#include "config.h"
#include "businterconnect/BusConnection.h"
#include "businterconnect/Control_DevManager_dbusproxy.h"
#include "businterconnect/Control_Execution_dbusproxy.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"
#include "businterconnect/Receiver_Execution_dbusproxy.h"

#include "DeviceListModel.h"
#include "DriverListModel.h"
#include "TargetListModel.h"
#include "ExecutionListModel.h"
#include "messageswindow.h"
#include "driverlistwindow.h"
#include "selectremotewindow.h"

MainWindow::MainWindow(BusConnection* busconnection, QWidget *parent) :
	KXmlGuiWindow(parent), busconnection(busconnection) {

	// create the user interface, the parent widget is "widget"
	QWidget *widget = new QWidget( this );
	ui.setupUi(widget);
	setCentralWidget( widget );
	KStandardAction::quit(qApp, SLOT(closeAllWindows()), actionCollection());
	//KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());
	actionMessages = new KAction(this);
	actionMessages->setText(i18n("Messages"));
	actionMessages->setIcon(KIcon(QLatin1String("view-list-text")));
	actionMessages->setShortcut(Qt::CTRL+Qt::Key_M);
	actionCollection()->addAction(QLatin1String("messages"), actionMessages);
	connect(actionMessages, SIGNAL(triggered(bool)), SLOT(btnMessages()));
	actionDriverlist = new KAction(this);
	actionDriverlist->setText(i18n("Installed drivers"));
	actionDriverlist->setIcon(KIcon(QLatin1String("view-list-text")));
	actionDriverlist->setShortcut(Qt::CTRL+Qt::Key_I);
	actionCollection()->addAction(QLatin1String("driverlist"), actionDriverlist);
	connect(actionDriverlist, SIGNAL(triggered(bool)), SLOT(btnDriverlist()));
	actionReloadRemote = new KAction(this);
	actionReloadRemote->setText(i18n("Reload Remote"));
	actionReloadRemote->setIcon(KIcon(QLatin1String("view-refresh")));
	actionReloadRemote->setShortcut(Qt::CTRL+Qt::Key_R);
	actionCollection()->addAction(QLatin1String("reloadRemote"), actionReloadRemote);
	connect(actionReloadRemote, SIGNAL(triggered(bool)), SLOT(btnReloadRemote()));
	actionReloadProfile = new KAction(this);
	actionReloadProfile->setText(i18n("Reload Profiles"));
	actionReloadProfile->setIcon(KIcon(QLatin1String("view-refresh")));
	actionReloadProfile->setShortcut(Qt::CTRL+Qt::Key_P);
	actionCollection()->addAction(QLatin1String("reloadProfiles"), actionReloadProfile);
	connect(actionReloadProfile, SIGNAL(triggered(bool)), SLOT(btnReloadProfiles()));
	actionDevManShutdown = new KAction(this);
	actionDevManShutdown->setText(i18n("Shutdown device manager"));
	actionDevManShutdown->setIcon(KIcon(QLatin1String("process-stop")));
	actionDevManShutdown->setShortcut(Qt::CTRL+Qt::Key_D);
	actionCollection()->addAction(QLatin1String("shutdownDevicemanager"), actionDevManShutdown);
	connect(actionDevManShutdown, SIGNAL(triggered(bool)), SLOT(btnDeviceManagerShutdown()));
	actionExecutionShutdown = new KAction(this);
	actionExecutionShutdown->setText(i18n("Shutdown execution engine"));
	actionExecutionShutdown->setIcon(KIcon(QLatin1String("process-stop")));
	actionExecutionShutdown->setShortcut(Qt::CTRL+Qt::Key_E);
	actionCollection()->addAction(QLatin1String("shutdownExecutionEngine"), actionExecutionShutdown);
	connect(actionExecutionShutdown, SIGNAL(triggered(bool)), SLOT(btnExecutionEngineShutdown()));
	actionExecutionStart = new KAction(this);
	actionExecutionStart->setText(i18n("Start execution engine"));
	actionExecutionStart->setIcon(KIcon(QLatin1String("system-run")));
	actionExecutionStart->setShortcut(Qt::CTRL+Qt::Key_S);
	actionCollection()->addAction(QLatin1String("startExecutionEngine"), actionExecutionStart);
	connect(actionExecutionStart, SIGNAL(triggered(bool)), SLOT(btnExecutionEngineStart()));
	setupGUI();

	// icons
	onlineIcon = KIcon(QLatin1String("flag-green"));
	offlineIcon = KIcon(QLatin1String("flag-red"));

	// create some objects
	devicelistmodel = new DeviceListModel(busconnection);
	targetlistmodel = new TargetListModel(busconnection);
	executionlistmodel = new ExecutionListModel(busconnection);

	// setup some connections
	connect(busconnection, SIGNAL( devicemanagerStateChanged(int) ), SLOT( devicemanagerStateChanged(int) ));
	connect(busconnection, SIGNAL( executionengineStateChanged(int) ), SLOT( executionengineStateChanged(int) ));
	connect(ui.viewDeviceList, SIGNAL( clicked(const QModelIndex&) ), SLOT( clicked(const QModelIndex&) ));
	connect(ui.viewDeviceList, SIGNAL( doubleClicked(const QModelIndex&) ), SLOT( doubleClicked(const QModelIndex&) ));

	/* models */
	ui.viewDeviceList->setModel(devicelistmodel);
	ui.viewExecution->setModel(executionlistmodel);
	ui.viewTargetList->setModel(targetlistmodel);
	ui.viewDeviceList->setIconSize(QSize(32,32));

	// ui init
	actionReloadRemote->setEnabled(false);
	actionReloadProfile->setEnabled(false);

	// init
	messageswindow = 0;
	driverlistwindow = 0;
	devicemanagerStateChanged(busconnection->deviceManagerState());
	executionengineStateChanged(busconnection->executionEngineState());
}

MainWindow::~MainWindow() {
	delete devicelistmodel;
	delete targetlistmodel;
	delete executionlistmodel;
	delete messageswindow;
}

void MainWindow::clicked ( const QModelIndex & index ) {
	if (!index.isValid()) return;
	current_rid = devicelistmodel->data(index, RIDRole).toInt();

	if (busconnection->getDeviceManagerReceiver(current_rid))
		actionReloadRemote->setEnabled(true);

	if (busconnection->getExecutionEngineReceiver(current_rid))
		actionReloadProfile->setEnabled(true);

}

void MainWindow::doubleClicked ( const QModelIndex & index ) {
	if (!index.isValid()) return;
	current_rid = devicelistmodel->data(index, RIDRole).toString();
	QString remoteuid = devicelistmodel->data(index, RemoteUIDRole).toString();
	SelectRemoteWindow rm(busconnection, current_rid, remoteuid);
	rm.exec();
}

void MainWindow::btnReloadRemote() {
	OrgLiriDevManagerReceiverInterface* iface = busconnection->getDeviceManagerReceiver(current_rid);
	if (iface) {
		iface->reloadAssociatedRemote();
		statusBar()->showMessage(i18n("Device Manager: Reload all remotes..."));
	}
}

void MainWindow::btnReloadProfiles() {
	OrgLiriExecutionReceiverInterface* iface = busconnection->getExecutionEngineReceiver(current_rid);
	if (iface) {
		iface->reload();
		statusBar()->showMessage(i18n("Execution Engine: Reload profiles..."));
	}
}

void MainWindow::btnDeviceManagerShutdown() {
	if (!busconnection->getDeviceManagerControl()) return;
	busconnection->getDeviceManagerControl()->quit();
	statusBar()->showMessage(i18n("Device Manager: Shutdown..."));
}

void MainWindow::btnExecutionEngineShutdown() {
	if (!busconnection->getExecutionEngineControl()) {
		statusBar()->showMessage(i18n("Execution Engine: Not running!"));
	} else {
		busconnection->getExecutionEngineControl()->quit();
		statusBar()->showMessage(i18n("Execution Engine: Shutdown..."));
	}
}

void MainWindow::btnExecutionEngineStart() {
	if (!busconnection->getExecutionEngineControl()) {
		statusBar()->showMessage(i18n("Execution Engine: Starting..."));
		QProcess::startDetached(QLatin1String("liriexec"));
	} else {
		statusBar()->showMessage(i18n("Execution Engine: Already running!"));
	}
}

void MainWindow::btnMessages() {
	if (!messageswindow) {
		messageswindow = new MessagesWindow(busconnection, this);
		messageswindow->setAttribute(Qt::WA_DeleteOnClose);
		messageswindow->show();
		connect(messageswindow, SIGNAL(closednow()), SLOT(closedMessages()));
	}
}

void MainWindow::btnDriverlist() {
	if (!driverlistwindow) {
		driverlistwindow = new DriverlistWindow(busconnection, this);
		driverlistwindow->setAttribute(Qt::WA_DeleteOnClose);
		driverlistwindow->show();
		connect(driverlistwindow, SIGNAL(closednow()), SLOT(closedDriverlist()));
	}
}

void MainWindow::closedMessages() {
	messageswindow = 0;
}

void MainWindow::closedDriverlist() {
	driverlistwindow = 0;
}

void MainWindow::devicemanagerStateChanged(int state) {
	if (state == LIRI_RUNNING) {
		statusBar()->showMessage(i18n("Device Manager: Online"));

		QString datetime = busconnection->getDeviceManagerControl()->started();
		datetime = QDateTime::fromString(datetime, Qt::ISODate).toString(Qt::SystemLocaleShortDate);
		QString version = busconnection->getDeviceManagerControl()->version();
		ui.lblDevManState->setText(i18n("Version: %1 - Started: %2", version, datetime));

		ui.lblDevManState->setStyleSheet(QLatin1String("QLabel#lblDevManState {color: rgb(0, 170, 0); }"));
		actionDevManShutdown->setEnabled(true);
		datetime = busconnection->getDeviceManagerControl()->lastHalCallout();
		if (datetime.size()) {
			datetime = QDateTime::fromString(datetime, Qt::ISODate).toString(Qt::SystemLocaleShortDate);
			ui.lblDevManLastCallout->setText(i18n("Working (%1)", datetime));
			ui.lblDevManLastCallout->setStyleSheet(
				QLatin1String("QLabel#lblDevManLastCallout {color: rgb(0, 170, 0); }"));
		} else {
			ui.lblDevManLastCallout->setStyleSheet(QString());
			ui.lblDevManLastCallout->setText(i18n("The device manager was started manually"));
		}
	} else if (state == LIRI_UNKNOWN) {
		statusBar()->showMessage(i18n("Device Manager: Offline"));
		ui.lblDevManState->setText(i18n("Offline"));
		ui.lblDevManState->setStyleSheet(QLatin1String("QLabel#lblDevManState {color: rgb(255, 0, 0); }"));
		actionDevManShutdown->setEnabled(false);
		actionReloadRemote->setEnabled(false);
		actionReloadProfile->setEnabled(false);

		ui.lblDevManLastCallout->setStyleSheet(QString());
		ui.lblDevManLastCallout->setText(i18n("Status unknown"));

	}
}

void MainWindow::executionengineStateChanged(int state) {
	if (state == LIRI_RUNNING) {
		statusBar()->showMessage(i18n("Execution Engine: Online"));

		QString datetime = busconnection->getExecutionEngineControl()->started();
		datetime = QDateTime::fromString(datetime, Qt::ISODate).toString(Qt::SystemLocaleShortDate);
		QString version = busconnection->getExecutionEngineControl()->version();
		ui.lblExecutionState->setText(i18n("Version: %1 - Started: %2", version, datetime));

		ui.lblExecutionState->setStyleSheet(QLatin1String("QLabel#lblExecutionState {color: rgb(0, 170, 0); }"));
		actionExecutionShutdown->setEnabled(true);
		actionExecutionStart->setEnabled(false);
	} else if (state == LIRI_UNKNOWN) {
		statusBar()->showMessage(i18n("Execution Engine: Offline"));
		ui.lblExecutionState->setText(i18n("Offline"));
		ui.lblExecutionState->setStyleSheet(QLatin1String("QLabel#lblExecutionState {color: rgb(255, 0, 0); }"));
		actionExecutionShutdown->setEnabled(false);
		actionExecutionStart->setEnabled(true);
		actionReloadProfile->setEnabled(false);
	}
}
