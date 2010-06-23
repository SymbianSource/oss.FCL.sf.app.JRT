/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Comms server, part of Java Sif plugin.
*               When started  in 'commsresult' mode from Java Sif plugin
*               Java Installer sends the results of the operation
*               it executes (install, uninstall or component info)
*               to this server.
*/


#ifndef RESULTSSERVER_H
#define RESULTSSERVER_H

#include <string>
#include <vector>

#include <e32base.h>
#include <usif/sif/sifplugin.h>

#include "commslistener.h"
#include "commsserverendpoint.h"
#include "commsmessage.h"

using namespace Usif;
using namespace java::comms;

/**
 * The messages known by Results Server
 */

const int INSTALLER_RESULT_MESSAGE_ID = 601;
const int INSTALLER_RESULT_RESPONSE_MESSAGE_ID = 602;


/**
 * The known operations inside messages
 */

const int INSTALL_OPERATION = 0;
const int UNINSTALL_OPERATION = 1;
const int COMPONENT_INFO_OPERATION = 2;

class ResultsServer : public CommsListener
{
public:
    ResultsServer(COpaqueNamedParams& aResults, CComponentInfo& aInfo);
    virtual ~ResultsServer();

    // CommsListener methods
    virtual void processMessage(CommsMessage& aMessage);

    int start();
    int stop();

    CommsEndpoint* getComms()
    {
        return &iComms;
    }

private:
    void clearData();
    void setComponentInfoL();
    void resetDefaultErrorValues();
    void setCommonErrorInfo(int aResult);

private:
    int iRunning;   // 1 if running, 0 if not running
    CommsServerEndpoint  iComms;
    COpaqueNamedParams  &iResults;
    CComponentInfo      &iInfo;

    std::map<std::wstring, int> iIntPairs;
    std::map<std::wstring, std::wstring> iStringPairs;
};

#endif // RESULTSSERVER_H
