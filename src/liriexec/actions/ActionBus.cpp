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

#include "liriexec/actions/ActionBus.h"
#include "liriexec/device/TargetList.h"

#include "config.h"
#include <QDBusMessage>
#include <QDBusConnection>
#include <QStringList>
#include <QDebug>

ActionBus::ActionBus() : Action(Action::Bus), timeout(LIRI_DBUS_TIMEOUT), targetlist(0) {}
ActionBus::~ActionBus() {}

void ActionBus::setBusText(const QString& txt) {
	QStringList tmp = txt.split(QLatin1Char(' '));
	if (tmp.size() != 4) return;

	busserviceid = tmp[0];
	object = tmp[1];
	interface = tmp[2];
	method = tmp[3];
}

QString ActionBus::getBusText() {
	return busserviceid + QLatin1Char(' ') +
		object + QLatin1Char(' ') + interface + QLatin1Char(' ') + method;
}


bool ActionBus::work() {
	if (!targetlist) return false;

	/* We need to loop through all targetlist that are direct or indirect listed in the
	application profile. Usually just one, but may be more if the multiple instance
	function is used */
	QStringList servicelist = targetlist->getActiveServices();

	foreach (QString service, servicelist) {
		// lock this service so that no other thread try so communicate with the same
		// application at the same time
		if (!targetlist->lockService(service)) return false;

		// create message with arguments
		QList<QVariant> args;
		QDBusMessage message = QDBusMessage::createMethodCall(service, object, interface, method);
		foreach (ActionArgument arg, argin) {
			if (arg.argvalue) {
				args.append(*(arg.argvalue));
			} else {
				// error with generating a valid value -> make an empty default one
				args.append(QVariant(Action::getVariantType(arg.dbustype)));
			}
		}
		message.setArguments(args);
		// qDebug() << "message" << message.service() << message.path()
		// << message.interface() << message.member() << message.arguments();

		// send
		message = targetlist->connection().call(message, QDBus::Block, timeout);

		// and unlock the service to allow other thread to communicate with it again
		targetlist->unlockService(service);

		//evaluate reply
		if (message.type() == QDBusMessage::ErrorMessage) {
			QString details = service + QLatin1Char(' ') +
				interface + QLatin1Char(' ') + object + QLatin1Char(' ') + method;
			emit executed(deviceinstance, LIRIERR_noservices, details);
			if (servicelist.size() == 1) return false;
			continue;
		}

		//reply arguments (set dynamic_variables)
		args = message.arguments();
		for (int i=0; i<args.size(); ++i) {
			if (argout.size() <= i) break;
			if (args[i].type() != argout[i].argvalue->type()) continue;
			*(argout[i].argvalue) = args[i];
		}

		emit executed(deviceinstance, LIRIOK_executed, object+QLatin1Char('/')+method);
	} // end foreach service
	return true;
}

