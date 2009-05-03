#include "ActionTree.h"
#include "liriexec/actions/Action.h"
#include "liriexec/actions/ActionGroup.h"
#include <QDebug>

ActionTree::ActionTree() {}

ActionTree::~ActionTree()
{
	clear();
}

ActionGroup* ActionTree::get(uint channel, const QString& key, const QString& mode)
{
	qDebug() << "ActionTree find:" << mode << key << channel;
	// Find key
	itKey itK = actions.find(key);
	if (itK == actions.end()) return 0;
	qDebug() << "KEY ok";

	// Find mode
	itMode itM = itK->find(mode);
	if (itM == itK->end())
	{
		// Try with empty mode
		itM = itK->find(QLatin1String(" "));
		if (itM == itK->end())
			return 0;
	}
	qDebug() << "MODE ok";

	// Find channel
	itChannel itC = itM->find((int)channel);
	if (itC == itM->end())
	{
		// Try with wildspace channel
		itC = itM->find(-1);
		if (itC == itM->end()) // this should never happen, actually
			return 0;
	}
	qDebug() << "CHANNEL ok";

	return itC.value();
}

void ActionTree::clear()
{
	qDeleteAll(actions_cache);
	actions_cache.clear();
	actions.clear();
}

void ActionTree::add(ActionGroup* actiongroup)
{
	actions_cache.append(actiongroup);

	// key
	itKey itK = actions.find(actiongroup->key);
	if (itK == actions.end()) itK = actions.insert(actiongroup->key, ModeChannelMap());

	// mode
	QString mode = actiongroup->mode;
	if (mode.isEmpty()) mode = QLatin1String(" ");
	itMode itM = itK->find(mode);
	if (itM == itK->end()) itM = itK->insert(mode, ChannelMap());

	// channel
	QSet<int> channels;
	foreach(unsigned int chan, actiongroup->channels)
		channels.insert((int)chan);
	if (channels.isEmpty()) channels.insert(-1);

	foreach(int channel, channels)
	{
		itChannel itC = itM->find(channel);
		if (itC == itM->end())
		{
			qDebug() << "ActionTree insert (Mode/Key/Channels):" << actiongroup->mode << actiongroup->key << actiongroup->channels;
			itC = itM->insert(channel, actiongroup);
		}
		else
		{
			qWarning() << "ActionTree same triggers (Mode/Key/Channels):" << actiongroup->mode << actiongroup->key << actiongroup->channels;
		}
	}
}