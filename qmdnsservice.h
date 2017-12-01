#ifndef QMDNSSERVICE_H
#define QMDNSSERVICE_H

#include <QObject>

class QmDNSService : public QObject
{
    Q_OBJECT
public:
    explicit QmDNSService(QObject *parent = nullptr);
    virtual ~QmDNSService();

private:
    bool resolved;

    QString name;
    QString type;
    QString domain;

    QString hostname;
    QString port;
    QStringList txt;

signals:


public slots:
};

#endif // QMDNSSERVICE_H
