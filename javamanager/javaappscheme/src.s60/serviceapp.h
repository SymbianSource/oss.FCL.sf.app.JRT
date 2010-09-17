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

#ifndef JAVAQTSERVICEAPP_H
#define JAVAQTSERVICEAPP_H


// #include <QCoreApplication>
#include <QApplication>
#include <xqserviceprovider.h>


class UriService;

class ServiceApp : public QApplication // public QCoreApplication
{
    Q_OBJECT

public:
    ServiceApp(int &argc, char **argv);
    ~ServiceApp();

public slots:
    static void quit();

private:
    UriService* mUriService;
};


class UriService : public XQServiceProvider
{
    Q_OBJECT
    public:
        UriService( ServiceApp *parent = 0 );
        ~UriService();

    public slots:
        bool view(const QString& uri);
        bool view(const QString& uri, bool returnValue);
};


#endif  // JAVAQTSERVICEAPP_H
