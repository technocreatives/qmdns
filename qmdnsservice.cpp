#include "qmdnsservice.h"

QmDNSService::QmDNSService(QObject *parent, int ifIndex, QString name, QString type, QString domain)
    : QObject(parent), ifIndex(ifIndex), name(name), type(type), domain(domain) {

}

QmDNSService::~QmDNSService() {

}

int QmDNSService::getInterface() const {
    return ifIndex;
}

QString QmDNSService::getName() const {
    return name;
}

QString QmDNSService::getType() const {
    return type;
}

QString QmDNSService::getDomain() const {
    return domain;
}

QString QmDNSService::getHostname() const {
    return hostname;
}

QString QmDNSService::getIPv4Address() const {
    return ipv4Address;
}

QString QmDNSService::getIPv6Address() const {
    return ipv6Address;
}

quint16 QmDNSService::getPort() const {
    return port;
}

QStringList QmDNSService::getTxt() const {
    return txt;
}

void QmDNSService::setIPv4Address(QString address) {
    this->ipv4Address = address;
}

void QmDNSService::setIPv6Address(QString address) {
    this->ipv6Address = address;
}

void QmDNSService::setHostname(QString hostname) {
    this->hostname = hostname;
}

void QmDNSService::setPort(quint16 port) {
    this->port = port;
}

void QmDNSService::setTxt(QStringList txt) {
    this->txt = txt;
}


bool QmDNSService::isResolved() const {
    return resolved;
}

void QmDNSService::setResolved() {
    this->resolved = true;

    emit serviceResolved();
}
