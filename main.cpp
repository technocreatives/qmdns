#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>

#include "qmdns.h"


QmDNS mdns;
QString type("_http._tcp");

void start() {
    qDebug() << "start";

    try {
        mdns.init();

        mdns.startServiceBrowse(type);
    } catch (const std::exception& e) {
        qFatal("Interrupted by exception: %s", e.what());
    }
}

void stop() {
    qDebug() << "stop";

    try {
        mdns.stopServiceBrowse(type);
    } catch (const std::exception& e) {
        qFatal("Interrupted by exception: %s", e.what());
    }

    QCoreApplication::quit();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTimer::singleShot(0, &start);
    QTimer::singleShot(60* 1000, &stop);
    return a.exec();
}
