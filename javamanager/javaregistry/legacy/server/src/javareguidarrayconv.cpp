/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregconverter implementation
*
*/


// INCLUDES
#include <s32mem.h>
#include "javareguidarrayconv.h"
#include "logger.h"

using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// JavaRegUidArrayConverter::GetTUids
// ---------------------------------------------------------------------------
//
TInt JavaRegUidArrayConverter::
GetTUids(const TDesC& aValue, RArray<TUid>& aUids)
{
    TRAPD(err, JavaRegUidArrayConverter::GetTUidsL(aValue, aUids));
    if (err != KErrNone)
    {
        LOG1(EJavaStorage, EInfo, "Conversion error, error code %d",  err);
        aUids.Reset();
    }
    return err;
}

// ---------------------------------------------------------------------------
// JavaRegUidArrayConverter::GetTUidsL
// ---------------------------------------------------------------------------
//
void JavaRegUidArrayConverter::
GetTUidsL(const TDesC& aValue, RArray<TUid>& aUids)
{
    TInt32 count;

    HBufC8* buf = HBufC8::NewLC(aValue.Length());
    TPtr8 ptr = buf->Des();
    ptr.Copy(aValue);

    RDesReadStream  stream(*buf);
    CleanupClosePushL(stream);

    stream >> count;
    for (TInt i = 0; i < count; i++)
    {
        TUid uidValue;
        stream >> uidValue.iUid;
        aUids.AppendL(uidValue);
    }

    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(buf);
}

// ---------------------------------------------------------------------------
// JavaRegUidArrayConverter::StoreTUidsL
// ---------------------------------------------------------------------------
//
void JavaRegUidArrayConverter::
StoreTUidsL(const RArray<TUid>& aValue, HBufC*& aDes)
{
    TInt32 count = aValue.Count();

    TInt length = (count+1) * sizeof(TInt32);
    HBufC8* uidBuf8 = HBufC8::NewLC(length);
    TPtr8 ptr8 = uidBuf8->Des();
    RDesWriteStream stream(ptr8);
    CleanupClosePushL(stream);

    stream << count;

    for (TInt i=0; i < count; i++)
    {
        stream << aValue[i].iUid;
    }
    stream.CommitL();
    CleanupStack::PopAndDestroy(&stream);

    aDes = HBufC::NewL(2 * length);
    TPtr ptr = aDes->Des();
    ptr.Copy(ptr8);

    CleanupStack::PopAndDestroy(uidBuf8);
}
