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
#include "testcases.h"
#include "ui_testcases.h"
#include "stella.h"
#include <QtPlugin>
#include <QInputDialog>
#include <QVBoxLayout>

testcases::testcases()
{
    w = 0;
    ui = 0;
    countedCrobjobs = 0;
}

testcases::~testcases()
{
    delete ui;
    delete w;
}

QObject* testcases::createInstance()
{
    return new testcases();
}

void testcases::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        if (ui) ui->retranslateUi(w);
        break;
    default:
        break;
    }
}

void testcases::initInstance(stellaConnection* connection)
{
    this->connection = connection;

    // ui
    if (!w)
    {
        w = new QWidget();
        ui = new Ui::testcases;
        ui->setupUi(w);

        connect(connection, SIGNAL(cronjobs_update()), SLOT(cronjobs_update()));
        connect(connection, SIGNAL(cronjobs_counted(int)), SLOT(cronjobs_counted(int)));

        QVBoxLayout *gridLayout = new QVBoxLayout;
        QCommandLinkButton *button;

        button = new QCommandLinkButton(tr("Fade up"), tr("Test stella fade up method"),w);
        connect(button, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(button, 0);
        gridLayout->addWidget(button);

        button = new QCommandLinkButton(tr("Fade down"), tr("Test stella fade down method"),w);
        connect(button, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(button, 1);
        gridLayout->addWidget(button);

        button = new QCommandLinkButton(tr("Fade flash"), tr("Flash to 100% brightness and fade down afterwards"),w);
        connect(button, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(button, 2);
        gridLayout->addWidget(button);

        button = new QCommandLinkButton(tr("Fade step"), tr("Set stella fade step. 1 fast, 10+ slow"),w);
        connect(button, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(button, 3);
        gridLayout->addWidget(button);

        button = new QCommandLinkButton(tr("Count cronjobs"), tr("Count all cronjobs and show result in label"),w);
        connect(button, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(button, 4);
        gridLayout->addWidget(button);

        countedCrobjobs = new QLabel(tr("Not counted..."), w);
        gridLayout->addWidget(countedCrobjobs);

        button = new QCommandLinkButton(tr("Remove cron job"), tr("Remove first cronjob"),w);
        connect(button, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(button, 5);
        gridLayout->addWidget(button);

        button = new QCommandLinkButton(tr("Add cron job"), tr("Add stella cron for channel0 fade to value 250 at 7:30h every day"),w);
        connect(button, SIGNAL(clicked()), &signalMapper, SLOT(map()));
        signalMapper.setMapping(button, 6);
        gridLayout->addWidget(button);

        connect(&signalMapper, SIGNAL(mapped(int)),
             this, SLOT(clicked(int)));

        gridLayout->addStretch();

        w->setLayout(gridLayout);
    }
}

void testcases::connectionEstablished()
{

}

stellaConnection* testcases::getConnection()
{
    return this->connection;
}

QString testcases::about()
{
    return tr("Check stella server functionality");
}

QString testcases::plugname()
{
    return tr("Testcases");
}

QString testcases::authors()
{
    return tr("");
}

QWidget* testcases::widget()
{
    return w;
}

QList<QAction*> testcases::menuActions()
{
    return QList<QAction*>();
}

void testcases::clicked(int i)
{
    QByteArray data;
    switch (i)
    {
        case 0: //fade up
            for (char i=0;i<8;++i) {
                data.append(ECMDS_SET_STELLA_FADE_COLOR);
                data.append(i);
                data.append(255);
            }
            connection->sendBytes(data);
            break;

        case 1: //fade down
            for (char i=0;i<8;++i) {
                data.append(ECMDS_SET_STELLA_FADE_COLOR);
                data.append(i);
                data.append((char)0);
            }
            connection->sendBytes(data);
            break;

        case 2: // flash
            for (char i=0;i<8;++i) {
                data.append(ECMDS_SET_STELLA_FLASH_COLOR);
                data.append(i);
                data.append(255);
            }
            connection->sendBytes(data);
            break;

        case 3: // fade step
            bool ok;
            int i;
            i = QInputDialog::getInteger(w, tr("Get fading speed rate"),
                                          tr("Value:"), 10, 1, 20, 1, &ok);
            if (ok)
            {
                QByteArray data;
                data.append(ECMDS_SET_STELLA_FADE_STEP);
                data.append((unsigned char)i);
                connection->sendBytes(data);
            }
            break;

        case 4: // count cronjobs
            connection->countCronjobs();
            break;

        case 5: // remove cronjob
            connection->removeCronjob(0);
            break;

        case 6: // add cronjob (channel, value, function(1=fade), position, time(2x), date(3x), repeat)
            connection->addCronjob(250,30,1,
                                   -1,
                                   1, 7,
                                   -1,-1,-1,
                                   0);
            break;

        default:
        break;
    }
}

void testcases::cronjobs_update()
{

}

void testcases::cronjobs_counted(int size)
{
    if (countedCrobjobs)
        countedCrobjobs->setText(tr("Counted: %1").arg(QString::number(size)));
}

Q_EXPORT_PLUGIN2(testcases, testcases);
