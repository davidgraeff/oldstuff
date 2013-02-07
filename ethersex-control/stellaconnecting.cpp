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
#include "stellaconnecting.h"
#include "ui_stellaconnecting.h"
#include "stellaconnection.h"
#include "plugins_interface.h"
#include <QTimer>

stellaConnecting::stellaConnecting(stellaConnection* connection, StellaControlPlugin* plugin, int position, QWidget *parent) :
    QWidget(parent),
    plugin(plugin),
    position(position),
    m_ui(new Ui::StellaConnectingUI)
{
    m_ui->setupUi(this);
    m_ui->lblConnect->setText(tr("Init socket..."));
    connect(connection, SIGNAL(established()), SLOT(established()));
    connect(connection, SIGNAL(write_failure()), SLOT(write_failure()));
    connect(connection, SIGNAL(socket_failure()), SLOT(socket_failure()));
    connect(connection, SIGNAL(connecting()), SLOT(connecting()));

    state = StateConnecting;
    QTimer::singleShot(500,this, SLOT(init()));
}

stellaConnecting::~stellaConnecting()
{
    delete m_ui;
}

void stellaConnecting::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void stellaConnecting::init()
{
    plugin->getConnection()->init();
}

QString stellaConnecting::getName()
{
    if (state == StateConnecting)
        return tr("Loading (%1)").arg(plugin->getConnection()->getDestIP());
    else if (state == StateWriteFailure)
        return tr("Write failure (%1)").arg(plugin->getConnection()->getDestIP());
    else if (state == StateSocketFailure)
        return tr("Socket failure (%1)").arg(plugin->getConnection()->getDestIP());
    else
        return tr("Unknown (%1)").arg(plugin->getConnection()->getDestIP());
}

void stellaConnecting::established()
{
    // wrong protocol version
    int pversion = plugin->getConnection()->getProtocolVersion();
    if (pversion < STELLA_MIN && pversion > STELLA_MAX)
    {
        m_ui->lblConnect->setText(tr("Wrong stella protocol version.\n"
                                     "You have: v1.%1\nDevice reported: v1.%2")
                                  .arg(QString::number(STELLA_MAX))
                                  .arg(QString::number(pversion)));
    } else
        emit connected(plugin, position);
}

void stellaConnecting::socket_failure()
{
    state = StateSocketFailure;
    emit statechanged(plugin, position);
    m_ui->lblConnect->setText(tr("Socket bind failure!\nMaybe another program already use that port?"));
}

void stellaConnecting::write_failure()
{
    state = StateWriteFailure;
    emit statechanged(plugin, position);
    m_ui->lblConnect->setText(tr("Socket write failure!"));
}

void stellaConnecting::connecting()
{
    m_ui->lblConnect->setText(tr("Connecting..."));
}

void stellaConnecting::on_btnAbortConnection_clicked()
{
    emit aborted(plugin, position);
}
