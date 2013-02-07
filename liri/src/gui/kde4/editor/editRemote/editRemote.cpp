#include "editRemote/editRemote.h"
#include "fileformats/RemoteFile.h"
#include "Remotes_Model.h"

#include "businterconnect/BusConnection.h"
#include "businterconnect/Control_DevManager_dbusproxy.h"
#include "businterconnect/Control_Execution_dbusproxy.h"
#include "businterconnect/Receiver_DevManager_dbusproxy.h"
#include "businterconnect/Receiver_Execution_dbusproxy.h"

#include "editRemote/Settings_Model.h"
#include "editRemote/Keys_Model.h"
#include "editRemote/Receivers_Model.h"

#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QDBusReply>
#include <QList>

#include "config.h"

EditRemoteDialog::EditRemoteDialog(BusConnection* connection, RemoteFile* remotefile, QWidget *parent) : QDialog(parent), connection(connection), remotefile(remotefile) {
	setupUi( this );
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(btnFinish()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(btnCancel()));
	connect(Ui_EditRemote::btnSettingsAdd, SIGNAL(clicked()), this, SLOT(btnSettingsAdd()));
	connect(Ui_EditRemote::btnSettingsRemove, SIGNAL(clicked()), this, SLOT(btnSettingsRemove()));
	connect(Ui_EditRemote::btnKeyAdd, SIGNAL(clicked()), this, SLOT(btnKeyAdd()));
	connect(Ui_EditRemote::btnKeyRemove, SIGNAL(clicked()), this, SLOT(btnKeyRemove()));
	Ui_EditRemote::btnKeyRemove->setEnabled(false);
	Ui_EditRemote::btnSettingsRemove->setEnabled(false);

	settings_model = new Settings_Model(remotefile, this);
	keys_model = new Keys_Model(remotefile, this);
	receivers_model = new Receivers_Model(connection, this);
	connect(keys_model, SIGNAL(keyfocus(const QModelIndex&)), SLOT(keysView_keyfocus(const QModelIndex&)));
	connect(settings_model, SIGNAL(keyfocus(const QModelIndex&)), SLOT(settingsView_keyfocus(const QModelIndex&)));
	connect(receivers_model, SIGNAL( keyevent(const QString &) ), keys_model, SLOT( keyevent(const QString &) ));

	settingsView->setModel(settings_model);
	keysView->setModel(keys_model);
	receiversView->setModel(receivers_model);
	connect(Ui_EditRemote::settingsView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(settingsView_clicked(const QModelIndex &)));
	connect(Ui_EditRemote::keysView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(keysView_clicked(const QModelIndex &)));

	/* sync with text fields */
	txtname->setText(remotefile->getName());
	txtcomment->setPlainText(remotefile->getComment());
	txtauthors->setPlainText(remotefile->getAuthors());
	txticon->setText(remotefile->getIconFilename());

	lblStatus->setText(QLatin1String("Ready"));
}

EditRemoteDialog::~EditRemoteDialog() {}

void EditRemoteDialog::undoWarningColour() {
	txtname->setStyleSheet(QString());
	txtauthors->setStyleSheet(QString());
}

void EditRemoteDialog::btnSearch() {

}

void EditRemoteDialog::btnCancel() {
	reject();
}

void EditRemoteDialog::btnFinish() {
	/* check for conditions */
	bool meetallcon = true;

	if (!txtname->text().size()) {
		txtname->setStyleSheet(QLatin1String("background-color: rgb(255, 0, 0);"));
		meetallcon = false;
		lblStatus->setText(QLatin1String("Name is missing"));
	}

	if (!txtauthors->toPlainText().size()) {
		txtauthors->setStyleSheet(QLatin1String("background-color: rgb(255, 0, 0);"));
		meetallcon = false;
		lblStatus->setText(QLatin1String("Authors are missing"));
	}

	if (!meetallcon) {
		QTimer::singleShot(1500, this, SLOT(undoWarningColour()));
		return;
	}

	/* sync with text fields */
	remotefile->setName(txtname->text());
	remotefile->setComment(txtcomment->toPlainText());
	remotefile->setAuthors(txtauthors->toPlainText());
	remotefile->setIconFilename(txticon->text());

	accept();
}

void EditRemoteDialog::btnSettingsAdd() {
	settings_model->addSetting();
}

void EditRemoteDialog::btnSettingsRemove() {
	foreach (QModelIndex index, settingsView->selectionModel()->selectedIndexes())
		settings_model->removeRow(index.row(), QModelIndex());
	if (!settings_model->rowCount())
		Ui_EditRemote::btnSettingsRemove->setEnabled(false);
}

void EditRemoteDialog::btnKeyAdd() {
	keys_model->addKey();
}

void EditRemoteDialog::btnKeyRemove() {
	foreach (QModelIndex index, keysView->selectionModel()->selectedIndexes())
		keys_model->removeRow(index.row(), QModelIndex());
	if (!keys_model->rowCount())
		Ui_EditRemote::btnKeyRemove->setEnabled(false);
}

void EditRemoteDialog::keysView_keyfocus(const QModelIndex & index) {
	keysView->setCurrentIndex(QModelIndex());
	keysView->setCurrentIndex(index);
	keysView->edit(index);
}

void EditRemoteDialog::settingsView_keyfocus(const QModelIndex & index) {
	settingsView->setCurrentIndex(index);
	settingsView->edit(index);
}

void EditRemoteDialog::keysView_clicked ( const QModelIndex & index ) {
	if (!index.isValid()) return;
	Ui_EditRemote::btnKeyRemove->setEnabled(true);
}

void EditRemoteDialog::settingsView_clicked ( const QModelIndex & index ) {
	if (!index.isValid()) return;
	Ui_EditRemote::btnSettingsRemove->setEnabled(true);
}

