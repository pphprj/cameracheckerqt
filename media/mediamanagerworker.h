#ifndef MEDIAMANAGERWORKER_H
#define MEDIAMANAGERWORKER_H

#include "device/devicedescriptionworker.h"

class MediaManagerWorker : public DeviceDescriptionWorker
{
    Q_OBJECT
public:
    explicit MediaManagerWorker(const QString& xaddr, const QString& login, const QString& password, QObject *parent = 0);
    ~MediaManagerWorker();

signals:
    void resultReady(QString uri);

public slots:
    void process();
};

#endif // MEDIAMANAGERWORKER_H
