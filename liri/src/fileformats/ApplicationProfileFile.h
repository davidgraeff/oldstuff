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
#ifndef LIRI_ApplicationProfileFile_H_
#define LIRI_ApplicationProfileFile_H_

#include "fileformats/DesktopFile.h"
#include <QList>
#include <QString>

class Action;
class ActionGroup;

struct VariablesInAppProfile {
	QString name; // var name
	QString init; // var init
	char dbustype; // var type
};

struct BusServiceWithOptions {
	QString serviceid;
	QList< QString > options;
	BusServiceWithOptions(const QString& serviceid, const QList< QString >& options) {
		this->serviceid = serviceid;
		this->options = options;
	}
};

class ApplicationProfileFile : public DesktopFile, public QList<ActionGroup*> {
public:
	ApplicationProfileFile(const QString& uid = QString());
	~ApplicationProfileFile();
	void setNoClear();
	QList< VariablesInAppProfile >& getVariables();
	QList< BusServiceWithOptions >& getBusserviceNames();

	virtual void clear();
private:
	virtual void readgroup(const QString& group);
	virtual bool readline(const QString& group, const QString& key, const QString& keylocale, const QString& value);
	virtual bool save_private(QFile&);
	void saveAction(QFile&, Action*);
	virtual const QString path() const;
	virtual const QString type() const;
	virtual double minversion() const;
	virtual double maxversion() const;
	//void saveAction(QFile&, const Action* action, const Action* base);
	/* temp stuff for loading */
	Action* lastAction;
	ActionGroup* lastActionGroup;
	bool noclear;
	QList< VariablesInAppProfile > variables;
	QList< BusServiceWithOptions > busservices;

};

#endif /*LIRI_ApplicationProfileFile_H_*/
