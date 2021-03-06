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

#pragma once
#include <QObject>
#include <QStringList>
#include "_sharedsrc/abstractplugin.h"
#include "shared/abstractserver_collectioncontroller.h"

#include "shared/abstractserver_propertycontroller.h"
#include "shared/pluginservicehelper.h"
#include "shared/abstractplugin_services.h"

#include <QVector>
#include <shared/plugin_interconnect.h>

class Controller;
class plugin : public AbstractPlugin
{
    Q_OBJECT


public:
    plugin(const QString& instanceid);
    virtual ~plugin();

    virtual void initialize();
    virtual void clear();
    virtual void requestProperties(int sessionid);
    virtual void configChanged(const QByteArray& configid, const QVariantMap& data);
    virtual void execute ( const QVariantMap& data);
    virtual bool condition ( const QVariantMap& data) ;
    virtual void register_event ( const QVariantMap& data, const QString& collectionuid);
    virtual void unregister_event ( const QString& eventid);
private:
    virtual void dataFromPlugin(const QByteArray& plugin_id, const QByteArray& data);
    Controller* m_controller;
    EventMap<int> m_events; //mode->set of uids
    QVector<bool> m_sensors;
    bool m_read;
private Q_SLOTS:
    /**
     * Updated led state.
     */
    void ledChanged ( QString,QString = QString::null,int = -1);
    void watchpinChanged(const unsigned char port, const unsigned char pinmask);
    void ledsCleared();
};
