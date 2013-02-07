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
#include "cronjobs.h"
#include "stellaconnection.h"
#include "ui_cronjobs.h"
#include "stella.h"
#include <QtPlugin>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QDate>
#include <QFile>
#include <QFileDialog>

QListWidgetItemEx::QListWidgetItemEx(int ch, int val, int func, const QString & text, QListWidget * parent) :
        QListWidgetItem(text, parent), ch(ch), val(val), func(func)
{}

cronjobs::cronjobs()
{
    w = 0;
    ui = 0;
    countedCrobjobs = 0;
    trlist.append(QLatin1String("get protocol version"));
    trlist.append(QLatin1String("get ethersex version"));
    trlist.append(QLatin1String("get ethersex mac ip gw mask"));
    trlist.append(QLatin1String("set ethersex mac"));
    trlist.append(QLatin1String("set ethersex ip"));
    trlist.append(QLatin1String("set ethersex gw ip"));
    trlist.append(QLatin1String("set ethersex netmask"));
    trlist.append(QLatin1String("set ethersex eventmask"));
    trlist.append(QLatin1String("reserved1a"));
    trlist.append(QLatin1String("reserved2a"));
    trlist.append(QLatin1String("reserved3a"));
    trlist.append(QLatin1String("action reset"));
    trlist.append(QLatin1String("action bootloader"));
    trlist.append(QLatin1String("get stella colors"));
    trlist.append(QLatin1String("get stella fade func step"));
    trlist.append(QLatin1String("get stella moodlight data"));
    trlist.append(QLatin1String("stella instant color"));
    trlist.append(QLatin1String("stella fade color"));
    trlist.append(QLatin1String("stella flash color"));
    trlist.append(QLatin1String("stella fade func"));
    trlist.append(QLatin1String("stella fade step"));
    trlist.append(QLatin1String("stella save to eeprom"));
    trlist.append(QLatin1String("stella load from eeprom"));
    trlist.append(QLatin1String("stella moodlight mask"));
    trlist.append(QLatin1String("stella moodlight threshold"));
    trlist.append(QLatin1String("reserved1b"));
    trlist.append(QLatin1String("reserved2b"));
    trlist.append(QLatin1String("reserved3b"));
    trlist.append(QLatin1String("get cron count"));
    trlist.append(QLatin1String("get crons"));
    trlist.append(QLatin1String("set cron remove"));
    trlist.append(QLatin1String("set cron add"));
    trlist.append(QLatin1String("reserved1c"));
    trlist.append(QLatin1String("get portpins"));
    trlist.append(QLatin1String("set portpin"));
    trlist.append(QLatin1String("jump to function"));
}

cronjobs::~cronjobs()
{
    delete ui;
    delete w;
}

QObject* cronjobs::createInstance()
{
    return new cronjobs();
}

void cronjobs::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        if (ui) ui->retranslateUi(w);
        break;
    default:
        break;
    }
}

void cronjobs::initInstance(stellaConnection* connection)
{
    this->connection = connection;

    // ui
    if (!w)
    {
        w = new QWidget();

        cw = new QDialog(w);
        cw->setWindowModality(Qt::WindowModal);
        QHBoxLayout* lay = new QHBoxLayout(cw);
        QCalendarWidget* cww = new QCalendarWidget(cw);
        cww->showToday();
        connect(cww,SIGNAL(clicked(QDate)), SLOT(calendar_clicked(QDate)));
        lay->addWidget(cww);
        cw->setLayout(lay);

        ui = new Ui::cronjobs;
        ui->setupUi(w);
        ui->btnRemoveAction->setEnabled(false);
        ui->btnAddCronjob->setEnabled(false);

        connect(connection, SIGNAL(cronjobs_counted(int)), SLOT(cronjobs_counted(int)));
        connect(ui->stellaActions,SIGNAL(clicked(QModelIndex)), SLOT(stellaActions_clicked(QModelIndex)));
        connect(ui->btnNow,SIGNAL(clicked()), SLOT(btnNow_clicked()));
        connect(ui->btnAddCronjob,SIGNAL(clicked()), SLOT(btnAddCronjob_clicked()));
        connect(ui->btnAddAction,SIGNAL(clicked()), SLOT(btnAddAction_clicked()));
        connect(ui->btnRemoveAction,SIGNAL(clicked()), SLOT(btnRemoveAction_clicked()));
        connect(ui->btnCalendar,SIGNAL(clicked()),SLOT(btnCalendar_clicked()));

        ui->spinFunction->addItem(tr("Set channel"));
        ui->spinFunction->addItem(tr("Fade channel"));
        ui->spinFunction->addItem(tr("Flash channel"));

        ui->spinDayofweek->addItem(tr("Always"), (int)-1);
        for (int i=1;i<8;++i)
            ui->spinDayofweek->addItem(QDate::longDayName(i), (int)((i==7)?0:i));

    }
}

void cronjobs::connectionEstablished()
{
    connection->refetchCronjobs();
}

stellaConnection* cronjobs::getConnection()
{
    return this->connection;
}

QString cronjobs::about()
{
    return tr("Add stella cronjobs");
}

QString cronjobs::plugname()
{
    return tr("Cronjobs (Stella)");
}

QString cronjobs::authors()
{
    return tr("");
}

QWidget* cronjobs::widget()
{
    return w;
}

QList<QAction*> cronjobs::menuActions()
{
    return QList<QAction*>();
}

void cronjobs::cronjobs_counted(int size)
{
    emit errorMessage(tr("Counted %1 cron jobs").arg(size));
}

void cronjobs::calendar_clicked ( const QDate & date )
{
    cw->hide();
    ui->spinMonth->setValue(date.month());
    ui->spinDay->setValue(date.day());
}

void cronjobs::btnCalendar_clicked()
{
    cw->show();
    //cw->exec();
}

void cronjobs::btnNow_clicked()
{
    QTime t = QTime().currentTime();

    ui->spinHour->setValue(t.hour());
    ui->spinMinute->setValue(t.minute());
}

void cronjobs::btnAddCronjob_clicked()
{
    stellaConnection::cron_event_t cron;
    cron.minute = ui->spinMinute->value();
    cron.hour = ui->spinHour->value();
    cron.day = ui->spinDay->value();
    cron.month = ui->spinMonth->value();
    cron.dayofweek = ui->spinDayofweek->itemData(ui->spinDayofweek->currentIndex()).toInt();
    cron.repeat = ui->spinRepeat->value();
    cron.cmdsize = ui->stellaActions->count() * 3;
    for (int i=0;i< ui->stellaActions->count(); ++i)
    {
        QListWidgetItemEx* item = (QListWidgetItemEx*)ui->stellaActions->item(i);
        if (item->func==0)
            cron.cmddata[i*3+0] = ECMDS_SET_STELLA_INSTANT_COLOR;
        else if (item->func==1)
            cron.cmddata[i*3+0] = ECMDS_SET_STELLA_FADE_COLOR;
        else if (item->func==2)
            cron.cmddata[i*3+0] = ECMDS_SET_STELLA_FLASH_COLOR;
        else
            cron.cmddata[i*3+0] = ECMDS_SET_STELLA_FADE_COLOR;
        cron.cmddata[i*3+1] = item->ch;
        cron.cmddata[i*3+2] = item->val;
    }

    connection->addCronjob(cron);
}

void cronjobs::btnRemoveAction_clicked()
{
    delete ui->stellaActions->currentItem();
    if (!ui->stellaActions->currentItem())
        ui->btnRemoveAction->setEnabled(false);
}

void cronjobs::btnAddAction_clicked()
{
    ui->btnAddCronjob->setEnabled(true);

    QListWidgetItemEx* item = new QListWidgetItemEx(ui->spinChannel->value(),
                                                    ui->spinValue->value(),
                                                    ui->spinFunction->currentIndex(),
                                                    tr("%1 %2 to %3").
                                                    arg(ui->spinFunction->currentText()).
                                                    arg(ui->spinChannel->value()).
                                                    arg(ui->spinValue->value()));
    ui->stellaActions->addItem(item);
}

void cronjobs::stellaActions_clicked(QModelIndex index)
{
    if (index.isValid())
    {
        ui->btnRemoveAction->setEnabled(true);
    }
}

Q_EXPORT_PLUGIN2(cronjobs_stella, cronjobs);
