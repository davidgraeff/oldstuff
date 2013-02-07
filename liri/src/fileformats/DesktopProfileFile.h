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
#ifndef LIRI_DesktopProfileFile_H_
#define LIRI_DesktopProfileFile_H_

#include "fileformats/DesktopFile.h"
#include <QSet>

/**
* \brief Can load and save a desktop profile
*
* Load and write the ini style desktop profile file format.
* Remotes can bound desktop profiles and load them if they
* are loaded. A desktop profile pools a set of application
* profiles which are able to communicate with the actuall
* applications. Now remote events can be delivered to all
* applications that are part of the set.
*
*/
class DesktopProfileFile : public DesktopFile, public QSet<QString> {
public:
	DesktopProfileFile(const QString& uid = QString());
	virtual ~DesktopProfileFile() {}
	virtual void clear();
private:
	virtual void readgroup(const QString& group);
	virtual bool readline(const QString& group, const QString& key, const QString& keylocale, const QString& value);
	virtual bool save_private(QFile&);
	virtual const QString path() const;
	virtual const QString type() const;
	virtual double minversion() const;
	virtual double maxversion() const;
};

#endif /*LIRI_DesktopProfileFile_H_*/
