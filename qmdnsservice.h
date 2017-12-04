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

    Q_PROPERTY(bool ipv4Supported READ isIPv4Supported)
    Q_PROPERTY(bool ipv6Supported READ isIPv6Supported)

    Q_PROPERTY(QString ipv4Address READ getIPv4Address)
    Q_PROPERTY(QString ipv6Address READ getIPv6Address)
    Q_PROPERTY(QStringList txt READ getTxt)

    Q_PROPERTY(QString error READ getErrorMessage)
    Q_PROPERTY(ResolutionStatus resolutionStatus READ getResolutionStatus)

public:
    explicit QmDNSService(QObject *parent = nullptr, int ifIndex = -1, QString name = "", QString type = "", QString domain = "");
    virtual ~QmDNSService();

    enum ResolutionStatus {
        RESOLUTION_PENDING, RESOLUTION_SUCCESS, RESOLUTION_FAILURE
    };
    Q_ENUM(ResolutionStatus);

    int getInterface() const;
    QString getName() const;
    QString getType() const;
    QString getDomain() const;

    bool isIPv4Supported() const;
    bool isIPv6Supported() const;

    QString getIPv4Address() const;
    QString getIPv6Address() const;
    QString getHostname() const;
    quint16 getPort() const;

    QStringList getTxt() const;

    void setIPv4Supported(bool supported);
    void setIPv6Supported(bool supported);

    void setIPv4Address(QString address);
    void setIPv6Address(QString address);
    void setHostname(QString hostname);
    void setPort(quint16 port);
    void setTxt(QStringList txt);

    ResolutionStatus getResolutionStatus() const;
    void setResolutionStatus(ResolutionStatus status);

    QString getErrorMessage();
    void setErrorMessage(QString message);

private:
    ResolutionStatus status;
    QString errorMessage;

    const int ifIndex = -1;
    const QString name;
    const QString type;
    const QString domain;

    bool ipv4Support;
    bool ipv6Support;

    QString ipv6Address;
    QString ipv4Address;
    QString hostname;
    quint16 port = 0;

    QStringList txt;

signals:

public slots:
};

#endif // QMDNSSERVICE_H
