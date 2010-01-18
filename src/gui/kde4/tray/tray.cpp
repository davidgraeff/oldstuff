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
#include "tray.h"

#include <KDE/KLocale>
#include <KApplication>
#include <kaction.h>
#include <kstandardaction.h>
#include <kcmdlineargs.h>
#include <knotification.h>
#include <knotifyconfigwidget.h>
#include <kcomponentdata.h>
#include <ktoolinvocation.h>
#include <kdialog.h>

#include <QMenu>

#include <QDebug>

#include "businterconnect/BusConnection.h"
#include "businterconnect/Control_DevManager_dbusproxy.h"
#include "businterconnect/Control_Execution_dbusproxy.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"
#include "businterconnect/Receiver_Execution_dbusproxy.h"
#include "config.h"

TrayIconClient::TrayIconClient(const KIcon& icon, const KIcon& icon_event, BusConnection* connection) : KSystemTrayIcon (0), icon(icon), icon_event(icon_event), connection(connection) {

	KAction* action = new KAction(KIcon(QLatin1String("liri-controlcenter")), i18n("&Editor"), this);
	action->setShortcut(i18n("Ctrl+E"));
	QObject::connect((QObject*)action, SIGNAL(triggered()), this, SLOT(execEditor()));
	contextMenu()->addAction(action);

	action = new KAction(KIcon(QLatin1String("document-preview")), i18n("Liri &Control"), this);
	action->setShortcut(i18n("Ctrl+C"));
	QObject::connect((QObject*)action, SIGNAL(triggered()), this, SLOT(execControl()));
	contextMenu()->addAction(action);

	contextMenu()->addAction(KStandardAction::configureNotifications(this, SLOT(execConfigureNotifications()), this));

	contextMenu()->addAction(KStandardAction::aboutApp(this, SLOT(execAbout()), this));

	aboutdialog = new KAboutApplicationDialog(KCmdLineArgs::aboutData(),0);

	connect(this, SIGNAL( activated (QSystemTrayIcon::ActivationReason) ), this, SLOT( activated (QSystemTrayIcon::ActivationReason) ));

	notifyWidget=new KDialog(0);
	notifyWidget->setCaption(i18n("Configure Notifications"));
	KNotifyConfigWidget *w=new KNotifyConfigWidget(notifyWidget);
	notifyWidget->setMainWidget(w);
	connect(notifyWidget,SIGNAL(applyClicked()),w,SLOT(save()));
	connect(notifyWidget,SIGNAL(okClicked()),w,SLOT(save()));
	connect(w,SIGNAL(changed(bool)) , notifyWidget , SLOT(enableButtonApply(bool)));
	w->setApplication( KGlobal::mainComponent().componentName() );

	connect(connection, SIGNAL( devicemanagerStateChanged(int) ), SLOT( devicemanagerStateChanged(int) ));
	connect(connection, SIGNAL( executionengineStateChanged(int) ), SLOT( executionengineStateChanged(int) ));
	connect(connection, SIGNAL( deviceAdded(int) ), SLOT( deviceAdded(int) ));
	connect(connection, SIGNAL( deviceRemoved(int) ), SLOT( deviceRemoved(int) ));
	connect(connection, SIGNAL( executed(int, int, const QString &) ), SLOT( executed(int, int, const QString &) ));
	connect(connection, SIGNAL( modeChanged(int, const QString &, const QString &) ), SLOT( modeChanged(int, const QString &, const QString &) ));
	connect(connection, SIGNAL( targetChanged(int, const QString &) ), SLOT( targetChanged(int, const QString &) ));

	setIcon(icon);
	setToolTip(i18n("Liri Tray Client"));
	show();

	QList<int> receivers = connection->receivers();
	foreach(int rid, receivers) emit deviceAdded(rid);
}

TrayIconClient::~TrayIconClient() {
	delete aboutdialog;
	delete notifyWidget;
}

void TrayIconClient::anotherInstance() {
	KNotification *notification = new KNotification(QLatin1String("alreadyRunning"));
	notification->setText(i18n("You can start only one instance of this program per user"));
 	notification->setComponentData(KGlobal::mainComponent());
    notification->sendEvent();
}

void TrayIconClient::activated ( QSystemTrayIcon::ActivationReason reason ) {
	if (reason == QSystemTrayIcon::Trigger) {
		execControl();
	}
}

void TrayIconClient::execAbout() {
	aboutdialog->show();
}

void TrayIconClient::execEditor() {
	KToolInvocation::kdeinitExec(QLatin1String("liri-editor-kde4"));
}

void TrayIconClient::execControl() {
	KToolInvocation::kdeinitExec(QLatin1String("liri-control-kde4"));
}

void TrayIconClient::execConfigureNotifications() {
	notifyWidget->show();
}



void TrayIconClient::devicemanagerStateChanged(int state) {
	Q_UNUSED(state);
	//qDebug() << "devicemanagerStateChanged" << state;
}

void TrayIconClient::executionengineStateChanged(int state) {
	Q_UNUSED(state);
	//qDebug() << "executionengineStateChanged" << state;
}

void TrayIconClient::deviceAdded(int rid) {
	//qDebug() << "deviceAdded" << rid;
	OrgLiriDevManagerReceiverInterface * const ri = connection->getDeviceManagerReceiver(rid);
	if (!ri) return;
	connect(ri, SIGNAL( driverChangedSettings(const QMap<QString,QString> &) ),
		SLOT( driverChangedSettings(const QMap<QString,QString> &) ));
	connect(ri, SIGNAL( key(const QString &, const QString &, uint, int) ),
		SLOT( key(const QString &, const QString &, uint, int) ));
	connect(ri, SIGNAL( receiverStateChanged(int) ),
		SLOT( receiverStateChanged(int) ));
}

void TrayIconClient::deviceRemoved(int rid) {
	Q_UNUSED(rid);
	//qDebug() << "deviceRemoved" << rid;
}

void TrayIconClient::executed(int receiverinstance, int result, const QString &executed) {
	Q_UNUSED(receiverinstance);
	if (result == 0) {
		KNotification *notification = new KNotification(QLatin1String("commandSuccessful"));
		notification->setText(i18n("Command successful: \"%1\"", executed));
   	notification->setComponentData(KGlobal::mainComponent());
		notification->sendEvent();
	} else {
		KNotification *notification = new KNotification(QLatin1String("commandFailure"));
		notification->setText( i18n("Command failure: \"%1\". Errcode: %2", executed, QString::number(result)) );
   	notification->setComponentData(KGlobal::mainComponent());
		notification->sendEvent();
	}
}

void TrayIconClient::modeChanged(int receiverinstance, const QString &oldmode, const QString &newmode) {
	KNotification *notification = new KNotification(QLatin1String("ModeChanged"));
	notification->setText(i18n("Mode of instance %1 changed from \"%2\" to \"%3\"", QString::number(receiverinstance), oldmode, newmode));
  	notification->setComponentData(KGlobal::mainComponent());
	notification->sendEvent();
}

void TrayIconClient::targetChanged(int state, const QString &targetname) {
	Q_UNUSED(state);
	Q_UNUSED(targetname);
	//qDebug() << "targetChanged" << state << targetname;
}

void TrayIconClient::driverChangedSettings(const QMap<QString,QString> &changedsettings) {
	Q_UNUSED(changedsettings);
	//qDebug() << "driverChangedSettings" << changedsettings;
}

void TrayIconClient::key(const QString &keycode, const QString &keyname, uint channel, int pressed) {
	Q_UNUSED(channel);
	Q_UNUSED(keycode);
	Q_UNUSED(keyname);
	if (pressed) {
		setIcon(icon_event);
	} else {
		setIcon(icon);
	}
}

void TrayIconClient::receiverStateChanged(int state) {
	if (state == LIRI_DEVICE_RUNNING_WITH_LAYOUT) {
		KNotification *notification = new KNotification(QLatin1String("receiverPluggedInLayout"));
		notification->setText( i18n("Receiver plugged in with layout.") );
 		notification->setComponentData(KGlobal::mainComponent());
		notification->sendEvent();
	}
	else if (state == LIRI_DEVICE_RUNNING_WITHOUT_LAYOUT) {
		KNotification *notification = new KNotification(QLatin1String("receiverPluggedIn"));
		notification->setText( i18n("Receiver plugged in without layout.") );
 		notification->setComponentData(KGlobal::mainComponent());
		notification->sendEvent();
	}
	else if (state == LIRI_DEVICE_OFFLINE) {
		KNotification *notification = new KNotification(QLatin1String("receiverPluggedOut"));
		notification->setText( i18n("Receiver plugged out") );
 		notification->setComponentData(KGlobal::mainComponent());
		notification->sendEvent();
	}
	else if (state < 0) {
		KNotification *notification = new KNotification(QLatin1String("receiverFailure"));
		notification->setText( i18n("Receiver error.\nError number: %1", state) );
 		notification->setComponentData(KGlobal::mainComponent());
		notification->sendEvent();
	}
}

