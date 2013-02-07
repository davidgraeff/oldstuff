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
#include <QPlainTextEdit> // for the log dialog
#include <stdlib.h>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QByteArray>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPointer>

#include <KDE/KLocale>
#include <ktoolinvocation.h>

#include "config.h"
#include "businterconnect/DBusServiceList.h"
#include "businterconnect/ExecutionConnection.h"
#include "businterconnect/Dbus_proxy_execution.h"
#include "businterconnect/Dbus_proxy_executioninstance.h"
#include "businterconnect/DaemonConnection.h"
#include "businterconnect/Dbus_proxy_receiver.h"
#include "businterconnect/Dbus_proxy_receiverlist.h"
#include "businterconnect/Dbus_proxy_receiverloadremote.h"

#include "fileformats/RemoteFile.h"

#include "ReceiversList.h"
#include "ActionsModel.h"


MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent),
	onlineIcon(QLatin1String("liri-status-online")),
	offlineIcon(QLatin1String("liri-status-offline"))
{
	// create the user interface, the parent widget is "widget"
	QWidget *widget = new QWidget( this );
	ui = new Ui::MainWindow;
	ui->setupUi(widget);
	setCentralWidget( widget );
	setupActions();
	setupGUI();
	if (this->toolBar()) this->toolBar()->hide();

	ui->ReceiversView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	//create some objects
	dlist = new DBusServiceList();
	dconnection = new DaemonConnection(dlist);
	econnection = new ExecutionConnection(dlist);
	receiverlist = new ReceiverList();
	actionsmodel = new ActionsModel(dconnection, econnection);
	actionsmodelproxy = new KCategorizedSortFilterProxyModel();
	actionsmodelproxy->setCategorizedModel(true);
	actionsmodelproxy->setSourceModel(actionsmodel);

	// setup some connections
	connect(ui->btnReloadRemotes, SIGNAL(clicked(bool)), SLOT(btnReloadRemotes()));
	connect(ui->btnLiridShutdown, SIGNAL(clicked(bool)), SLOT(btnLiridShutdown()));
	connect(ui->btnReloadProfiles, SIGNAL(clicked(bool)), SLOT(btnReloadProfiles()));
	connect(ui->btnExecutionShutdown, SIGNAL(clicked(bool)), SLOT(btnExecutionShutdown()));
	
	connect(ui->btnLiridLogfile, SIGNAL(clicked(bool)), SLOT(btnLiridLogfile()));
	connect(ui->btnExecutionLogfile, SIGNAL(clicked(bool)), SLOT(btnExecutionLogfile()));

	connect(dconnection, SIGNAL( daemonStateChanged(int) ), SLOT( daemonStateChanged(int) ));
	connect(dconnection, SIGNAL( receiverStateChanged(int, int) ), SLOT( receiverStateChanged(int, int) ));
	connect(dconnection, SIGNAL( remoteStateChanged(int, int) ), SLOT( remoteStateChanged(int, int) ));
	
	connect(econnection, SIGNAL( executionStateChanged(int) ), SLOT( executionStateChanged(int) ));

	connect(&fwatcher, SIGNAL( fileChanged(const QString &) ), SLOT( halhelperLogfileChanged() ));

	if (dconnection->getReceiverListInterface()) {
		QList<int> instancelist = dconnection->getReceiverListInterface()->getAllInstances().value();
		foreach(int instanceNr, instancelist) {
			OrgLiriReceiverInterface * const ri = dconnection->getReceiverInterface(instanceNr);
			emit receiverStateChanged(instanceNr, ri->ReceiverState());
			emit remoteStateChanged(instanceNr, ri->RemoteState());
			dconnection->releaseReceiverInterface(instanceNr);
		}
	}

	/* models */
	ui->ReceiversView->setModel(receiverlist);
	
	ui->loadedActionsView->setModel(actionsmodelproxy);
	ui->loadedActionsView->setCategoryDrawer(new KCategoryDrawer());

	// init
	logfile = 0;
	daemonStateChanged(dconnection->isDaemonRunning());
	executionStateChanged(econnection->isExecutionEngineRunning());
}

MainWindow::~MainWindow() {
	delete dconnection;
	delete receiverlist;
	delete actionsmodel;
	delete actionsmodelproxy;
	delete logfile;
	delete ui;
	QCoreApplication::exit(0);
}

void MainWindow::btnLiridLogfile() {
	delete logfile;
	logfile = 0;

	logfilename = QLatin1String(LIRI_LOGFILE_DAEMON);
	QFile file(logfilename);
	file.open(QIODevice::ReadOnly);
	logfile = new QWidget(this);
	logfile->setWindowTitle(i18n("Logfile \"%1\"", logfilename));
	logfile->setWindowFlags(Qt::Dialog);
	logfile->setMinimumHeight(200);
	logfile->setMinimumWidth(450);
	QPlainTextEdit* textedit = new QPlainTextEdit(QString::fromUtf8(file.readAll()), logfile);
	QPushButton* button = new QPushButton(i18n("Clear"), logfile);
	QVBoxLayout* layout = new QVBoxLayout(logfile);
	layout->addWidget(textedit);
	layout->addWidget(button);
	connect(button, SIGNAL(clicked(bool)), this, SLOT(btnClearLogfile()));
	logfile->show();
}

void MainWindow::btnExecutionLogfile() {
	delete logfile;
	logfile = 0;

	logfilename = QDir::home().absoluteFilePath(QLatin1String(LIRI_LOGFILE_EXECUTION));
	QFile file(logfilename);
	file.open(QIODevice::ReadOnly);
	logfile = new QWidget(this);
	logfile->setWindowTitle(i18n("Logfile \"%1\"", logfilename));
	logfile->setWindowFlags(Qt::Dialog);
	logfile->setMinimumHeight(200);
	logfile->setMinimumWidth(450);
	QPlainTextEdit* textedit = new QPlainTextEdit(QString::fromUtf8(file.readAll()), logfile);
	QPushButton* button = new QPushButton(i18n("Clear"), logfile);
	QVBoxLayout* layout = new QVBoxLayout(logfile);
	layout->addWidget(textedit);
	layout->addWidget(button);
	connect(button, SIGNAL(clicked(bool)), this, SLOT(btnClearLogfile()));
	logfile->show();
}

void MainWindow::btnClearLogfile() {
	QFile file(logfilename);
	if (file.permissions() & QFile::WriteGroup || file.permissions() & QFile::WriteUser || file.permissions() & QFile::WriteOther) {
		file.resize(0);
		if (logfile)
			logfile->close();
	} else
		statusBar()->showMessage(i18n("Failed to clear the file \"%1\"!", logfilename));
}

void MainWindow::btnReloadRemotes() {
	if (!dconnection->getReceiverListInterface()) return;
	dconnection->getReceiverListInterface()->reloadAllRemotes();
	statusBar()->showMessage(i18n("Device Manager: Reload all remotes..."));
}

void MainWindow::btnLiridShutdown() {
	if (!dconnection->getControlInterface()) return;
	dconnection->getControlInterface()->quit();
	statusBar()->showMessage(i18n("Device Manager: Shutdown..."));
}

void MainWindow::btnExecutionShutdown() {
	if (!econnection->getExecutionInterface()) {
		statusBar()->showMessage(i18n("Execution Engine: Starting..."));
		QProcess::startDetached(QLatin1String("liriexec"));
	} else {
		econnection->getExecutionInterface()->quit();
		statusBar()->showMessage(i18n("Execution Engine: Shutdown..."));
	}
}

void MainWindow::btnReloadProfiles() {
	if (!econnection->getExecutionInterface()) return;
	econnection->getExecutionInterface()->reloadAllProfiles();
	updateActionView();
}

void MainWindow::halhelperLogfileChanged() {
	QFile file(QLatin1String(LIRI_LOGFILE_CALLOUT));
	file.open(QIODevice::ReadOnly);
	if (file.size()) {
		QString text = QString::fromLatin1(file.readLine()).replace(QLatin1Char('\n'), QString());
		ui->lblLiridLastCallout->setText(text);
	}
}

void MainWindow::updateActionView() {
	ui->loadedActionsView->setModel(0);
	actionsmodel->refresh();
	actionsmodelproxy->setSourceModel(0);
	actionsmodelproxy->setSourceModel(actionsmodel);
	ui->loadedActionsView->setModel(actionsmodelproxy);
	statusBar()->showMessage(i18n("Acions refreshed"));
}

inline void MainWindow::setupActions()
{
	KStandardAction::quit(qApp, SLOT(closeAllWindows()), actionCollection());
	//KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());
}

void MainWindow::daemonStateChanged(int state) {
	if (state == LIRIOK_ready) {
		statusBar()->showMessage(i18n("Device Manager: Online"));
		ui->tabWidget->setTabIcon(0, onlineIcon);
		QString tmp = dconnection->getControlInterface()->started();
		ui->lblLiridState->setText(i18n("Running (%1)", QDateTime::fromString(tmp, Qt::ISODate).toString(Qt::SystemLocaleShortDate)));
		ui->lblLiridState->setStyleSheet(QLatin1String("QLabel#lblLiridState {color: rgb(0, 170, 0); }"));
		ui->btnReloadRemotes->setEnabled(true);
		ui->btnLiridShutdown->setEnabled(true);
		tmp = dconnection->getControlInterface()->lastHalCallout();
		if (tmp.size()) {
			fwatcher.removePath(QLatin1String(LIRI_LOGFILE_CALLOUT));
			ui->lblLiridLastCallout->setText(i18n("Working (%1)",
				QDateTime::fromString(tmp, Qt::ISODate).toString(Qt::SystemLocaleShortDate)));
			ui->lblLiridLastCallout->setStyleSheet(
				QLatin1String("QLabel#lblLiridLastCallout {color: rgb(0, 170, 0); }"));
		}
	} else if (state == LIRIOK_offline) {
		actionsmodel->clear();
		statusBar()->showMessage(i18n("Device Manager: Offline"));
		ui->tabWidget->setTabIcon(0, offlineIcon);
		receiverlist->clear();
		ui->lblLiridLastCallout->setStyleSheet(QString());
		ui->lblLiridLastCallout->setText(i18n("No events yet"));
		fwatcher.addPath(QLatin1String(LIRI_LOGFILE_CALLOUT));
		ui->lblLiridState->setText(i18n("Offline"));
		ui->lblLiridState->setStyleSheet(QLatin1String("QLabel#lblLiridState {color: rgb(255, 0, 0); }"));
		ui->btnReloadRemotes->setEnabled(false);
		ui->btnLiridShutdown->setEnabled(false);
	}
}

void MainWindow::executionStateChanged(int state) {
	if (state == LIRIOK_ready) {
		updateActionView();
		statusBar()->showMessage(i18n("Execution Engine: Online"));
		ui->tabWidget->setTabIcon(1, onlineIcon);
		ui->lblExecutionState->setText(i18n("Running (User: %1)", QLatin1String(getenv("USER"))));
		ui->lblExecutionState->setStyleSheet(QLatin1String("QLabel#lblExecutionState {color: rgb(0, 170, 0); }"));
		ui->btnReloadProfiles->setEnabled(true);
		ui->btnExecutionShutdown->setText(i18n("Shutdown"));
		int pstate = econnection->getExecutionInterface()->getPowerstate();
		ui->lblExecutionPowerstate->setText(QString::number(pstate));
	} else if (state == LIRIOK_offline) {
		actionsmodel->clear();
		statusBar()->showMessage(i18n("Execution Engine: Offline"));
		ui->tabWidget->setTabIcon(1, offlineIcon);
		ui->lblExecutionState->setText(i18n("Offline"));
		ui->lblExecutionState->setStyleSheet(QLatin1String("QLabel#lblExecutionState {color: rgb(255, 0, 0); }"));
		ui->lblExecutionPowerstate->setText(i18n("unknown"));
		ui->btnReloadProfiles->setEnabled(false);
		ui->btnExecutionShutdown->setText(i18n("Start"));
	}
}

void MainWindow::receiverStateChanged(int instance, int state) {
	QPointer<OrgLiriReceiverInterface> ri = dconnection->getReceiverInterface(instance);
	if (!ri) {
		qWarning() << "receiverStateChanged failed";
		return;
	}

	QStringList keys;
	keys << QLatin1String("time.connected") << QLatin1String("time.disconnected") << QLatin1String("udi") << QLatin1String("remotereceiver.driver") << QLatin1String("remotereceiver.id");
	QStringList values = ri->getSettings(keys);

	if (values.size() == 5) {
		values[0] = QDateTime::fromString(values[0], Qt::ISODate).toString(Qt::SystemLocaleShortDate);
		values[1] = QDateTime::fromString(values[1], Qt::ISODate).toString(Qt::SystemLocaleShortDate);
		receiverlist->changed(instance, state, values[0], values[1], values[2], values[3], values[4]);
	} else {
		qWarning() << "receiverStateChanged failed";
	}

	//dconnection->releaseReceiverInterface(instance);
	
	updateActionView();
}

void MainWindow::remoteStateChanged(int instance, int state) {
	QPointer<OrgLiriReceiverInterface> ri = dconnection->getReceiverInterface(instance);
	if (!ri) {
		qWarning() << "remoteStateChanged failed";
		return;
	}
	
	QStringList keys;
	keys << QLatin1String("remote.uid");
	QStringList values = ri->getSettings(keys);
	
	if (values.size() == 1) {
		RemoteFile remotefile(values[0]);
		receiverlist->changedRemote(instance, state, values[0], remotefile.getName());
	} else {
		qWarning() << "remoteStateChanged failed";
	}
	
	//dconnection->releaseReceiverInterface(instance);
}
