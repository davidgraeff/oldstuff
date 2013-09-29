/***************************************************************************
 *   Copyright (C) 2009 by david   *
 *   david.graeff@web.de           *
 *                                                                         *
 *   StellaControl is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   StellaControl is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with StellaControl.  If not, see <http://www.gnu.org/licenses/> *
 ***************************************************************************/
#include "channels.h"
#include "ui_channels.h"
#include "stella.h"
#include <QtPlugin>
#include <QSettings>
#include <QInputDialog>
#include "../../stellaconnection.h"

Channels::Channels()
{
    w = 0;
    ui = 0;
}

Channels::~Channels()
{
    delete ui;
    delete w;
}

QObject* Channels::createInstance()
{
    return new Channels();
}

void Channels::initInstance(stellaConnection* connection)
{
    this->connection = connection;

    // ui
    if (!w)
    {
        w = new QWidget();
        ui = new Ui::ChannelsUI();
        ui->setupUi(w);
        connect(ui->actionChannelCount,SIGNAL(triggered()), SLOT(actionChannelCount_triggered()));
        connect(ui->btnRemovePredefined,SIGNAL(clicked()), SLOT(btnRemovePredefined_clicked()));
        connect(ui->btnSavePredefined,SIGNAL(clicked()), SLOT(btnSavePredefined_clicked()));
        connect(ui->predefinedlist,SIGNAL(activated(const QString&)), SLOT(predefinedlist_activated(const QString&)));
        connect(ui->predefinedlist,SIGNAL(editTextChanged(const QString&)), SLOT(predefinedlist_editTextChanged(const QString&)));

        connect(ui->actionLoad_from_EEProm,SIGNAL(triggered()),SLOT(actionLoad_from_EEProm_triggered()));
        connect(ui->actionSaveToEEProm,SIGNAL(triggered()),SLOT(actionSaveToEEProm_triggered()));
    }

    connect(connection, SIGNAL(timer_signal()), SLOT(timer_signal()));
    connect(connection, SIGNAL(channels_update()), SLOT(channels_update()));

    // read predefined channel sets
    QSettings settings;
    settings.beginGroup(QLatin1String("predefined"));
    QStringList predefined = settings.childGroups();
    ui->predefinedlist->clear();
    foreach (QString aset, predefined) ui->predefinedlist->addItem(aset);
    ui->predefinedlist->setCurrentIndex(-1);
    ui->predefinedlist->setEditText(tr("Enter new name here..."));
    ui->btnRemovePredefined->setEnabled(false);
    ui->btnSavePredefined->setEnabled(false);
    enternameText = true;

    settings_path = QLatin1String("hosts/") + connection->getDestIP() + QLatin1String("/");

}

void Channels::connectionEstablished()
{
    connection->refetchValues();
    makeChannelsAndLayout();
}

stellaConnection* Channels::getConnection()
{
    return this->connection;
}

QString Channels::about()
{
    return tr("Manipulate stella channels");
}

QString Channels::plugname()
{
    return tr("Channels");
}

QString Channels::authors()
{
    return tr("");
}

QWidget* Channels::widget()
{
    return w;
}

QList<QAction*> Channels::menuActions()
{
    QList<QAction*> list;
    list.append(ui->actionChannelCount);
    list.append(ui->actionLoad_from_EEProm);
    list.append(ui->actionSaveToEEProm);
    list.append(ui->actionRefetch);
    list.append(ui->actionResend);
    return list;
}

void Channels::on_actionRefetch_triggered()
{
    connection->refetchValues();
}

void Channels::on_actionResend_triggered()
{
    connection->resendValues();
}

void Channels::actionChannelCount_triggered()
{
     bool ok;
     int value = QInputDialog::getInteger(w, tr("Set channel count"),
                                      tr("Value:"), channels.size(), 1, connection->getChannelCount(), 1, &ok);
     if (ok)
     {
         makeChannelsAndLayout(value);
     }
}

void Channels::btnRemovePredefined_clicked()
{
    ui->btnSavePredefined->setEnabled(false);
    ui->btnRemovePredefined->setEnabled(false);

    QSettings settings;
    QString set_name = ui->predefinedlist->currentText().trimmed();
    QString path = QLatin1String("predefined/")+set_name;
    settings.remove(path);

    ui->predefinedlist->removeItem(ui->predefinedlist->currentIndex());
    ui->predefinedlist->setEditText(QString());
}

void Channels::btnSavePredefined_clicked()
{
    ui->btnSavePredefined->setEnabled(false);

    // get set name
    QString set_name = ui->predefinedlist->currentText().trimmed();
    if (set_name.size()==0) {
        emit errorMessage(tr("Not a valid predefined channel set name!"));
        return;
    }

    // add set with set_name to the combo box if not already inserted
    if ( ui->predefinedlist->findText(set_name)==-1) ui->predefinedlist->addItem(set_name);

    // save channel values
    QSettings settings;
    QString path = QLatin1String("predefined/")+set_name+QLatin1String("/ch");
    foreach (pwmchannel* ch, channels)
        settings.setValue(path+QString::number(ch->channel_no),ch->getValue());

    emit errorMessage(tr("Saved predefined channel set: %1").arg(set_name));
}

void Channels::predefinedlist_activated(const QString& set_name)
{
    ui->btnSavePredefined->setEnabled(false);
    ui->btnRemovePredefined->setEnabled(true);

    // get set name
    if (set_name.size()==0) {
        emit errorMessage(tr("Not a valid predefined channel set name!"));
        return;
    }
    // read channel values
    QSettings settings;
    QString path = QLatin1String("predefined/")+set_name+QLatin1String("/ch");
    foreach (pwmchannel* ch, channels)
        ch->setValue((unsigned char) settings.value(path+QString::number(ch->channel_no),0).toInt());

    emit errorMessage(tr("Loaded predefined channel set: %1").arg(set_name));
}

void Channels::predefinedlist_editTextChanged(const QString& str)
{
    if (str.isEmpty())
        ui->btnSavePredefined->setEnabled(false);
    else
        ui->btnSavePredefined->setEnabled(true);
    ui->btnRemovePredefined->setEnabled(false);
}

void Channels::makeChannelsAndLayout(int channel_count)
{
    QSettings settings;

    if (channel_count == -1)
        channel_count = settings.value(settings_path+QLatin1String("channel_count"), connection->getChannelCount()).toInt();
    else
        settings.setValue(settings_path+QLatin1String("channel_count"),channel_count);

    // get layout for removing and inserting channel ui parts
    QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(w->layout());
    Q_ASSERT(layout);

    // remove old channels
    foreach (QWidget* w, channels) layout->removeWidget(w);
    qDeleteAll(channels);
    channels.clear();

    // create channels (up to 8)
    for (unsigned char i=0;i<(unsigned char)channel_count;++i) {
        pwmchannel* ch = new pwmchannel(i, connection->getDestIP(), w);
        ch->setValue(connection->getChannelValue(i),true);
        layout->insertWidget(i, ch);
        ch->setVisible(true);
        channels.append(ch);
        connect(ch, SIGNAL(value_changed(unsigned char,unsigned char)),
                SLOT(value_changed(unsigned char, unsigned char)));
    }
}

void Channels::value_changed(unsigned char value, unsigned char channel)
{
    connection->setChannelValue(channel, value);
}


void Channels::actionSaveToEEProm_triggered()
{
    QByteArray data;
    data.append(ECMDS_SET_STELLA_SAVE_TO_EEPROM);
    connection->sendBytes(data);
    emit errorMessage(tr("Saved to EEProm"));
}

void Channels::actionLoad_from_EEProm_triggered()
{
    QByteArray data;
    data.append(ECMDS_SET_STELLA_LOAD_FROM_EEPROM);
    data.append(ECMDS_GET_STELLA_COLORS);
    connection->sendBytes(data);
    emit errorMessage(tr("Request values from EEProm"));
}

void Channels::channels_update()
{
    for (unsigned char i=0; i<channels.size();++i)
        channels[i]->setValue(connection->getChannelValue(i),true);
}

void Channels::timer_signal()
{
    // update channel values (fade buttons, animations)
    foreach (pwmchannel* ch, channels) ch->update();
}

Q_EXPORT_PLUGIN2(channels, Channels);

