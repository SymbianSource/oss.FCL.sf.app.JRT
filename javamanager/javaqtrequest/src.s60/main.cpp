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
* Description:  QT Service application the makes Qt Highway API
*               request based on the command line of the application
*
*/

#include <QTimer>

#include "logger.h"
#include "requestapp.h"


int main(int argc, char **argv)
{
    LOG1(EJavaQtServiceApp, EInfo, "javaqtrequest main() called with %d args", argc);
    for (int nInd = 0; nInd < argc; nInd++)
    {
        LOG2(EJavaQtServiceApp, EInfo, "javaqtrequest main() arg %d is %s", nInd, *(argv + nInd));
    }

    RequestApp app(argc, argv);

    QTimer::singleShot(0, &app, SLOT(sendRequest()));

    int ret = app.exec();

    LOG1(EJavaQtServiceApp, EInfo, "javaqtrequest main() exiting with value %d", ret);

    return ret;
}

