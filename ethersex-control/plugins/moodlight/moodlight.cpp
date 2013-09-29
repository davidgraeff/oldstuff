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
#include "moodlight.h"
#include "ui_moodlight.h"
#include "modelusedchannels.h"
#include "stella.h"
#include <cstdlib> //rand
#include <QtPlugin>
#include <QSettings>

Moodlight::Moodlight()
{
    w = 0;
    ui = 0;
    mChannels = 0;
}

Moodlight::~Moodlight()
{
    delete ui;
    delete mChannels;
    delete w;
}

QObject* Moodlight::createInstance()
{
    return new Moodlight();
}

void Moodlight::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        if (ui) ui->retranslateUi(w);
        break;
    default:
        break;
    }
}

void Moodlight::initInstance(stellaConnection* connection)
{
    this->connection = connection;

    // ui
    if (!w)
    {
        w = new QWidget();
        ui = new Ui::Moodlight;
        ui->setupUi(w);
		  mChannels = new modelUsedChannels(connection);
		  ui->usedchannels->setModel(mChannels);
		  connect(ui->fadestep, SIGNAL(valueChanged(int)), SLOT(fadestep_valueChanged(int)));
		  connect(ui->threshold,SIGNAL(valueChanged(int)), SLOT(threshold_valueChanged(int)));
		  connect(mChannels,SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &)), SLOT(data_changed()));
        QSettings settings;
        settings.beginGroup(QLatin1String("moodlight"));
        ui->fadestep->setValue(settings.value(QLatin1String("fadestep"),10).toInt());
		  ui->threshold->setValue(settings.value(QLatin1String("threshold"),120).toInt());
    }

    // initialy we want channel 1-3 for disco flickering ;)
}

void Moodlight::connectionEstablished()
{
	threshold_valueChanged(ui->threshold->value());
	fadestep_valueChanged(ui->fadestep->value());
}

stellaConnection* Moodlight::getConnection()
{
    return this->connection;
}

QString Moodlight::about()
{
    return tr("Set channels used for moodlight");
}

QString Moodlight::plugname()
{
    return tr("Moodlight");
}

QString Moodlight::authors()
{
    return tr("");
}

QWidget* Moodlight::widget()
{
    return w;
}

QList<QAction*> Moodlight::menuActions()
{
    return QList<QAction*>();
}

void Moodlight::data_changed()
{
        uint8_t v = 0;
	foreach (int channel, mChannels->usedchannels)
		v |= (1<<channel);
	QByteArray data;
        data.append(ECMDS_SET_STELLA_MOODLIGHT_MASK);
	data.append(v);
	connection->sendBytes(data);
}

void Moodlight::threshold_valueChanged(int v)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("moodlight"));
    settings.setValue(QLatin1String("threshold"), v);

	 QByteArray data;
         data.append(ECMDS_SET_STELLA_MOODLIGHT_THRESHOLD);
	 data.append(v);
	 connection->sendBytes(data);
}

void Moodlight::fadestep_valueChanged(int v)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("moodlight"));
    settings.setValue(QLatin1String("fadestep"), v);

    QByteArray data;
    data.append(ECMDS_SET_STELLA_FADE_STEP);
    data.append(v);
    connection->sendBytes(data);
}


Q_EXPORT_PLUGIN2(moodlight, Moodlight);
