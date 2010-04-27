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


#ifndef SERVERTESTUTILS_H
#define SERVERTESTUTILS_H

#include <string>

namespace java
{

namespace comms
{
class CommsClientEndpoint;
class CommsMessage;
}


namespace storage
{

class ServerTestUtils
{
public:
    ServerTestUtils();
    ~ServerTestUtils();

    bool connect();
    bool disconnect();
    bool sendReceive(java::comms::CommsMessage& aSendMsg,
                     java::comms::CommsMessage& aReceivedMsg);

private:
    java::comms::CommsClientEndpoint* mCliConn;

};

} // end namespace storage
} // end namespace java

#endif // SERVERTESTUTILS_H

