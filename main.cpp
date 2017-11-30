#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include <iostream>

#include "qmdns.h"
#include "main.h"

int main(int argc, char *argv[])
{
    QmDNS mdns;

    try {
        mdns.init();
        mdns.startServiceDiscovery("_mqtt-n2k._tcp");

        QThread::sleep(60);

        mdns.stopServiceDiscovery();

    } catch (const std::exception& e) {
        std::cout << "Interrupted by exception: " << e.what() << "\n";

        return 1;
    }

    return 0;
}
