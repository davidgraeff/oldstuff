#include "ApplicationProfileActionModel.h"
#include "ApplicationAction.h"

ApplicationProfileActionModel::ApplicationProfileActionModel(Action* action, QObject *parent) : QAbstractListModel(parent), action(action) {
}

ApplicationProfileActionModel::~ApplicationProfileActionModel() {

}

bool ApplicationProfileActionModel::removeRows ( int, int, const QModelIndex & ) {
	return false;
}

int ApplicationProfileActionModel::rowCount(const QModelIndex &) const {
	return action->cmds.size();
}

int ApplicationProfileActionModel::columnCount ( const QModelIndex &  ) const {
	return 2;
}

QVariant ApplicationProfileActionModel::data(const QModelIndex &index, int role) const {
	QVariant theData;
	if ( !index.isValid() ) {
	    return QVariant();
	}

	liri::Commando* cmd = action->cmds.value(index.row());

	switch ( role ) {
	    case Qt::DisplayRole:
			if (index.column() == 0) {
				if (cmd->type == liri::ProgramType) {
					theData.setValue( i18n("Program") );
				}
				else if (cmd->type == liri::IPCType) {
					theData.setValue( i18n("IPC") );
				}
				else if (cmd->type == liri::ChangeMode) {
					theData.setValue( i18n("Mode") );
				}
			} else if (index.column() == 1)
				theData.setValue( cmd->command );
			break;
	    case Qt::ToolTipRole:
			if (index.column() == 0) {
				if (cmd->type == liri::ProgramType) {
					theData.setValue( i18n("Run external program") );
				}
				else if (cmd->type == liri::IPCType) {
					theData.setValue( i18n("Interprocess Communication Command (see DBUS)") );
				}
				else if (cmd->type == liri::ChangeMode) {
					theData.setValue( i18n("Change Mode") );
				}
			}
			break;
	    default:
			break;
	}
    return theData;
}

QVariant ApplicationProfileActionModel::headerData(int section,
	Qt::Orientation orientation, int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
	switch (section) {
		case 0: return i18n("Type");
		case 1: return i18n("Execute");
	}
	return QVariant();
}

