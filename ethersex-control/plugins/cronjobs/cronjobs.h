#ifndef CRONJOBS_H
#define CRONJOBS_H

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QTableWidgetItem>
#include <QCalendarWidget>
#include <QDialog>
#include "plugins_interface.h"

namespace Ui {
    class cronjobs;
}

class cronjobs : public QObject, StellaControlPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(cronjobs)
    Q_INTERFACES(StellaControlPlugin)

public:
    cronjobs();
    ~cronjobs();
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
    void btnRmSelected_clicked();
    void btnUpdate_clicked();
    void btnLoadFromFile_clicked();
    void btnSaveToFile_clicked();
    void tableWidget_clicked(QModelIndex index);
    void cronjobs_update();
    void cronjobs_counted(int size);

Q_SIGNALS:
    void errorMessage(const QString& msg);

private:
    QWidget* w;
    QDialog* cw;
    stellaConnection* connection;
    Ui::cronjobs *ui;
    QStringList trlist;

};

#endif // CRONJOBS_H
