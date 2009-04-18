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
#include "config.h"

#include <dlfcn.h> //dlopen
#include <QStringList>
#include <QDateTime>
#include <QFile>
#include <QDebug>

DeviceInstance::DeviceInstance(QDBusConnection* conn, int instanceNr, const QString& udi) :
conn(conn), instanceNr(instanceNr) {


	psettings.set(QLatin1String("remotereceiver.haludi"), udi);
	this->udi = udi;

	receiverState = LIRI_DEVICE_OFFLINE;
	remoteState = LIRI_REMOTE_NO;
	driver = 0;
	free_driver = 0;
	listenThread = new DeviceListenThread(this);
	connect(listenThread, SIGNAL( finished() ), SLOT( listenfinished() ));

	//important: Does not need to be deleted explicitly
	new ReceiverAdaptor(this);

	//register object
	qRegisterMetaType< QMap<QString,QString> >("QMap<QString,QString>");
	QString objectname;
	objectname.append(QLatin1String(LIRI_DBUS_OBJECT_RECEIVERS"/"));
	objectname.append(QString::number(instanceNr));
	if ( !conn->registerObject(objectname, static_cast<QObject*>(this)) ) {
		qWarning() << "RID:" << getInstanceNr() << ", Couldn't register DeviceInstance object:" << objectname;
	}
	setObjectName(objectname);
}

DeviceInstance::~DeviceInstance() {
	if (free_driver) {
		qWarning() << "Destroying DeviceInstance while listenThread is still running!";
		release();
		listenfinished();
	}
	QString objectname;
	objectname.append(QLatin1String(LIRI_DBUS_OBJECT_RECEIVERS"/"));
	objectname.append(QString::number(instanceNr));
	conn->unregisterObject(objectname);
	delete listenThread;
}

void DeviceInstance::listenfinished() {
	if (!listenThread->isFinished()) {
		/* be rude and kill listen thread if still running */
		listenThread->terminate();
		listenThread->wait();
	}

	if (free_driver) free_driver(driver);
	free_driver = 0;
	driver = 0;
	/* DriverHandle??? Shared between instances, but who frees it? */
	//free (driverhandle);
	//driverhandle = 0;

	/* disconnection time */
	psettings.set(QLatin1String("remotereceiver.disconnected"), QDateTime::currentDateTime().toString(Qt::ISODate));

	/* inform about new states
	   if receiverState is not LIRI_DEVICE_CANCEL then an error occurred
	   and the state has already been propagated to the list */
	if (ReceiverState() == LIRI_DEVICE_CANCEL)
		updateReceiverState(LIRI_DEVICE_OFFLINE);

	int rstate = RemoteState();
	if (rstate == LIRI_REMOTE_LOADED|| rstate == LIRI_REMOTE_RELOADED)
		updateRemoteState(LIRI_REMOTE_UNLOADED);

	qDebug() << "Released device:";
	qDebug() << "\tRID:" << getInstanceNr();
	qDebug() << "\tUDI:" << getUdi();

	emit releasedDevice(this);
}

void DeviceInstance::release() {
	/* if the driver is already released just emit some signals */
	if (!free_driver)
	{
		listenfinished();
	}
	/* else end listen thread */
	else {
		updateReceiverState(LIRI_DEVICE_CANCEL);
	}
}

/* load driver */
bool DeviceInstance::loaddriver() {

	// debug info - part 1/2
	qDebug() << "Initialise device:";
	qDebug() << "\tRID:" << getInstanceNr();
	qDebug() << "\tUDI:" << udi;
	qDebug() << "\tvendorId:" << psettings.get(QLatin1String("usb_device.vendor_id"));
	qDebug() << "\tproductId:" << psettings.get(QLatin1String("usb_device.product_id"));

	/* udi - unique device id (= hal id) */
	if (!udi.size()) {
		qWarning() << "\tFailed: Udi missing. Initalizing aborted!";
		updateReceiverState(LIRIERR_settings);
		return false;
	}

	/* connection time */
	psettings.set(QLatin1String("remotereceiver.connected"), QDateTime::currentDateTime().toString(Qt::ISODate));

	/* new state */
	updateReceiverState(LIRI_DEVICE_INIT);

	// check if filename exists
	if (!psettings.has(QLatin1String("remotereceiver.driver"))) {
		qWarning() << "\tFailed: Driver filename not set!";
		updateReceiverState(LIRIERR_filename);
		release();
		return false;
	}

	// check if filename exists - part 2
	QString driverid = psettings.get(QLatin1String("remotereceiver.driver"));
	QString drivername = QString::fromLatin1(LIRI_SYSTEM_DRIVERS_DIR"/%1.so").arg(driverid);
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
	liriDriver_Create* create_driver = (liriDriver_Create*) dlsym(driverhandle, "create");
	if (!create_driver) {
		qWarning() << "\tFailed: Symbol not found:" <<  dlerror();
		updateReceiverState(LIRIERR_symbolCreate);
		release();
		return false;
	}

	// load the symbol for destroying the class instance
	free_driver = (liriDriver_Free*) dlsym(driverhandle, "destroy");
	if (!free_driver) {
		qWarning() << "\tFailed: Symbol not found:" <<  dlerror();
		updateReceiverState(LIRIERR_symbolDestroy);
		release();
		return false;
	}

	// create an instance
	driver = create_driver(&psettings);

	// read version info from the meta driver file "driver.desktop"
	drivername = QString::fromLatin1(LIRI_SYSTEM_DRIVER_DESCRIPTION_DIR"/%1.desktop").arg(driverid);
	QFile file(drivername);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		// not successful, default = 0.0
		psettings.set(QLatin1String("remotereceiver.version"), QLatin1String("0.0"));
		qWarning() << "\tFailed: Can not read version for driver. Filename:" << drivername;
	} else {
		QStringList line;
		while (file.isReadable() && !file.atEnd()) {
			line = QString::fromUtf8(file.readLine()).split(QLatin1Char('='));
			if (line.size() && line[0] == QLatin1String("X-Driver-Version")) {
				// success
				psettings.set(QLatin1String("remotereceiver.version"), line[1].left(line[1].size()-1));
				break;
			}
		}
	}
	file.close();

	//read out config file -> get associated remote uid
	p_reloadAssociatedRemote();

	// debug info - part 2/2
	qDebug() << "\tDriver:" << driverid;
	qDebug() << "\tVersion:" << psettings.get(QLatin1String("remotereceiver.version"));
	qDebug() << "\tConfigfile:" << getConfigPath();
	qDebug() << "\tAss. Remote:" << psettings.get(QLatin1String("remote.new.uid"));

	// start listen thread
	listenThread->start();
	return true;
}

QString DeviceInstance::getConfigPath() {
	return QLatin1String(LIRI_SYSTEM_ASSOCIATED_REMOTE_DIR"/") + psettings.get(QLatin1String("remotereceiver.did"));
}

//read out config file -> get associated remote uid
void DeviceInstance::p_reloadAssociatedRemote() {
	QFile file(getConfigPath());
	file.open(QIODevice::ReadOnly);
	psettings.set(QLatin1String("remote.new.uid"), QString::fromAscii(file.readLine()));
}

void DeviceInstance::reloadAssociatedRemote() {
	/* do nothing if not active */
	if (ReceiverState() != LIRI_DEVICE_RUNNING) return;

	p_reloadAssociatedRemote();

	/* do the actuall work in the listen thread */
	updateRemoteState(LIRI_REMOTE_RELOAD);
}

void DeviceInstance::setAssociatedRemote(const QString &remoteid) {
	/* do nothing if not active */
	if (ReceiverState() != LIRI_DEVICE_RUNNING) return;

	psettings.set(QLatin1String("remote.new.uid"), remoteid);

	QFile file(getConfigPath());
	file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	file.write(remoteid.toAscii());
	file.close();

	/* do the actuall work in the listen thread */
	updateRemoteState(LIRI_REMOTE_RELOAD);
}

/* access */

const QString DeviceInstance::getUdi() const {
	return udi;
}

const int DeviceInstance::getInstanceNr() const {
	return instanceNr;
}

DeviceSettings* DeviceInstance::deviceSettings() {
	return &psettings;
}

/* states */
int DeviceInstance::ReceiverState() {
	int tmp;
	lockReceiverState.lockForRead();
	tmp = receiverState;
	lockReceiverState.unlock();
	return tmp;
}

int DeviceInstance::RemoteState() {
	int tmp;
	lockRemoteState.lockForRead();
	tmp = remoteState;
	lockRemoteState.unlock();
	return tmp;
}

void DeviceInstance::updateReceiverState(int receiverState) {
	lockReceiverState.lockForWrite();
	this->receiverState = receiverState;
	lockReceiverState.unlock();
	emit receiverStateChanged(receiverState);
}

void DeviceInstance::updateRemoteState(int remoteState) {
	lockRemoteState.lockForWrite();
	this->remoteState = remoteState;
	lockRemoteState.unlock();
	emit remoteStateChanged(remoteState);
}

QMap<QString,QString> DeviceInstance::getAllSettings() {
	return psettings.getSettings();
}

QStringList DeviceInstance::getSettings(const QStringList &keys) {
	QStringList tmp;
	foreach (QString key, keys) {
		tmp.append(psettings.get(key));
	}
	return tmp;
}

void DeviceInstance::setSettings(const QMap<QString,QString> &settings) {
	QMap<QString,QString>::const_iterator i = settings.constBegin();
	while (i != settings.constEnd()) {
		psettings.set(i.key(), i.value());
		++i;
	}
}

