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
#include "stellaconnection.h"
#include "stella.h"
#include <QDebug>

stellaConnection::stellaConnection(const QHostAddress& dest_ip, int dest_port, int source_port)
{
    // init values
    this->stella_host = dest_ip;
    this->stella_port = dest_port;
    this->source_port = source_port;
    connID = stella_host.toString()+QString::number(stella_port);
    protocol_version = 0;
    channel_count = 0;

    // init timers
    timer_connect.setInterval(500); // timeout for fetching data
    timer_send.setInterval(20);
    connect((&timer_send), SIGNAL(timeout()), SLOT(timer_send_timeout()));
    connect((&timer_send), SIGNAL(timeout()), SIGNAL(timer_signal()));
    connect((&timer_connect), SIGNAL(timeout()), SLOT(timer_connect_timeout()));

    // reset byte counter
    counter_out = 0;
    counter_in = 0;
    counter_connection = 0;

    connect(&socket, SIGNAL(readyRead()), SLOT(readPendingDatagrams()));
    socket.close();
    valid_socket = socket.bind(source_port);
    valid_connection = false;

    reference_counter = 0;

    // init with 0
    for (int i=0;i<8;++i)
        channels[i] = 0;
}

stellaConnection::~stellaConnection()
{
    // stop all timers
    timer_send.stop();
    timer_connect.stop();
}

void stellaConnection::addRef()
{
    ++reference_counter;
}

void stellaConnection::removeRef()
{
    --reference_counter;
}

bool stellaConnection::isReferenced()
{
    return (reference_counter>0);
}

int stellaConnection::getReferences()
{
    return reference_counter;
}

void stellaConnection::init()
{
    if (!valid_socket)
    {
        emit socket_failure();
    }
    else if (valid_connection)
    {
        refetchValues();
        emit established();
    } else
    {
        emit connecting();
        timer_connect.start();
    }
}

void stellaConnection::reset()
{
    valid_connection = false;
    emit reseted();
    timer_connect.stop();
    timer_send.stop();
}

bool stellaConnection::validSocket()
{
    return valid_socket;
}

bool stellaConnection::validConnection()
{
    return valid_connection;
}

int stellaConnection::getSourcePort()
{
    return source_port;
}

int stellaConnection::getDestPort()
{
    return stella_port;
}

QString stellaConnection::getDestIP()
{
    return stella_host.toString();
}

QString stellaConnection::getConnectionID()
{
    return connID;
}

int stellaConnection::getConnectionAttempts()
{
    return counter_connection;
}

int stellaConnection::getBytesIn()
{
    return counter_in;
}

int stellaConnection::getBytesOut()
{
    return counter_out;
}

int stellaConnection::getProtocolVersion()
{
    return protocol_version;
}

int stellaConnection::getChannelCount()
{
    return channel_count;
}

int stellaConnection::getChannelValue(unsigned char index)
{
    return channels[index];
}

void stellaConnection::setChannelValue(unsigned char index, unsigned char value)
{
    channels[index] = value;
    changes[index] = value;
}

bool stellaConnection::getBroadcastFlag()
{
    return broadcast_flag;
}

void stellaConnection::setBroadcastFlag(bool flag)
{
    broadcast_flag = flag;
    broadcast_overflow = -1;
}

void stellaConnection::resendValues()
{
    for (int i=0;i<channel_count;++i)
        changes[i] = channels[i];
}

void stellaConnection::refetchValues()
{
    QByteArray data;
    data.append(ECMDS_GET_STELLA_COLORS);
    sendBytes(data);
}

int stellaConnection::getCronjobsSize()
{
    return cronjobs.size();
}

stellaConnection::cron_event_t stellaConnection::getCronjob(int i)
{
    return cronjobs.at(i);
}

void stellaConnection::addCronjobs(QList<cron_event_t> jobs)
{
    QByteArray data;
    foreach(cron_event_t job, jobs)
    {
        data.append(ECMDS_SET_CRON_ADD);
        data.append(-1); //position
        data.append((const char*)&job, cron_event_size+sizeof(uint8_t)*job.cmdsize);
    }
    // refetch all cronjobs
    data.append(ECMDS_GET_CRONS);
    sendBytes(data);
}

void stellaConnection::addCronjob(cron_event_t job)
{
    QByteArray data;
    data.append(ECMDS_SET_CRON_ADD);
    data.append(-1); //position
    data.append((const char*)&job, cron_event_size+sizeof(uint8_t)*job.cmdsize);
    // refetch all cronjobs
    data.append(ECMDS_GET_CRONS);
    sendBytes(data);
}

void stellaConnection::removeCronjob(unsigned char jobno)
{
    // remove remotely (there is no confirmation)
    QByteArray data;
    data.append(ECMDS_SET_CRON_REMOVE);
    data.append(jobno);
    // refetch all cronjobs
    data.append(ECMDS_GET_CRONS);
    sendBytes(data);
}

void stellaConnection::refetchCronjobs() {
    QByteArray data;
    data.append(ECMDS_GET_CRONS);
    sendBytes(data);
}

void stellaConnection::countCronjobs() {
    QByteArray data;
    data.append(ECMDS_GET_CRON_COUNT);
    sendBytes(data);
}


void stellaConnection::readPendingDatagrams() {
    QByteArray datagram;
    QHostAddress sender;
    quint16 senderPort;
    while (socket.hasPendingDatagrams()) {
        datagram.resize(socket.pendingDatagramSize());
        socket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        counter_in += datagram.size();

        if (!datagram.size() || datagram[0] != 'S') {
            qWarning() << "Your ethersex firmware does not use the ecmd speed extension. Returned size:" << datagram.size() << "Returned raw data:" << datagram.toHex();
            return;
        }

        if (datagram[1] == (char)ECMDS_GET_PROTOCOL_VERSION) {
            // stop connection timeout timer
            timer_connect.stop();

            // get some values
            protocol_version = datagram[2];
            protocol_version_combat = datagram[3];
            if (!valid_connection)
            {
                // we have established a connection
                valid_connection = true;
                timer_send.start();
                emit established();
            }
        } else
        if (datagram[1] == (char)ECMDS_GET_STELLA_COLORS) {
            channel_count = datagram[2];

            // sync internal channel values with devices values
            int ccount = qMin((int)channel_count, datagram.size()-3);
            for (int i=0;i<ccount;++i)
                channels[i] = datagram[i+3];

            //qDebug() << "STELLA_UNICAST_RESPONSE" << (int)protocol_version << (int)channel_count << channels[0] << channels[1];

            // we don't want stella to broadcast in the near future
            // (we already have up-to-date values)
            broadcast_overflow = -1;


            emit channels_update();
        } else
        if (datagram[1] == (char)ECMDS_GET_CRON_COUNT) {
            int size = (unsigned char)datagram[2]; // reported cronjobs
            emit cronjobs_counted(size);
            //qDebug() << "STELLA_COUNT_CRONJOBS" << size;
        } else
        if (datagram[1] == (char)ECMDS_GET_CRONS) {
            //qDebug() << "STELLA_GET_CRONJOBS" << datagram.toHex();
            cronjobs.clear();
            int count = (unsigned char)datagram[2]; // reported cronjobs
            emit cronjobs_counted(count);
            int pos = 3; // header is 3 bytes long
            for (int i=0;i<count;++i)
            {
                // Emergency plan
                if (pos>=datagram.size()) break;

                cron_event_t *job = (cron_event_t*)(datagram.data()+pos);
                /*
                qDebug() << job->appid << (int)job->minute << ":" << (int)job->hour << (int)job->dayofweek
                        << (int)job->day << "." << (int)job->month
                        << (int)job->extrasize << "Extra:"
                        << (int)job->extradata[0] << (int)job->extradata[1];
                */

                pos += cron_event_size+sizeof(uint8_t)*job->cmdsize;
                cronjobs.append(cron_event_t(*job));
            }
            emit cronjobs_update();
        } else
        {
            qDebug() << "Random data receiving. Size:" << datagram.size() << "Data:" << datagram.toHex();
        }
    }
}


void stellaConnection::timer_connect_timeout() {
    QByteArray data;
    data.append(ECMDS_GET_PROTOCOL_VERSION);
    sendBytes(data);
    ++counter_connection;
}

void stellaConnection::timer_send_timeout() {
    // no changes: just check broadcast
    if (changes.size())
    // sync the devices' values with ours
    {
        // create a bytearray from changes
        QByteArray data;
        QMapIterator<unsigned char, unsigned char> i(changes);
        while (i.hasNext()) {
         i.next();
         data.append(ECMDS_SET_STELLA_INSTANT_COLOR);
         data.append(i.key());
         data.append(i.value());
        }

        sendBytes(data);

        // we do not want the same change set again: clear
        changes.clear();
    }
}

void stellaConnection::sendBytes(QByteArray& data) {
    data.prepend('\n');
    // send bytearray as udp datagramm to stella
    qint64 written = socket.writeDatagram(data, stella_host, stella_port);
    if (written != data.size()) {
        emit write_failure();
    } else {
        counter_out += written;
        //qDebug() << "write:" << counter_out << data.toHex();
    }
}
