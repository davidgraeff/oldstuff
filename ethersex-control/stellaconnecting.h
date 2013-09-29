#ifndef CONNECTING_H
#define CONNECTING_H

#include <QtGui/QWidget>

namespace Ui {
    class StellaConnectingUI;
}
class stellaConnection;
class StellaControlPlugin;

enum state_enum
{
    StateConnecting,
    StateWriteFailure,
    StateSocketFailure
};

class stellaConnecting : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(stellaConnecting)
public:
    explicit stellaConnecting(stellaConnection* connection, StellaControlPlugin* plugin, int position, QWidget *parent = 0);
    virtual ~stellaConnecting();
    QString getName();

Q_SIGNALS:
    void aborted(StellaControlPlugin* plugin, int position);
    void connected(StellaControlPlugin* plugin, int position);
    void statechanged(StellaControlPlugin* plugin, int position);

private Q_SLOTS:
    void on_btnAbortConnection_clicked();
    void established();
    void write_failure();
    void socket_failure();
    void connecting();
    void init();

protected:
    virtual void changeEvent(QEvent *e);

private:
    StellaControlPlugin* plugin;
    int position;
    state_enum state;
    Ui::StellaConnectingUI *m_ui;
};

#endif // CONNECTING_H
