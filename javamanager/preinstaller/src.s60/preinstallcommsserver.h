/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Comms server, part of OMJ S60 preinstaller process
 *                When started  in 'poll' mode Java Installer connects to
 *                this server to get install commands.
 *
*/


#ifndef PREINSTALLCOMMSSERVER_H
#define PREINSTALLCOMMSSERVER_H

#include <string>
#include <vector>

#include "commslistener.h"
#include "commsserverendpoint.h"
#include "commsmessage.h"

using namespace java::comms;

/**
 * The messages known by Preinstall Server
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

OS_NONSHARABLE_CLASS(PreinstallCommsServer) : public CommsListener
{
public:
    PreinstallCommsServer();
    virtual ~PreinstallCommsServer();

    void setJadFiles(RPointerArray<HBufC> &aJadFiles);

    // CommsListener methods
    virtual void processMessage(CommsMessage& aMessage);

    int start();
    int stop();

    CommsEndpoint* getComms()
    {
        return &mComms;
    }

private:
    int mRunning; // 1 if running, 0 if not running
    int mJadIndex; // Index of the next JAD file to be installed
    std::vector<std::wstring> mJadFiles; // The JAD files to be installer
    CommsServerEndpoint mComms;
};

#endif // PREINSTALLCOMMSSERVER_H
