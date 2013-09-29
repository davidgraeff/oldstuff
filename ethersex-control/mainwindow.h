#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QTimer>
#include <QByteArray>
#include <QMap>
#include <QUdpSocket>
#include <QHostAddress>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QLabel>
#include <QPair>
#include <QList>

namespace Ui
{
    class MainWindowClass;
}
class stellaConnection;
class StellaControlPlugin;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindowClass *ui;
    QLabel* permMesg;         // status bar permanent messages
    QLabel* statInMesg;
    QLabel* statOutMesg;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QMap<QString, stellaConnection*> devices;
    QList<StellaControlPlugin*> tabs;
    StellaControlPlugin* currentTab();
    void closeTab(int index);

private Q_SLOTS:
    // tray icon
    void on_tabWidget_tabCloseRequested(int index);
    void on_actionCloseTab_triggered();
    void on_actionAboutConnection_triggered();
    void on_actionBroadcastChanges_toggled(bool );
    void on_btnConnect_clicked();
    void on_listPlugins_currentIndexChanged(int index);
    void on_tabWidget_currentChanged(int );
    void on_actionShow_tray_icon_toggled(bool );
    void on_actionMinimize_to_tray_triggered();
    void on_actionRestore_triggered();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    // "program" menu
    void on_actionAbout_triggered();
    void on_actionClose_triggered();

    // gui logic: choose
    void on_lineIP_editTextChanged(const QString& );

    void load_connected(StellaControlPlugin* plugin, int position);
    void load_aborted(StellaControlPlugin* plugin, int position);
    void load_statechanged(StellaControlPlugin* plugin, int position);

    void errorMessage(const QString& msg);

};

#endif // MAINWINDOW_H
