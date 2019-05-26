#include "mediamanagerworker.h"


MediaManagerWorker::MediaManagerWorker(const QString& xaddr, const QString& login, const QString& password, QObject *parent) :
    DeviceDescriptionWorker(xaddr, login, password, parent)
{

}

MediaManagerWorker::~MediaManagerWorker()
{

}

void MediaManagerWorker::process()
{
    OnvifClientDevice onvifDevice(_xAddress.toStdString(), _login.toStdString(), _password.toStdString());
    onvifDevice.GetCapabilities();

    OnvifClientMedia media(onvifDevice);
    _trt__GetProfilesResponse profiles;
    media.GetProfiles(profiles);

    if (profiles.Profiles.size() > 0)
    {
        std::string token = profiles.Profiles[0]->token;

        _trt__GetStreamUriResponse response;
        tt__StreamSetup setup;
        setup.Stream = tt__StreamType__RTP_Unicast;
        setup.Transport = new tt__Transport();
        setup.Transport->Protocol = tt__TransportProtocol__RTSP;
        media.GetStreamUri(response, setup, token);

        std::string uri = response.MediaUri->Uri;

        emit resultReady(QString::fromStdString(uri));

        delete setup.Transport;
    }

    emit finished();
}
