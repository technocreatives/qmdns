#include "qmdns.h"

#include <stdexcept>
#include <thread>
#include <utility>

#include <QDebug>
#include <QThread>

#include "utils.h"

QmDNS::QmDNS(QObject *parent) : QObject(parent) {

}

void QmDNS::browserServiceFound(QmDNSService* service) {
    emit serviceFound(service);
}

void QmDNS::browserServiceLost(QmDNSService* service) {
    emit serviceLost(service);
}

void QmDNS::browserServiceResolved(QmDNSService* service) {
    emit serviceResolved(service);
}

void QmDNS::browserServiceNotResolved(QmDNSService* service) {
    emit serviceNotResolved(service);
}

#ifdef linux

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
        throw std::runtime_error("QmDNS init: already initialized");
    }

    try {
        mDNSPoll = avahi_simple_poll_new();
        if (mDNSPoll == nullptr) {
            throw std::runtime_error("QmDNS init: fail to create poll");
        }

        int clientError = 0;
        mDNSClient = avahi_client_new(avahi_simple_poll_get(mDNSPoll), (AvahiClientFlags) 0, QmDNS::clientCallback, this, &clientError);
        if (mDNSClient == nullptr) {
            throw std::runtime_error(std::string("QmDNS init: fail to create client ") + avahi_strerror(clientError));
        }

        const char* version = avahi_client_get_version_string(mDNSClient);
        qDebug() << "Init mDNSClient using " << version;

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
    connect(browser, &QmDNSBrowser::serviceLost, this, &QmDNS::browserServiceLost);
    connect(browser, &QmDNSBrowser::serviceFound, this, &QmDNS::browserServiceFound);
    connect(browser, &QmDNSBrowser::serviceResolved, this, &QmDNS::browserServiceResolved);
    connect(browser, &QmDNSBrowser::serviceNotResolved, this, &QmDNS::browserServiceNotResolved);

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
        throw std::runtime_error("QmDNS stopServiceDiscovery: not initialized");
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
        throw std::runtime_error("QmDNS stopServiceDiscovery: not initialized");
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

    qDebug("QmDNS clientCallback: %s", qUtf8Printable(avahiClientStateToQString(state)));
}

#else

QmDNS::~QmDNS() {
    // Stub
}

void QmDNS::init() {
    // Stub
}

void QmDNS::startServiceBrowse(QString serviceType) {
    Q_UNUSED(serviceType);
    // Stub
}

void QmDNS::stopServiceBrowse(QString serviceType) {
    Q_UNUSED(serviceType);
    // Stub
}

QList<QmDNSService*> QmDNS::getServices(QString serviceType) {
    Q_UNUSED(serviceType);
    // Stub
    return QList<QmDNSService*>();
}

#endif // linux
