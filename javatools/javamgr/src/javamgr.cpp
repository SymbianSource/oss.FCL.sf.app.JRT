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
* Description:  JavaCap
*
*/


#include "comms.h"
#include "rtcmessages.h"
#include "coremessages.h"
#include "commsclientendpoint.h"
#include "exceptionbase.h"

#include "logger.h"

using namespace java::util;
using namespace java::comms;
using namespace java::captain;


int main(int argc, char *argv[])
{
    LOG(EUtils, EInfo, "TCK RUNNER main()");
    int result = -1;
    try
    {
        if (argc > 0 && argv != 0)
        {
            CommsMessage message;
            message.setModuleId(PLUGIN_ID_JAVACAPTAIN_CORE_C);
            message.setMessageId(CORE_MSG_ID_DO_THREAD_DUMP);
            CommsClientEndpoint comms;
            comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
            comms.send(message);
            comms.disconnect();

            result = 0;
        }
    }

    catch (ExceptionBase& e)
    {
        ELOG1(ETckRunner, "TCK RUNNER main() ExceptionBase caught: %s ",
              e.toString().c_str());
    }

    catch (std::exception& e)
    {
        ELOG1(ETckRunner, "TCK RUNNER main() Exception %s caught", e.what());
    }

    LOG1(ETckRunner, EInfo, "TCK RUNNER EXIT = %d", result);
    return result;
}

// End of File
