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
#include "fileformats/DesktopFile.h"
#include <QDir>
#include <QFile>
#include <QUuid>
#include <QFileInfo>
#include <QDebug>

DesktopFile::DesktopFile(const QString& uid) : uid(uid) {
	if (!uid.size()) {
		this->uid = QUuid::createUuid().toString();
		this->uid = this->uid.replace(QLatin1Char('{'), QString()).replace(QLatin1Char('}'), QString());
	}
}

QString DesktopFile::getUidOfFilename(const QString& filename) {
	return QFileInfo(filename).baseName();
}

const QString& DesktopFile::getUid() const {
	return uid;
}

const QString DesktopFile::getFilename() const {
	if (!uid.size()) return QString();
	return QDir(path()).absoluteFilePath(uid + QLatin1String(".desktop"));
}

QString DesktopFile::localekey(const QString& nsp ) const {
	return nsp + ( (locale.size()) ? QLatin1Char('[')+locale+QLatin1Char(']') : QString() );
}

QString DesktopFile::localekey(const QString& nsp, const QString& locale ) const {
	return nsp + ( (locale.size()) ? QLatin1Char('[')+locale+QLatin1Char(']') : QString() );
}

void DesktopFile::setLocale(const QString& locale) {
	this->locale = locale;
}

DesktopFile::DesktopFileState DesktopFile::getState() const {
	return state;
}

void DesktopFile::setName(const QString& name) {
	if (!name.size())
		names.erase(names.find(localekey(QLatin1String("Name"))));
	else
		names[localekey(QLatin1String("Name"))] = name;
}

const QString DesktopFile::getName() const {
	QMap< QString, QString >::const_iterator it = names.find(localekey(QLatin1String("Name")));
	if (it != names.end())
		return it.value();
	else if (!locale.size())
		return QString();

	it = names.find(QLatin1String("Name"));
	if (it == names.end())
		return QString();
	else
		return it.value();
}

void DesktopFile::setGenericName(const QString& name) {
	if (!name.size())
		genericnames.erase(genericnames.find(localekey(QLatin1String("GenericName"))));
	else
		genericnames[localekey(QLatin1String("GenericName"))] = name;
}

const QString DesktopFile::getGenericName() const {
	QMap< QString, QString >::const_iterator it = genericnames.find(localekey(QLatin1String("GenericName")));
	if (it != genericnames.end())
		return it.value();
	else if (!locale.size())
		return QString();

	it = genericnames.find(QLatin1String("GenericName"));
	if (it == genericnames.end())
		return QString();
	else
		return it.value();
}

void DesktopFile::setComment(const QString& comment) {
	if (!comment.size())
		comments.erase(comments.find(localekey(QLatin1String("Comment"))));
	else
		comments[localekey(QLatin1String("Comment"))] = comment;
}

const QString DesktopFile::getComment() const {
	QMap< QString, QString >::const_iterator it = comments.find(localekey(QLatin1String("Comment")));
	if (it != comments.end())
		return it.value();
	else if (!locale.size())
		return QString();

	it = comments.find(QLatin1String("Comment"));
	if (it == comments.end())
		return QString();
	else
		return it.value();
}

void DesktopFile::setAuthors(const QString& authors) {
	this->authors = authors;
}

const QString DesktopFile::getAuthors() const {
	return authors;
}

void DesktopFile::setIconFilename(const QString& iconfilename) {
	iconfile = iconfilename;
}

const QString& DesktopFile::getIconFilename() const {
	return iconfile;
}

void DesktopFile::clear() {
	changed = false;
	locale = QLatin1String(getenv("LANG"));
	locale = locale.left(locale.indexOf(QLatin1Char('.')));
	authors = QString();
	names.clear();
	comments.clear();
	version = 0.0;
	state = Empty;
}

bool DesktopFile::reload() {
	/* clear */
	clear();
	QString filename = getFilename();

	if (!filename.size()) return false;

	/* some variables */
	QByteArray line;
	QString group;
	QString key;
	QString keylocale;
	QString value;
	int found;
	int found2;

	/* open file */
	QFile inifile(filename);
	if (!inifile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		state = FileAccessDenied;
		return false;
	}

	while (inifile.isReadable() && !inifile.atEnd()) {
		/* read line by line */
		line = inifile.readLine();

		/* we don't want to have a new line character at the end of the line */
		if (line.size() && line[line.size()-1]=='\n') line.resize(line.size()-1);

		/* ignore empty lines */
		if (!line.size()) continue;

		/* line valid: whitespace not allowed as first character */
		if (line[0] == ' ') continue;

		// ignore comments
		if (line[0] == '#') continue;

		// is this a group name
		if (line[0] == '[' && line[line.size()-1] == ']') {
			group = QString::fromUtf8(line.mid(1,line.size()-2).trimmed());
			readgroup(group);
			continue;
		}
		// a group name has to be set to allow key=value
		if (!group.size()) continue;

		/* separate key and value */
		QList<QByteArray> keyvalue = line.split('=');
		key = QString::fromUtf8(keyvalue[0].trimmed());
		if (keyvalue.size()>1) value = QString::fromUtf8(keyvalue[1].trimmed()); else value.clear();

		/* is the key locale encoded like key[en_EN]=value? */
		found = key.indexOf(QLatin1Char('['));
		found2 = key.indexOf(QLatin1Char(']'), found);
		if (found != -1 && found2 != -1) {
			keylocale = key.mid(found+1,found2-found-1).trimmed();
			key = key.left(found);
		}

		/* if key is empty skip this entry */
		if (!key.size())
			continue;

		/* interpret */
		if (group == QLatin1String("Desktop Entry")) {
			if (key == QLatin1String("Version")) {
				if (value != QLatin1String("1.0")) break; else continue;
			}
			if (key == QLatin1String("Encoding")) {
				if (value != QLatin1String("UTF-8")) break; else continue;
			}
			if (key == QLatin1String("Type")) {
				if (value != type()) break; else continue;
			}
			if (key == QLatin1String("Authors")) { authors = value; continue; }
			if (key == QLatin1String("Icon")) { iconfile = value; continue; }
			if (key == QLatin1String("Name")) { names[localekey(key,keylocale)] = value; continue; }
			if (key == QLatin1String("Comment")) { comments[localekey(key,keylocale)] = value; continue; }
			if (key == QLatin1String("X-Version")) { version = value.toDouble(); continue; }
		}
		if (!readline(group, key, keylocale, value)) break;
	}

	if (!inifile.atEnd()) {
		clear();
		state = FileInvalid;
		inifile.close();
		return false;
	}



	/* version info */
	if (minversion() > version)
		qWarning() << "File" << filename << "too old! Fileversion:" << version << "Min version:" << minversion();
	if (maxversion() < version)
		qWarning() << "File" << filename << "too new! Fileversion:" << version << "Max version:" << maxversion();

	inifile.close();
	state = Valide;

	return true;
}

void DesktopFile::setChanged(bool c) {
	changed = c;
}

bool DesktopFile::save(bool ifchanged) {
	QString filename = getFilename();
	if (!filename.size()) return false;
	if (ifchanged && !changed) return true;

	QFile inifile(filename);
	if (!inifile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qDebug() << "save failed with file" << filename << inifile.error();
		return false;
	}

	inifile.write("[Desktop Entry]\n");
	inifile.write("Version=1.0\n");
	inifile.write("Encoding=UTF-8\n");
	inifile.write("Type=" + type().toUtf8() + "\n");
	inifile.write("Icon=" + iconfile.toUtf8() + "\n");
	inifile.write("Authors=" + authors.toUtf8() + "\n");
	QMap< QString, QString >::iterator itnames;
	QMap< QString, QString >::iterator itcomments;
	for ( itnames=names.begin() ; itnames != names.end(); itnames++ )
		inifile.write(itnames.key().toUtf8() + '=' + itnames.value().toUtf8() + "\n");
	for ( itcomments=comments.begin() ; itcomments != comments.end(); itcomments++ )
		inifile.write(itcomments.key().toUtf8() + '=' + itcomments.value().toUtf8() + "\n");
	inifile.write("X-Version=" + QString::number(version).toUtf8() + "\n");
	inifile.write("\n");
	bool success = save_private(inifile);

	inifile.close();
	return success;
}
