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
/* window */
#include <kstatusbar.h>
#include <kxmlguifactory.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>
#include <ktoolbar.h>

#include <QStringList>
#include <QFile>
#include <QByteArray>
#include <QHeaderView>
/* i18n */
#include <KDE/KLocale>
#include <QDebug>

#include "AP_Model.h"
#include "DP_Model.h"
#include "Remotes_Model.h"
#include "businterconnect/BusConnection.h"
#include "fileformats/DesktopFile.h"
#include "fileformats/RemoteFile.h"
#include "fileformats/RemoteLoadProfilesFile.h"
#include "fileformats/DesktopProfileFile.h"
#include "fileformats/ApplicationProfileFile.h"
/* Edit dialogs */
#include "editRemote/editRemote.h"
#include "editDesktopProfile/editDesktopProfile.h"
#include "editAppProfile/editAppProfile.h"

#include "config.h"

MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent) {
	// create the user interface, the parent widget is "widget"
	QWidget *widget = new QWidget( this );
	ui = new Ui::MainWindow;
	ui->setupUi(widget);
	setCentralWidget( widget );
	KStandardAction::quit(qApp, SLOT(closeAllWindows()), actionCollection());

	actionRemoteNew = new KAction(KIcon(QLatin1String("document-new")), QString(), this);
	ui->btnRemoteNew->setDefaultAction(actionRemoteNew);
	actionCollection()->addAction(QLatin1String("actionRemoteNew"), actionRemoteNew);

	actionRemoteEdit = new KAction(KIcon(QLatin1String("document-edit")), QString(), this);
	ui->btnRemoteEdit->setDefaultAction(actionRemoteEdit);
	actionCollection()->addAction(QLatin1String("actionRemoteEdit"), actionRemoteEdit);

	actionRemoteRemove = new KAction(KIcon(QLatin1String("edit-delete")), QString(), this);
	ui->btnRemoteRemove->setDefaultAction(actionRemoteRemove);
	actionCollection()->addAction(QLatin1String("actionRemoteRemove"), actionRemoteRemove);

	actionRemoteDownload = new KAction(KIcon(QLatin1String("get-hot-new-stuff")), QString(), this);
	ui->btnRemoteDownload->setDefaultAction(actionRemoteDownload);
	actionCollection()->addAction(QLatin1String("actionRemoteDownload"), actionRemoteDownload);

	actionDPNew = new KAction(KIcon(QLatin1String("document-new")), QString(), this);
	ui->btnDPNew->setDefaultAction(actionDPNew);
	actionCollection()->addAction(QLatin1String("actionDPNew"), actionDPNew);

	actionDPEdit = new KAction(KIcon(QLatin1String("document-edit")), QString(), this);
	ui->btnDPEdit->setDefaultAction(actionDPEdit);
	actionCollection()->addAction(QLatin1String("actionDPEdit"), actionDPEdit);

	actionDPRemove = new KAction(KIcon(QLatin1String("edit-delete")), QString(), this);
	ui->btnDPRemove->setDefaultAction(actionDPRemove);
	actionCollection()->addAction(QLatin1String("actionDPRemove"), actionDPRemove);

	actionDPDownload = new KAction(KIcon(QLatin1String("get-hot-new-stuff")), QString(), this);
	ui->btnDPDownload->setDefaultAction(actionDPDownload);
	actionCollection()->addAction(QLatin1String("actionDPDownload"), actionDPDownload);

	actionAPNew = new KAction(KIcon(QLatin1String("document-new")), QString(), this);
	ui->btnAPNew->setDefaultAction(actionAPNew);
	actionCollection()->addAction(QLatin1String("actionAPNew"), actionAPNew);

	actionAPEdit = new KAction(KIcon(QLatin1String("document-edit")), QString(), this);
	ui->btnAPEdit->setDefaultAction(actionAPEdit);
	actionCollection()->addAction(QLatin1String("actionAPEdit"), actionAPEdit);

	actionAPRemove = new KAction(KIcon(QLatin1String("edit-delete")), QString(), this);
	ui->btnAPRemove->setDefaultAction(actionAPRemove);
	actionCollection()->addAction(QLatin1String("actionAPRemove"), actionAPRemove);

	actionAPDownload = new KAction(KIcon(QLatin1String("get-hot-new-stuff")), QString(), this);
	ui->btnAPDownload->setDefaultAction(actionAPDownload);
	actionCollection()->addAction(QLatin1String("actionAPDownload"), actionAPDownload);

	connect(actionRemoteNew, SIGNAL(triggered()), this, SLOT(btnRemoteNew()));
	connect(actionRemoteRemove, SIGNAL(triggered()), this, SLOT(btnRemoteRemove()));
	connect(actionRemoteEdit, SIGNAL(triggered()), this, SLOT(btnRemoteEdit()));
	connect(actionRemoteDownload, SIGNAL(triggered()), this, SLOT(btnRemoteDownload()));
	connect(actionDPNew, SIGNAL(triggered()), this, SLOT(btnDPNew()));
	connect(actionDPRemove, SIGNAL(triggered()), this, SLOT(btnDPRemove()));
	connect(actionDPEdit, SIGNAL(triggered()), this, SLOT(btnDPEdit()));
	connect(actionDPDownload, SIGNAL(triggered()), this, SLOT(btnDPDownload()));
	connect(actionAPNew, SIGNAL(triggered()), this, SLOT(btnAPNew()));
	connect(actionAPRemove, SIGNAL(triggered()), this, SLOT(btnAPRemove()));
	connect(actionAPEdit, SIGNAL(triggered()), this, SLOT(btnAPEdit()));
	connect(actionAPDownload, SIGNAL(triggered()), this, SLOT(btnAPDownload()));
	connect(ui->chkControlAll, SIGNAL(stateChanged(int)), this, SLOT(chkControlAll_stateChanged(int)));

	ui->chkControlAll->setEnabled(false);
	actionRemoteRemove->setEnabled(false);
	actionRemoteEdit->setEnabled(false);
	actionRemoteDownload->setEnabled(false);
	actionDPRemove->setEnabled(false);
	actionDPEdit->setEnabled(false);
	actionDPDownload->setEnabled(false);
	actionAPRemove->setEnabled(false);
	actionAPEdit->setEnabled(false);
	actionAPDownload->setEnabled(false);

	setupGUI();

	/* init */
	busconnection = new BusConnection();

	/* create the models */
	ap_model = new AP_Model();
	dp_model = new DP_Model();
	remotes_model = new Remotes_Model();

	/* connect the models with the views */
	ui->remoteslistView->setModel(remotes_model);
	ui->desktopRemoteView->setModel(dp_model);
	ui->desktopRemoteView->setModelColumn(1);
	ui->desktoplistView->setModel(dp_model);
	ui->desktoplistView->setModelColumn(0);
	ui->applicationlistView->setModel(ap_model);

	/* icon size */
	ui->remoteslistView->setIconSize(QSize(48,48));
	ui->desktopRemoteView->setIconSize(QSize(16,16));
	ui->desktoplistView->setIconSize(QSize(48,48));
	ui->applicationlistView->setIconSize(QSize(32,32));

	/* connect view signals */
	connect(ui->remoteslistView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(remotes_clicked(const QModelIndex &)));
	connect(ui->remoteslistView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(remotes_doubleClicked(const QModelIndex &)));
	connect(ui->desktoplistView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(dp_clicked(const QModelIndex &)));
	connect(ui->desktoplistView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(dp_doubleClicked(const QModelIndex &)));
	connect(ui->applicationlistView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(ap_clicked(const QModelIndex &)));
	connect(ui->applicationlistView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(ap_doubleClicked(const QModelIndex &)));
	/* connect model signals */
	connect(ap_model, SIGNAL(saveDesktopfile(DesktopProfileFile*,bool)), this, SLOT(saveDesktopfile(DesktopProfileFile*,bool)));
}

MainWindow::~MainWindow() {
	delete busconnection;
	delete ap_model;
	delete dp_model;
	delete remotes_model;
	delete ui;
	QCoreApplication::exit(0);
}

void MainWindow::chkControlAll_stateChanged(int state) {
	if (!remotefile) return;

	RemoteLoadProfilesFile loader(remotefile->getUid());
	loader.setAll( ((state == Qt::Checked) ? true : false) );
	loader.save();
	dp_model->markControlled(remotefile);
}

void MainWindow::saveRemotefile(RemoteFile* file,bool deleteOnFailure) {
	if (!file->save()) {
		statusBar()->showMessage(i18n("Error: Remote file %1 NOT saved!", file->getName()));
		if (deleteOnFailure) delete file;
	} else {
		/* update the model */
		remotes_model->edit(file);
	}
}

void MainWindow::saveAppfile(ApplicationProfileFile* file,bool deleteOnFailure) {
	if (!file->save()) {
		statusBar()->showMessage(i18n("Error: Application Profile %1 NOT saved!", file->getName()));
		if (deleteOnFailure) delete file;
	} else {
		/* update the model */
		ap_model->edit(file);
	}
}

void MainWindow::saveDesktopfile(DesktopProfileFile* file,bool deleteOnFailure) {
	if (!file->save()) {
		statusBar()->showMessage(i18n("Error: Desktop Profile %1 NOT saved!", file->getName()));
		if (deleteOnFailure) delete file;
	} else {
		/* update the model */
		dp_model->edit(file);
	}
}

void MainWindow::remotes_clicked ( const QModelIndex & index ) {
	remotefile = static_cast<RemoteFile*>(remotes_model->data(index, Qt::UserRole).value<void*>());
	ui->btnRemoteRemove->setEnabled(true);
	ui->btnRemoteEdit->setEnabled(true);
	ui->btnRemoteDownload->setEnabled(true);

	ui->chkControlAll->setEnabled(false); // do not update checkstate while saving
	dp_model->markControlled(remotefile);
	RemoteLoadProfilesFile loader( remotefile->getUid() );
	ui->chkControlAll->setChecked( loader.isAll() );
	ui->chkControlAll->setEnabled(true);
}

void MainWindow::remotes_doubleClicked ( const QModelIndex & index ) {
	remotefile = static_cast<RemoteFile*>(remotes_model->data(index, Qt::UserRole).value<void*>());
	btnRemoteEdit();
}

void MainWindow::dp_clicked ( const QModelIndex & index ) {
	desktopfile = static_cast<DesktopProfileFile*>(dp_model->data(index, Qt::UserRole).value<void*>());
	ap_model->markUsed(desktopfile);
	ui->btnDPRemove->setEnabled(true);
	ui->btnDPEdit->setEnabled(true);
	ui->btnDPDownload->setEnabled(true);
}

void MainWindow::dp_doubleClicked ( const QModelIndex & index ) {
	desktopfile = static_cast<DesktopProfileFile*>(dp_model->data(index, Qt::UserRole).value<void*>());
	btnDPEdit();
}

void MainWindow::ap_clicked ( const QModelIndex & index) {
	appfile = static_cast<ApplicationProfileFile*>(ap_model->data(index, Qt::UserRole).value<void*>());
	ui->btnAPRemove->setEnabled(true);
	ui->btnAPEdit->setEnabled(true);
	ui->btnAPDownload->setEnabled(true);
}

void MainWindow::ap_doubleClicked ( const QModelIndex & index ) {
	appfile = static_cast<ApplicationProfileFile*>(ap_model->data(index, Qt::UserRole).value<void*>());
	btnAPEdit();
}

/////////////////////////////// ACTIONS /////////////////////////////////


void MainWindow::btnRemoteNew() {
	RemoteFile* re = new RemoteFile();
	EditRemoteDialog* dialog = new EditRemoteDialog(busconnection, re, this);
	bool save = dialog->exec();
	delete dialog;

	if (save) saveRemotefile(re,true);
	else delete re;
}

void MainWindow::btnRemoteEdit() {
	if (!remotefile) return;

	RemoteFile* re = new RemoteFile(remotefile->getUid());
	EditRemoteDialog* dialog = new EditRemoteDialog(busconnection, re, this);
	bool save = dialog->exec();
	delete dialog;

	if (save) saveRemotefile(re,true);
	else delete re;
}

void MainWindow::btnRemoteRemove() {
	if (!remotefile) return;

	// try to remove the file
	if (!QFile::remove(remotefile->getFilename())) {
		statusBar()->showMessage(i18n("Error: Remote file %1 NOT removed!", remotefile->getFilename()));
		return;
	}

	dp_model->markControlled(0);

	// remove from model
	remotes_model->remove(remotefile);
	remotefile = 0;

	// success: disable controls
	if (!remotes_model->rowCount()) {
		ui->btnRemoteRemove->setEnabled(false);
		ui->btnRemoteEdit->setEnabled(false);
		ui->btnRemoteDownload->setEnabled(false);
	} else remotes_clicked(ui->remoteslistView->currentIndex());
}

void MainWindow::btnRemoteDownload() {
	statusBar()->showMessage(i18n("Webaccess not possible. No release version!"));
}

void MainWindow::btnDPNew() {
	DesktopProfileFile* dp = new DesktopProfileFile();
	EditDesktopProfileDialog* dialog = new EditDesktopProfileDialog(dp, this);
	bool save = dialog->exec();
	delete dialog;

	if (save) saveDesktopfile(dp,true);
	else delete dp;
}

void MainWindow::btnDPEdit() {
	if (!desktopfile) return;

	DesktopProfileFile* dp = new DesktopProfileFile(desktopfile->getUid());
	EditDesktopProfileDialog* dialog = new EditDesktopProfileDialog(dp, this);
	bool save = dialog->exec();

	delete dialog;

	if (save) saveDesktopfile(dp,true);
	else delete dp;
}

void MainWindow::btnDPRemove() {
	if (!desktopfile) return;

	// try to remove the file
	if (!QFile::remove(desktopfile->getFilename())) {
		statusBar()->showMessage(i18n("Error: Desktop profile %1 NOT removed!", desktopfile->getFilename()));
		return;
	}

	ap_model->markUsed(0);

	// remove from model
	dp_model->remove(desktopfile);
	desktopfile = 0;

	// success: disable controls
	if (!dp_model->rowCount()) {
		ui->btnDPRemove->setEnabled(false);
		ui->btnDPEdit->setEnabled(false);
		ui->btnDPDownload->setEnabled(false);
	} else dp_clicked(ui->desktoplistView->currentIndex());
}

void MainWindow::btnDPDownload() {
	statusBar()->showMessage(i18n("Webaccess not possible. No release version!"));
}

void MainWindow::btnAPNew() {
	ApplicationProfileFile* ap = new ApplicationProfileFile();
	EditAppProfileDialog* dialog = new EditAppProfileDialog(ap, this);
	bool save = dialog->exec();
	delete dialog;

	if (save) saveAppfile(ap,true);
	else delete ap;
}

void MainWindow::btnAPEdit() {
	if (!appfile) return;

	ApplicationProfileFile* ap = new ApplicationProfileFile(appfile->getUid());
	EditAppProfileDialog* dialog = new EditAppProfileDialog(ap, this);
	bool save = dialog->exec();
	delete dialog;

	if (save) saveAppfile(ap,true);
	else delete ap;
}

void MainWindow::btnAPRemove() {
	if (!appfile) return;

	// try to remove the file
	if (!QFile::remove(appfile->getFilename())) {
		statusBar()->showMessage(i18n("Error: Desktop profile %1 NOT removed!", appfile->getFilename()));
		return;
	}

	// remove from model
	ap_model->remove(appfile);
	appfile = 0;

	// success: disable controls
	if (!ap_model->rowCount()) {
		ui->btnAPRemove->setEnabled(false);
		ui->btnAPEdit->setEnabled(false);
		ui->btnAPDownload->setEnabled(false);
	} else ap_clicked(ui->applicationlistView->currentIndex());
}

void MainWindow::btnAPDownload() {
	statusBar()->showMessage(i18n("Webaccess not possible. No release version!"));
}
