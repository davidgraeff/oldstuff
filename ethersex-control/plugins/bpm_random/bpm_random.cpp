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
#include "bpm_random.h"
#include "ui_bpm_random.h"
#include "modelusedchannels.h"
#include "stella.h"
#include <cstdlib> //rand
#include <QtPlugin>
#include <QSettings>

BPMRandom::BPMRandom()
{
    w = 0;
    ui = 0;
    mChannels = 0;
    connect(&timer_beatcontrol, SIGNAL(timeout()), SLOT(timer_beatcontrol_timeout()));
}

BPMRandom::~BPMRandom()
{
    delete ui;
    delete mChannels;
    delete w;
}

QObject* BPMRandom::createInstance()
{
    return new BPMRandom();
}

void BPMRandom::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        if (ui) ui->retranslateUi(w);
        break;
    default:
        break;
    }
}

void BPMRandom::initInstance(stellaConnection* connection)
{
    this->connection = connection;

    // ui
    if (!w)
    {
        w = new QWidget();
        ui = new Ui::BPMRandom;
        ui->setupUi(w);
        connect(ui->fadestep, SIGNAL(valueChanged(int)), SLOT(on_fadestep_valueChanged(int)));
        connect(ui->bpm,SIGNAL(valueChanged(int)), SLOT(on_bpm_valueChanged(int)));
        mChannels = new modelUsedChannels(connection);
        ui->usedchannels->setModel(mChannels);
    }

    // initialy we want channel 1-3 for disco flickering ;)
}

void BPMRandom::connectionEstablished()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("bpm_random"));
    ui->useFade->setChecked(settings.value(QLatin1String("useFade"),true).toBool());
    ui->fadestep->setValue(settings.value(QLatin1String("fadestep"),10).toInt());
    ui->bpm->setValue(settings.value(QLatin1String("bpm"),120).toInt());
    ui->minColor->setValue(settings.value(QLatin1String("minColor"),100).toInt());
    ui->maxColor->setValue(settings.value(QLatin1String("maxColor"),255).toInt());

    //on_bpm_valueChanged(ui->bpm->value());
    //on_fadestep_valueChanged(ui->fadestep->value());
}

stellaConnection* BPMRandom::getConnection()
{
    return this->connection;
}

QString BPMRandom::about()
{
    return tr("Change colors of selected channels randomly according to beats per minute value");
}

QString BPMRandom::plugname()
{
    return tr("Beats (Random Colors)");
}

QString BPMRandom::authors()
{
    return tr("");
}

QWidget* BPMRandom::widget()
{
    return w;
}

QList<QAction*> BPMRandom::menuActions()
{
    return QList<QAction*>();
}

void BPMRandom::on_bpm_valueChanged(int v)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("bpm_random"));
    settings.setValue(QLatin1String("bpm"), v);

    v = 60000/v;
    timer_beatcontrol.start(v);
}

void BPMRandom::on_fadestep_valueChanged(int v)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("bpm_random"));
    settings.setValue(QLatin1String("fadestep"), v);

    QByteArray data;
    data.append(ECMDS_SET_STELLA_FADE_STEP);
    data.append(v);
    connection->sendBytes(data);
}

void BPMRandom::on_minColor_valueChanged(int v)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("bpm_random"));
    settings.setValue(QLatin1String("minColor"), v);

}

void BPMRandom::on_maxColor_valueChanged(int v)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("bpm_random"));
    settings.setValue(QLatin1String("maxColor"), v);

}

void BPMRandom::on_useFade_clicked(bool checked)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("bpm_random"));
    settings.setValue(QLatin1String("useFade"), checked);

}

void BPMRandom::timer_beatcontrol_timeout()
{
    QByteArray data;

    int min = ui->minColor->value();
    int max = ui->maxColor->value() - ui->minColor->value();
    if (max<=0) return;

    for (char i=0;i<mChannels->usedchannels.size();++i)
    {
        if (ui->useFade->isChecked())
        { // fade
            data.append(ECMDS_SET_STELLA_FADE_COLOR);
            data.append(mChannels->usedchannels[i]);
            data.append((char)(rand() % max+min));
        } else
        { // no fade
            data.append(ECMDS_SET_STELLA_INSTANT_COLOR);
            data.append(mChannels->usedchannels[i]);
            data.append((char)(rand() % max+min));
        }
    }
    if (mChannels->usedchannels.size()) connection->sendBytes(data);
}

Q_EXPORT_PLUGIN2(bpmrandom, BPMRandom);
