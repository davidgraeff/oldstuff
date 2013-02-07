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
#include "device/DeviceInstance_dbusif.h"
#include "device/DeviceInstance.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class DeviceInstanceAdaptor
 */

DeviceInstanceAdaptor::DeviceInstanceAdaptor(DeviceInstance *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

DeviceInstanceAdaptor::~DeviceInstanceAdaptor()
{
    // destructor
}

inline DeviceInstance* DeviceInstanceAdaptor::parent() const
{
	return static_cast<DeviceInstance *>(QObject::parent());
}

QStringList DeviceInstanceAdaptor::getProfileUids()
{
    // handle method call org.liri.Receiver.getAppProfileUids
    return parent()->getProfileUids();
}

QString DeviceInstanceAdaptor::getMode()
{
    // handle method call org.liri.Receiver.getMode
    return parent()->getMode();
}

void DeviceInstanceAdaptor::reload()
{
    // handle method call org.liri.Receiver.reload
    parent()->reload();
}

void DeviceInstanceAdaptor::setMode(const QString &mode)
{
    // handle method call org.liri.Receiver.setMode
    parent()->setMode(mode);
}

QStringList DeviceInstanceAdaptor::getVariables()
{
    // handle method call org.liri.Receiver.sharedVars
    return parent()->getVariables();
}
