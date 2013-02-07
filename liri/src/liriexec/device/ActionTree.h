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
#ifndef LIRI_APPLICATION_ACTIONTREE_H_
#define LIRI_APPLICATION_ACTIONTREE_H_

#include <QString>
#include <QList>
#include <QMap>
#include <QSet>

class Action;
class ActionGroup;

class ActionTree {
	public:
		ActionTree();
		~ActionTree();
	public:
		void add(ActionGroup* actiongroup);
		ActionGroup* get(uint channel, const QString& key, const QString& mode);
		void clear();
	private:
		typedef QMap< int, ActionGroup* > ChannelMap;
		typedef QMap< QString, ChannelMap > ModeChannelMap;
		typedef QMap< QString, ModeChannelMap > KeyModeChannelMap;
		typedef KeyModeChannelMap::iterator itKey;
		typedef ModeChannelMap::iterator itMode;
		typedef ChannelMap::iterator itChannel;
		KeyModeChannelMap actions;
		QList<ActionGroup*> actions_cache;
};

#endif
