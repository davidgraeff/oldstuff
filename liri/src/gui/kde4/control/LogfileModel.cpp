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
#include <QDebug>

#include <QStringList>
#include <KDE/KLocale>

#include "config.h"
#include "LogfileModel.h"


LogfileModel::LogfileModel(const QString& filename) : filename(filename) {
	iconDebug = KIcon(QLatin1String("dialog-information"));
	iconWarning = KIcon(QLatin1String("dialog-warning"));
	iconCritical = KIcon(QLatin1String("dialog-error"));
	iconUnknown = KIcon(QLatin1String("image-missing"));
	typeDebug = i18n("Debug");
	typeWarning = i18n("Warning");
	typeCritical = i18n("Critical");
	typeUnknown = i18n("Unknown");
	reload();
}

LogfileModel::~LogfileModel() {

}

void LogfileModel::reload() {
	lines.clear();
 	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		reset(); // reset model
		return;
	}

	int pos = 0;
	int pos2 = 0;
	QByteArray type;
	QByteArray datetime;
	QByteArray text;
	KIcon* icontype;
	QString* typetext;
	// Logfile line format: "TYPE (DATETIME): TEXT"
	while (!file.atEnd()) {
		QByteArray line = file.readLine();
		// get type
		pos = line.indexOf(' ');
		if (pos == -1) {lines.append(TextLine(line, &iconUnknown)); continue;} // format not parseable
		type = line.mid(0, pos);

		// determine type icon and text
		if (type[0]=='W') { icontype = &iconWarning; typetext = &typeWarning; }
		else if (type[0]=='D') { icontype = &iconDebug; typetext = &typeDebug; }
		else if (type[0]=='C') { icontype = &iconCritical; typetext = &typeCritical; }
		else { icontype = &iconUnknown; typetext = &typeUnknown; }

		// get datetime
		pos  = line.indexOf('(', pos);
		pos2 = line.indexOf(')', pos);
		if (pos == -1 || pos2 == -1) {lines.append(TextLine(line, &iconUnknown)); continue;} // format not parseable
		datetime = line.mid(pos+1,pos2-pos-1);
		// get text
		pos2 += 3;
		text = line.mid(pos2, line.indexOf('\n', pos2)-pos2);

		// append to list
		lines.prepend(TextLine(text, datetime, icontype, typetext));
	}

	reset(); // reset model
}

void LogfileModel::clearfile() {
	QFile file(filename);
	if (file.resize(0)) {
		reload();
		emit saveFeedback(QFile::NoError);
	} else {
		emit saveFeedback(file.error());
	}
}

int LogfileModel::rowCount(const QModelIndex &) const {
	return lines.size();
}

int LogfileModel::columnCount(const QModelIndex &) const {
	return 2;
}

QVariant LogfileModel::data(const QModelIndex &index, int role) const {
	if ( !index.isValid() ) {
		return QVariant();
	}

	const TextLine* line = &(lines[index.row()]);

	switch ( role ) {
		case Qt::DecorationRole:
			if (index.column()==1) return *(static_cast<QIcon*>(line->typeicon));
			break;
		case Qt::DisplayRole:
			switch (index.column()) {
				case 0: return line->datetime;
				case 1: return line->text; qDebug() << "text" << line->text;
				default: break;
			}
			break;
		case Qt::ToolTipRole:
			if (index.column()==1) return *(line->type);
			break;
		default:
			break;
	}
	return QVariant();
}

QVariant LogfileModel::headerData(int section, Qt::Orientation orientation,
                         int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		if (section==0)
			return i18n("Time"); // datetime
		else if (section==1)
			return i18n("Text"); // text
	}

	return QVariant();
}

