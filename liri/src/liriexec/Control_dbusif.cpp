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
#include "Control_dbusif.h"
#include "Control.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class ControlAdaptor
 */

ControlAdaptor::ControlAdaptor(Control *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

ControlAdaptor::~ControlAdaptor()
{
    // destructor
}

inline Control* ControlAdaptor::parent() const
{
	return static_cast<Control *>(QObject::parent());
}

void ControlAdaptor::quit()
{
    // handle method call org.liri.Execution.Quit
    parent()->quit();
}

QString ControlAdaptor::started()
{
	// handle method call org.liri.Control.started
	return parent()->started();
}

QStringList ControlAdaptor::getTargets()
{
    // handle method call org.liri.Receiver.getTargets
    return parent()->getTargets();
}

void ControlAdaptor::setTarget(const QString &targetid, bool active)
{
    // handle method call org.liri.Receiver.setTarget
    parent()->setTarget(targetid, active);
}

QString ControlAdaptor::version()
{
	// handle method call org.liri.Control.version
	return parent()->version();
}
