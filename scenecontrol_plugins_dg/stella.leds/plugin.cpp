/*
 *    RoomControlServer. Home automation for controlling sockets, leds and music.
 *    Copyright (C) 2010  David Gräff
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <QDebug>

#include "plugin.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    if (argc<4) {
		qWarning()<<"Usage: instance_id server_ip server_port";
		return 1;
	}
    
    if (AbstractPlugin::createInstance<plugin>(PLUGIN_ID,argv[1],argv[2],argv[3])==0)
        return 2;
    return app.exec();
}

plugin::~plugin() {
    delete m_socket;
}

void plugin::clear() {
    m_connectTimer.stop();
    m_leds.clear();
	
	changeProperty(SceneDocument::createModelReset("udpled.values", "channel").getData());
	
	SceneDocument target;
	target.setComponentID(m_pluginid);
	target.setInstanceID(m_instanceid);
	SceneDocument doc;
	doc.setMethod("clear");
	doc.setData("target",target.getData());	
	doc.setComponentID(QLatin1String("scenecontrol.leds"));
	doc.setInstanceID(QLatin1String("null"));
	callRemoteComponent(doc.getData());
}

void plugin::initialize() {
	connect(&m_connectTimer, SIGNAL(timeout()), SLOT(resendConnectSequence()));
	m_connectTimer.setSingleShot(true);
	m_connectTime=1000;
	delete m_socket;
	m_socket = new QUdpSocket(this);
	m_socket->bind();
	connect(m_socket,SIGNAL(readyRead()),SLOT(readyRead()));
}

void plugin::instanceConfiguration(const QVariantMap& data) {
    
    if (!data.contains(QLatin1String("port")))
		return;
	
	clear();
	m_sendPort = data[QLatin1String("port")].toInt();
	resendConnectSequence();
}

bool plugin::isLedValue( const QString& channel, int lower, int upper ) {
    const int v = getLed ( channel );
    if ( v>upper ) return false;
    if ( v<lower ) return false;
    return true;
}

void plugin::requestProperties() {
	changeProperty(SceneDocument::createModelReset("udpled.values", "channel").getData(), m_lastsessionid);

    QMap<QString, plugin::ledchannel>::iterator i = m_leds.begin();
    for (;i!=m_leds.end();++i) {
        {
			SceneDocument sc = SceneDocument::createModelChangeItem("udpled.values");
            sc.setData("channel", i.key());
            sc.setData("value", i.value().value);
            changeProperty(sc.getData(), m_lastsessionid);
        }
    }
}

void plugin::ledChanged(QString channel, int value) {
	SceneDocument sc = SceneDocument::createModelChangeItem("udpled.values");
    sc.setData("channel", channel);
    if (value != -1) sc.setData("value", value);
    changeProperty(sc.getData());

	SceneDocument target;
	target.setComponentID(m_pluginid);
	target.setInstanceID(m_instanceid);
	SceneDocument doc;
	doc.setMethod("subpluginChange");
	doc.setData("target",target.getData());
	doc.setComponentID(QLatin1String("scenecontrol.leds"));
	doc.setInstanceID(QLatin1String("null"));
	doc.setData("channel",channel);
	doc.setData("value",value);
	doc.setData("name",QString());
	callRemoteComponent(doc.getData());
}

int plugin::getLed ( const QString& channel ) const {
    return m_leds.value ( channel ).value;
}

bool plugin::toggleLed ( const QString& channel, int fade ) {
    if ( !m_leds.contains(channel) ) return false;
    const unsigned int newvalue = 255 - m_leds[channel].value;
    return setLed ( channel, newvalue, fade );
}

bool plugin::setLedExponential ( const QString& channel, int multiplikator, int fade ) {
    if ( !m_leds.contains(channel) ) return false;
    unsigned int v = m_leds[channel].value;
    if ( multiplikator>100 ) {
        if ( v==0 )
            v=1;
        else if ( v==1 )
            v=2;
        else
            v = ( v * multiplikator ) /100+1;
    } else {
        if ( v<2 )
            v = 0;
        else
            v = ( v * multiplikator ) /100-1;
    }

    return setLed ( channel, v, fade );
}

bool plugin::setLedRelative ( const QString& channel, int value, int fade ) {
    return setLed ( channel,  value + m_leds[channel].value, fade );
}


int plugin::countChannels() {
    return m_channels;
}

bool plugin::setLed ( const QString& channel, int value, int fade ) {
    if ( !m_socket ) return false;
    if ( !m_leds.contains(channel) ) return false;
    ledchannel* l = &(m_leds[channel]);

    value = qBound ( 0, value, 255 );
    l->value = value;
    ledChanged ( channel, value );

    struct
    {
        uint8_t type;    // see above
        uint8_t channel; // if port: pin
        uint8_t value;
    } data;

    data.type = fade;
    data.channel = l->channel;
    data.value = value;

	m_socket->writeDatagram ( (char*)&data, sizeof ( data ), l->remote, m_sendPort );
	return true;
}

void plugin::readyRead() {
    m_connectTimer.stop();
    while (m_socket->hasPendingDatagrams()) {
        QByteArray bytes;
		QHostAddress remote;
        bytes.resize ( m_socket->pendingDatagramSize());
		m_socket->readDatagram ( bytes.data(), bytes.size(), &remote  );
		//qDebug() << "receive leds" << bytes.size();
        while ( bytes.size() > 6 ) {
            if (bytes.startsWith("stella") && bytes.size() >= 7+bytes[6])  {
                m_channels = bytes[6];
                clear();
                for (uint8_t c=0;c<m_channels;++c) {
                    const unsigned int value = (uint8_t)bytes[7+c];
					m_leds[QString::number(c)] = ledchannel(c, value, remote);
                    ledChanged(QString::number(c), value);
                }
                bytes = bytes.mid(7+m_channels);
            } else {
                qWarning() << "Failed to parse" << bytes << bytes.size() << 7+bytes[6];
                break;
            }
        } //while
    }
}


void plugin::resendConnectSequence() {
    // request all channel values
    const unsigned char b[] = {255,0,0};
	//qDebug() << "request leds" << m_sendPort;
	m_socket->writeDatagram ( (const char*)b, sizeof ( b ), QHostAddress::Broadcast, m_sendPort);
    m_socket->flush();
    m_connectTime *= 2;
    if (m_connectTime>60000)
        m_connectTime=60000;
    m_connectTimer.start(m_connectTime);
}

