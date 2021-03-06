#ifndef QMDNSBROWSER_H
#define QMDNSBROWSER_H

#include <QObject>
#include <QList>

#include <mutex>

#include "qmdnsservice.h"

#ifdef linux
#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#endif

class QmDNSBrowser : public QObject
{
    Q_OBJECT
public:
    explicit QmDNSBrowser(QObject *parent, QString serviceType);
    virtual ~QmDNSBrowser();

    void destroy();
    QList<QmDNSService*> getServices();

#ifdef linux
    void init(AvahiServiceBrowser* browser);

    static void browserCallback(
        AvahiServiceBrowser* serviceBrowser, AvahiIfIndex interface, AvahiProtocol protocol, AvahiBrowserEvent event,
        const char *name, const char *type, const char *domain,
        AvahiLookupResultFlags flags, void* userdata);

    static void resolveCallback(
        AvahiServiceResolver *resolver, AvahiIfIndex interface, AvahiProtocol protocol, AvahiResolverEvent event,
        const char *name, const char *type, const char *domain, const char *host_name,
        const AvahiAddress *a, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags flags, void *userdata);
#endif

private:
    QString serviceType;

#ifdef linux
    AvahiServiceBrowser* browser = nullptr;

    std::mutex resolversMutex;
    std::vector<AvahiServiceResolver*> resolvers;

    std::mutex servicesMutex;
    std::vector<QmDNSService*> services;
#endif

signals:
    void serviceFound(QmDNSService* service);
    void serviceLost(QmDNSService* service);
    void serviceResolved(QmDNSService* service);
    void serviceNotResolved(QmDNSService* service);

public slots:


};

#endif // QMDNSBROWSER_H
