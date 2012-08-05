// Qt GUI
#include <QtGui/QApplication>
#include <QDeclarativeView>
#include <QDeclarativeComponent>
#include <QDeclarativeProperty>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QStyle>
#include <QDesktopWidget>
#include "_sharedsrc/qmlhelper.h"
// Qt
#include <QDebug>
// Models
#include "models/couchdatamodel.h"
#include "models/collectiontagsmodel.h"
#include "models/servicedatamodel.h"
#include "models/collectionsmodel.h"
#include "models/collectiontagfilterproxy.h"
// Controller
#include "actioncontroller.h"
#include "_sharedsrc/networkcontroller.h"
#include "libdatabase/database.h"

int main(int argc, char *argv[])
{
    int r = 1;
    QApplication app(argc, argv);
    app.setApplicationName(QLatin1String("roomcontrol.windows.client"));
    app.setApplicationVersion(QLatin1String("1.0"));
	
    ActionController* ac = new ActionController();
    NetworkController *nc = NetworkController::instance();
    Database* database = Database::instance();
    nc->connect(nc, SIGNAL(serverJSON(QVariantMap)), ac, SLOT(serverJSON(QVariantMap)));
    // Models
    CollectionsModel* collectionModel = new CollectionsModel();
    database->connect(database, SIGNAL(doc_changed(QString,QVariantMap)),
                      collectionModel, SLOT(change(QString,QVariantMap)));
    database->connect(database, SIGNAL(doc_removed(QString)),
                      collectionModel, SLOT(remove(QString)));
    CollectiontagsModel* collectiontagModel = new CollectiontagsModel();
    database->connect(database, SIGNAL(doc_changed(QString,QVariantMap)),
                      collectiontagModel, SLOT(change(QString,QVariantMap)));
    database->connect(database, SIGNAL(doc_removed(QString)),
                      collectiontagModel, SLOT(remove(QString)));
    collectionTagFilterProxy* collectionModelProxy = new collectionTagFilterProxy(collectionModel);
	collectiontagModel->connect(collectiontagModel, SIGNAL(tagChecked(QString)),
								collectionModelProxy, SLOT(addTag(QString)));
	collectiontagModel->connect(collectiontagModel, SIGNAL(tagUnchecked(QString)),
								collectionModelProxy, SLOT(removeTag(QString)));

    DataModel* schemaModel = new DataModel(QLatin1String("schema"), QLatin1String("type_"), QLatin1String("name"));
    database->connect(database, SIGNAL(doc_changed(QString,QVariantMap)),
                      schemaModel, SLOT(change(QString,QVariantMap)));
    database->connect(database, SIGNAL(doc_removed(QString)),
                      schemaModel, SLOT(remove(QString)));
    ServiceDataModel* servicemodel = new ServiceDataModel();
    database->connect(database, SIGNAL(doc_changed(QString,QVariantMap)),
                      servicemodel, SLOT(change(QString,QVariantMap)));
    database->connect(database, SIGNAL(doc_removed(QString)),
                      servicemodel, SLOT(remove(QString)));
    database->connect(database, SIGNAL(dataOfCollection(QString,QList<QVariantMap>)),
                      servicemodel, SLOT(dataOfCollection(QString,QList<QVariantMap>)));

	QMLHelper h;
	
    QDeclarativeView *view = new QDeclarativeView;
    view->setMinimumSize(550,400);
    view->setWindowFlags(Qt::WindowSystemMenuHint|Qt::Window);
    view->setWindowTitle(qApp->applicationName());
    view->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, view->size(), qApp->desktop()->availableGeometry()));
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view->rootContext()->setContextProperty("networkController", nc);
    view->rootContext()->setContextProperty("database", database);
    view->rootContext()->setContextProperty("actioncontroller", ac);
    view->rootContext()->setContextProperty("collectionsModel", collectionModel);
	view->rootContext()->setContextProperty("collectionModelProxy", collectionModelProxy);
    view->rootContext()->setContextProperty("collectiontagModel", collectiontagModel);
    view->rootContext()->setContextProperty("schemaModel", schemaModel);
    view->rootContext()->setContextProperty("servicesModel", servicemodel);
	view->rootContext()->setContextProperty("qmlhelper", &h);
    view->rootContext()->setBaseUrl(QUrl(QLatin1String("qrc:/qml/")));
    view->setSource(QUrl(QLatin1String("qrc:/qml/core/main.qml")));
    if (view->status() == QDeclarativeView::Ready) {
        view->show();
        r = app.exec();
    } else
        qWarning() << view->errors();
    delete view;
    delete nc;
    delete ac;
    delete collectionModel;
    delete collectiontagModel;
    delete servicemodel;
    //delete ce;
    return r;
}
