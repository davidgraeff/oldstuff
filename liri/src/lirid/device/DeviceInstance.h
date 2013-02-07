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
#ifndef DEVICEINSTANCE_H_
#define DEVICEINSTANCE_H_

#include <QString>
#include <QObject>
#include <QTimer>
#include <QDBusConnection>

#include "driver-common.h"

/**
* \brief Instance representing a device
*
*  Responsible for loading the corresponding driver for the device,
*  providing dbus object access to this instance and controlling the
*  thread that listens to the device for key events.
*
* \sa DeviceListenThread, DeviceInitThread
**/
class DeviceInstance : public QObject
{
    Q_OBJECT
public:
    DeviceInstance(const QString& uid);
    ~DeviceInstance();

    /* release claimed device, release driver
       if wait is set to false the terminating process is asynchronous.
       When terminating completed the signal "released" will be emitted. */
    void release();

    /* load driver and start listen thread */
    bool loaddriver();

    /* access */
    const QString getUid() const;

    /* states */
    int ReceiverState();
    void updateReceiverState(int);

    /* settings manipulation */
    QStringList getAllSettings();
    QStringList getSettings(const QStringList &keys);
    void setSettings(const QStringList &settings);
    void setSetting(const QString& key, const QString& value);
private:

Q_SIGNALS:
    // a signal to the list to remove this instance
    void releasedDevice(DeviceInstance* di);
    void key(const QString &keycode, const QString &keyname, uint channel, int pressed);
    void receiverStateChanged(int state);

private Q_SLOTS:
    void activity();
    void keyevent_timeout();
private:
    // system bus connection
    QDBusConnection* conn;
    QMap<QString, QString> m_settings;
    QString uid;
    QString busname;
    int receiverState;

    KeyCode lastkey;
    QTimer keyevent_timer;

    /* to not always reallocate */
    QByteArray hexcode;
    QString keyname;

    /* remote */
    QMap< QByteArray, QString > keys;
    void keyevent(KeyCode key);

    /* dlopen, driver loading and freeing */
    void* driverhandle;
    typedef struct pollstr** (*driver_open_t)(const char* udi,
                        const char* usbVendorID,
                        const char* usbProductID,
                        const char* usbSerialID,
                        char* error_string);
    driver_open_t driver_open;
    typedef void (*driver_init_t)();
    driver_init_t driver_init;
    typedef KeyCode (*driver_activity_t)(const char* cmd, const char* value);
    driver_activity_t driver_activity;
    typedef void (*driver_close_t)();
    driver_close_t driver_close;
};

#endif
