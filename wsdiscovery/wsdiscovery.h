#ifndef WSDISCOVERY_H
#define WSDISCOVERY_H

#include <QObject>

#include <windows.h>
#include <stdlib.h>
#include <strsafe.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <wsdapi.h>

#include "Common.h"
#include "ClientNotificationSink.h"
#include "Client.h"

class WsDiscovery : public QObject
{
    Q_OBJECT

public:
    WsDiscovery(QObject* parent = nullptr);
    ~WsDiscovery();

    void start();
    void stop();

    void sendXaddr();

signals:
    void deviceFound(QString name);

private slots:
    void founded(QString xaddr);

private:
    IWSDiscoveryProvider* _provider;
    IWSDiscoveryProviderNotify* _tempNotify;
    CClientNotificationSink* _sink;
    LPCWSTR _tag;
    LPCWSTR _epr;
    LPCWSTR _matchByRule;
    WSD_URI_LIST* _scopesList;
    BOOL _isSinkAttached;
};

#endif // WSDISCOVERY_H
