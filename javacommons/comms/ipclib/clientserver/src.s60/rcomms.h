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
* Description: Wrapper for low level Symbian IPC methods
*
*/

#ifndef RCOMMS_H
#define RCOMMS_H

namespace java
{
namespace comms
{

NONSHARABLE_CLASS(RComms): public RSessionBase
{
public:
    TInt Connect(const TDesC& aName);
    void Disconnect();
    TInt Send(const TDesC8& aData);
    void Receive(RBuf8& aBuffer, TPckgBuf<TInt>& aRequiredSize, TRequestStatus& aStatus);
    void CancelReceive();
};

} // namespace comms
} // namespace java

#endif // RCOMMS_H
