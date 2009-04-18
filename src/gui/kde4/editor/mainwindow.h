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
/* window */
#include <KXmlGuiWindow>
#include "ui_mainwindow.h"
/* model/view signals */
#include <QModelIndex>

class AP_Model;
class DP_Model;
class Remotes_Model;
class BusConnection;
class RemoteFile;
class ApplicationProfileFile;
class DesktopProfileFile;
class KAction;

class MainWindow : public KXmlGuiWindow
{
  Q_OBJECT
public:
    MainWindow(QWidget *parent=0);
    ~MainWindow();

private:
	Ui::MainWindow *ui;
	AP_Model* ap_model;
	DP_Model* dp_model;
	Remotes_Model* remotes_model;

	// update on click of a view
	RemoteFile* remotefile;
	ApplicationProfileFile* appfile;
	DesktopProfileFile* desktopfile;

	BusConnection* busconnection;

	KAction* actionRemoteNew;
	KAction* actionRemoteEdit;
	KAction* actionRemoteRemove;
	KAction* actionRemoteDownload;
	KAction* actionDPNew;
	KAction* actionDPEdit;
	KAction* actionDPRemove;
	KAction* actionDPDownload;
	KAction* actionAPNew;
	KAction* actionAPEdit;
	KAction* actionAPRemove;
	KAction* actionAPDownload;

private Q_SLOTS:
	void chkControlAll_stateChanged(int);
	void remotes_clicked ( const QModelIndex & index );
	void remotes_doubleClicked ( const QModelIndex & index );
	void dp_clicked ( const QModelIndex & index );
	void dp_doubleClicked ( const QModelIndex & index );
	void ap_clicked ( const QModelIndex & index );
	void ap_doubleClicked ( const QModelIndex & index );

	void saveRemotefile(RemoteFile* file,bool deleteOnFailure);
	void saveAppfile(ApplicationProfileFile* file,bool deleteOnFailure);
	void saveDesktopfile(DesktopProfileFile* file,bool deleteOnFailure);

	void btnRemoteNew();
	void btnRemoteEdit();
	void btnRemoteRemove();
	void btnRemoteDownload();

	void btnDPNew();
	void btnDPEdit();
	void btnDPRemove();
	void btnDPDownload();

	void btnAPNew();
	void btnAPEdit();
	void btnAPRemove();
	void btnAPDownload();

};

#endif
