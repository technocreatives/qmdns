#include "qmdnsbrowser.h"

#include "utils.h"

#include <algorithm>
#include <iostream>
#include <QDebug>

QmDNSBrowser::QmDNSBrowser(QObject *parent, QString serviceType) : QObject(parent), serviceType(serviceType) {

}

QmDNSBrowser::~QmDNSBrowser() {
    destroy();
}

void QmDNSBrowser::init(AvahiServiceBrowser *browser) {
    this->browser = browser;
}

void QmDNSBrowser::destroy() {
    if (browser) {
        avahi_service_browser_free(browser);
    }
    browser = nullptr;
}

void QmDNSBrowser::browserCallback(AvahiServiceBrowser* serviceBrowser, AvahiIfIndex interface, AvahiProtocol protocol,
    AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags flags, void* userdata) {

    QmDNSBrowser* self = reinterpret_cast<QmDNSBrowser*>(userdata);

    std::cout << "QmDNS browserCallback: " << avahiBrowserEventToStdString(event) << "\n";

    if (event == AVAHI_BROWSER_NEW) {
        std::cout << "QmDNS browserCallback: "
            << " " << interface
            << " " << avahiProtocolToStdString(protocol)
            << " " << type
            << " " << domain
            << " " << avahiLookupResultFlagsToStdString(flags)
            <<  " " << name
            << "\n";

        // Add service entry
        {
            std::lock_guard<std::mutex> lock(self->servicesMutex);
            Q_UNUSED(lock);

            int i = static_cast<int>(interface);
            QString n(name);
            QString d(domain);
            QString t(type);

            QmDNSService* entry = nullptr;
            for (auto p : self->services) {
                if (i == p->getInterface() && n == p->getName()) {
                    entry = p;
                    break;
                }
            }

            if (entry == nullptr) {
                entry = new QmDNSService(nullptr, i, n, t, d);
                connect(entry, &QmDNSService::serviceResolved, self, &QmDNSBrowser::serviceAdded);
                self->services.push_back(entry);
            }
        }

        AvahiClient* client = avahi_service_browser_get_client(serviceBrowser);

        AvahiServiceResolver* resolver = avahi_service_resolver_new(client, interface, protocol, name, type, domain,
            protocol, static_cast<AvahiLookupFlags>(0), QmDNSBrowser::resolveCallback, userdata);

        if (resolver != nullptr) {
            std::lock_guard<std::mutex> lock(self->resolversMutex);
            Q_UNUSED(lock);

            self->resolvers.push_back(resolver);

        } else {
            std::cerr << "QmDNS browserCallback: Failed to create service resolver for"
                      << " " << interface
                      << " " << avahiProtocolToStdString(protocol)
                      << " " << type
                      << " " << domain
                      << " " << avahiLookupResultFlagsToStdString(flags)
                      <<  " " << name
                      << "\n";
        }
    }
}

void QmDNSBrowser::resolveCallback(AvahiServiceResolver *resolver, AvahiIfIndex interface, AvahiProtocol protocol, AvahiResolverEvent event,
        const char *name, const char *type, const char *domain, const char *host_name, const AvahiAddress *a, uint16_t port, AvahiStringList *txt,
        AvahiLookupResultFlags flags, void *userdata) {

    QmDNSBrowser* self = reinterpret_cast<QmDNSBrowser*>(userdata);

    std::cout << "QmDNSBrowser resolveCallback success: " << avahiResolveEventToStdString(event) << "\n";

    if (event == AVAHI_RESOLVER_FOUND) {
        char address[AVAHI_ADDRESS_STR_MAX];
        avahi_address_snprint(address, sizeof(address), a);

        // Add service entry
        {
            std::lock_guard<std::mutex> lock(self->servicesMutex);
            Q_UNUSED(lock);

            int i = static_cast<int>(interface);
            QString n(name);

            QmDNSService* entry = nullptr;
            for (auto p : self->services) {
                if (i == p->getInterface() && n == p->getName()) {
                    entry = p;
                    break;
                }
            }

            if (entry != nullptr) {
                if (protocol == AVAHI_PROTO_INET) {
                    entry->setIPv4Address(address);
                } else if (protocol == AVAHI_PROTO_INET6) {
                    entry->setIPv6Address(address);
                }

                if (entry->getHostname().size() == 0) {
                    entry->setHostname(host_name);
                }

                if (entry->getTxt().size() == 0) {
                    QStringList txtList;

                    AvahiStringList* p = txt;
                    while (p != nullptr) {
                        const char* text = reinterpret_cast<const char*>(avahi_string_list_get_text(p));
                        if (text != nullptr) {
                            txtList.append(QString(text));
                        }
                        p = avahi_string_list_get_next(p);
                    }
                }

                if (entry->getPort() == 0) {
                    entry->setPort(port);
                }

                if (entry->getIPv4Address().size() > 0 && entry->getIPv6Address() >0) {
                    entry->setResolved();
                }
            }
        }

        char* txtList = avahi_string_list_to_string(txt);

        std::cout << "QmDNSBrowser resolveCallback: "
                  << " " << interface
                  << " " << avahiProtocolToStdString(protocol)
                  << " " << type
                  << " " << domain
                  << " " << address
                  << " " << host_name
                  << ":" << port
                  << " " << avahiLookupResultFlagsToStdString(flags)
                  << " " << name
                  << " " << txtList
                  << "\n";

        avahi_free(txtList);
    } else if (event == AVAHI_RESOLVER_FAILURE) {
         std::cerr << "QmDNSBrowser resolveCallback: " << avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(resolver))) << "\n";
    }

    /*
    {
        std::lock_guard<std::mutex> lock(self->resolversMutex);
        Q_UNUSED(lock);

        auto it = std::find(self->resolvers.begin(), self->resolvers.end(), resolver);
        if (it != self->resolvers.end()) {
            self->resolvers.erase(it);
        }
    }
    avahi_service_resolver_free(resolver);
    */
}

void QmDNSBrowser::serviceAdded() {
    qDebug() << Q_FUNC_INFO;

    QmDNSService* s = reinterpret_cast<QmDNSService*>(sender());
    std::cout << "QmDNSBrowser serviceAdded: " << s->getName().toStdString()
              << " " << s->getHostname().toStdString()
              << "/" << s->getIPv4Address().toStdString()
              << ":" << s->getPort() << "\n";
}
