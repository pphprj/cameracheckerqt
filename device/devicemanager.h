#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QVector>

#include "devicedescription.h"


class DeviceManager : public QObject
{
    Q_OBJECT
public:
    explicit DeviceManager(const QString& login, const QString& password, QObject *parent = 0);
    ~DeviceManager();

    void setLoginPasword(const QString& login, const QString& password);

signals:
    void deviceDescript(DeviceDescription description);

public slots:
    void deviceFounded(QString xAddr);
    void deviceDescripted(DeviceDescription description);

private:
    QVector<DeviceDescription> _devices;
    QString _login;
    QString _password;
};


#endif // DEVICEMANAGER_H
