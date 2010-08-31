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

#include <QTimer>

#include "logger.h"
#include "serviceapp.h"


int main(int argc, char **argv)
{
    LOG1(EJavaQtServiceApp, EInfo, "javaappscheme main() called with %d args", argc);
    for (int nInd = 0; nInd < argc; nInd++)
    {
        LOG2(EJavaQtServiceApp, EInfo, "javaappscheme main() arg %d is %s", nInd, *(argv + nInd)); 
    }
    
    ServiceApp app(argc, argv);
   
    // Close this service app after 14 seconds if no one has called us
    QTimer::singleShot(14 * 1000, &app, SLOT(quit()));

    int ret = app.exec();
    
    LOG1(EJavaQtServiceApp, EInfo, "javaappscheme main() exiting with value %d", ret);

    return ret;
}

