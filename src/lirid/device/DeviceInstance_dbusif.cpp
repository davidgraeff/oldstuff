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
#include "DeviceInstance_dbusif.h"
#include "DeviceInstance.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class ReceiverAdaptor
 */

ReceiverAdaptor::ReceiverAdaptor(DeviceInstance *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

ReceiverAdaptor::~ReceiverAdaptor()
{
    // destructor
}

inline DeviceInstance* ReceiverAdaptor::parent() const
{
	return static_cast<DeviceInstance *>(QObject::parent());
}

int ReceiverAdaptor::ReceiverState()
{
    // handle method call org.liri.Receiver.ReceiverState
    return parent()->ReceiverState();
}

int ReceiverAdaptor::RemoteState()
{
    // handle method call org.liri.Receiver.RemoteState
    return parent()->RemoteState();
}

QMap<QString,QString> ReceiverAdaptor::getAllSettings()
{
	// handle method call org.liri.Receiver.getAllSettings
	return parent()->getAllSettings();
}

QStringList ReceiverAdaptor::getSettings(const QStringList &keys)
{
    // handle method call org.liri.Receiver.getSettings
    return parent()->getSettings(keys);
}

void ReceiverAdaptor::reloadAssociatedRemote()
{
    // handle method call org.liri.Receiver.reloadAssociatedRemote
    parent()->reloadAssociatedRemote();
}

void ReceiverAdaptor::setAssociatedRemote(const QString &remoteid)
{
    // handle method call org.liri.Receiver.setAssociatedRemote
    parent()->setAssociatedRemote(remoteid);
}

void ReceiverAdaptor::setSettings(const QMap<QString,QString> &settings)
{
    // handle method call org.liri.Receiver.setSettings
    parent()->setSettings(settings);
}
