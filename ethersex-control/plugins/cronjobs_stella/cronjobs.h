#ifndef CRONJOBS_H
#define CRONJOBS_H

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QListWidgetItem>
#include <QCalendarWidget>
#include <QDialog>
#include "plugins_interface.h"

namespace Ui {
    class cronjobs;
}

class QListWidgetItemEx : public QListWidgetItem
{
    public:
    QListWidgetItemEx (int ch, int val, int func, const QString & text, QListWidget * parent = 0);
    int ch;
    int val;
    int func;
};

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
    void btnRemoveAction_clicked();
    void btnAddAction_clicked();
    void btnAddCronjob_clicked();
    void btnNow_clicked();
    void btnCalendar_clicked();
    void stellaActions_clicked(QModelIndex index);
    void cronjobs_counted(int size);
    void calendar_clicked ( const QDate & date );

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
