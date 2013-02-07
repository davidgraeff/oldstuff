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
#ifndef LIRI_LOGFILEMODEL_H_
#define LIRI_LOGFILEMODEL_H_

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QFile>
#include <kicon.h>

class TextLine {
	public:
		QString text;
		QString datetime;
		KIcon* typeicon;
		QString* type;
		TextLine(const QByteArray& text, const QByteArray& datetime, KIcon* typeicon, QString* type) {
			this->text = QString::fromUtf8(text);
			this->datetime = QString::fromUtf8(datetime);
			this->typeicon = typeicon; this->type = type;
		}
		TextLine(const QByteArray& text, KIcon* typeicon) {
			this->text = QString::fromUtf8(text); this->typeicon = typeicon;
		}
};

class LogfileModel : public QAbstractTableModel {
     Q_OBJECT
public:
	LogfileModel(const QString& filename);
	~LogfileModel();
	void reload();
	void clearfile();
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
			    int role = Qt::DisplayRole) const;
Q_SIGNALS:
	void saveFeedback(QFile::FileError);

private:
	QString filename;
	QList<TextLine> lines;
	KIcon iconDebug;
	KIcon iconCritical;
	KIcon iconWarning;
	KIcon iconUnknown;
	QString typeDebug;
	QString typeCritical;
	QString typeWarning;
	QString typeUnknown;
};

#endif
