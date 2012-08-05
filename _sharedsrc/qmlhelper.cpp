#include "qmlhelper.h"

QVariantMap QMLHelper::deepcopy(const QVariantMap &orig)
{
    QVariantMap copy(orig);
    copy.detach();
    return copy;
}

#include "qmlhelper.moc"