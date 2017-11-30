#include "qmdns.h"

#include <stdexcept>
#include <iostream>
#include <thread>

QmDNS::QmDNS(QObject *parent) : QObject(parent) {

}

#ifdef linux
QmDNS::~QmDNS() {
    initialized = false;

    /* Cleanup things */

    if (mDNSBrowser) {
        avahi_service_browser_free(mDNSBrowser);
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

void QmDNS::startServiceDiscovery(QString serviceType) {
    if (!initialized) {
        throw std::runtime_error(std::string("QmDNS startDiscovery: not initialized"));
    }

    stopServiceDiscovery();

    mDNSBrowser = avahi_service_browser_new(mDNSClient, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, serviceType.toStdString().data(), NULL, (AvahiLookupFlags) 0, QmDNS::browserCallback, this);
    if (mDNSBrowser == nullptr) {
        throw std::runtime_error(std::string("Failed to create service browser: %s\n", avahi_strerror(avahi_client_errno(mDNSClient))));
    }
}

void QmDNS::stopServiceDiscovery() {
    if (!initialized) {
        throw std::runtime_error(std::string("QmDNS stopServiceDiscovery: not initialized"));
    }

    if (mDNSBrowser) {
        avahi_service_browser_free(mDNSBrowser);
        mDNSBrowser = nullptr;
    }
}

void QmDNS::clientCallback(AvahiClient* client, AvahiClientState state, void * userdata) {
    std::cout << "QmDNS clientCallback: " << state << "\n";
}

void QmDNS::browserCallback(AvahiServiceBrowser* serviceBrowser, AvahiIfIndex interface, AvahiProtocol protocol,
    AvahiBrowserEvent event, const char *name, const char *type, const char *domain, AvahiLookupResultFlags flags, void* userdata) {

    std::cout << "QmDNS browserCallback: " << event << " " << interface << " " << protocol <<  " " << name << " " << type << " " << domain << " " << flags << "\n";

}

#endif // linux
