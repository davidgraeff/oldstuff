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
#include "driverlistwindow.h"

#include <KDE/KLocale>

#include <QStringList>
#include <QByteArray>
#include <QHeaderView>
#include <QDir>

#include "config.h"
#include "businterconnect/BusConnection.h"

#include "DriverListModel.h"

DriverlistWindow::DriverlistWindow(BusConnection* busconnection, QWidget *parent) : QDialog(parent) {
	Q_UNUSED(busconnection);

	// ui
	setupUi(this);

	// watch files
//	fwatcher.addPath(filenameDevMan);
//	connect(&fwatcher, SIGNAL( fileChanged(const QString &) ), SLOT( logfileChanged(const QString &) ));

	//create models
	driverlistmodel = new DriverListModel();

	/* models */
	viewDriverList->setModel(driverlistmodel);
	viewDriverList->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	viewDriverList->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
}

DriverlistWindow::~DriverlistWindow() {
	delete driverlistmodel;
	emit closednow();
}

void DriverlistWindow::pathChanged(const QString & path) {
	Q_UNUSED(path);
}
