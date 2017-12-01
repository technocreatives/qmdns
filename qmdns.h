#ifndef QMDNS_H
#define QMDNS_H

#include <QObject>
#include <QMap>

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

    void init(); // May thrown an exception

    void startServiceBrowse(QString serviceType);

    void stopServiceBrowse(QString serviceType);

private:
    bool initialized = false;

    #ifdef linux
    AvahiSimplePoll*     mDNSPoll = nullptr;
    AvahiClient*         mDNSClient = nullptr;

    std::thread worker;

    std::mutex resolversMutex;
    std::map<AvahiServiceBrowser*, std::vector<AvahiServiceResolver*>> resolvers;

    std::mutex browsersMutex;
    std::map<std::string, AvahiServiceBrowser*> browsers;

    static void clientCallback(
        AvahiClient* client,
        AvahiClientState state,
        void * userdata);

    static void browserCallback(
        AvahiServiceBrowser* serviceBrowser,
        AvahiIfIndex interface,
        AvahiProtocol protocol,
        AvahiBrowserEvent event,
        const char *name,
        const char *type,
        const char *domain,
        AvahiLookupResultFlags flags,
        void* userdata);

    static void resolveCallback(
        AvahiServiceResolver *resolver,
        AvahiIfIndex interface,
        AvahiProtocol protocol,
        AvahiResolverEvent event,
        const char *name,
        const char *type,
        const char *domain,
        const char *host_name,
        const AvahiAddress *a,
        uint16_t port,
        AvahiStringList *txt,
        AvahiLookupResultFlags flags,
        void *userdata);

    #endif

signals:

public slots:
};

#endif // QMDNS_H
