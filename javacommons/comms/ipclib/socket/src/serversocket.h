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
* Description:  ?Description
*
*/

#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H
#include "javaosheaders.h"

namespace java
{
namespace comms
{

OS_NONSHARABLE_CLASS(ServerSocket)
{
public:
    ServerSocket();
    virtual ~ServerSocket();

    int open(int port);
    int accept();
    int close();

    int getSocket() const;

private:
    int mSocket;
};

} // namespace comms
} // namespace java

#endif // SERVERSOCKET_H
