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

#include <QList>
#include <QUrl>
#include <xqappmgr.h>
#include <xqaiwrequest.h>

#include "logger.h"
#include "requestapp.h"


RequestApp::RequestApp(int &argc, char **argv) : QApplication(argc, argv)
{
    LOG(EJavaQtServiceApp, EInfo, "RequestApp (QApplication) constructor called");
}

void RequestApp::sendRequest()
{
    QString uriString = arguments().at(1);
    std::wstring stdWStrUri = uriString.toStdWString();
    LOG1(EJavaQtServiceApp, EInfo,
        "SLOT RequestApp::sendRequest called with Uri %S", stdWStrUri.c_str());

    QUrl uri(uriString);
    XQApplicationManager appMgr;
    XQAiwRequest *request = appMgr.create(uri);
    if (!request)
    {
        // No handlers for the URI
        ELOG1(EJavaQtServiceApp,
            "RequestApp::sendRequest No QtHighway service found for URI: %S",
            stdWStrUri.c_str());
        exit(KErrNotFound);
        return;
    }

    // Set function parameters
    QList<QVariant> args;
    args << uri.toString();
    request->setArguments(args);

    // Send the request
    if  (!request->send())
    {
        // Request failed.
        int error = request->lastError();
        ELOG2(EJavaQtServiceApp,
            "RequestApp::sendRequest QtHighway request failed "
            "with err %d, URI was %S",
            error, stdWStrUri.c_str());
        delete request;
        exit(KErrCompletion);
        return;
    }

    delete request;

    exit(KErrNone); // Exit with OK status
}



