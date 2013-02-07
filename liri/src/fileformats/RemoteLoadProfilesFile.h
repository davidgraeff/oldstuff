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
#ifndef REMOTELOADPROFILESFILE_H_
#define REMOTELOADPROFILESFILE_H_

#include <QString>
#include <QSet>

class RemoteLoadProfilesFile: public QSet<QString> {
	public:
		RemoteLoadProfilesFile(const QString& uid);
		QString& getFilename();
		static QString getFilenameFromRemoteUid(const QString& uid);

		void clear();
		void reload();
		bool save();
		int state();


		/* loader logic */
		bool isAll();
		void setAll(bool);

		QSet<QString> controlled();

	private:
		bool all;
		int pstate;
		QString filename;
};

#endif
