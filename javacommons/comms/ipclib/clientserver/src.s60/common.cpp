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
* Description: Comms helper functions and constants
*
*/


#include "logger.h"

#include "common.h"


namespace java
{
namespace comms
{

HBufC8* messageToDes(const ipcMessage_t& aMsg)
{
    HBufC8* resultBuf = 0;

    int len = aMsg.ipcHeader.length;
    resultBuf = HBufC8::New(len);
    TPtr8 ptr = resultBuf->Des();
    TPtr8 ptr8((TUint8 *)&aMsg, len);
    ptr8.SetLength(len);
    ptr.Copy(ptr8);

    return resultBuf;
}

char* desToMessage(const TDesC8& aDes)
{
    int len = aDes.Length();
    char* buf = new char[len];
    ::memcpy(buf, aDes.Ptr(), len);
    return buf;
}


} // namespace comms
} // namespace java

