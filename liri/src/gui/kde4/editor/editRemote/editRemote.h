#ifndef EDITREMOTEFILEDIALOG_H
#define EDITREMOTEFILEDIALOG_H
#include <QDialog>
#include "ui_edit_remote.h"
/* model/view signals */
#include <QModelIndex>

class Remotes_Model;
class BusConnection;
class RemoteFile;
class Settings_Model;
class Keys_Model;
class Receivers_Model;

class EditRemoteDialog : public QDialog, Ui::EditRemote {
	Q_OBJECT
	public:
		EditRemoteDialog(BusConnection* connection, RemoteFile* remotefile, QWidget *parent=0);
		~EditRemoteDialog();
	private:
		BusConnection* connection;
		RemoteFile* remotefile;
		Settings_Model* settings_model;
		Keys_Model* keys_model;
		Receivers_Model* receivers_model;
	private Q_SLOTS:
		void undoWarningColour();
		void btnSearch();
		void btnCancel();
		void btnFinish();
		void btnSettingsAdd();
		void btnSettingsRemove();
		void btnKeyAdd();
		void btnKeyRemove();
		void keysView_clicked ( const QModelIndex & index );
		void settingsView_clicked ( const QModelIndex & index );
		void keysView_keyfocus(const QModelIndex&);
		void settingsView_keyfocus(const QModelIndex&);
};

#endif
