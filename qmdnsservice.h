#ifndef QMDNSSERVICE_H
#define QMDNSSERVICE_H

#include <QObject>

class QmDNSService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int interfaceIndex READ getInterface)

    Q_PROPERTY(QString name READ getName)
    Q_PROPERTY(QString type READ getType)
    Q_PROPERTY(QString domain READ getDomain)

    Q_PROPERTY(quint16 port READ getPort)
    Q_PROPERTY(QString hostname READ getHostname)
    Q_PROPERTY(QString ipv4Address READ getIPv4Address)
    Q_PROPERTY(QString ipv6Address READ getIPv6Address)
    Q_PROPERTY(QStringList txt READ getTxt)

    Q_PROPERTY(bool resolved READ isResolved NOTIFY resolvedChanged)

public:
    explicit QmDNSService(QObject *parent = nullptr, int ifIndex = -1, QString name = "", QString type = "", QString domain = "");
    virtual ~QmDNSService();

    int getInterface() const;
    QString getName() const;
    QString getType() const;
    QString getDomain() const;

    QString getIPv4Address() const;
    QString getIPv6Address() const;
    QString getHostname() const;
    quint16 getPort() const;

    QStringList getTxt() const;

    void setIPv4Address(QString address);
    void setIPv6Address(QString address);
    void setHostname(QString hostname);
    void setPort(quint16 port);
    void setTxt(QStringList txt);

    bool isResolved() const;
    void setResolved();

private:
    bool resolved;

    const int ifIndex;
    const QString name;
    const QString type;
    const QString domain;

    QString ipv6Address;
    QString ipv4Address;
    QString hostname;
    quint16 port;

    QStringList txt;

signals:
    void resolvedChanged(bool);

public slots:
};

#endif // QMDNSSERVICE_H
