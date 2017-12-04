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

void svc_found(QmDNSService* svc) {
    qDebug() << "ServiceFound: " << qUtf8Printable(svc->getName());
}

void svc_lost(QmDNSService* svc) {
    qDebug() << "ServiceLost: " << qUtf8Printable(svc->getName());
}

void svc_resolved(QmDNSService* svc) {
    qDebug() << "ServiceResolved: " << qUtf8Printable(svc->getName()) << qUtf8Printable(svc->getHostname()) << qUtf8Printable(svc->getIPv4Address()) << svc->getPort();
}

void svc_not_resolved(QmDNSService* svc) {
    qDebug() << "ServiceNotResolved: " << qUtf8Printable(svc->getName());
}

int main(int argc, char *argv[])
{
    QObject::connect(&mdns, &QmDNS::serviceFound, svc_found);
    QObject::connect(&mdns, &QmDNS::serviceLost, svc_lost);
    QObject::connect(&mdns, &QmDNS::serviceResolved, svc_resolved);
    QObject::connect(&mdns, &QmDNS::serviceNotResolved, svc_not_resolved);

    QCoreApplication a(argc, argv);
    QTimer::singleShot(0, &start);
    QTimer::singleShot(60* 1000, &stop);
    return a.exec();
}
