#ifndef propertymonitor_H
#define propertymonitor_H

#include <QtCore/QObject>
#include <QSslSocket>

class propertymonitor : public QObject
{
Q_OBJECT
public:
    propertymonitor();
    virtual ~propertymonitor();
private:
  QSslSocket m_socket;
private slots:
    void readyRead();
    void serverconnected();
    void serverdisconnected();
    void sslErrors(const QList<QSslError>&);
};

#endif // propertymonitor_H
