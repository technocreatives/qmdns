#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include <iostream>

#include "qmdns.h"

int main(int argc, char *argv[])
{
    QString type("_mqtt-n2k._tcp");

    QmDNS mdns;

    try {
        mdns.init();
        mdns.startServiceBrowse(type);

        QThread::sleep(60);

        mdns.stopServiceBrowse(type);

    } catch (const std::exception& e) {
        std::cout << "Interrupted by exception: " << e.what() << "\n";

        return 1;
    }

    return 0;
}
