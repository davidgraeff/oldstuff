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
#include <QtPlugin>

#include "plugin.h"



plugin::plugin(const QString& instanceid) : AbstractPlugin(instanceid) {
}

plugin::~plugin() {
    close_pulseaudio();
}

void plugin::clear() {}
void plugin::initialize() {
    reconnect_to_pulse(this);
}

void plugin::configChanged(const QByteArray& configid, const QVariantMap& data) {}

void plugin::execute ( const QVariantMap& data) {
	Q_UNUSED(sessionid);
    if ( ServiceData::isMethod(data, "pulsechannelmute" ) ) {
        set_sink_muted(DATA("sinkid").toUtf8().constData(), INTDATA ( "mute" ) );
    } else if ( ServiceData::isMethod(data, "pulsechannelvolume" ) ) {
        if (BOOLDATA ( "relative" )) {
            set_sink_volume_relative(DATA("sinkid").toUtf8(), DOUBLEDATA ( "volume" ));
        } else {
            set_sink_volume(DATA("sinkid").toUtf8(), DOUBLEDATA ( "volume" ));
        }
    }
}

bool plugin::condition ( const QVariantMap& data)  {
    Q_UNUSED ( data );
	Q_UNUSED(sessionid);
    return false;
}

void plugin::register_event ( const QVariantMap& data, const QString& collectionuid) { 
	Q_UNUSED(sessionid);
    Q_UNUSED ( data );
	Q_UNUSED(collectionuid);
}

void plugin::unregister_event ( const QString& eventid) { 
	Q_UNUSED(sessionid);
	Q_UNUSED(data);
	Q_UNUSED(collectionuid);
}

void plugin::requestProperties(int sessionid) {
    Q_UNUSED(sessionid);

    {
        ServiceData sc = ServiceData::createNotification(PLUGIN_ID,  "pulse.version" );
        sc.setData("protocol", getProtocolVersion());
        sc.setData("server", getServerVersion());
		changeProperty(sc.getData());
    }
    {
        ServiceData sc = ServiceData::createModelReset( "pulse.channels", "sinkid" );
        changeProperty(sc.getData());
    }
    QList<PulseChannel> channels = getAllChannels();
    foreach(PulseChannel channel, channels) {
        ServiceData sc = ServiceData::createModelChangeItem( "pulse.channels" );
        sc.setData("sinkid", channel.sinkid);
        sc.setData("mute", channel.mute);
        sc.setData("volume", channel.volume);
        changeProperty(sc.getData());
    }
    return l;
}

void plugin::pulseSinkChanged ( const PulseChannel& channel ) {
    ServiceData sc = ServiceData::createModelChangeItem( "pulse.channels" );
    sc.setData("sinkid", channel.sinkid);
    sc.setData("mute", channel.mute);
    sc.setData("volume", channel.volume);
    changeProperty ( sc.getData() );
}

void plugin::pulseVersion(int protocol, int server) {
    ServiceData sc = ServiceData::createNotification(PLUGIN_ID,  "pulse.version" );
    sc.setData("protocol", protocol);
    sc.setData("server", server);
    changeProperty ( sc.getData() );
}
