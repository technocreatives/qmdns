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

    std::vector<QmDNSBrowser*> list;
    {
        std::lock_guard<std::mutex> lock(browsersMutex);
        Q_UNUSED(lock);

        for (auto& pair : browsers) {
            QmDNSBrowser* b = pair.second;
            b->destroy();
        }

        browsers.clear();
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

        isBrowsingForService = browsers.find(serviceType) != browsers.end();
    }

    if (isBrowsingForService) {
        // Browse already in progress
        return;
    }

    QmDNSBrowser* browser = new QmDNSBrowser(this, serviceType);

    AvahiServiceBrowser* serviceBrowser = avahi_service_browser_new(mDNSClient, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
        serviceType.toStdString().data(), NULL, (AvahiLookupFlags) 0, QmDNSBrowser::browserCallback, browser);

    if (serviceBrowser != nullptr) {
        std::lock_guard<std::mutex> lock(browsersMutex);
        Q_UNUSED(lock);

        browser->init(serviceBrowser);
        browsers.insert(std::make_pair(serviceType, browser));
    } else {
        delete browser;
        throw std::runtime_error(std::string("Failed to create service browser: %s\n", avahi_strerror(avahi_client_errno(mDNSClient))));
    }
}

void QmDNS::stopServiceBrowse(QString serviceType) {
    if (!initialized) {
        throw std::runtime_error(std::string("QmDNS stopServiceDiscovery: not initialized"));
    }

    {
        std::lock_guard<std::mutex> lock(browsersMutex);
        Q_UNUSED(lock);

        auto it = browsers.find(serviceType);
        if (it != browsers.end()) {
            QmDNSBrowser* b = it->second;
            b->destroy();
            b->deleteLater();

            browsers.erase(it);
        }
    }
}

QList<QmDNSService*> QmDNS::getServices(QString serviceType) {
    if (!initialized) {
        throw std::runtime_error(std::string("QmDNS stopServiceDiscovery: not initialized"));
    }

    QmDNSBrowser* browser = nullptr;
    {
        std::lock_guard<std::mutex> lock(browsersMutex);
        Q_UNUSED(lock);

        auto it = browsers.find(serviceType);
        if (it != browsers.end()) {
            browser = it->second;
        }
    }

    if (browser != nullptr) {
        return browser->getServices();
    }

    return QList<QmDNSService*>();
}

void QmDNS::clientCallback(AvahiClient* client, AvahiClientState state, void * userdata) {
    Q_UNUSED(client);
    Q_UNUSED(userdata);

    std::cout << "QmDNS clientCallback: " << avahiClientStateToStdString(state) << "\n";
}

#endif // linux
