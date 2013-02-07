/***************************************************************************
 *   Copyright (C) 2009 by david   *
 *   david.graeff@web.de           *
 *                                                                         *
 *   StellaControl is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   StellaControl is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with StellaControl.  If not, see <http://www.gnu.org/licenses/> *
 ***************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QStatusBar>
#include <QSettings>
#include <QCoreApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QDir>
#include <QPluginLoader>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include "stellaconnection.h"
#include "stellaconnecting.h"
#include "plugins_interface.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    // init random seed
    srand ( time(NULL) );

    // init application info
    QCoreApplication::setOrganizationName(QLatin1String("Ethersex"));
    QCoreApplication::setOrganizationDomain(QLatin1String("www.ethersex.de"));
    QCoreApplication::setApplicationName(QLatin1String("Stella Control"));
    #if QT_VERSION >= 0x040400
    QCoreApplication::setApplicationVersion(QLatin1String("1.2"));
    #endif

    // init ui
    ui->setupUi(this);
    this->setWindowTitle(QCoreApplication::applicationName());
    ui->actionSeparator->setSeparator(true);
    #if QT_VERSION >= 0x040500
    ui->tabWidget->setTabsClosable(true);
    //ui->tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, 0);
    #endif

    // statusbar
    permMesg = new QLabel(this);
    statInMesg = new QLabel(this);
    statOutMesg = new QLabel(this);
    ui->statusBar->addWidget(statInMesg);
    ui->statusBar->addWidget(statOutMesg);
    ui->statusBar->addPermanentWidget(permMesg);

    // center window
    int scrn = QApplication::desktop()->screenNumber(this);
    QRect desk(QApplication::desktop()->availableGeometry(scrn));
    move((desk.width() - frameGeometry().width()) / 2,
      (desk.height() - frameGeometry().height()) / 2);

    // system tray
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(ui->actionRestore);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(ui->actionClose);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(this->windowIcon());
    ui->actionMinimize_to_tray->setEnabled(false);
    QSettings settings;
    ui->actionShow_tray_icon->setChecked(settings.value(QLatin1String("showTrayIcon"),false).toBool());

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    // read stella hosts
    settings.beginGroup(QLatin1String("hosts"));
    QStringList hosts = settings.childGroups();
    settings.endGroup();
    ui->lineIP->clear();
    foreach (QString host, hosts) ui->lineIP->addItem(host);
    ui->lineIP->setCurrentIndex(0);

    // default value
    // host part is enough here, port will be set on host part change
    if (ui->lineIP->count()==0)
        ui->lineIP->setEditText(QLatin1String("192.168.1.10"));

    // disable connect button (no plugin selected)
    ui->btnConnect->setEnabled(false);

    // add plugin names to comboBox and cache plugin paths in "plugins"
    QDir pluginsDir(qApp->applicationDirPath());
    #if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
    #elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
    #endif
    pluginsDir.cd("plugins");
    QObject *plugin_1;
    StellaControlPlugin *plugin_2;
    ui->listPlugins->clear();
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        plugin_1 = pluginLoader.instance();
        if (plugin_1) {
            plugin_2 = qobject_cast<StellaControlPlugin *>(plugin_1);
            if (plugin_2)
            {
                ui->listPlugins->addItem(plugin_2->plugname(), qVariantFromValue((void*)plugin_2));
            } else {
                delete plugin_1;
            }
        }
    }

    // no plugins?
    if (ui->listPlugins->count()==0)
        permMesg->setText(tr("Application useless! No plugins found!"));

    // inital tab is the "select device"-tab
    on_tabWidget_currentChanged(0);
}

MainWindow::~MainWindow()
{
    // free plugin instances
    for (int i=0;i< tabs.count();++i)
        delete tabs[i];

    // free plugins
    for (int i=0;i< ui->listPlugins->count();++i)
        delete (StellaControlPlugin*)ui->listPlugins->itemData(i).value<void*>();

    // free ui
    delete ui;
}

void MainWindow::errorMessage(const QString& msg)
{
    statusBar()->showMessage(msg);
}

void MainWindow::on_actionShow_tray_icon_toggled(bool v)
{
    if (v) trayIcon->show();
    else trayIcon->hide();
    ui->actionMinimize_to_tray->setEnabled(v);
    QSettings().setValue(QLatin1String("showTrayIcon"), v);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason==QSystemTrayIcon::Trigger) {
        if (this->isVisible())
            on_actionMinimize_to_tray_triggered();
        else
            on_actionRestore_triggered();
    }
}

void MainWindow::on_actionRestore_triggered()
{
    showNormal();
}

void MainWindow::on_actionMinimize_to_tray_triggered()
{
    hide();
}

void MainWindow::on_actionClose_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QString version;
    #if QT_VERSION >= 0x040400
    version = QCoreApplication::applicationVersion();
    #else
    version = tr("Unknown");
    #endif

    QMessageBox::about(this, tr("About %1").arg(QCoreApplication::applicationName()),
        tr("Controls software called stella_lights on the ethersex firmware "
           "for embedded devices with atmel avr processors. Stella is able to "
           "generate up to 8 pwm signals.\n\nVersion: ")+version);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 0)
    {
        // remove menu from mainwindow
        ui->menuBar->removeAction(ui->menuStella_host->menuAction());

    } else {
        // is the current widget a "loading" widget?
        stellaConnecting* loading = qobject_cast<stellaConnecting*>(ui->tabWidget->currentWidget());
        if (loading)
        {
            ui->menuStella_host->setTitle(ui->tabWidget->tabText(index));
            ui->menuStella_host->clear();
            ui->menuBar->removeAction(ui->menuStella_host->menuAction());
            return;
        }

        // ui update
        StellaControlPlugin* stellaplugin = tabs[index-1];
        Q_ASSERT (stellaplugin);

        ui->menuStella_host->setTitle(ui->tabWidget->tabText(index));
        ui->menuStella_host->clear();
        /* common actions */
        ui->menuStella_host->addAction(ui->actionCloseTab);
        ui->menuStella_host->addAction(ui->actionAboutConnection);
        /* plugin specific actions */
        QList<QAction*> actions = stellaplugin->menuActions();
        if (actions.size())
        {
            ui->menuStella_host->addAction(ui->actionSeparator);
            ui->menuStella_host->addActions(actions);
        }

        // add menu to mainwindow
        ui->menuBar->removeAction(ui->menuStella_host->menuAction());
        ui->menuBar->addAction(ui->menuStella_host->menuAction());
    }
}

StellaControlPlugin* MainWindow::currentTab()
{
    int index = ui->tabWidget->currentIndex()-1;
    if (index==-1) return 0;
    return tabs[index];
}

// if the host part of the stella server host+port is changed, update the port, too.
void MainWindow::on_lineIP_editTextChanged(const QString& host )
{
    QString settings_path;
    settings_path = QLatin1String("hosts/") + host + QLatin1String("/port_out");
    ui->spinPortOut->setValue(QSettings().value(settings_path, 2701).toInt());
    settings_path = QLatin1String("hosts/") + host + QLatin1String("/port_in");
    ui->spinPortIn->setValue(QSettings().value(settings_path, 2701).toInt());
}

// return true if port and host is valid
void MainWindow::on_btnConnect_clicked()
{
    // abort if no plugin is selected
    if (ui->listPlugins->currentIndex() == -1)
    {
        statusBar()->showMessage(tr("Failed: No Plugin selected!"));
        return;
    }

    // check host address, abort if not valid
    QHostAddress stella_host;
    if (!stella_host.setAddress(ui->lineIP->currentText()))
    {
        statusBar()->showMessage(tr("Failed: Host address not valid!"));
        return;
    }

    // add host+port to history
    QString settings_path;
    settings_path = QLatin1String("hosts/") + stella_host.toString() + QLatin1String("/port_out");
    QSettings().setValue(settings_path, ui->spinPortOut->value());
    settings_path = QLatin1String("hosts/") + stella_host.toString() + QLatin1String("/port_in");
    QSettings().setValue(settings_path, ui->spinPortIn->value());

    // determine connection id (host+port)
    QString connection_id = stella_host.toString() + QString::number(ui->spinPortOut->value());

    // load plugin, abort on error
    int index = ui->listPlugins->currentIndex();
    if (index==-1)
    {
        statusBar()->showMessage(tr("Failed: No plugin selected"));
        return;
    }

    // get StellaControlPlugin pointer from plugin list
    StellaControlPlugin* stellaplugin = (StellaControlPlugin*)ui->listPlugins->itemData(index).value<void*>();
    // create new instance (QObject*) and cast to StellaControlPlugin
    QObject* plugin = stellaplugin->createInstance();
    stellaplugin = qobject_cast<StellaControlPlugin *>(plugin);
    // connect signals
    connect(plugin, SIGNAL(errorMessage(const QString&)), SLOT(errorMessage(const QString&)));

    if (!plugin || !stellaplugin)
    {
        statusBar()->showMessage(tr("Failed: Plugin not ready"));
        return;
    }

    // create or use existing connection
    stellaConnection* connection;
    if (devices.contains(connection_id))
        connection = devices[connection_id];
    else {
        connection = new stellaConnection(stella_host, ui->spinPortOut->value(), ui->spinPortIn->value());
        devices[connection_id] = connection;
    }

    // create and add loading widget to tabWidget
    // will be deleted when replaced by the real plugin widget or when
    // the user abort
    stellaplugin->initInstance(connection);
    connection->addRef();
    stellaConnecting* loading = new stellaConnecting(connection, stellaplugin, /*position*/tabs.size());

    connect(loading, SIGNAL(aborted(StellaControlPlugin*,int)), this, SLOT(load_aborted(StellaControlPlugin*,int)));
    connect(loading, SIGNAL(connected(StellaControlPlugin*,int)), this, SLOT(load_connected(StellaControlPlugin*,int)));
    connect(loading, SIGNAL(statechanged(StellaControlPlugin*,int)), this, SLOT(load_statechanged(StellaControlPlugin*,int)));

    tabs.append(stellaplugin);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->addTab(loading, loading->getName()));
}

void MainWindow::on_listPlugins_currentIndexChanged(int index)
{
    if (index == -1)
        ui->btnConnect->setEnabled(false);
    else
        ui->btnConnect->setEnabled(true);
}

void MainWindow::on_actionBroadcastChanges_toggled(bool b)
{
    StellaControlPlugin* stellaplugin = currentTab();
    if (!stellaplugin) return;
    QString settings_path;
    settings_path = QLatin1String("hosts/") + stellaplugin->getConnection()->getDestIP() + QLatin1String("/broadcast");
    QSettings().setValue(settings_path, b);
    stellaplugin->getConnection()->setBroadcastFlag(b);
}

void MainWindow::on_actionAboutConnection_triggered()
{
    StellaControlPlugin* stellaplugin = currentTab();
    if (!stellaplugin) return;
    QString stats = tr("Server IP: %1\nServer Port: %2\nProtocol version: 1.%3\nChannels/Pins: %4"
           "\nBytes written: %5\nBytes read: %6\nConnection attempts: %7\nReferences: %8")
        .arg(stellaplugin->getConnection()->getDestIP())
        .arg(stellaplugin->getConnection()->getDestPort())
        .arg(stellaplugin->getConnection()->getProtocolVersion())
        .arg(stellaplugin->getConnection()->getChannelCount())
        .arg(stellaplugin->getConnection()->getBytesOut())
        .arg(stellaplugin->getConnection()->getBytesIn())
        .arg(stellaplugin->getConnection()->getConnectionAttempts())
        .arg(stellaplugin->getConnection()->getReferences());

    QMessageBox::information(this, tr("About %1").arg(stellaplugin->plugname()),
        stellaplugin->about()+QLatin1String("\n\n")+stats);
}

void MainWindow::on_actionCloseTab_triggered()
{
    closeTab(ui->tabWidget->currentIndex()-1);
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    closeTab(index-1);
}

void MainWindow::closeTab(int index)
{
    // error check
    if (index==-1 || index>=tabs.size()) return;

    // remove tab
    ui->tabWidget->removeTab(index+1);

    if (!tabs[index]) return;

    // remove one reference from the connection
    tabs[index]->getConnection()->removeRef();
    // if not referenced anymore, delete the connection
    if (!tabs[index]->getConnection()->isReferenced())
    {
        devices.remove(tabs[index]->getConnection()->getConnectionID());
        delete tabs[index]->getConnection();
    }

    // delete plugin instance
    delete tabs[index];
    tabs.removeAt(index);
}

void MainWindow::load_connected(StellaControlPlugin* plugin, int position)
{
    // let the plugin interface know, that we successfully
    // established a connection and want to show it now
    plugin->connectionEstablished();

    // restore "broadcast"-flag
    QString settings_path;
    settings_path = QLatin1String("hosts/") + plugin->getConnection()->getDestIP() + QLatin1String("/broadcast");
    plugin->getConnection()->setBroadcastFlag(QSettings().value(settings_path).toBool());

    // swap loading-widget with "plugin instance"-widget
    QWidget* loadingwidget = ui->tabWidget->widget(position+1);
    ui->tabWidget->removeTab(position+1);
    delete loadingwidget;
    ui->tabWidget->insertTab(position+1, plugin->widget(), tr("%1 (%2)")
                             .arg(plugin->plugname())
                             .arg(plugin->getConnection()->getDestIP()));
    ui->tabWidget->setCurrentIndex(position+1);
}

void MainWindow::load_aborted(StellaControlPlugin* plugin, int position)
{
    // remove loading widget
    QWidget* loadingwidget = ui->tabWidget->widget(position+1);
    ui->tabWidget->removeTab(position+1);
    // remove plugin
    tabs.removeAt(position);
    // remove one reference from the connection
    plugin->getConnection()->removeRef();
    // if not referenced anymore, delete the connection
    if (!plugin->getConnection()->isReferenced())
    {
        devices.remove(plugin->getConnection()->getConnectionID());
        delete plugin->getConnection();
    }
    // delete loadingWidget and plugin
    delete loadingwidget;
    delete plugin;
}

void MainWindow::load_statechanged(StellaControlPlugin* plugin, int position)
{
    Q_UNUSED(plugin);
    stellaConnecting* loading = qobject_cast<stellaConnecting*>(ui->tabWidget->widget(position+1));
    if (loading)
        ui->tabWidget->setTabText(position+1, loading->getName());
}
