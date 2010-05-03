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
* Description: CReceiver enables asynchronous communication with server side
*
*/

#ifndef CRECEIVER_H
#define CRECEIVER_H

#include <e32base.h>

namespace java
{
namespace comms
{

class RComms;
class IpcListener;
class IpcClient;

NONSHARABLE_CLASS(CReceiver): public CActive
{
public:
    CReceiver(IpcClient& aParent, IpcListener& aListener, RComms& aSession);
    ~CReceiver();
    void Receive();

private:
    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);

    IpcClient&      mParent;
    IpcListener&    mListener;
    RComms&         mSession;
    RBuf8           mBuffer;
    TPckgBuf<TInt>  mRequiredLength;
};

} // namespace comms
} // namespace java

#endif // CRECEIVER_H
