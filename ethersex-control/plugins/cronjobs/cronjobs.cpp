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
#include "ui_cronjobs.h"
#include "stella.h"
#include <QtPlugin>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QDate>
#include <QFile>
#include <QFileDialog>


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

        ui = new Ui::cronjobs;
        ui->setupUi(w);
        ui->btnRmSelected->setEnabled(false);

        connect(connection, SIGNAL(cronjobs_update()), SLOT(cronjobs_update()));
        connect(connection, SIGNAL(cronjobs_counted(int)), SLOT(cronjobs_counted(int)));
        connect(ui->tableWidget,SIGNAL(clicked(QModelIndex)), SLOT(tableWidget_clicked(QModelIndex)));
        connect(ui->btnRmSelected,SIGNAL(clicked()), SLOT(btnRmSelected_clicked()));
        connect(ui->btnUpdate,SIGNAL(clicked()), SLOT(btnUpdate_clicked()));
        connect(ui->btnLoadFromFile,SIGNAL(clicked()),SLOT(btnLoadFromFile_clicked()));
        connect(ui->btnSaveToFile,SIGNAL(clicked()),SLOT(btnSaveToFile_clicked()));

        ui->tableWidget->setColumnCount(7);

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
    return tr("List of cronjobs");
}

QString cronjobs::plugname()
{
    return tr("List of cronjobs");
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

void cronjobs::cronjobs_update()
{
    ui->tableWidget->clear();
    while (ui->tableWidget->rowCount()) ui->tableWidget->removeRow(0);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() <<
        tr("Repeat") << tr("Hour") << tr("Minute") << tr("Day") << tr("Month") <<
        tr("Day of week") << tr("Command"));
    ui->btnRmSelected->setEnabled(false);

    int size = connection->getCronjobsSize();
    for (int i=0;i<size;++i)
    {
        ui->tableWidget->insertRow(i);
        // time
        int minute = connection->getCronjob(i).minute;
        int hour = connection->getCronjob(i).hour;
        QString Hour;
        QString Minute;
        if (hour>=0)
            Hour = QString(QLatin1String("%1")).arg(hour,2,10,QLatin1Char('0'));
        else if (hour < -1)
            Hour = QString(QLatin1String("Every %1")).arg(-hour);
        else
            Hour = tr("Always");

        if (minute>=0)
            Minute = QString(QLatin1String("%1")).arg(minute,2,10,QLatin1Char('0'));
        else if (minute < -1)
            Minute = QString(QLatin1String("Every %1")).arg(-minute);
        else
            Minute = tr("Always");
        // date
        int day = connection->getCronjob(i).day;
        int month = connection->getCronjob(i).month;
        QString Day;
        QString Month;
        if (day>=0)
            Day = QString(QLatin1String("%1")).arg(day,2,10,QLatin1Char('0'));
        else if (day < -1)
            Day = QString(QLatin1String("Every %1")).arg(-day);
        else
            Day = tr("Always");

        if (month>=0)
            Month = QString(QLatin1String("%1")).arg(month,2,10,QLatin1Char('0'));
        else if (month < -1)
            Month = QString(QLatin1String("Every %1")).arg(-month);
        else
            Month = tr("Always");
        //dayofweek
        int dayofweek = connection->getCronjob(i).dayofweek;
        if (dayofweek==0) dayofweek=7;
        QString dow = ((dayofweek==-1)?tr("Always"):QDate::longDayName(dayofweek));
        //repeat
        unsigned int rep = connection->getCronjob(i).repeat;
        QString repeat = ((rep==0)?tr("Unlimited"):QString::number(rep));

        QString cmd;
        if (connection->getCronjob(i).cmdsize>1)
        {
            cmd = trlist[ (uint8_t)connection->getCronjob(i).cmddata[0] ];
            if (connection->getCronjob(i).cmddata[0] == ECMDS_SET_STELLA_INSTANT_COLOR ||
                connection->getCronjob(i).cmddata[0] == ECMDS_SET_STELLA_FADE_COLOR ||
                connection->getCronjob(i).cmddata[0] == ECMDS_SET_STELLA_FLASH_COLOR)
            {
                cmd.append(tr(" (Channel: %1, Value: %2)").arg(QString::number((uint8_t)connection->getCronjob(i).cmddata[1]),
                                                    QString::number((uint8_t)connection->getCronjob(i).cmddata[2])));
            }
        }

        ui->tableWidget->setItem(i,0,new QTableWidgetItem(repeat,0));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(Hour,0));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(Minute,0));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(Day,0));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(Month,0));
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(dow,0));
        ui->tableWidget->setItem(i,6,new QTableWidgetItem(cmd,0));
    }
}

void cronjobs::cronjobs_counted(int size)
{
    emit errorMessage(tr("Counted %1 cron jobs").arg(size));
}

void cronjobs::btnUpdate_clicked()
{
    connection->refetchCronjobs();
}

void cronjobs::btnRmSelected_clicked()
{
    if (ui->tableWidget->currentRow()>=0)
        connection->removeCronjob(ui->tableWidget->currentRow());
}

void cronjobs::tableWidget_clicked(QModelIndex index)
{
    if (index.isValid())
    {
        ui->btnRmSelected->setEnabled(true);
    }
}

void cronjobs::btnSaveToFile_clicked()
{
    // select file name
    QString filename = QFileDialog::getSaveFileName(w,tr("Select file to save cronjob list"),QString(),tr("Cronjobs (*.crons)"));
    if (filename.isEmpty()) return;

    // create datastream
    QByteArray data;
    int c = connection->getCronjobsSize();
    for (int i=0;i<c;++i)
    {
        stellaConnection::cron_event_t job = connection->getCronjob(i);
        QByteArray t(cron_event_size+job.cmdsize,0);
        memcpy(t.data(),&job,t.size());
        data.append(t);
        data.append('\n');
    }
    // save
    QFile f(filename);
    f.open(QIODevice::Truncate|QIODevice::Text|QIODevice::WriteOnly);
    f.write(data);
    f.close();

    emit errorMessage(tr("Have %1 cronjobs written to file").arg(c));
}

void cronjobs::btnLoadFromFile_clicked()
{
    // select file name
    QString filename = QFileDialog::getOpenFileName(w,tr("Select file to open cronjob list"),QString(),tr("Cronjobs (*.crons)"));
    if (filename.isEmpty()) return;

    QList<stellaConnection::cron_event_t> jobs;
    // read file line by line
    QFile f(filename);
    f.open(QIODevice::Text|QIODevice::ReadOnly);
    while (!f.atEnd())
    {
        QByteArray t(f.readLine(sizeof(stellaConnection::cron_event_t)));
        t.chop(1); // remove new line charachter
        if (t.size()>=7 && t.size()<(int)sizeof(stellaConnection::cron_event_t))
        {
            stellaConnection::cron_event_t job;
            memcpy(&job,t.data(),t.size());
            jobs.append(job);
        }
    }
    f.close();

    emit errorMessage(tr("Read %1 cronjobs").arg(jobs.size()));

    // send to device (and hope that we did not exceed the udp packet size)
    if (jobs.size())
        connection->addCronjobs(jobs);
}

Q_EXPORT_PLUGIN2(cronjobs, cronjobs);
