#ifndef DEVICEDESCRIPTIONWORKER_H
#define DEVICEDESCRIPTIONWORKER_H

#include <QObject>

#include <devicedescription.h>
#include "onvifclient.hpp"

class DeviceDescriptionWorker : public QObject
{
    Q_OBJECT

public:
    explicit DeviceDescriptionWorker(const QString& xAddr, const QString& login, const QString& password, QObject *parent = 0);
    ~DeviceDescriptionWorker();

signals:
    void finished();
    void resultsReady(DeviceDescription description);

public slots:
    void process();

protected:
    QString _login;
    QString _password;
    QString _xAddress;
};

#endif // DEVICEDESCRIPTIONWORKER_H
