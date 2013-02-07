#ifndef EDITDESKTOPPROFILE_H
#define EDITDESKTOPPROFILE_H
#include <QDialog>
#include "ui_edit_desktop.h"

class DesktopProfileFile;

class EditDesktopProfileDialog : public QDialog, Ui::EditDesktop
{
  Q_OBJECT
	public:
		EditDesktopProfileDialog(DesktopProfileFile* file, QWidget *parent=0);
		~EditDesktopProfileDialog();
		DesktopProfileFile* getFile();
	private:
		DesktopProfileFile* file;
	private Q_SLOTS:
		void undoWarningColour();
		void btnSearch();
		void btnCancel();
		void btnFinish();
};

#endif
