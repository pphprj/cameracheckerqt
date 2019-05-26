#include "wsdiscovery.h"

WsDiscovery::WsDiscovery(QObject *parent) : QObject(parent),
    _provider(nullptr),
    _tempNotify(nullptr),
    _sink(nullptr),
    _tag(nullptr),
    _epr(nullptr),
    _matchByRule(nullptr),
    _scopesList(nullptr),
    _isSinkAttached(FALSE)
{
    HRESULT hr = WSDCreateDiscoveryProvider( NULL, &_provider );

    if ( S_OK != hr )
    {
        PrintErrorMessage( L"Failed to create discovery provider", hr );
    }

    if ( S_OK == hr )
    {
        // Create the notification sink for callbacks
        // in the provider
        wprintf( L"Creating the notification sink...\r\n" );
        hr = CreateClientNotificationSink(&_sink);

        if ( S_OK != hr )
        {
            PrintErrorMessage( L"Failed to create notification sink", hr );
        }

        connect(_sink, SIGNAL(deviceFound(QString)), this, SLOT(founded(QString)));
    }

    if ( S_OK == hr )
    {
        // Obtain the discovery provider notify (callback object)
        // needed to be attached to the provider
        wprintf( L"Obtaining the IWSDiscoveryProviderNotify interface " );
        wprintf( L"from the notification sink...\r\n" );

        hr = _sink->QueryInterface(
            __uuidof( IWSDiscoveryProviderNotify ), (void**)&_tempNotify );

        if ( S_OK != hr )
        {
            PrintErrorMessage( L"Failed to query interface for IWSDiscoveryProviderNotify", hr );
        }
    }

    if ( S_OK == hr )
    {
        // Attach the notification sink to the provider to receive
        // callbacks of Hello, ProbeMatches and Bye messages.  Once the
        // sink is attached successfully, the notification sink begins
        // to listen to callbacks.  If you have custom routines in the
        // notification sink that requires resources such as internal
        // variables, you should ensure that those variables are properly
        // initialized before attaching the sink to the provider.
        wprintf( L"Attaching the notification sink...\r\n" );
        hr = _provider->Attach( _tempNotify );

        if ( S_OK != hr )
        {
            PrintErrorMessage(
                L"Failed to attach notification sink to the provider", hr );
        }
    }

    if ( S_OK == hr )
    {
        // Set flag to true so that the sink will be detached on exit.
        _isSinkAttached = TRUE;

        // Generate tag for use with the search query.
        // Note that the tag generation is done for demo purposes only.
        // In reality, any string may be used as a tag.  It may even be
        // a hardcoded string.  The tag is sent as part of the Probe
        // or Resolve message, and will be returned in the ProbeMatches
        // and ResolveMatches messages, making it easier for you to
        // distinguish between what messages are responses to your search
        // requests.  The use of a tag, however, is optional.
        wprintf( L"Generating a tag for use in this client: " );
        hr = GenerateTag( &_tag );

        if ( S_OK != hr )
        {
            PrintErrorMessage( L"Failed to generate tag", hr );
        }
        else
        {
            wprintf( L"%s\r\n", _tag );
        }
    }

}

WsDiscovery::~WsDiscovery()
{
    if ( NULL != _provider && _isSinkAttached )
    {
        // This detaches the notification sink.  We do this regardless whether
        // S_OK == hr or not, and it is important to do this upon exit if a
        // notification sink has previously been attached to the provider.  Not
        // doing so may result in an access violation when the provider is being
        // destroyed.  When this method returns, all callback methods would have
        // been completed, and will cease to listen to any more messages.
        //
        // Use hr2 so that the original value of hr is not lost.
        HRESULT hr2 = _provider->Detach();

        if ( S_OK != hr2 )
        {
            PrintErrorMessage( L"Failed to detach", hr2 );
        }
    }

    // clean up and exit routines below

    if ( NULL != _tag )
    {
        delete[] _tag;
        _tag = NULL;
    }

    if ( NULL != _sink )
    {
        _sink->Release();
        _sink = NULL;
    }

    if ( NULL != _tempNotify )
    {
        _tempNotify->Release();
        _tempNotify = NULL;
    }

    if ( NULL != _provider )
    {
        _provider->Release();
        _provider = NULL;
    }

    if ( NULL != _epr )
    {
        delete[] _epr;
        _epr = NULL;
    }

    if ( NULL != _matchByRule )
    {
        delete[] _matchByRule;
        _matchByRule = NULL;
    }

    if ( NULL != _scopesList )
    {
        WSDFreeLinkedMemory( _scopesList );
        _scopesList = NULL;
    }


    {
        // display usage if it fails for any reason
        wprintf( L"\r\n" );
        DisplayUsages();
        wprintf( L"\r\n" );
    }
}

void WsDiscovery::start()
{
    HRESULT hr = S_OK;

    if ( S_OK == hr )
    {
        wprintf( L"\r\n" );
        wprintf( L"----------------------------------------\r\n" );
        wprintf( L"Search begins - press any key to terminate the client...\r\n" );
        wprintf( L"----------------------------------------\r\n" );

        // A general client message pattern looks like the following.
        //
        // Client sends a Probe message (IWSDiscoveryProvider::SearchByType)
        // to search for the types of target services (or all services) within
        // a given number of scopes (or all scopes available).
        //
        // (IWSDiscoveryProvider::SearchByType searches both type and scope.
        // In this sample, only the scope is used.)
        //
        // Client receives a ProbeMatches message from those target services
        // that matches (IWSDiscoveryProviderNotify::Add).
        //
        // If the received ProbeMatches message does not contain an XAddrs list,
        // then the client, if it wishes, shall send a Resolve message
        // (IWSDiscoveryProvider::SearchById) to request the given target service
        // to provide an XAddrs list of transport addresses.
        //
        // Client receives a ResolveMatches message from the given target service
        // (IWSDiscoveryProviderNotify::Add).
        //
        // Note that this sample does not implement the full message pattern listed above.
        // It only executes either IWSDiscoveryProvider::SearchById or
        // IWSDiscoveryProvider::SearchByType one at a time depending on what
        // the command line arguments are given to this application.

        // epr != NULL - /a - SearchById - Resolve
        if ( NULL != _epr )
        {
            // Calls SearchById to send a Resolve message.  The notification
            // sink will listen to ResolveMatches message, and shall call
            // IWSDiscoveryProviderNotify::Add (implemented by
            // CClientNotificationSink::Add in this case) when a ResolveMatches
            // message arrives.
            //
            // Note that SearchById is an asynchronous call.
            // The return code for this is solely based on whether that
            // async call has been started successfully.  The results of the
            // search are done through callback methods implemented in the
            // notification sink.
            hr = _provider->SearchById( _epr, _tag );

            if ( S_OK != hr )
            {
                PrintErrorMessage( L"SearchById failed...", hr );
            }
        }
        // case 1 - matchByRule != NULL && scopesList != NULL (they both will be non-null)
        // - /sc or /sr - SearchByType - Probe
        // case 2 - they are all NULL
        // - no arguments - SearchByType - Probe
        else
        {
            // Calls SearchByType to send a Probe message.  Types, Scopes and
            // MatchBy rule are included in the Probe message if they are provided.
            // If none of them are provided, the Probe message searches for all
            // WS-Discovery enabled target services.  The notification sink will
            // listen to ProbeMatches messages, and shall call IWSDiscoveryProviderNotify::Add
            // (implemented by CClientNotificationSink::Add in this case) when
            // a ProbeMatches message arrives.
            //
            // Note that Types, Scopes and MatchBy rule are all optional.
            // If scopes are specified but MatchBy rule is not, then RFC2396
            // scope matching rule is used as defined by WS-Discovery specifications.
            //
            // Also note that this sample does not cover the usage of Types.
            // If you need to send a Probe message with Types, you should
            // include the IWSDXMLContext when calling WSDCreateDiscoveryProvider
            // above, and build your own WSD_NAME_LIST structure here.  Building
            // this structure requires you to call IWSDXMLContext::AddNameToNamespace
            // method in order to extract WSDXML_NAME objects from the XML context.
            //
            // Please see MSDN documentation for details.
            //
            // SearchByType is also an async call.  See above for comments.
            hr = _provider->SearchByType( NULL, _scopesList , _matchByRule, _tag );

            if ( S_OK != hr )
            {
                PrintErrorMessage( L"SearchByType", hr );
            }
        }
    }
}

void WsDiscovery::stop()
{
}

void WsDiscovery::founded(QString xaddr)
{
    emit deviceFound(xaddr);
}

void WsDiscovery::sendXaddr()
{
    emit deviceFound("http://192.168.1.46/onvif/device_service");
}
