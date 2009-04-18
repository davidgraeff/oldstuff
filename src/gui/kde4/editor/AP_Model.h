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
#ifndef LIRI_ApplicationProfileFileSLIST_H_
#define LIRI_ApplicationProfileFileSLIST_H_

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>
#include <kicon.h>

class ApplicationProfileFile;
class DesktopProfileFile;

class AP_Model : public QAbstractListModel {
	Q_OBJECT
	public:
		AP_Model(QObject *parent = 0);
		~AP_Model();

		void markUsed(DesktopProfileFile* dp);

		void reload();
		void edit(ApplicationProfileFile* newprofile);
		void remove(ApplicationProfileFile* newprofile);

		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

	private:
		void clear();
		inline void mark(int i, DesktopProfileFile* dp);
		QList<ApplicationProfileFile*> list;
		QList<int> marked;
		QList<KIcon> decoration;
		DesktopProfileFile* marker;

	Q_SIGNALS:
		void saveDesktopfile(DesktopProfileFile* file,bool deleteOnFailure);
};

#endif
