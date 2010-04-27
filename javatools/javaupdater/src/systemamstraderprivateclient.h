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
* Description:  systemamstraderprivateclient.h
*               Part of OMJ S60 environment updater process.
*               Used to close the old S60 java environment in a controlled way.
*
*/


#ifndef SYSTEMAMSTRADERPRIVATECLIENT_H
#define SYSTEMAMSTRADERPRIVATECLIENT_H

#include <e32base.h>


NONSHARABLE_CLASS(RSystemAMSTraderPrivateClient): public RSessionBase
{
public:

    /**
     * Tries to connect to SystemAMS trader private server and send
     * java enviroment shutdown request
     *
     * @param aStatus - TRequestStatus of active object, which is notified
     *                  when shutdown is done
     * @return - ETrue if connection to server was succesfull
     **/
    TBool ShutdownJavaEnv(TRequestStatus& aStatus);
};


#endif // SYSTEMAMSTRADERPRIVATECLIENT_H
