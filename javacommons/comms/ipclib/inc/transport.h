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
* Description: IPC interface
*
*/

#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "javaosheaders.h"

namespace java
{
namespace comms
{

typedef struct ipcHeader_s
{
    int messageId;
    int length;
    int receiver;
    int sender;
    int moduleId;
    int messageRef;
    int permissions;
} ipcHeader_t;

typedef struct ipcMessage_s
{
    ipcHeader_t ipcHeader;
    char        data[1];
} ipcMessage_t;

class IpcListener
{
public:
    virtual void processMessage(const ipcMessage_t* aMsg) = 0;
    virtual void onStart() = 0;
    virtual void onExit() = 0;
};

class IpcConnectionInterface
{
public:
    virtual ~IpcConnectionInterface() {};
    virtual int connect(int aAddr) = 0;
    virtual void disconnect() = 0;
    virtual int send(ipcMessage_t* aMsg) = 0;
};

class IpcServerConnectionInterface
{
public:
    virtual ~IpcServerConnectionInterface() {};
    virtual int start(int aAddr) = 0;
    virtual void stop() = 0;
    virtual int send(ipcMessage_t* aMsg) = 0;
};


class IpcConnectionFactory
{
public:
    OS_IMPORT static IpcConnectionInterface* createConnection(IpcListener* aListener);
    OS_IMPORT static IpcServerConnectionInterface* createServerConnection(IpcListener* aListener);
};

} // namespace comms
} // namespace java

#endif // TRANSPORT_H
