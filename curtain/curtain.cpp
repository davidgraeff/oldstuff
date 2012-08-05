#include "curtain.h"

#include <QDebug>
#include <QCoreApplication>
#include <QSocketNotifier>
#include <iostream>
#include "_sharedsrc/networkcontroller.h"

curtain::curtain()
{
    QSocketNotifier* input = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
    connect(input, SIGNAL(activated(int)), this, SLOT(onInputData()));
    input->setEnabled(true);
}

curtain::~curtain()
{}

void curtain::onInputData()
{
    QSocketNotifier* input = (QSocketNotifier*)sender();
    input->setEnabled(false);
    QTextStream stream(stdin, QIODevice::ReadOnly);
	NetworkController* nc = NetworkController::instance();
    QString cmd;
    stream >> cmd;
    if (cmd == "help") {
	qDebug() << "open, close, set, stop, debug, dirnormal, dirinverted, dirok";
    } else if (cmd == "set" && !stream.atEnd()) {
      int v; stream >> v;
		QVariantMap data;
		data.insert("type_", "execute");
		data.insert("plugin_", "curtain_udp");
		data.insert("instanceid_", "0");
		data.insert("member_", "setValue");
		data.insert("value", v);
        nc->write(data);
        qDebug() << cmd;
    } else if (cmd == "open") {
		QVariantMap data;
		data.insert("type_", "execute");
		data.insert("plugin_", "curtain_udp");
		data.insert("instanceid_", "0");
		data.insert("member_", "setValue");
		data.insert("value", 0);
        nc->write(data);
        qDebug() << cmd;
    } else if (cmd == "close") {
		QVariantMap data;
		data.insert("type_", "execute");
		data.insert("plugin_", "curtain_udp");
		data.insert("instanceid_", "0");
		data.insert("member_", "setValue");
		data.insert("value", 100);
        nc->write(data);
        qDebug() << cmd;
    } else if (cmd == "stop") {
		QVariantMap data;
		data.insert("type_", "execute");
		data.insert("plugin_", "curtain_udp");
		data.insert("instanceid_", "0");
		data.insert("member_", "stop");
        nc->write(data);
        qDebug() << cmd;
    } else if (cmd == "debug") {
		QVariantMap data;
		data.insert("type_", "execute");
		data.insert("plugin_", "curtain_udp");
		data.insert("instanceid_", "0");
		data.insert("member_", "request_debugdata");
        nc->write(data);
        qDebug() << cmd;
    } else if (cmd == "dirnormal") {
		QVariantMap data;
		data.insert("type_", "execute");
		data.insert("plugin_", "curtain_udp");
		data.insert("instanceid_", "0");
		data.insert("member_", "start_direction_calibration");
        nc->write(data);
        qDebug() << cmd;
    } else if (cmd == "dirinverted") {
		QVariantMap data;
		data.insert("type_", "execute");
		data.insert("plugin_", "curtain_udp");
		data.insert("instanceid_", "0");
		data.insert("member_", "start_direction_calibration_inverted");
        nc->write(data);
        qDebug() << cmd;
    } else if (cmd == "dirok") {
		QVariantMap data;
		data.insert("type_", "execute");
		data.insert("plugin_", "curtain_udp");
		data.insert("instanceid_", "0");
		data.insert("member_", "direction_ok");
        nc->write(data);
        qDebug() << cmd;
    } else {
        qDebug() << "Unknown";
    }
    input->setEnabled(true);
    std::cout << "Command: ";
	std::flush(std::cout);
}

void curtain::message(const QString& msg)
{
	qWarning() << msg;
}

void curtain::serverJSON(const QVariantMap& data)
{
	qDebug() << data;
}

void curtain::stateChanged()
{
	qDebug() << "NetworkController State:" << NetworkController::instance()->state();
}

#include "curtain.moc"
