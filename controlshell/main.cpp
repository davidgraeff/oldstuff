#include <QCoreApplication>
#include "propertymonitor.h"


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    propertymonitor foo;
    return app.exec();
}
