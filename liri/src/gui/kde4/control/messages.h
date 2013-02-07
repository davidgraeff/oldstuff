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
#ifndef TRANSLATED_LIRI_MESSAGES
#define TRANSLATED_LIRI_MESSAGES

#include <QString>
#include <QHash>

class trLiriClass {
	public:
		trLiriClass();
		QString msg(int state);
	private:
		QHash<int, QString> msgs;

};

#endif
