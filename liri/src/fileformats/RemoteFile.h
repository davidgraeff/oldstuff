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
#ifndef REMOTEFILE_H_
#define REMOTEFILE_H_

#include "fileformats/DesktopFile.h"
#include <QString>
#include <QPair>
#include <QList>

class RemoteFile : public DesktopFile {
public:
	//for clients to rate remotes' capabilities
	enum KeySubset {
		Nothing = 0,
		Cursor = 1, //top, down, left, right
		Multimedia = 2, //stop, play, pause, next|chan+, previous|chan-, vol+, vol-
		MultimediaModeKeys = 4, //video(mode), audio(mode)
		DesktopControl = 8 //showDesktop, ok, fullscreen
	};
	
	RemoteFile(const QString& uid = QString());
	virtual void clear();

	//* eg Desktop, Multimedia, Cursor etc, see "KeySubset" */
	int getKeySubset() const;
	void updateKeySubsetCache();

	/* settings */
	int countSettings() const;
	const QPair<QString, QString> getSetting(int index) const;

	int addSetting(const QString& key, const QString& value);
	void setSettingKey(int index, const QString& key);
	void setSettingValue(int index, const QString& key);
	
	/* keys */
	int countKeys() const;
	int posKeyCode(const QString& keycode);
	QPair<QString, QString> getKey(int index);

	int addKey(const QString& keycode, const QString& keyname);
	void setKeyCode(int index, const QString& keycode);
	void setKeyName(int index, const QString& keyname);
private:
	virtual void readgroup(const QString& group);
	virtual bool readline(const QString& group, const QString& key, const QString& keylocale, const QString& value);
	virtual bool save_private(QFile&);
	virtual const QString path() const;
	virtual const QString type() const;
	virtual double minversion() const;
	virtual double maxversion() const;
	virtual bool containsKeyname(const QString& keyname);

	/* remote keys, settings, receivers */
	QList< QPair<QString, QString> > settings;
	QList< QPair<QString, QString> > compatibleReceivers;
	QList< QPair<QString, QString> > keylist;

	/* supported subsets such as multimedia, desktop control etc */
	int keysubsetCache;
};

#endif
