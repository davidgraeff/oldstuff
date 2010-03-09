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
#ifndef SELECTREMOTEWINDOW_H
#define SELECTREMOTEWINDOW_H
#include <QDialog>
#include "ui_selectremotewindow.h"
#include <QFileSystemWatcher>
#include <QFile>

class BusConnection;
class RemotesModel;

class SelectRemoteWindow : public QDialog, public Ui::SelectRemoteWindow
{
  Q_OBJECT
public:
	SelectRemoteWindow(BusConnection* busconnection, const QString & rid, const QString& remoteuid, QWidget *parent=0);
	~SelectRemoteWindow();

private:
	QFileSystemWatcher fwatcher;
	RemotesModel* remotesmodel;
	BusConnection* busconnection;
	const QString & rid;
	bool initok;
private Q_SLOTS:
	// to react on signals from QFileSystemWatcher
	void pathChanged(const QString & path);
	void deviceRemoved(const QString & rid);
	void currentIndexChanged(int index);
Q_SIGNALS:
	void closednow();
};

#endif
