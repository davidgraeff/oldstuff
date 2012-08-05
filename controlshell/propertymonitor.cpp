#include "propertymonitor.h"

#include <QDebug>
#include "../../config.h"
#include <QCoreApplication>

propertymonitor::propertymonitor()
{
    connect(&m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(&m_socket, SIGNAL(connected()), SLOT(serverconnected()));
    connect(&m_socket, SIGNAL(disconnected()), SLOT(serverdisconnected()));
    connect(&m_socket, SIGNAL(sslErrors(QList<QSslError>)),SLOT(sslErrors(QList<QSslError>)));

    m_socket.ignoreSslErrors();
    m_socket.connectToHostEncrypted("192.168.1.2", ROOM_LISTENPORT);
}

propertymonitor::~propertymonitor()
{}

void propertymonitor::serverconnected()
{
    qDebug() << "Connected";
    m_socket.write("{\"type_\":\"execute\",\"member_\":\"requestAllProperties\",\"plugin_\":\"server\"}\n");
}

void propertymonitor::serverdisconnected()
{
    qDebug() << "Disconnected";
    QCoreApplication::exit();
}

void propertymonitor::sslErrors ( const QList<QSslError> & errors ) {
    qDebug() << errors;
    m_socket.ignoreSslErrors();
}

void propertymonitor::readyRead()
{
    qDebug() << m_socket.readAll();
}

#include "propertymonitor.moc"
