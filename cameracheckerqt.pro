#-------------------------------------------------
#
# Project created by QtCreator 2019-05-12T17:11:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cameracheckerqt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += wsdiscovery \
               device \
               onvif/include \
               onvif/onvifgen \
               onvif/gsoap/gsoap/runtime \
               onvif/3rdparty/openssl-1.0.1g/include


DEFINES += WITH_OPENSSL
DEFINES += WITH_DOM
DEFINES += WITH_PURE_VIRTUAL

SOURCES += main.cpp\
        mainwindow.cpp \
    wsdiscovery/Client.cpp \
    wsdiscovery/ClientNotificationSink.cpp \
    wsdiscovery/Common.cpp \
    wsdiscovery/wsdiscovery.cpp \
    device/devicedescription.cpp \
    device/devicemanager.cpp \
    device/devicedescriptionworker.cpp \
    media/mediamanager.cpp \
    media/mediamanagerworker.cpp

HEADERS  += mainwindow.h \
    wsdiscovery/Client.h \
    wsdiscovery/ClientNotificationSink.h \
    wsdiscovery/Common.h \
    wsdiscovery/wsdiscovery.h \
    device/devicedescription.h \
    device/devicemanager.h \
    device/devicedescriptionworker.h \
    media/mediamanager.h \
    media/mediamanagerworker.h \
    stdafx.h

FORMS    += mainwindow.ui

LIB_PATH += onvif/3rdparty/openssl-1.0.1g/lib
LIB_ONVIF = onvif/lib/debug/

LIBS     += -lWS2_32 \
            -lwsdapi \
            -lrpcrt4 \
            -lstrsafe \
            c:/Work\shumeyko/cameracheckerqt/onvif/lib/debug/onvifcpplib.lib \
            c:/Work/shumeyko/cameracheckerqt/onvif/3rdparty/openssl-1.0.1g/lib/libeay32MTd.lib \
            c:/Work/shumeyko/cameracheckerqt/onvif/3rdparty/openssl-1.0.1g/lib/ssleay32MTd.lib
