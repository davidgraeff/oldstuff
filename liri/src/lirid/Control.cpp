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
#include "Control.h"
#include "Control_dbusif.h"
#include "config.h"

#include <QCoreApplication>
#include <QDebug>

// unix sockets / unix signals / don't echo control characters
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <iostream>


int Control::sigintFd[2];
int Control::sigtermFd[2];
QDBusConnection* Control::m_conn;

Control::Control(QDBusConnection* conn) {
    m_conn = conn;
    setObjectName(QLatin1String("Control"));
    quitFlag = false;
    startedtimedate = QDateTime::currentDateTime();

    // important: Does not need to be deleted explicitly
    new ControlAdaptor(this);

    // register object to dbus
    if ( !conn->registerObject(QLatin1String(LIRI_DBUS_OBJECT_RECEIVERS), static_cast<QObject*>(this)) ) {
        qWarning() << "Couldn't register Control object";
    }

    // sockets for unix signals -> qt signals
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd))
        qWarning() << "Couldn't create INT socketpair";

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
        qWarning() << "Couldn't create TERM socketpair";

    snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
    connect(snInt, SIGNAL(activated(int)), this, SLOT(handleSigInt()));
    snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    connect(snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));

    // connect unix signals
    struct sigaction sig;

    sig.sa_handler = Control::intSignalHandler;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;
    sig.sa_flags |= SA_RESTART;

    if (sigaction(SIGINT, &sig, 0) > 0)
        qWarning() << "Couldn't create SIGINT handler";

    sig.sa_handler = Control::termSignalHandler;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &sig, 0) > 0)
        qWarning() << "Couldn't create SIGTERM handler";

    sig.sa_handler = SIG_IGN;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags |= SA_RESTART;

    if (sigaction(SIGALRM, &sig, 0) > 0)
        qWarning() << "Couldn't create SIGALRM handler";

    sig.sa_handler = SIG_IGN;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags |= SA_RESTART;

    if (sigaction(SIGHUP, &sig, 0) > 0)
        qWarning() << "Couldn't create SIGHUP handler";

}

Control::~Control() {
    m_conn->unregisterObject(QLatin1String(LIRI_DBUS_OBJECT_RECEIVERS));
    delete snInt;
    delete snTerm;
}

QString Control::version() {
    return QLatin1String(ABOUT_VERSION);
}

void Control::intSignalHandler(int) {
    char a = 1;
    write(sigintFd[0], &a, sizeof(a));
    std::cout << '\n';
}

void Control::termSignalHandler(int) {
    char a = 1;
    write(sigtermFd[0], &a, sizeof(a));
    std::cout << '\n';
}

void Control::handleSigTerm() {
    snTerm->setEnabled(false);
    char tmp;
    read(sigtermFd[1], &tmp, sizeof(tmp));

    quit();

    snTerm->setEnabled(true);
}

void Control::handleSigInt() {
    snInt->setEnabled(false);
    char tmp;
    read(sigintFd[1], &tmp, sizeof(tmp));

    quit();

    snInt->setEnabled(true);
}

void Control::quit() {
    if (quitFlag) return;
    quitFlag = true;
    emit shutdown();
}

void Control::start() {
    halcallout = QDateTime::currentDateTime();
}

QString Control::lastHalCallout() {
    return halcallout.toString(Qt::ISODate);
}

QString Control::started() {
    return startedtimedate.toString(Qt::ISODate);
}
