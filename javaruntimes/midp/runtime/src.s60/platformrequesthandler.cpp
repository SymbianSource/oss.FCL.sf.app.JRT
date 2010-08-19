/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This class provides container for message.
*
*/


#include <memory>

#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
#include <schemehandler.h>
#endif

#include <apgcli.h>               // for RApaLsSession
#include <apacmdln.h>
#include <sstream>

#include "platformrequesthandler.h"

#include "javasymbianoslayer.h"
#include "javacommonutils.h"
#include "s60commonutils.h"
#include "exceptionbase.h"
#include "logger.h"

using namespace java::runtime;
using namespace java::util;


PlatformRequestInterface* getPlatformRequestHandlerObj()
{
    JELOG2(EJavaRuntime);
    return new PlatformRequestHandler();
}

/**
 *
 */
PlatformRequestHandler::PlatformRequestHandler()
{
}

/**
 *
 */
PlatformRequestHandler::~PlatformRequestHandler()
{
}

/**
 * handleUri() supports at least following URIs:
 * file://, http://, https://, rtsp://, mailto://, wtai:, tel:, vtel:,
 * mms:, cti:, javaapp: and localapp:
 */
void PlatformRequestHandler::handleUri(const std::wstring& aUri)
{
    JELOG2(EJavaRuntime);
    TRAPD(err, launchAppL(aUri));
    if (err != KErrNone)
    {
        ELOG1(EJavaRuntime, "ERROR!!! PlatformRequestHandler::handleUri() %d",
              err);
        if (KErrNotFound == err)
        {
            if ((aUri.find(L"localapp:jam/launch?") == 0) ||
                (aUri.find(L"localapp://jam/launch?") == 0) ||
                (aUri.find(L"javaapp:") == 0))
            {
                // The URI is supported but the MIDlet specified by the URI
                // does not exist.
                throw ExceptionBase(PlatformRequestInterface::UNEXPECTED_ERROR,
                                    "Cannot start Java application",__FILE__,__FUNCTION__,__LINE__);
            }

            throw ExceptionBase(PlatformRequestInterface::CONNECTION_NOT_SUPPORTED,
                                "Not supported URI",__FILE__,__FUNCTION__,__LINE__);
        }
        else
        {
            std::string errTxt("Execution PlatformRequestHandler::handleUri() "
                               "failed to following error: ");
            errTxt.append(java::util::JavaCommonUtils::intToString(err));
            throw ExceptionBase(PlatformRequestInterface::UNEXPECTED_ERROR,
                                errTxt,__FILE__,__FUNCTION__,__LINE__);
        }
    }
}

/**
 *
 */
void PlatformRequestHandler::launchAppL(const std::wstring& aUri)
{
    JELOG2(EJavaRuntime);
    LOG1(EJavaRuntime, EInfo, "Platform request. URI: %S", aUri.c_str());
    if (startsArbitraryNativeApp(aUri))
    {
        launchNativeAppL(aUri);
    }
    else
    {
#ifdef RD_JAVA_S60_RELEASE_10_1_ONWARDS
        std::wstring uri;
        std::wstring okScheme(L"javaapp:");
        if (aUri.find(L"localapp:jam/launch?") == 0)
        {
            // Remove the scheme not supported by the devices in 10.1 from the
            // beginning, replace it with okScheme that is supported
            uri = okScheme;
            uri += aUri.substr(20);  // 20 == wstrlen(L"localapp:jam/launch?")
        }
        else if (aUri.find(L"localapp://jam/launch?") == 0)
        {
            uri = okScheme;
            uri += aUri.substr(22);  // 22 == wstrlen(L"localapp://jam/launch?")
        }
        else if (aUri.find(L"javaapp://") == 0)
        {
            // TODO: after QtHighway has been fixed to that
            // it can recognize "javaapp://" scheme, remove this.
            // At 2010wk26 it recognizes only "javaapp:"
            uri = okScheme;
            uri += aUri.substr(10);  // 10 == wstrlen(L"javaapp://")
        }
        else
        {
            uri = aUri;
        }
        LOG1(EJavaRuntime, EInfo, "Platform request. Converted uri: %S", uri.c_str());

        TPtrC ptr((const TUint16 *)uri.c_str(), uri.length());
        // Start javaqtrequest.exe so that url is command line argument.
        // javaqtrequest is a Qt application that will use Qt Highway API
        // to send the url request to correct XQServiceProvider
        _LIT(KJavaQtRequestExe, "javaqtrequest.exe");
        RProcess rProcess;
        TInt err = rProcess.Create(KJavaQtRequestExe, ptr);
        if (KErrNone != err)
        {
            ELOG1(EJavaRuntime,
                "PlatformRequestHandler: launchAppL: Starting javaqtrequest.exe failed, err %d",
                err);
            User::Leave(err);
        }

        // Wait until javaqtrequest exits
        TRequestStatus status;
        rProcess.Logon(status);
        rProcess.Resume();
        User::WaitForRequest(status);

        // Check the exit code of javaqtrequest
        err = status.Int();
        rProcess.Close();
        if (err != KErrNone)
        {
            ELOG1(EJavaRuntime,
                "PlatformRequestHandler: launchAppL: javaqtrequest.exe exited with err %d",
                err);
            User::Leave(err);
        }
#else
        TPtrC ptr((const TUint16 *)aUri.c_str(), aUri.length());
        std::auto_ptr<CSchemeHandler> schemeHandler(CSchemeHandler::NewL(ptr));
        schemeHandler->HandleUrlStandaloneL(); // Process Uri in standalone mode.
#endif
    }
}

/**
 *
 */
bool PlatformRequestHandler::startsArbitraryNativeApp(const std::wstring& aUri)
{
    if (aUri.find(L"nativeapp:") == 0)
    {
        return true;
    }

    if (aUri.find(L"localapp:native/launch?") == 0 ||
            aUri.find(L"localapp://native/launch?") == 0)
    {
        return true;
    }

    return false;
}

/**
 *
 */
void PlatformRequestHandler::launchNativeAppL(const std::wstring& aUri)
{
    std::wstring appName;

    RApaLsSession apaSession;
    TInt err = apaSession.Connect();
    User::LeaveIfError(err);
    CleanupClosePushL(apaSession);

    // Support percent encoding in the URI
    std::wstring uri = JavaCommonUtils::percentDecode(aUri);

    // Start by by Uid?
    const std::wstring appUid(L"application-uid=");
    const std::wstring appExe(L"application-exe=");
    const std::wstring semiColon(L";");
    std::string::size_type idx = uri.find(appUid);
    std::string::size_type idxSemiColon;
    if (idx != std::string::npos)
    {
        // Parse Uid
        TUid uid;
        idx += appUid.size(); // skip argument name
        idxSemiColon = uri.find(semiColon, idx);
        if (idxSemiColon == std::string::npos)
        {
            idxSemiColon = uri.size();
        }
        std::wstring wstrUid = uri.substr(idx, (idxSemiColon - idx));

        LOG1(EJavaRuntime, EInfo,
             "PlatformRequestHandler: launchNativeAppL: Uid in req is : %S", wstrUid.c_str());

        long long tmpInt;
        std::wstringstream stream(wstrUid);
        if (wstrUid.find(L"0x") == 0 || wstrUid.find(L"0X") == 0)
        {
            // hexadecimal
            stream >> std::hex >> tmpInt;
        }
        else
        {
            stream >> tmpInt;
        }
        uid.iUid = tmpInt;

        // Find executable name from AppArc
        TApaAppInfo info;
        err = apaSession.GetAppInfo(info, uid);
        if (KErrNone != err)
        {
            ELOG2(EJavaRuntime,
                  "PlatformRequestHandler: launchNativeAppL: GetAppInfo for uid %x failed, err %d",
                  uid.iUid,
                  err);
            User::Leave(err);
        }
        info.iFullName.Append('\0');
        appName = (wchar_t *)(&(info.iFullName[0]));
        LOG1(EJavaRuntime, EInfo,
             "PlatformRequestHandler: launchNativeAppL: app to start is : %S", appName.c_str());
    }
    else if ((idx = uri.find(appExe)) != std::string::npos)
    {
        // Parse exe name
        idx += appExe.size(); // skip argument name
        idxSemiColon = uri.find(semiColon, idx);
        if (idxSemiColon == std::string::npos)
        {
            idxSemiColon = uri.size();
        }
        appName = uri.substr(idx, (idxSemiColon - idx));

        LOG1(EJavaRuntime, EInfo,
             "PlatformRequestHandler: launchNativeAppL: Exe name in req is : %S", appName.c_str());
    }
    else
    {
        // Illegal uri
        User::Leave(KErrArgument);
    }

    // Start forming command line
    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();

    // Set executable name
    HBufC *pAppName = S60CommonUtils::wstringToDes(appName.c_str());
    if (pAppName)
    {
        CleanupStack::PushL(pAppName);
        cmdLine->SetExecutableNameL(*pAppName);
        CleanupStack::PopAndDestroy(pAppName);
    }
    cmdLine->SetCommandL(EApaCommandRun);

    // Content (document) specified?
    const std::wstring contentArg(L"content=");
    if ((idx = uri.find(contentArg)) != std::string::npos)
    {
        // Parse document name
        idx += contentArg.size(); // skip argument name
        idxSemiColon = uri.find(semiColon, idx);
        if (idxSemiColon == std::string::npos)
        {
            idxSemiColon = uri.size();
        }
        std::wstring content = uri.substr(idx, (idxSemiColon - idx));

        LOG1(EJavaRuntime, EInfo,
             "PlatformRequestHandler: launchNativeAppL: content: %S", content.c_str());

        HBufC *pContent = S60CommonUtils::wstringToDes(content.c_str());
        if (pContent)
        {
            CleanupStack::PushL(pContent);
            cmdLine->SetDocumentNameL(*pContent);
            CleanupStack::PopAndDestroy(pContent);
        }
    }

    // Other application arguments specified?
    const std::wstring appArgs(L"application-args=");
    if ((idx = uri.find(appArgs)) != std::string::npos)
    {
        // Parse applications args
        idx += appArgs.size(); // skip argument name
        // Rest of the command URI contains args
        idxSemiColon = uri.size();
        std::wstring args = uri.substr(idx, (idxSemiColon - idx));

        LOG1(EJavaRuntime, EInfo,
             "PlatformRequestHandler: launchNativeAppL: app args: %S", args.c_str());

        if (args.size() > 0)
        {
            // Convert args to 8 bit descriptor
            char* utf8string = NULL;
            try
            {
                utf8string = JavaCommonUtils::wstringToUtf8(args);
            }
            catch (std::exception& e)
            {
                ELOG1(EJavaRuntime,
                      "PlatformRequestHandler: launchNativeAppL: Cannot convert app args to"
                      " UTF-8. Error %s", e.what());
                User::Leave(KErrArgument);
            }
            TInt argsLen = strlen(utf8string);
            TPtr8 ptrArgs((unsigned char *)utf8string, argsLen, argsLen + 4);

            cmdLine->SetTailEndL(ptrArgs);
            delete[] utf8string;
        }
    }

    // Start executable
    err = apaSession.StartApp(*cmdLine);
    if (KErrNone != err)
    {
        ELOG1(EJavaRuntime,
              "PlatformRequestHandler: launchNativeAppL: StartApp error %d", err);
        User::Leave(err);
    }

    CleanupStack::PopAndDestroy(cmdLine);
    CleanupStack::PopAndDestroy(); // apaSession
}
