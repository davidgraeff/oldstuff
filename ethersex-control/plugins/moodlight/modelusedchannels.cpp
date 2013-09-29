#include "modelusedchannels.h"
#include <QDebug>
#include <QSettings>
#include "stellaconnection.h"

modelUsedChannels::modelUsedChannels(stellaConnection* connection)
{
    QSettings settings;
    QString settings_path = QLatin1String("hosts/") + connection->getDestIP() +
                    QLatin1String("/channel");
    QString name;
    QString path;
    for (int i=0;i<8;++i)
    {
        path = settings_path+QString::number(i)+QLatin1String("/name");
        name = settings.value(path,tr("Channel %1").arg(QString::number(i))).toString();
        channels.append(QPair<QString,int>(name, Qt::Unchecked));
    }
}

QVariant modelUsedChannels::data ( const QModelIndex & index, int role ) const
{
    if (!index.isValid()) return QVariant();

    if (role == Qt::DisplayRole)
        return channels[index.row()].first;
    else if (role == Qt::CheckStateRole)
        return channels[index.row()].second;
    else
        return QVariant();
}

Qt::ItemFlags modelUsedChannels::flags ( const QModelIndex & index ) const
{
    if (index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    else
        return Qt::NoItemFlags;
}

int modelUsedChannels::rowCount ( const QModelIndex & ) const
{
    return this->channels.size();
}

bool modelUsedChannels::setData ( const QModelIndex & index, const QVariant & value, int role)
{
    if (!index.isValid()) return false;

    if (role == Qt::CheckStateRole)
    {
        bool checked = (value.toInt() == Qt::Checked);
        channels[index.row()].second = value.toInt();
        if (checked)
        { // true
            if (!usedchannels.contains(index.row()))
                usedchannels.append(index.row());
        } else
        { // false
            int i = usedchannels.indexOf(index.row());
            if (i != -1)
                usedchannels.remove(i);
        }
		  emit dataChanged(index, index);
        return true;
    }
    else
        return false;
}
