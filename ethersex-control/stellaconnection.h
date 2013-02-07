#ifndef STELLACONNECTION_H
#define STELLACONNECTION_H

#include <QObject>
#include <QByteArray>
#include <QChar>
#include <QTimer>
#include <QHostAddress>
#include <QUdpSocket>

#define STELLA_MIN 5
#define STELLA_MAX 5

class stellaConnection : public QObject
{
        Q_OBJECT
public:
    struct cron_event_t {
            union{
                    int8_t fields[5];
                    struct {
                            int8_t minute;
                            int8_t hour;
                            int8_t day;
                            int8_t month;
                            int8_t dayofweek;
                    };
            };
            uint8_t repeat;
            uint8_t cmdsize;
            char cmddata[400];
    }; // copy of ethersex/cron/cron.h
    #define cron_event_size (sizeof(stellaConnection::cron_event_t)-sizeof(char[400]))

    stellaConnection(const QHostAddress& dest_ip, int dest_port, int source_port);
    ~stellaConnection();
    /* some functions for initialising this class */
    void init();
    void reset();
    bool validSocket();
    bool validConnection();
    /* reference counter */
    void addRef();
    void removeRef();
    bool isReferenced();
    /* connection details */
    int getSourcePort();
    int getDestPort();
    QString getDestIP();
    QString getConnectionID();
    /* statistics */
    int getConnectionAttempts();
    int getBytesIn();
    int getBytesOut();
    int getProtocolVersion();
    int getChannelCount();
    int getReferences();
    /* channel values */
    int getChannelValue(unsigned char index);
    void setChannelValue(unsigned char index, unsigned char value);
    void resendValues();
    void refetchValues();
    /* send to device */
    void sendBytes(QByteArray& data);
    /* if the broadcast flag is set and broadcast_overflow is zero
       (checked and decremented in timer_send) then make stella
       broadcast all channel values. Of course only available if
       stella is compiled with broadcast support. */
    bool getBroadcastFlag();
    void setBroadcastFlag(bool flag);
    /* cronjobs */
    void addCronjob(cron_event_t job);
    void addCronjobs(QList<cron_event_t> jobs);
    void removeCronjob(unsigned char jobno);
    void refetchCronjobs();
    void countCronjobs();
    /* cronjobs - cached */
    int getCronjobsSize();
    cron_event_t getCronjob(int i);
private:
    int reference_counter;
    bool valid_socket;
    bool valid_connection;
    bool broadcast_flag;
    #define BROADCAST_OVERFLOW_VALUE 10
    char broadcast_overflow;  // init with BROADCAST_OVERFLOW_VALUE,
                              // if zero make stella broadcast all channel values
    int counter_out;          // byte counter, for statistics only
    int counter_in;           // byte counter, for statistics only
    int counter_connection;   // how many times did we try to connect
    QTimer timer_send;        // send datagrams every 20ms (if changes only)
    QTimer timer_connect;     // try to connect to stella
    QUdpSocket socket;        // socket

    QMap<unsigned char, unsigned char> changes; // every channel may save one change for the timer
    QString connID;
    QHostAddress stella_host; // stella ipaddress (mode: channels)
    int          stella_port; // stella udp port   (mode: channels)
    int          source_port; // local port
    char protocol_version;     // response from stella: protocol version
    char protocol_version_combat; // response from stella: minimal compatible version
    char channel_count;        // response from stella: channel count
    unsigned char channels[8];

    QList<cron_event_t> cronjobs;

private Q_SLOTS:
    // receive and send udp datagrams
    void readPendingDatagrams();
    void timer_send_timeout();
    void timer_connect_timeout();
Q_SIGNALS:
    void timer_signal();
    void established();
    void socket_failure();
    void write_failure();
    void connecting();
    void reseted();
    void received_ack();
    void channels_update();
    void cronjobs_update();
    void cronjobs_counted(int size);
};

#endif // STELLACONNECTION_H
