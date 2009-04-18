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
#ifndef HAL_INTERFACE_PROXY_H_1218802042
#define HAL_INTERFACE_PROXY_H_1218802042

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.freedesktop.Hal.Manager
 */
class OrgFreedesktopHalManagerInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.Hal.Manager"; }

public:
    OrgFreedesktopHalManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~OrgFreedesktopHalManagerInterface();

public Q_SLOTS: // METHODS
	inline QDBusReply<QStringList > FindDeviceByCapability(const QString &capability)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(capability);
        return callWithArgumentList(QDBus::Block, QLatin1String("FindDeviceByCapability"), argumentList);
    }

	inline QDBusReply<QStringList > FindDeviceStringMatch(const QString &key, const QString &value)
	{
		QList<QVariant> argumentList;
		argumentList << qVariantFromValue(key) << qVariantFromValue(value);
		return callWithArgumentList(QDBus::Block, QLatin1String("FindDeviceStringMatch"), argumentList);
	}

	inline QDBusReply<QStringList > GetAllDevices()
	{
		QList<QVariant> argumentList;
		return callWithArgumentList(QDBus::Block, QLatin1String("GetAllDevices"), argumentList);
	}

Q_SIGNALS: // SIGNALS
    void DeviceAdded(const QString &udi);
    void DeviceRemoved(const QString &udi);
};

namespace org {
  namespace freedesktop {
    namespace Hal {
      typedef ::OrgFreedesktopHalManagerInterface Manager;
    }
  }
}
#endif
