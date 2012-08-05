#ifndef CONFIGURATIONWIDGET_H
#define CONFIGURATIONWIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QAbstractSocket>
#include <QTimer>

namespace Ui {
class ConfigurationWidget;
}

class ConfigurationWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ConfigurationWidget(QWidget *parent = 0);
    ~ConfigurationWidget();
    QString networkConfigHost();
    int networkConfigPort();
private:
    Ui::ConfigurationWidget *ui;
    QSystemTrayIcon m_tray;
    QTimer m_reconnectTimer;
private Q_SLOTS:
    void activated(QSystemTrayIcon::ActivationReason r);
    void on_btnExit_clicked(bool);
    void on_btnMonitorOff_clicked(bool);
    void on_btnConnect_clicked(bool);

    void message(const QString& msg);
    void networkStateChanged(QAbstractSocket::SocketState);
    void timeoutReconnect();
};

#endif // CONFIGURATIONWIDGET_H
