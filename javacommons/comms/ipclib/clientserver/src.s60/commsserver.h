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
* Description: CCommsServer implements the Symbian IPC server
*
*/

#ifndef COMMSSERVER_H
#define COMMSSERVER_H

#include <e32base.h>
#include <e32cmn.h>

namespace java
{
namespace comms
{
class IpcListener;

OS_NONSHARABLE_CLASS(CCommsServer) : public CPolicyServer
{
    friend class IpcServer;

public:
    CCommsServer(IpcListener& aListener);
    virtual ~CCommsServer();
    void StartL(const TDesC& aName);

private:
    virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
    virtual int getPermissions(const RMessage2& aMessage) const;

private:
    IpcListener& mListener;
};


} // namespace comms
} // namespace java

#endif // COMMSSERVER_H
