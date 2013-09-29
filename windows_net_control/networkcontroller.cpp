#include "networkcontroller.h"
#include "libdatabase/json.h"
#include <QDebug>
#include <QHostAddress>

static NetworkController *i;

NetworkController::NetworkController(QObject *parent) :
    QSslSocket(parent), m_state(0)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(this, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(this, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(this, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketerror(QAbstractSocket::SocketError)));
    this->ignoreSslErrors();
    this->setProtocol(QSsl::SslV3);

    m_identifier = "winclient1";
}

void NetworkController::connectToServer(QString host, int port)
{
    if (state()==QAbstractSocket::ConnectedState || state() ==QAbstractSocket::ConnectingState) {
        emit message(tr("Already connected!"));
        return;
    }
    m_state = 1;
    emit stateChanged();
    this->connectToHostEncrypted(host, port);
}

NetworkController *NetworkController::instance()
{
    if(!i)
        i = new NetworkController();
    return i;
}

void NetworkController::setId(const QByteArray &id)
{
    m_identifier = id;
}

void NetworkController::requestAllProperties()
{
    if(m_state == 2)
        QSslSocket::write("{\"type_\":\"execute\",\"member_\":\"requestAllProperties\",\"plugin_\":\"server\"}\n");
}

void NetworkController::registerAsRemotesystemClient()
{
    if(m_state == 2)
        QSslSocket::write("{\"type_\":\"execute\", \"plugin_\":\"remotesystem\", \"member_\":\"registerclient\","
                          "\"host\":\"" + this->localAddress().toString().toUtf8() + "\", \"identifier\":\"" + m_identifier + "\"}\n");
}

void NetworkController::write(const QVariantMap &data)
{
    if(m_state == 2)
        QSslSocket::write(JSON::stringify(data).toUtf8() + "\n");
}

void NetworkController::sslErrors(const QList<QSslError> & errors)
{
    QList<QSslError> relevantErrors(errors);
	for (int i=relevantErrors.size()-1;i>=0;--i)
		if (relevantErrors[i].error() == QSslError::SelfSignedCertificate ||
			relevantErrors[i].error() == QSslError::SelfSignedCertificateInChain ||
			QSslError::HostNameMismatch
		)
			relevantErrors.removeAt(i);
    this->ignoreSslErrors();

	if (relevantErrors.size()) {
		qWarning() << relevantErrors;
		m_state = 0;
		emit stateChanged();
	}
}

void NetworkController::socketerror(QAbstractSocket::SocketError e)
{
	qWarning() <<  e;
    m_state = 0;
    emit stateChanged();
	emit message(tr("NetworkController: Error: %1").arg(this->errorString()));
}

void NetworkController::readyRead()
{
    QSslSocket *serverSocket = (QSslSocket *)sender();
    while(serverSocket->canReadLine()) {
        const QByteArray rawdata = serverSocket->readLine();
        if(!rawdata.length())
            continue;
        QVariant v = JSON::parse(rawdata);
        if(!v.isNull() || v.type() != QVariant::Map) {
            const QVariantMap m = v.toMap();
            if(m.value(QLatin1String("response"), 0).toInt() == 0)
                emit serverJSON(m);
            else
                qWarning() << "Server response: failure" << m;
        } else {
            qWarning() << "Failed to parse json from server" << rawdata;
        }
    }
}

void NetworkController::socketDisconnected()
{
	m_state = 0;
	emit stateChanged();
	qDebug() << "NetworkController: Disconnected" << this->peerAddress().toString();
	emit message(tr("NetworkController: Disconnected from %1").arg(this->peerAddress().toString()));
}

void NetworkController::socketConnected()
{
	m_state = 2;
	emit stateChanged();
	qDebug() << "NetworkController: Connected" << this->peerAddress().toString();
	emit message(tr("NetworkController: Connected to %1").arg(this->peerAddress().toString()));
}
