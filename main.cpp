#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include <iostream>

#include "qmdns.h"

QmDNS mdns;
QString type("_http._tcp");

void start() {

    try {
        mdns.init();
        mdns.startServiceBrowse(type);
    } catch (const std::exception& e) {
        std::cout << "Interrupted by exception: " << e.what() << "\n";
    }
}

void stop() {
    try {
        mdns.stopServiceBrowse(type);
    } catch (const std::exception& e) {
        std::cout << "Interrupted by exception: " << e.what() << "\n";
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTimer::singleShot(0, &start);
    QTimer::singleShot(60* 1000, &stop);
    return a.exec();
}
