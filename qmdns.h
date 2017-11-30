#ifndef QMDNS_H
#define QMDNS_H

#include <QObject>

#include <thread>

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

    void init(); // May thrown an exception

    void startServiceDiscovery(QString serviceType);

    void stopServiceDiscovery();

private:
    bool initialized = false;

    #ifdef linux
    AvahiSimplePoll* mDNSPoll = nullptr;
    AvahiClient* mDNSClient = nullptr;
    AvahiServiceBrowser* mDNSBrowser = nullptr;

    std::thread worker;

    static void clientCallback(
        AvahiClient* client,
        AvahiClientState state,
        void * userdata);

    static void browserCallback(AvahiServiceBrowser* serviceBrowser,
        AvahiIfIndex interface,
        AvahiProtocol protocol,
        AvahiBrowserEvent event,
        const char *name,
        const char *type,
        const char *domain,
        AvahiLookupResultFlags flags,
        void* userdata);

    #endif

signals:

public slots:
};

#endif // QMDNS_H
