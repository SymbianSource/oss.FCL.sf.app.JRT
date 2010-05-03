/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Comms server, part of Java platform 2.0 javappconverter process
*               When started  in 'poll' mode Java Installer connects to
*               this server to get install commands.
*
*/


#ifndef CONVERTSERVER_H
#define CONVERTSERVER_H

#include <string>
#include <vector>

#include "commslistener.h"
#include "commsserverendpoint.h"
#include "commsmessage.h"

using namespace java::comms;

/**
 * The messages known by Convert Server
 */

const int GET_NEXT_OPERATION_MESSAGE_ID = 500;
const int OPERATION_MESSAGE_ID = 501;
const int OPERATION_RESULT_MESSAGE_ID = 502;

/**
 * The known operations inside messages
 */

const int INSTALL_OPERATION = 0;
const int UNINSTALL_OPERATION = 1;
const int EXIT_OPERATION = 2;

class ConvertServer : public CommsListener
{
public:
    ConvertServer();
    virtual ~ConvertServer();

    void setInstallFiles(RPointerArray<HBufC> &aInstallFiles);

    // CommsListener methods
    virtual void processMessage(CommsMessage& aMessage);

    int start();
    int stop();

    CommsEndpoint* getComms()
    {
        return &iComms;
    }

private:
    int iRunning; // 1 if running, 0 if not running
    int iFileIndex; // decremented when install files (.jad or .jar or .dcf) are handled
    int iFilesTotal;
    std::vector<std::wstring> iInstallFiles;
    CommsServerEndpoint iComms;
};

#endif // CONVERTSERVER_H
