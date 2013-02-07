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
#ifndef LIRI_REMOTE_MODEL_H_
#define LIRI_REMOTE_MODEL_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <kicon.h>
#include <string>

class RemoteFile;

class Remotes_Model : public QAbstractListModel {
	Q_OBJECT
	public:
		Remotes_Model(QObject *parent = 0);
		~Remotes_Model();

		void reload();
		void edit(RemoteFile* newprofile);
		void remove(RemoteFile* newprofile);

		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	private:
		void clear();
		QList<RemoteFile*> list;
		KIcon decorationIcon;

	Q_SIGNALS:
		void saveRemotefile(RemoteFile* file,bool deleteOnFailure);

};


#endif
