/*
    RoomControlServer. Home automation for controlling sockets, leds and music.
    Copyright (C) 2012  David Gräff

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Purpose: Network ssl socket for a communication to the server socket to allow to
	(1) request all properties
	(2) write a json request to the server
	(3) register as a remotesystem client
*/

#pragma once
#include <QObject>
#include <QSslSocket>

class NetworkController : public QSslSocket
{
    Q_OBJECT
    Q_PROPERTY(int state READ state NOTIFY stateChanged)
public:
	/// Singleton
    static NetworkController* instance();
	/// Remotesystem id (e.g. winclient1)
    void setId(const QByteArray& id);
	/// Register as a remotesystem client. The server may issue commands to you in reaction.
	void registerAsRemotesystemClient();
	int state() {return m_state;}
public Q_SLOTS:
	/// Request all properties
	void requestAllProperties();
	/// Send a document to the server
	void write(const QVariantMap& data);
	/// Connect to the server
    void connectToServer(QString host, int port);
private Q_SLOTS:
    void readyRead();
    void socketDisconnected();
    void socketConnected();
    void sslErrors ( const QList<QSslError> & errors );
	void socketerror(QAbstractSocket::SocketError);
private:
    QByteArray m_identifier;
    explicit NetworkController(QObject *parent = 0);
	int m_state;
Q_SIGNALS:
    void message(const QString& msg);
	/// A json document has been received
    void serverJSON(const QVariantMap& data);
	void stateChanged();
};
