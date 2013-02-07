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
#ifndef LIRI_DESKTOPFILE_H_
#define LIRI_DESKTOPFILE_H_

#include <QString>
#include <QMap>
#include <QFile>

class DesktopFile {
	public:
		enum DesktopFileState {
			Valide,
			Empty,
			FileInvalid,
			FileAccessDenied
		};

	DesktopFile(const QString& uid);
	virtual ~DesktopFile() {}

	const QString& getUid() const;
	const QString getFilename() const;
	static QString getUidOfFilename(const QString& filename);
	
	void setLocale(const QString& locale = "");

	DesktopFileState getState() const;

	/* name */
	void setName(const QString& name);
	const QString getName() const;

	/* generic name */
	void setGenericName(const QString& name);
	const QString getGenericName() const;

	/* comments */
	void setComment(const QString& comment);
	const QString getComment() const;

	/* authors */
	void setAuthors(const QString& authors);
	const QString getAuthors() const;

	/* icon filename */
	void setIconFilename(const QString& iconfilename);
	const QString& getIconFilename() const;

	/* changed flag */
	void setChanged(bool c = true);
	
	/* some operations like clearing, reloading after a filename change and saving */
	virtual void clear();
	bool reload();
	bool save(bool ifchanged = false);

	protected:
	virtual void readgroup(const QString& group) = 0;
	virtual bool readline(const QString& group, const QString& key, const QString& keylocale, const QString& value) = 0;
	virtual bool save_private(QFile&) = 0;
	virtual const QString path() const = 0;
	virtual const QString type() const = 0;
	virtual double minversion() const = 0;
	virtual double maxversion() const = 0;

	private:
	/* information */
	bool changed;
	QString uid;
	double version;
	QString locale;
	QString authors;
	QMap< QString, QString > names;
	QMap< QString, QString > genericnames;
	QMap< QString, QString > comments;
	QString iconfile;
	DesktopFileState state;
	inline QString localekey(const QString& locale) const;
	inline QString localekey(const QString& nsp, const QString& locale ) const;
};

#endif
