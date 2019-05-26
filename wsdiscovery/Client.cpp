//////////////////////////////////////////////////////////////////////////////
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//////////////////////////////////////////////////////////////////////////////

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

//Usages: WSDiscoveryClient.exe [/a EndpointReference] | [/sr | /sc] scope [...]
//You can specify exactly one of /a, /sr or /sc - not a combination of them.
//
//An EndpointReference is the ID of the target service, which must be a valid
//URI as per Section 2.6 of the WS-Discovery Specifications.  Specifically, it must
//begin with one of http://, https://, uri:, urn:uuid: or uuid:.
//
//Examples:
//
//To find all WS-Discovery enabled target services: (Probe)
//WSDiscoveryClient.exe
//
//To find a device with a given EndpointReference: (Resolve)
//WSDiscoveryClient.exe /a urn:uuid:91022ed0-7d3d-11de-8a39-0800200c9a66
//
//To find a device with two scopes using the RFC2396 scope matching rule: (Probe)
//WSDiscoveryClient.exe /sr http://www.contoso.com/wsd/scope1 http://www.contoso.com/wsd/scope2
//
//To find a device with a given scope using the custom scope matching rule: (Probe)
//WSDiscoveryClient.exe /sc http://www.contoso.com/wsd/special/123

void DisplayUsages()
{
    wprintf( L"Usages: WSDiscoveryClient.exe [/a EndpointReference] | [/sr | /sc] scope [...]\r\n" );
    wprintf( L"You can specify exactly one of /a, /sr or /sc - not a combination of them.\r\n" );
    wprintf( L"\r\n" );
    wprintf( L"An EndpointReference is the ID of the target service, which must be a valid\r\n" );
    wprintf( L"URI as per Section 2.6 of the WS-Discovery Specifications.  Specifically, it must\r\n" );
    wprintf( L"begin with one of http://, https://, uri:, urn:uuid: or uuid:.\r\n" );
    wprintf( L"\r\n" );
    wprintf( L"Examples:\r\n" );
    wprintf( L"\r\n" );
    wprintf( L"To find all WS-Discovery enabled target services: (Probe)\r\n" );
    wprintf( L"WSDiscoveryClient.exe\r\n" );
    wprintf( L"\r\n" );
    wprintf( L"To find a device with a given EndpointReference: (Resolve)\r\n" );
    wprintf( L"WSDiscoveryClient.exe /a urn:uuid:91022ed0-7d3d-11de-8a39-0800200c9a66\r\n" );
    wprintf( L"\r\n" );
    wprintf( L"To find a device with two scopes using the RFC2396 scope matching rule: (Probe)\r\n" );
    wprintf( L"WSDiscoveryClient.exe /sr http://www.contoso.com/wsd/scope1 " );
    wprintf( L"http://www.contoso.com/wsd/scope2 \r\n" );
    wprintf( L"\r\n" );
    wprintf( L"To find a device with a given scope using the custom scope matching rule: (Probe)\r\n" );
    wprintf( L"WSDiscoveryClient.exe /sc http://www.contoso.com/wsd/special/123 \r\n" );
}

// Generates a tag name of the following format:
// Tag0000
// where 0000 is a random number between 0000 to 9999.
// The caller should call delete[] on the string when
// it is no longer needed.
_Success_( return == S_OK )
HRESULT GenerateTag
(   _Outptr_ LPCWSTR *generatedTag
)
{
    HRESULT hr = S_OK;
    LPWSTR tempTag = NULL;
    int randomNum = 0;

    if ( NULL == generatedTag )
    {
        hr = E_POINTER;
    }
    else
    {
        *generatedTag = NULL;
    }

    if ( S_OK == hr )
    {
        // 12345678
        // Tag0000
        tempTag = new WCHAR[8];
        
        if ( NULL == tempTag )
        {
            hr = E_OUTOFMEMORY;
        }
    }

    if ( S_OK == hr )
    {
        // generate a random number between 0000 to 9999
        srand( (unsigned int)time( NULL ) );
        randomNum = rand() % 10000;

        // create the tag string
        // (StringCchPrintfW always NULL terminates the string)
        hr = StringCchPrintfW( tempTag, 8, L"Tag%04d", randomNum );
    }

    if ( S_OK == hr )
    {
        // outside pointer now owns temp string
        *generatedTag = tempTag;
        tempTag = NULL;
    }

    if ( NULL != tempTag )
    {
        delete[] tempTag;
        tempTag = NULL;
    }

    return hr;
}

// Parses the command line arguments according to rules
// specified in the DisplayUsages() function.
// The caller should pass in argc and argv as they were
// being passed into the wmain() function.
// The caller should call WSDFreeLinkedMemory on the
// returned scopesList, and delete[] on epr and
// matchByRule when they are no longer needed.
_Success_( return == S_OK )
HRESULT ParseArguments
(   _In_ int argc
,   _In_reads_( argc ) LPWSTR* argv
,   _Outptr_result_maybenull_ LPCWSTR *epr
,   _Outptr_result_maybenull_ WSD_URI_LIST **scopesList
,   _Outptr_result_maybenull_ LPCWSTR *matchByRule
)
{
    HRESULT hr = S_OK;
    LPWSTR tempEpr = NULL;
    LPWSTR tempMatchByRule = NULL;
    WSD_URI_LIST *tempScopesList = NULL;

    // should be at least 1 - 1st arg = exe name
    if ( 1 > argc || NULL == argv || NULL == *argv )
    {
        hr = E_INVALIDARG;
    }
    else if ( NULL == epr ||
              NULL == scopesList || 
              NULL == matchByRule )
    {
        hr = E_POINTER;
    }
    else
    {
        *epr = NULL;
        *scopesList = NULL;
        *matchByRule = NULL;
    }

    // expecting arguments to be in this format:
    // argv[0] = executable name - ignore
    // argv[1] = if present, must be one of /a, /sr or /sc
    //           if not present, then argc = 1

    // if argv[1] = /a,
    // then argv[2] = EndpointReference
    // argc must be = 3

    // if argv[1] = /sr or /sc,
    // then argv[2] and above will be the scopes
    //              to search for
    // argc must be >= 3

    if ( 1 >= argc )
    {
        // do nothing - no parsing required
        // will discover all WS-Discovery enabled services
        // Probe
    }
    else if ( 0 == wcscmp( L"/a", argv[1] ) )
    {
        // search using endpoint reference address
        // Resolve
        
        // argc must be = 3
        if ( 3 != argc )
        {
            hr = E_INVALIDARG; // fail to parse
        }

        if ( S_OK == hr )
        {
            hr = DeepCopyString(argv[2], &tempEpr); // deep copy epr
        }

        if ( S_OK == hr )
        {
            // outside pointer now owns temp epr
            *epr = tempEpr;
            tempEpr = NULL;
        }
    }
    else if ( 0 == wcscmp( L"/sr", argv[1] ) ||
              0 == wcscmp( L"/sc", argv[1] ) )
    {
        // search using scopes
        // Probe

        // argc must be >= 3
        if ( 3 > argc )
        {
            hr = E_INVALIDARG; // fail to parse
        }

        if ( S_OK == hr )
        {
            // /sr - use RFC2396
            // /sc - use Customs Rule

            if ( L'r' == argv[1][2] ) // /sr
            {
                hr = DeepCopyString( MATCHBY_RFC2396, &tempMatchByRule );
            }
            else // /sc
            {
                hr = DeepCopyString( MATCHBY_CUSTOM, &tempMatchByRule );
            }
        }

        if ( S_OK == hr )
        {
            // parse the scopes beginning with argv[2] and above
            hr = ParseScopes( argc, argv, 2, &tempScopesList );
        }

        if ( S_OK == hr )
        {
            // outside pointer now owns the temp list
            // and the match by rule
            *scopesList = tempScopesList;
            tempScopesList = NULL;

            *matchByRule = tempMatchByRule;
            tempMatchByRule = NULL;
        }
    }
    else
    {
        hr = E_INVALIDARG;
    }

    if ( NULL != tempScopesList )
    {
        WSDFreeLinkedMemory( tempScopesList );
        tempScopesList = NULL;
    }

    if ( NULL != tempEpr )
    {
        delete[] tempEpr;
        tempEpr = NULL;
    }

    if ( NULL != tempMatchByRule )
    {
        delete[] tempMatchByRule;
        tempMatchByRule = NULL;
    }

    return hr;
}

HRESULT ValidateEndpointReference
(   _In_ LPCWSTR epr
)
{
    HRESULT hr = S_OK;
    LPCWSTR tempEpr = NULL; // (soft copy) used to determine if this is a logical or physical address
    
    if ( NULL == epr )
    {
        hr = E_INVALIDARG;
    }
    
    if ( S_OK == hr )
    {
        // Determine whether this EndpointReference begins with one of
        // http://, https:// or uri:
        if ( NULL != wcsstr( epr, L"http://" ) ||
             NULL != wcsstr( epr, L"https://" ) ||
             NULL != wcsstr( epr, L"uri:" ) )
        {
            // this is a valid EndpointReference beginning with
            // http://, https:// or uri:
            
            tempEpr = epr;
        }
    }
    
    if ( S_OK == hr && NULL == tempEpr )
    {
        // Determine whether this EndpointReference begins with urn:uuid:
        tempEpr = wcsstr( epr, L"urn:uuid:" );
        
        if ( NULL != tempEpr )
        {
            // The EndpointReference begins with urn:uuid:.
            
            //          1         2        3          4
            // 123456789012345678901234567890123456789012345
            // urn:uuid:f452f1ae-fbb4-11de-a6bb-00cc30bfc300
            
            // It must therefore be exactly 45 characters long
            if ( wcslen( tempEpr ) != 45 )
            {
                hr = E_INVALIDARG;
            }
        }
    }
    
    if ( S_OK == hr && NULL == tempEpr )
    {
        // Determine whether this EndpointReference begins with urn:uuid:
        tempEpr = wcsstr( epr, L"uuid:" );
        
        if ( NULL != tempEpr )
        {
            // The EndpointReference begins with uuid:.
            
            //          1         2        3          4
            // 12345678901234567890123456789012345678901
            // uuid:f452f1ae-fbb4-11de-a6bb-00cc30bfc300
            
            // It must therefore be exactly 41 characters long
            if ( wcslen( tempEpr ) != 41 )
            {
                hr = E_INVALIDARG;
            }
        }
    }
    
    if ( S_OK == hr && NULL == tempEpr )
    {
        // The EndpointReference does not begin with one of the
        // appropriate schemes.
        
        hr = E_INVALIDARG;
    }
    
    tempEpr = NULL;
    return hr;
}
