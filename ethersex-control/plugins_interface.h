#ifndef STELLACONTROL_PLUGIN_H
#define STELLACONTROL_PLUGIN_H

#include <QtCore/qglobal.h>
#include <QtCore/QObject>
#include <QString>
#include <QWidget>
#include <QList>
#include <QAction>

#include "stellaconnection.h"

class StellaControlPlugin
{
public:
    // do not! do any initalisation in the constructor,
    // except setting heap variables to 0
    StellaControlPlugin() {}
    virtual ~StellaControlPlugin() {}

    // gui initalisation should be done here. You should not
    // expect "connection" to already be connected to the
    // stella device! (wrong channel_count, incorrect channel values)
    virtual void initInstance(stellaConnection* connection) = 0;

    // "connection" from above does provide accurate data now.
    // Now you can initialise anything that depends on e.g.
    // the channel_count.
    // If you want to be in-sync with the stella device, listen
    // to the channels_update signal of "connection".
    virtual void connectionEstablished() = 0;

    // return the connection from "initInstance"
    virtual stellaConnection* getConnection() = 0;

    // some statistics about your plugin
    virtual QString about() = 0;
    virtual QString plugname() = 0;
    virtual QString authors() = 0;

    // the main widget of your plugin. Must not be 0.
    virtual QWidget* widget() = 0;

    // actions, that you want to place in the menu.
    // You may return an empty list here.
    virtual QList<QAction*> menuActions() = 0;

    // return a new instance of your plugin.
    // Something like "return new PLUGIN_NAME;" is enough.
    virtual QObject* createInstance() = 0;

    // You may also implement the following signals:
    //public Q_SIGNALS:
    //// emit an error message. This will be shown in the statusbar.
    //void errorMessage(const QString& msg);
};

Q_DECLARE_INTERFACE(StellaControlPlugin,
                     "com.trolltech.Plugin.StellaControlPlugin/1.0");

#endif // STELLACONTROL_PLUGIN_H
