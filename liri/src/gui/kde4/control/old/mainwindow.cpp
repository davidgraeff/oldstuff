#include "mainwindow.h"

#include <QtGui/QPainter>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>
#include <QTimer>
#include <QHeaderView>

#include <kconfig.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

#include <kconfigdialog.h>
#include <kstatusbar.h>
#include <kxmlguifactory.h>
#include <kpushbutton.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>
#include <kcategorizedview.h>
#include <kcategorydrawer.h>
#include <ktoolbar.h>

#include <kfiledialog.h>
#include <KIO/NetAccess>

#include "profiles/DesktopProfilesList.h"
#include "profiles/ApplicationProfilesList.h"
#include "profiles/ApplicationAction.h"
#include "profiles/ApplicationInterface.h"
#include "profiles/ApplicationArgument.h"

#include <KDE/KLocale>

#include "config.h"

MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent) {
    // create the user interface, the parent widget is "widget"
    QWidget *widget = new QWidget( this );
    ui.setupUi(widget);
    setCentralWidget( widget );

    setupGUI();
    this->setStatusBar(0);
    if (this->toolBar()) this->toolBar()->hide();

	//dp and ap list
	dplist = new liri::DesktopProfilesList();
	dplist->setLoadIcons(true);
	aplist = new liri::ApplicationProfilesList();
	//init views
	ui.listDesktopProfiles->setModel(dplist);
	dplist_delegate = new DesktopprofileDelegate(ui.listDesktopProfiles);
	ui.listDesktopProfiles->setItemDelegate(dplist_delegate);
	ui.listApplicationProfiles->setModel(aplist);
}


MainWindow::~MainWindow() {
	delete dplist;
	delete dplist_delegate;
	delete aplist;
}

void MainWindow::removeDesktopProfile() {

}

void MainWindow::addDesktopProfile() {

}

void MainWindow::chooseUsedApplicationProfile() {

}

void MainWindow::removeUsedApplicationProfile() {

}

void MainWindow::chooseControlledByRemote() {

}

void MainWindow::removeControlledByRemote() {

}

//application profile
void MainWindow::removeApplicationProfile() {

}

void MainWindow::addApplicationProfile() {

}

void MainWindow::addActionToApplication() {

}

void MainWindow::removeActionToApplication() {

}

//something changed
void MainWindow::command_modechanged(int) {

}

void MainWindow::changed() {

}

