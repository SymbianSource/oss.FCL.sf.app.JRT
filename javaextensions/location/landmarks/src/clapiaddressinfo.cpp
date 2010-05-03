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
* Description:  Holds Location API address information
 *
*/


// INTERNAL INCLUDES
#include    "clapiaddressinfo.h"
#include    "lapipanics.h"
#include    "mlapilandmark.h"
#include    "logger.h"

// ---------------------------------------------------------------------------
// CLAPIAddressInfo::NewL
// ---------------------------------------------------------------------------
//
CLAPIAddressInfo* CLAPIAddressInfo::NewL()
{
    JELOG2(EJavaLocation);
    CLAPIAddressInfo* self = CLAPIAddressInfo::NewLC();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CLAPIAddressInfo::NewLC
// ---------------------------------------------------------------------------
//
CLAPIAddressInfo* CLAPIAddressInfo::NewLC()
{
    JELOG2(EJavaLocation);
    CLAPIAddressInfo* self = new(ELeave) CLAPIAddressInfo();
    CleanupStack::PushL(self);
    return self;
}

// ---------------------------------------------------------------------------
// CLAPIAddressInfo::~CLAPIAddressInfo
// ---------------------------------------------------------------------------
//
CLAPIAddressInfo::~CLAPIAddressInfo()
{
    JELOG2(EJavaLocation);
    iValues.ResetAndDestroy();
    iIds.Close();
}

// ---------------------------------------------------------------------------
// CLAPIAddressInfo::ValueCount
// ---------------------------------------------------------------------------
//
TInt CLAPIAddressInfo::ValueCount() const
{
    JELOG2(EJavaLocation);
    return iIds.Count();
}

// ---------------------------------------------------------------------------
// CLAPIAddressInfo::SetValueL
// ---------------------------------------------------------------------------
//
void CLAPIAddressInfo::SetValueL(TInt aFieldId, const TDesC* aValue)
{
    JELOG2(EJavaLocation);
    __ASSERT_DEBUG(aFieldId > 0 && aFieldId <= ELAPINumAddressInfos,
                   LAPIError::Panic(ELAPIPanicInvalidFieldId));

    TInt error = KErrNone;
    TInt index = iIds.Find(aFieldId);
    HBufC* value = aValue ? aValue->AllocLC() : NULL;
    // The information already exists in this info so replace the existing
    // Remove the old value because the pointer array owns the object
    if (index != KErrNotFound)
    {
        delete iValues[index];
        iValues.Remove(index);
        iValues.InsertL(value, index);
        // Do not leave yet since "value" may be in cleanup stack
        error = iIds.Insert(aFieldId, index);
    }
    else // New information, append to the end
    {
        iValues.AppendL(value);
        // Do not leave yet since "value" may be in cleanup stack
        error = iIds.Append(aFieldId);
    }

    // Pop the value if it was added to the cleanup stack
    if (value)
    {
        CleanupStack::Pop(value);
    }

    // Check the error and keep the arrays in sync with each other
    if (error != KErrNone)
    {
        delete iValues[index];
        iValues.Remove(index);
        User::Leave(error);
    }
}

// ---------------------------------------------------------------------------
// CLAPIAddressInfo::Value
// ---------------------------------------------------------------------------
//
const TDesC* CLAPIAddressInfo::Value(TInt aFieldId) const
{
    JELOG2(EJavaLocation);
    TInt index = iIds.Find(aFieldId);
    if (index != KErrNotFound)
    {
        return iValues[index];
    }

    return NULL; // Not found
}

// ---------------------------------------------------------------------------
// CLAPIAddressInfo::CLAPIAddressInfo
// ---------------------------------------------------------------------------
//
CLAPIAddressInfo::CLAPIAddressInfo()
{
    JELOG2(EJavaLocation);
}

// End of file
