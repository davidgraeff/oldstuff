#ifndef TESTCASES_H
#define TESTCASES_H

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QSignalMapper>
#include <QCommandLinkButton>
#include "plugins_interface.h"

namespace Ui {
    class testcases;
}

class testcases : public QObject, StellaControlPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(testcases)
    Q_INTERFACES(StellaControlPlugin)

public:
    testcases();
    ~testcases();
    void initInstance(stellaConnection* connection);
    void connectionEstablished();
    stellaConnection* getConnection();
    QString about();
    QString plugname();
    QString authors();
    QWidget* widget();
    QList<QAction*> menuActions();
    QObject* createInstance();
    QLabel* countedCrobjobs;

protected:
    virtual void changeEvent(QEvent *e);

private Q_SLOTS:
    void clicked(int i);
    void cronjobs_update();
    void cronjobs_counted(int size);

Q_SIGNALS:
    void errorMessage(const QString& msg);

private:
    QWidget* w;
    QSignalMapper signalMapper;
    stellaConnection* connection;
    Ui::testcases *ui;
};

#endif // TESTCASES_H
