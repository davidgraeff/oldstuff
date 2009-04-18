#include "editDesktopProfile/editDesktopProfile.h"
#include "fileformats/DesktopProfileFile.h"
#include "DP_Model.h"
#include <QTimer>
#include <QDebug>
#include <QDir>
#include "config.h"

EditDesktopProfileDialog::EditDesktopProfileDialog(DesktopProfileFile* file, QWidget *parent) : QDialog(parent), file(file) {
	setupUi( this );
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(btnFinish()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(btnCancel()));

	/* sync with text fields */
	txtname->setText(file->getName());
	txtcomment->setPlainText(file->getComment());
	txtauthors->setPlainText(file->getAuthors());
	txticon->setText(file->getIconFilename());

	lblStatus->setText(QLatin1String("Ready"));
}


EditDesktopProfileDialog::~EditDesktopProfileDialog() {
}

void EditDesktopProfileDialog::undoWarningColour() {
	txtname->setStyleSheet(QString());
	txtauthors->setStyleSheet(QString());
}

void EditDesktopProfileDialog::btnSearch() {
}

void EditDesktopProfileDialog::btnCancel() {
	reject();
}

void EditDesktopProfileDialog::btnFinish() {
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
