#ifndef EDITAPPPROFILE_H
#define EDITAPPPROFILE_H
#include <QDialog>
#include "ui_edit_application.h"

class ApplicationProfileFile;

class EditAppProfileDialog : public QDialog, Ui::EditApplication
{
  Q_OBJECT
	public:
		EditAppProfileDialog(ApplicationProfileFile* file, QWidget *parent=0);
		~EditAppProfileDialog();
		ApplicationProfileFile* getFile();
	private:
		ApplicationProfileFile* file;
	private Q_SLOTS:
		void undoWarningColour();
		void btnSearch();
		void btnCancel();
		void btnFinish();
};

#endif
