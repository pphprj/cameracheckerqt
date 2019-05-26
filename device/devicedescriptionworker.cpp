#include "devicedescriptionworker.h"

#include <QException>

DeviceDescriptionWorker::DeviceDescriptionWorker(const QString& xAddr, const QString& login, const QString& password, QObject *parent) :
    QObject(parent),
    _xAddress(xAddr),
    _login(login),
    _password(password)
{

}

DeviceDescriptionWorker::~DeviceDescriptionWorker()
{

}

void DeviceDescriptionWorker::process()
{
    try
    {
        OnvifClientDevice onvifDevice(_xAddress.toStdString(), _login.toStdString(), _password.toStdString());
        _tds__GetDeviceInformationResponse deviceInfo;
        onvifDevice.GetDeviceInformation(deviceInfo);

        DeviceDescription device(QString::fromStdString(deviceInfo.Manufacturer),
                                 QString::fromStdString(deviceInfo.Model),
                                 QString::fromStdString(deviceInfo.FirmwareVersion),
                                 QString::fromStdString(deviceInfo.SerialNumber),
                                 _xAddress);

        emit resultsReady(device);
    }
    catch (QException& exp)
    {
        QString error = exp.what();

    }
    emit finished();
}
