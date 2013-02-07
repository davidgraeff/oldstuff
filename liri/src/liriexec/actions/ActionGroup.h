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
#ifndef LIRI_APPLICATION_ACTIONGROUP_H_
#define LIRI_APPLICATION_ACTIONGROUP_H_

#include <QString>
#include <QList>
#include <QSet>

class Action;

class ActionGroup : public QList<Action*> {
public:
	ActionGroup();
	~ActionGroup();
public:
	void setRepeat(const QString& value);
	QString getRepeat();

	/* trigger conditions */
	QString key;
	QString mode;
	QSet<unsigned int> channels;
	int keystate;

	/* repeat action? */
	bool repeat;
	unsigned int repeatlag; //repeat with an initial lag time
	unsigned int repeattick; //repeat every tick ms
};

#endif
