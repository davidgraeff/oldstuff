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
#ifndef DRIVERLISTWINDOW_H
#define DRIVERLISTWINDOW_H
#include <QDialog>
#include "ui_driverlistwindow.h"
#include <QFileSystemWatcher>
#include <QFile>

class BusConnection;
class DriverListModel;

class DriverlistWindow : public QDialog, public Ui::DriverlistWindow
{
  Q_OBJECT
public:
	DriverlistWindow(BusConnection* busconnection, QWidget *parent=0);
	~DriverlistWindow();

private:
	QFileSystemWatcher fwatcher;
	DriverListModel* driverlistmodel;
private Q_SLOTS:
	// to react on signals from QFileSystemWatcher
	void pathChanged(const QString & path);

Q_SIGNALS:
	void closednow();
};

#endif
