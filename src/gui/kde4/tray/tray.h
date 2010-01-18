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
#include <ksystemtrayicon.h>
#include <kicon.h>
#include <kaboutapplicationdialog.h>

class BusConnection;
class ExecutionConnection;
class KAutostart;
class KDialog;
class QListWidget;

class TrayIconClient : public KSystemTrayIcon
{
  Q_OBJECT
public:
	TrayIconClient(const KIcon& icon, const KIcon& icon_event, BusConnection* connection);
    ~TrayIconClient();
private:
	QAction* autostartAction;
	const KIcon icon;
	const KIcon& icon_event;
	KAboutApplicationDialog* aboutdialog;
	KDialog* notifyWidget;
	BusConnection* connection;
	QString splitter(const QString& all, const QByteArray& search);
public Q_SLOTS:
	// framework notifications
	void devicemanagerStateChanged(int state);
	void executionengineStateChanged(int state);
	void deviceAdded(int rid);
	void deviceRemoved(int rid);
	void executed(int receiverinstance, int result, const QString &executed);
	void modeChanged(int receiverinstance, const QString &oldmode, const QString &newmode);
	void targetChanged(int state, const QString &targetname);
	void driverChangedSettings(const QMap<QString,QString> &changedsettings);
	void key(const QString &keycode, const QString &keyname, uint channel, int pressed);
	void receiverStateChanged(int state);

	// tray menu
	void execEditor();
	void execControl();
	void execConfigureNotifications();
	void execAbout();

	// application
	void anotherInstance();
	void activated ( QSystemTrayIcon::ActivationReason reason );
};

#endif
