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
#include "selectremotewindow.h"

#include <KDE/KLocale>

#include <QStringList>
#include <QByteArray>
#include <QHeaderView>
#include <QDir>

#include "config.h"
#include "businterconnect/BusConnection.h"
#include "businterconnect/Control_DevManager_dbusproxy.h"
#include "businterconnect/Control_Execution_dbusproxy.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"
#include "businterconnect/Receiver_Execution_dbusproxy.h"

#include "RemotesModel.h"

SelectRemoteWindow::SelectRemoteWindow(BusConnection* busconnection, int rid, const QString& remoteuid, QWidget *parent) :
	QDialog(parent), busconnection(busconnection), rid(rid)
{
	// ui
	setupUi(this);

	// watch files
	fwatcher.addPath(QLatin1String(LIRI_SYSTEM_REMOTES_DIR));
	connect(&fwatcher, SIGNAL( fileChanged(const QString &) ), SLOT( pathChanged(const QString &) ));
	connect(busconnection, SIGNAL( deviceRemoved(int) ), SLOT( deviceRemoved(int) ));
	connect(comboBox, SIGNAL( currentIndexChanged(int) ), SLOT( currentIndexChanged(int) ));

	initok = false;

	//create models
	remotesmodel = new RemotesModel();

	/* models */
	comboBox->setModel(remotesmodel);

	/* set to remote of rid device */
	comboBox->setCurrentIndex(remotesmodel->getIndexOf(remoteuid));

	initok = true;
}

SelectRemoteWindow::~SelectRemoteWindow() {
	delete remotesmodel;
	emit closednow();
}

void SelectRemoteWindow::deviceRemoved(int rid) {
	// the device we are currently editing doesn't exist anymore
	// -> close this modal dialog
	if (this->rid == rid) close();
}

void SelectRemoteWindow::currentIndexChanged(int index) {
	// do nothing during the init phase
	if (!initok) return;

	QString remoteuid = remotesmodel->getUidOf(index);
	OrgLiriDevManagerReceiverInterface *iface = busconnection->getDeviceManagerReceiver(rid);

	if (iface)
		iface->setAssociatedRemote(remoteuid);

	close();
}

void SelectRemoteWindow::pathChanged(const QString & path) {
	Q_UNUSED(path);
	remotesmodel->reload();
}
