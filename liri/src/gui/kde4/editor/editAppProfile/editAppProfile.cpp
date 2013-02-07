#include "editAppProfile/editAppProfile.h"
#include "fileformats/ApplicationProfileFile.h"
#include "AP_Model.h"
#include <QTimer>
#include <QDebug>
#include <QDir>
#include "config.h"

EditAppProfileDialog::EditAppProfileDialog(ApplicationProfileFile* file, QWidget *parent) : QDialog(parent), file(file) {
	setupUi( this );
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(btnFinish()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(btnCancel()));

	/* sync with text fields */
	txtname->setText(file->getName());
	txtcomment->setPlainText(file->getComment());
	txtauthors->setPlainText(file->getAuthors());
	txticon->setText(file->getIconFilename());

	lblStatus->setText(QLatin1String("Ready"));

	//viewVariables->setModel  (new Variables_Model(file, this));
	//viewBusservices->setModel(new Busservices_Model(file, this));
}


EditAppProfileDialog::~EditAppProfileDialog() {}

void EditAppProfileDialog::undoWarningColour() {
	txtname->setStyleSheet(QString());
	txtauthors->setStyleSheet(QString());
}

void EditAppProfileDialog::btnSearch() {
}

void EditAppProfileDialog::btnCancel() {
	reject();
}

void EditAppProfileDialog::btnFinish() {
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
	file->setName(txtname->text());
	file->setComment(txtcomment->toPlainText());
	file->setAuthors(txtauthors->toPlainText());
	file->setIconFilename(txticon->text());

	accept();
}
