#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include <iostream>

#include "qmdns.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    QString type("_http._tcp");

    QmDNS mdns;

    try {
        mdns.init();
        mdns.startServiceBrowse(type);

        QThread::sleep(20);

        mdns.stopServiceBrowse(type);

    } catch (const std::exception& e) {
        std::cout << "Interrupted by exception: " << e.what() << "\n";

        return 1;
    }

    return 0;
}
