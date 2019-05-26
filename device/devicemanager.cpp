#include "devicemanager.h"
#include "devicedescriptionworker.h"

#include <QThread>

DeviceManager::DeviceManager(const QString& login, const QString& password, QObject *parent) : QObject(parent),
    _login(login),
    _password(password)
{

}

DeviceManager::~DeviceManager()
{

}

void DeviceManager::setLoginPasword(const QString &login, const QString &password)
{
    _login = login;
    _password = password;
}

void DeviceManager::deviceFounded(QString xAddr)
{
    QThread* thread = new QThread();
    DeviceDescriptionWorker* worker = new DeviceDescriptionWorker(xAddr, _login, _password);

    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(resultsReady(DeviceDescription)), this, SLOT(deviceDescripted(DeviceDescription)));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void DeviceManager::deviceDescripted(DeviceDescription description)
{
    emit deviceDescript(description);
}
