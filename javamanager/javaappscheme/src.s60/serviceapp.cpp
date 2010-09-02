/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  QT Service application the implements support for
*               starting Java applications using "javaapp:" QUrl
*
*/

#include <QUrl>

#include "logger.h"

#include "serviceapp.h"
#include <xqserviceutil.h>


ServiceApp::ServiceApp(int &argc, char **argv)
    : QApplication(argc, argv), mUriService(NULL)
{
    LOG(EJavaQtServiceApp, EInfo, "ServiceApp (QApplication) constructor called");
    bool isService = XQServiceUtil::isService();
    if (isService)
    {
        mUriService = new UriService(this);
    }

    QString interface = XQServiceUtil::interfaceName();
    QString operation = XQServiceUtil::operationName();

    QString t = "SERVICEAPP:\n";
    t = t + (isService ?  "    Service launch\n" : "    Normal launch\n");
    t = t + (XQServiceUtil::isEmbedded() ? "    Embedded\n" : "    Not embedded\n");
    t = t + ("    Interface=" + interface + "\n");
    t = t + ("    Operation=" + operation + "\n");

    std::string callType = t.toStdString();
    LOG1(EJavaQtServiceApp, EInfo, "ServiceApp called as %s", callType.c_str());
}

ServiceApp::~ServiceApp()
{
    LOG(EJavaQtServiceApp, EInfo, "ServiceApp destructor called");
    delete mUriService;
}

void ServiceApp::quit()
{
    LOG(EJavaQtServiceApp, EInfo, "ServiceApp quit() called");
    exit(KErrNone); // Exit with OK status
}


// ----------UriService---------------

// The service string param given to XQServiceProvider must contain first
// the name of the name of the service and then the name of the interface
// (from service_conf.xml)
UriService::UriService(ServiceApp* parent)
: XQServiceProvider(QLatin1String("javaappscheme.com.nokia.symbian.IUriView"),parent),
    mServiceApp(parent)
{
    LOG(EJavaQtServiceApp, EInfo, "UriService::UriService called");
    publishAll();
}

UriService::~UriService()
{
    LOG(EJavaQtServiceApp, EInfo, "UriService::~UriService called");
}

bool UriService::view(const QString& uri)
{
    LOG(EJavaQtServiceApp, EInfo, "UriService::view(uri) called");
    return view(uri, true);
}

bool UriService::view(const QString& uri, bool retValue)
{
    LOG(EJavaQtServiceApp, EInfo, "UriService::view(uri, retValue) called");
    std::wstring stdWStrUri = uri.toStdWString();
    LOG1(EJavaQtServiceApp, EInfo, "url is %S", stdWStrUri.c_str());

    XQRequestInfo info = requestInfo();

    // Start javalauncher.exe and pass the Url to it
    _LIT(KJavaLauncherExe, "javalauncher.exe");
    RProcess rProcess;
    retValue = true;

    // start
    HBufC* bufUri = HBufC::New(stdWStrUri.size());
    if (0 == bufUri)
    {
        return false;
    }
    TPtr16 ptrUri(bufUri->Des());
    ptrUri.Append((const TUint16*)stdWStrUri.c_str(), stdWStrUri.size());

    TInt err = rProcess.Create(KJavaLauncherExe, ptrUri);
    if (KErrNone == err)
    {
        // This call will wait until javalauncher exits (or panics)
        TRequestStatus status;
        rProcess.Logon(status);
        rProcess.Resume();

        // now wait until javalauncher exits
        User::WaitForRequest(status);
        err = status.Int();
        if (err != KErrNone)
        {
            ELOG1(EJavaQtServiceApp,
                "UriService::view javalauncher exited with error %d", err);
            retValue = false;
        }
    }
    else
    {
        ELOG1(EJavaQtServiceApp,
            "UriService::view Cannot create javalauncher process, error %d", err);
        retValue = false;
    }

    // free resources before returning
    rProcess.Close();
    delete bufUri;

    if (KErrNone != err)
    {
#ifndef _DEBUG
        // Make sure Url is logged always if an error has happened
        ELOG1(EJavaQtServiceApp, "UriService::view url was %S", stdWStrUri.c_str());
#endif
        retValue = false;
    }

    return retValue;
}
