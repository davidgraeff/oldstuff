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
#include <QSet>

class CardThread;
class plugin : public QObject
{
    Q_OBJECT

    Q_INTERFACES(AbstractPlugin AbstractPlugin_settings AbstractPlugin_services)
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
    CardThread* m_thread;
	EventMap<QString> m_card_events; //atr->set of uids

private Q_SLOTS:
    void slotcardDetected ( const QString& atr, int state );
};

