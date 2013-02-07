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
#ifndef LIRI_DesktopProfileFileSLIST_H_
#define LIRI_DesktopProfileFileSLIST_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <kicon.h>

class DesktopProfileFile;
class RemoteFile;
class RemoteLoadProfilesFile;

class DP_Model : public QAbstractListModel {
	Q_OBJECT
	public:
		DP_Model(QObject *parent = 0);
		~DP_Model();

		void reload();
		void edit(DesktopProfileFile* newprofile);
		void remove(DesktopProfileFile* newprofile);

		void markControlled(RemoteFile* re);

		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

	private:
		void clear();
		inline void mark(int i);
		QList<DesktopProfileFile*> list;
		QList<KIcon> decoration;
		QList<int> marked;
		RemoteLoadProfilesFile* marker;
};

#endif
