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

class RemoteItem {
	public:
		QString text;
		QString uid;
		KIcon* icon;
};

class RemotesModel : public QAbstractListModel {
	public:
		RemotesModel(QObject *parent = 0);
		~RemotesModel();

		int getIndexOf(const QString& remoteuid);
		QString getUidOf(int index);

		void reload();
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role) const;
	private:
		QList<RemoteItem*> list;
		KIcon icon;
};


#endif
