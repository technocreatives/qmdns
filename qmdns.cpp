#include "qmdns.h"

#include <stdexcept>
#include <iostream>
#include <thread>
#include <utility>

QmDNS::QmDNS(QObject *parent) : QObject(parent) {

}

#ifdef linux
std::string avahiBrowserEventToStdString(AvahiBrowserEvent event);
std::string avahiClientStateToStdString(AvahiClientState state);
std::string avahiLookupResultFlagsToStdString(AvahiLookupResultFlags flags);
std::string avahiProtocolToStdString(AvahiProtocol protocol);
std::string avahiResolveEventToStdString(AvahiResolverEvent event);

QmDNS::~QmDNS() {
    initialized = false;

    /* Cleanup things */

    std::vector<AvahiServiceBrowser*> list;
    {
        std::lock_guard<std::mutex> lock(browsersMutex);
        Q_UNUSED(browsersMutex);

        for (auto p : browsers) {
            list.push_back(p.second);
        }
        browsers.clear();
    }

    for (AvahiServiceBrowser* b : list) {
        avahi_service_browser_free(b);
    }

    if (mDNSClient) {
        avahi_client_free(mDNSClient);
    }

    if (mDNSPoll) {
        avahi_simple_poll_quit(mDNSPoll);

        if (worker.joinable()) {
            worker.join();
        }

        avahi_simple_poll_free(mDNSPoll);
    }
}

void QmDNS::init() {
    if (initialized) {
        throw std::runtime_error(std::string("QmDNS init: already initialized"));
    }

    try {
        mDNSPoll = avahi_simple_poll_new();
        if (mDNSPoll == nullptr) {
            throw std::runtime_error(std::string("QmDNS init: fail to create poll"));
        }

        int clientError = 0;
        mDNSClient = avahi_client_new(avahi_simple_poll_get(mDNSPoll), (AvahiClientFlags) 0, QmDNS::clientCallback, this, &clientError);
        if (mDNSClient == nullptr) {
            throw std::runtime_error(std::string("QmDNS init: fail to create client ") + avahi_strerror(clientError));
        }

        const char* version = avahi_client_get_version_string(mDNSClient);
        std::cout << "Init mDNSClient using " << version << "\n";

    } catch (const std::exception& e) {
        if (mDNSClient != nullptr) {
            avahi_client_free(mDNSClient);
            mDNSClient = nullptr;
        }

        // Free allocated resources
        if (mDNSPoll != nullptr) {
            avahi_simple_poll_free(mDNSPoll);
            mDNSPoll = nullptr;
        }

        throw e;
    }

    initialized = true;

    worker = std::thread([&](){ avahi_simple_poll_loop(mDNSPoll); });
}

void QmDNS::startServiceBrowse(QString serviceType) {
    bool isBrowsingForService = false;
    {
        std::lock_guard<std::mutex> lock(browsersMutex);
        Q_UNUSED(lock);

        isBrowsingForService = browsers.find(serviceType.toStdString()) != browsers.end();
    }

    if (isBrowsingForService) {
        // Browse already in progress
        return;
    }

    AvahiServiceBrowser* browser = avahi_service_browser_new(mDNSClient, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
        serviceType.toStdString().data(), NULL, (AvahiLookupFlags) 0, QmDNS::browserCallback, this);

    if (browser == nullptr) {
        throw std::runtime_error(std::string("Failed to create service browser: %s\n", avahi_strerror(avahi_client_errno(mDNSClient))));
    }
}

void QmDNS::stopServiceBrowse(QString type) {
    if (!initialized) {
        throw std::runtime_error(std::string("QmDNS stopServiceDiscovery: not initialized"));
    }

    AvahiServiceBrowser* browser = nullptr;
    {
        std::lock_guard<std::mutex> lock(browsersMutex);
        Q_UNUSED(lock);

        auto it = browsers.find(type.toStdString());
        if (it != browsers.end()) {
            browser = it->second;
            browsers.erase(it);
        }
    }

    if (browser) {
        avahi_service_browser_free(browser);
    }
}

void QmDNS::clientCallback(AvahiClient* client, AvahiClientState state, void * userdata) {
    std::cout << "QmDNS clientCallback: " << avahiClientStateToStdString(state) << "\n";
}

void QmDNS::browserCallback(AvahiServiceBrowser* serviceBrowser, AvahiIfIndex interface, AvahiProtocol protocol,
    AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags flags, void* userdata) {

    QmDNS* self = reinterpret_cast<QmDNS*>(userdata);

    std::cout << "QmDNS browserCallback: " << avahiBrowserEventToStdString(event)
              << " " << interface
              << " " << avahiProtocolToStdString(protocol)
              << " " << type
              << " " << domain
              << " " << avahiLookupResultFlagsToStdString(flags)
              <<  " " << name
              << "\n";

    if (event == AVAHI_BROWSER_NEW) {
        AvahiClient* client = avahi_service_browser_get_client(serviceBrowser);

        AvahiServiceResolver* resolver = avahi_service_resolver_new(
            client, interface, protocol, name, type, domain,
            static_cast<AvahiProtocol>(AVAHI_PROTO_UNSPEC), static_cast<AvahiLookupFlags>(0),
            QmDNS::resolveCallback, userdata);

        if (resolver != nullptr) {
            std::lock_guard<std::mutex> lock(self->resolversMutex);
            Q_UNUSED(lock);

            auto it = self->resolvers.find(serviceBrowser);
            if (it != self->resolvers.end()) {
                // resolve list exist
                it->second.push_back(resolver);
            } else {
                // resolve list don't exist
                std::vector<AvahiServiceResolver*> list;
                list.push_back(resolver);
                self->resolvers.insert(std::make_pair(serviceBrowser, list));
            }

        } else {
            std::cerr << "Failed to create service resolver for "
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

void QmDNS::resolveCallback(AvahiServiceResolver *resolver, AvahiIfIndex interface, AvahiProtocol protocol, AvahiResolverEvent event,
        const char *name, const char *type, const char *domain, const char *host_name, const AvahiAddress *a, uint16_t port, AvahiStringList *txt,
        AvahiLookupResultFlags flags, void *userdata) {

    if (event == AVAHI_RESOLVER_FOUND) {
        char address[AVAHI_ADDRESS_STR_MAX];
        avahi_address_snprint(address, sizeof(address), a);

        char* txtList = avahi_string_list_to_string(txt);

        std::cout << "QmDNS resolveCallback success: " << avahiResolveEventToStdString(event)
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
         std::cerr << "QmDNS resolveCallback failed:" << avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(resolver))) << "\n";
    }
}

std::string avahiBrowserEventToStdString(AvahiBrowserEvent event) {
    if (event == AVAHI_BROWSER_NEW) {
        return std::string("New");
    } else if (event == AVAHI_BROWSER_REMOVE) {
        return std::string("Remove");
    } else if (event == AVAHI_BROWSER_FAILURE) {
        return std::string("Failure");
    } else if (event == AVAHI_BROWSER_ALL_FOR_NOW) {
        return std::string("All for now");
    } else if (event == AVAHI_BROWSER_CACHE_EXHAUSTED) {
        return std::string("Cache Exhausted");
    } else {
        return std::string("Unknown") + " (" + std::to_string(static_cast<int>(event)) + ")";
    }
}

std::string avahiClientStateToStdString(AvahiClientState state) {
    if (state == AVAHI_CLIENT_S_RUNNING) {
        return std::string("Running");
    } else if (state == AVAHI_CLIENT_S_REGISTERING) {
        return std::string("Registering");
    } else if (state == AVAHI_CLIENT_S_COLLISION) {
        return std::string("Collision");
    } else if (state == AVAHI_CLIENT_FAILURE) {
        return std::string("Failure");
    } else if (state == AVAHI_CLIENT_CONNECTING) {
        return std::string("Connecting");
    } else {
        return std::string("Unknown") + " (" + std::to_string(static_cast<int>(state)) + ")";
    }
}

std::string avahiLookupResultFlagsToStdString(AvahiLookupResultFlags flags) {
    std::string s;

    if (flags & AVAHI_LOOKUP_RESULT_CACHED) {
        s += "cached,";
    }

    if (flags & AVAHI_LOOKUP_RESULT_WIDE_AREA) {
        s += "wide,";
    }

    if (flags & AVAHI_LOOKUP_RESULT_MULTICAST) {
        s += "multi";
    }

    if (flags & AVAHI_LOOKUP_RESULT_LOCAL) {
        s += "local";
    }

    if (flags & AVAHI_LOOKUP_RESULT_OUR_OWN) {
        s += "self";
    }

    if (flags & AVAHI_LOOKUP_RESULT_STATIC) {
        s += "static,";
    }

    if (s.empty()) {
        s += "none";
    }

    return s;
}

std::string avahiResolveEventToStdString(AvahiResolverEvent event) {
    if (event == AVAHI_RESOLVER_FOUND) {
        return "Found";
    } else if (event == AVAHI_RESOLVER_FAILURE) {
        return "Failure";
    } else {
        return std::string("Unknown") + " (" + std::to_string(static_cast<int>(event)) + ")";
    }
}

std::string avahiProtocolToStdString(AvahiProtocol protocol) {
    if (protocol == AVAHI_PROTO_INET) {
        return std::string("IPv4");
    } else if (protocol == AVAHI_PROTO_INET6) {
        return std::string("IPv6");
    } else if (protocol == AVAHI_PROTO_UNSPEC) {
        return std::string("Unspecified");
    } else {
        return std::string("Unknown") + " (" + std::to_string(static_cast<int>(protocol)) + ")";
    }
}

#endif // linux
