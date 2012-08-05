#include <QCoreApplication>
#include "curtain.h"
#include "_sharedsrc/networkcontroller.h"

int main(int argc, char** argv)
{
	if (argc<2) {
		qWarning()<<"Aufruf" << argv[0] << "server [port]";
		return 0;
	}

	QCoreApplication app(argc, argv);
    app.setApplicationName(QLatin1String("roomcontrol.curtain.client"));
    app.setApplicationVersion(QLatin1String("1.0"));
	
    curtain curtainController;
    NetworkController *nc = NetworkController::instance();
    QObject::connect(nc, SIGNAL(serverJSON(QVariantMap)),
				&curtainController, SLOT(serverJSON(QVariantMap)));
    QObject::connect(nc, SIGNAL(message(QString)),
				&curtainController, SLOT(message(QString)));
    QObject::connect(nc, SIGNAL(stateChanged()),
				&curtainController, SLOT(stateChanged()));
	
	QByteArray server(argv[1]);
	int port = argc>=3 ? QByteArray(argv[2]).toInt() : 3101;
	nc->connectToServer(server,port);
    int r = app.exec();
    delete nc;
    return r;
}