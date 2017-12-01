#ifndef QMDNS_H
#define QMDNS_H

#include <QObject>
#include <QMap>

#include "qmdnsbrowser.h"
#include "qmdnsservice.h"

#include <map>
#include <mutex>
#include <thread>
#include <vector>

#ifdef linux
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#endif

class QmDNS : public QObject
{
    Q_OBJECT

public:
    explicit QmDNS(QObject *parent = nullptr);
    virtual ~QmDNS();

    Q_INVOKABLE void init();
    Q_INVOKABLE void startServiceBrowse(QString serviceType);
    Q_INVOKABLE void stopServiceBrowse(QString serviceType);
    Q_INVOKABLE QList<QmDNSService*> getServices(QString serviceType);

private:
    bool initialized = false;

    #ifdef linux
    AvahiSimplePoll*     mDNSPoll = nullptr;
    AvahiClient*         mDNSClient = nullptr;

    std::thread worker;

    std::mutex browsersMutex;
    std::map<QString, QmDNSBrowser*> browsers;

    static void clientCallback(
        AvahiClient* client,
        AvahiClientState state,
        void * userdata);

    #endif

signals:

public slots:
};

#endif // QMDNS_H
