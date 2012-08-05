#include <QtGui/QApplication>
#include "configurationwidget.h"
#include "networkcontroller.h"
#include "actioncontroller.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QLatin1String("roomcontrol.windows.client"));
    app.setApplicationVersion(QLatin1String("1.0"));
    app.setQuitOnLastWindowClosed(false);
    ConfigurationWidget* w = new ConfigurationWidget();
    NetworkController *nc = NetworkController::instance();
    ActionController* ac = new ActionController();

    QObject::connect(nc, SIGNAL(serverJSON(QVariantMap)), ac, SLOT(serverJSON(QVariantMap)));

    nc->connectToServer(w->networkConfigHost(), w->networkConfigPort());
    nc->registerAsRemotesystemClient();
    int r = app.exec();
    delete nc;
    delete w;
    delete ac;
    return r;
}
