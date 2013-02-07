#ifndef CHANNELS_H
#define CHANNELS_H

#include <QObject>
#include "plugins_interface.h"
#include "pwmchannel.h"

namespace Ui {
    class ChannelsUI;
}
class stellaConnection;

class Channels : public QObject, StellaControlPlugin
{
    Q_OBJECT
    Q_INTERFACES(StellaControlPlugin)

public:
    Channels();
    ~Channels();
    void initInstance(stellaConnection* connection);
    void connectionEstablished();
    stellaConnection* getConnection();
    QString about();
    QString plugname();
    QString authors();
    QWidget* widget();
    QList<QAction*> menuActions();
    QObject* createInstance();

private:
    bool enternameText;
    QWidget* w;
    stellaConnection* connection;
    Ui::ChannelsUI *ui;
    void makeChannelsAndLayout(int = -1);
    QVector<pwmchannel*> channels;
    QString settings_path;    // settings path (depending on stella_host)

Q_SIGNALS:
    void errorMessage(const QString& msg);

private slots:
    void on_actionRefetch_triggered();
    void on_actionResend_triggered();
    void actionLoad_from_EEProm_triggered();
    void actionSaveToEEProm_triggered();
    void channels_update();
    void timer_signal();
    void value_changed(unsigned char value, unsigned char channel);


    void actionChannelCount_triggered();
    void btnRemovePredefined_clicked();
    void btnSavePredefined_clicked();
    void predefinedlist_activated(const QString& set_name);
    void predefinedlist_editTextChanged(const QString&);
};

#endif // CHANNELS_H
