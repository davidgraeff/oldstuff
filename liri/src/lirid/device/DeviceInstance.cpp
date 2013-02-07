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
#include "DeviceInstance.h"
#include "DeviceInstance_dbusif.h"
#include "fileformats/RemoteFile.h"
#include "config.h"

#include <dlfcn.h> //dlopen
#include <poll.h>
#include <QStringList>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QDebug>
#include "../../drivers/driver-common.h"
#include "../Control.h"

bool operator==(KeyCode const& a, KeyCode const& b)
{
    bool equal = (strcmp(a.receiver, b.receiver)==0);
    equal = equal && (a.keycodeLen == b.keycodeLen) && (memcmp(a.keycode, b.keycode, a.keycodeLen)==0);
    equal = equal && (a.channel == b.channel) && (a.pressed == b.pressed) && (a.state == b.state);
    return equal;
}

bool operator!=(KeyCode const& a, KeyCode const& b)
{
    return !(a==b);
}

DeviceInstance::DeviceInstance(const QString& uid)
{
    this->uid = uid;
    driverhandle = 0;
    receiverState = LIRI_DEVICE_OFFLINE;
    lastkey.state = 0;

    keyevent_timer.setSingleShot(true);
    keyevent_timer.setInterval(100);
    connect(&keyevent_timer, SIGNAL(timeout()), SLOT(keyevent_timeout()));

    //important: Does not need to be deleted explicitly
    new DeviceAdaptor(this);

    //register object
    busname = uid;
    busname.replace(QLatin1String("/"), QLatin1String(""));
    busname.replace(QLatin1String(":"), QLatin1String(""));
    busname.replace(QLatin1String("."), QLatin1String(""));
    busname.replace(QLatin1String("-"), QLatin1String(""));
    busname = QLatin1String(LIRI_DBUS_OBJECT_RECEIVERS"/")+ busname.right(10);
    if ( !Control::getBus()->registerObject(busname, static_cast<QObject*>(this)) )
    {
        qWarning() << "UID:" << uid << ", Couldn't register DeviceInstance object:" << busname;
    }
    setObjectName(busname);
}

DeviceInstance::~DeviceInstance() {
    QDBusConnection::systemBus().unregisterObject(busname);
}

void DeviceInstance::release() {
    if (driverhandle)
    {
        if (driver_close)
            driver_close();
        dlclose (driverhandle);
        driverhandle = 0;
        updateReceiverState(LIRI_DEVICE_OFFLINE);
    }

    qDebug() << "Released device:";
    qDebug() << "\tUID:" << getUid();

    emit releasedDevice(this);
}

/* load driver */
bool DeviceInstance::loaddriver() {
    /* udi - unique device id (= hal id) */
    if (!uid.size()) {
        qWarning() << "\tFailed: Uid missing. Initalizing aborted!";
        updateReceiverState(LIRIERR_settings);
        return false;
    }

    /* connection time */
    m_settings.insert(QLatin1String("CONNECTED"), QDateTime::currentDateTime().toString(Qt::ISODate));

    /* new state */
    updateReceiverState(LIRI_DEVICE_INIT);

    // debug info - part 1/2
    qDebug() << "Initialise device:";
    qDebug() << "\tUID:" << uid;
    qDebug() << "\tvendorId:" << m_settings.value(QLatin1String("ID_VENDOR_ID"));
    qDebug() << "\tproductId:" << m_settings.value(QLatin1String("ID_MODEL_ID"));

    // check if driver is set
    const QString driverid = m_settings.value(QLatin1String("liri_receiver_driver"));
    if (driverid.isEmpty()) {
        qWarning() << "\tFailed: Driver not set!";
        updateReceiverState(LIRIERR_filename);
        release();
        return false;
    }

    // check if filename exists - part 2
    const QString drivername = QLatin1String(LIRI_SYSTEM_DRIVERS_DIR "/")
                               + driverid + QLatin1String(".so");
    if ( !QFile::exists(drivername) ) {
        qWarning() << "\tFailed: Driver not accessable:" << drivername;
        updateReceiverState(LIRIERR_permission);
        release();
        return false;
    }

    // open the file
    driverhandle = dlopen(drivername.toLatin1().data(), RTLD_LAZY);
    if (!driverhandle) {
        qWarning() << "\tFailed: Failed opening the driver:" << dlerror();
        updateReceiverState(LIRIERR_opendriver);
        release();
        return false;
    }

    // load the symbol for creating the class instance
    driver_open = (driver_open_t) dlsym(driverhandle, "open");
    if (!driver_open) {
        qWarning() << "\tFailed: Symbol not found:" <<  dlerror();
        updateReceiverState(LIRIERR_symbolCreate);
        release();
        return false;
    }

    // load the symbol for destroying the class instance
    driver_init = (driver_init_t) dlsym(driverhandle, "init");
    if (!driver_init) {
        qWarning() << "\tFailed: Symbol not found:" <<  dlerror();
        updateReceiverState(LIRIERR_symbolDestroy);
        release();
        return false;
    }

    // load the symbol for destroying the class instance
    driver_activity = (driver_activity_t) dlsym(driverhandle, "activity");
    if (!driver_activity) {
        qWarning() << "\tFailed: Symbol not found:" <<  dlerror();
        updateReceiverState(LIRIERR_symbolDestroy);
        release();
        return false;
    }

    // load the symbol for destroying the class instance
    driver_close = (driver_close_t) dlsym(driverhandle, "close");
    if (!driver_close) {
        qWarning() << "\tFailed: Symbol not found:" <<  dlerror();
        updateReceiverState(LIRIERR_symbolDestroy);
        release();
        return false;
    }

    // read version info from the meta driver file "driver.desktop"
    const QString driverdescname = QString::fromLatin1(LIRI_SYSTEM_DRIVER_DESCRIPTION_DIR"/%1.desktop").arg(driverid);
    QFile file(driverdescname);
    QString driver_version;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // not successful, default = 0.0
        m_settings.insert(QLatin1String("DRIVER_VERSION"), QLatin1String("0.0"));
        qWarning() << "\tFailed: Can not read version for driver. Filename:" << driverdescname;
    } else {
        QStringList line;
        while (file.isReadable() && !file.atEnd()) {
            line = QString::fromUtf8(file.readLine()).split(QLatin1Char('='));
            if (line.size() && line[0] == QLatin1String("X-Driver-Version")) {
                // success
                driver_version = line[1].left(line[1].size()-1);
                m_settings.insert(QLatin1String("DRIVER_VERSION"), driver_version);
                break;
            }
        }
    }
    file.close();

    // load layout if possible
    RemoteFile remote(m_settings.value(QLatin1String("liri_receiver_layout")));
    if (remote.getState() == DesktopFile::Valide)
    {
        const int size = remote.countKeys();
        // fill map with keys
        for (int i=0;i<size;++i) {
            keys.insert(remote.getKey(i).first.toAscii(), remote.getKey(i).second);
        }
    }

    // open device
    char errorBuffer[100] = {0};
    pollstr** fds = driver_open(uid.toAscii().data(),
                                m_settings.value(QLatin1String("ID_VENDOR_ID")).toAscii().data(),
                                m_settings.value(QLatin1String("ID_MODEL_ID")).toAscii().data(),
                                m_settings.value(QLatin1String("ID_SERIAL")).toAscii().data(),
                                errorBuffer);
    // watch for changes of the filehandles
    if (!fds)
    {
        qWarning() << uid << "Error:" << errorBuffer;
        driver_close = 0; // do not call this, as we didn't call init
        release();
        return false;
    }
    else {
        for (int r = 0 ; fds[r] ; r++) {
            QSocketNotifier::Type direction = ((fds[r]->events == POLLOUT) ? QSocketNotifier::Write : QSocketNotifier::Read);
            // Ignore writeable sockets
            //if (direction != QSocketNotifier::Read) continue;
            QSocketNotifier* sn = new QSocketNotifier(fds[r]->fd, direction, this);
            connect(sn, SIGNAL(activated(int)), SLOT(activity()));
            sn->setEnabled(true);
            //printf("%u: %p fd: %u, events %i, %i\n", r, fds, fds[r]->fd, fds[r]->events == POLLIN, fds[r]->events == POLLOUT);
        }

        // release the file handle list as requested by libusb
        free(fds);
    }
    // init device, wait for callback
    driver_init();

    // debug info - part 2/2
    qDebug() << "\tDriver:" << driverid;
    qDebug() << "\tVersion:" << driver_version;
    qDebug() << "\tLayout:" << QLatin1String("liri_receiver_layout");

    return true;
}


void DeviceInstance::keyevent(KeyCode ckey)
{
    keyevent_timer.stop();
    // if current key is pressed, activate the timeout timer
    if (ckey.pressed)
        keyevent_timer.start();

    // Ignore same keys
    if (ckey == lastkey) return;

    // synthetic key release event for last active key //
    if (lastkey.state == 1) {
        lastkey.pressed = 0;
        lastkey.state = 0;
        keyevent(lastkey);
    }

    /* get hex code */
    hexcode = QByteArray(ckey.keycode,ckey.keycodeLen).toHex();

    if (keys.size()) {
        keyname = keys.value(hexcode);
    } else {
        keyname.clear();
    }

    emit key( QString::fromAscii(hexcode), keyname, ckey.channel, ckey.pressed);
    lastkey = ckey;
}

void DeviceInstance::keyevent_timeout()
{
    // just synthetic key release event; no new key pressed //
    KeyCode t = lastkey;
    t.state = 0;
    t.pressed = 0;
    keyevent(t);
}

void DeviceInstance::activity()
{
    const KeyCode keycode = driver_activity("","");

    if (keycode.state == 1)
    {
        keyevent(keycode);
    }
}

int DeviceInstance::ReceiverState()
{
    return receiverState;
}

void DeviceInstance::updateReceiverState(int s)
{
    receiverState = s;
    emit receiverStateChanged(s);
}

/* access */

const QString DeviceInstance::getUid() const {
    return uid;
}

QStringList DeviceInstance::getAllSettings() {
    QStringList tmp;
    QMap<QString,QString>::const_iterator i = m_settings.constBegin();
    while (i != m_settings.constEnd()) {
        tmp.append(i.key()+QLatin1Char('\1')+i.value());
        ++i;
    }
    return tmp;
}

QStringList DeviceInstance::getSettings(const QStringList &keys) {
    QStringList tmp;
    foreach (QString key, keys) {
        tmp.append(m_settings.value(key));
    }
    return tmp;
}

void DeviceInstance::setSetting(const QString& key, const QString& value)
{
    m_settings.insert(key, value);
}

void DeviceInstance::setSettings(const QStringList &keys) {
    foreach (QString s, keys)
    {
        QStringList ls = s.split(QLatin1Char('\1'));
        if (ls.size()!=2) continue;
        m_settings.insert(ls[0], ls[1]);
    }
}

