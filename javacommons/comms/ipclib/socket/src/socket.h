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

#ifndef SOCKET_H
#define SOCKET_H

#include "javaosheaders.h"

namespace java
{
namespace comms
{

OS_NONSHARABLE_CLASS(Socket)
{
public:
    Socket();
    Socket(int aSocket);
    virtual ~Socket();

    int open(int aPort);
    int read(char* aBuf, int aLen);
    int write(const char* aBuf, int aLen);
    int shutdown(int aHow);
    void close();

    int getSocket() const;
private:
    int recv(char* aBuf, int aLen);
    void setSockOpts();

    int mSocket;
};

} // namespace comms
} // namespace java

#endif // SOCKET_H
