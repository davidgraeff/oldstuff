#ifndef BPMRANDOM_H
#define BPMRANDOM_H

#include <QtGui/QWidget>
#include <QTimer>
#include <QListWidgetItem>
#include "plugins_interface.h"

namespace Ui {
    class BPMRandom;
}
class modelUsedChannels;

class BPMRandom : public QObject, StellaControlPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(BPMRandom)
    Q_INTERFACES(StellaControlPlugin)

public:
    BPMRandom();
    ~BPMRandom();
    void initInstance(stellaConnection* connection);
    void connectionEstablished();
    stellaConnection* getConnection();
    QString about();
    QString plugname();
    QString authors();
    QWidget* widget();
    QList<QAction*> menuActions();
    QObject* createInstance();

protected:
    virtual void changeEvent(QEvent *e);

private Q_SLOTS:
    void on_useFade_clicked(bool checked);
    void on_maxColor_valueChanged(int );
    void on_minColor_valueChanged(int );
    void on_fadestep_valueChanged(int );
    void on_bpm_valueChanged(int );
    void timer_beatcontrol_timeout();

Q_SIGNALS:
    void errorMessage(const QString& msg);

private:
    QWidget* w;
    stellaConnection* connection;
    Ui::BPMRandom *ui;
    QTimer timer_beatcontrol;
    modelUsedChannels* mChannels;
};

#endif // TESTCASES_H
