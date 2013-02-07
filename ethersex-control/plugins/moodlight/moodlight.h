#ifndef BPMRANDOM_H
#define BPMRANDOM_H

#include <QtGui/QWidget>
#include <QTimer>
#include <QListWidgetItem>
#include "plugins_interface.h"

namespace Ui {
    class Moodlight;
}
class modelUsedChannels;

class Moodlight : public QObject, StellaControlPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(Moodlight)
    Q_INTERFACES(StellaControlPlugin)

public:
    Moodlight();
    ~Moodlight();
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
    void fadestep_valueChanged(int );
    void threshold_valueChanged(int );
	 void data_changed();

Q_SIGNALS:
    void errorMessage(const QString& msg);

private:
    QWidget* w;
    stellaConnection* connection;
    Ui::Moodlight *ui;
    modelUsedChannels* mChannels;
};

#endif // TESTCASES_H
