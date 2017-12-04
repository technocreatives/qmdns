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

bool QmDNSService::isIPv4Supported() const {
    return ipv4Support;
}

bool QmDNSService::isIPv6Supported() const {
    return ipv6Support;
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

QString QmDNSService::getErrorMessage() {
    return errorMessage;
}

QmDNSService::ResolutionStatus QmDNSService::getResolutionStatus() const {
    return status;
}

void QmDNSService::setIPv4Supported(bool supported) {
    this->ipv4Support = supported;
}

void QmDNSService::setIPv6Supported(bool supported) {
    this->ipv6Support = supported;
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

void QmDNSService::setErrorMessage(QString message) {
    this->errorMessage = message;
}

void QmDNSService::setResolutionStatus(ResolutionStatus status) {
    this->status = status;
}
