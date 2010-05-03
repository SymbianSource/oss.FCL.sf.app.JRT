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
* Description:
*
*/

#ifndef COMMSMSGSENDER_H
#define COMMSMSGSENDER_H

#include "commsclientendpoint.h"

class CommsMsgSender
{
public :

    CommsMsgSender();

    ~CommsMsgSender();

    void SendStartJavaCertStorePlugin();

    void SendStopJavaCertStorePlugin();

private :

    java::comms::CommsClientEndpoint mComms;

    void SendMsg(int aMsgId);

};


#endif // COMMSMSGSENDER_H
