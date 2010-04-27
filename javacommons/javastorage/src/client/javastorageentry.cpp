/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JavaStorageEntry
*
*/


#include <stdio.h>

#include "javastorageentry.h"
#include "javastorageexception.h"

using namespace java::storage;
using namespace std;

OS_EXPORT void JavaStorageEntry::setEntry(const std::wstring& aName,
        const std::wstring& aValue,
        JavaEntryColumnType aType)
{
    if (aName.size() == 0)
    {
        throw JavaStorageException(
            EInvalidArgument,
            "Invalid entry name",
            __FILE__,
            __FUNCTION__,
            __LINE__);
    }

    if (aType < 1 || aType > NULL_TYPE)
    {
        throw JavaStorageException(
            EInvalidArgument,
            "Invalid entry type",
            __FILE__,
            __FUNCTION__,
            __LINE__);
    }

    mName = aName;

    if (aType == NULL_TYPE)
    {
        mValue = L"NULL";
        mType = aType;
    }
    else
    {
        mValue = aValue;
        mType = aType;
    }
}

OS_EXPORT wstring JavaStorageEntry::entryName() const
{
    return mName;
}

OS_EXPORT wstring JavaStorageEntry::entryValue() const
{
    if (mType != NULL_TYPE)
        return mValue;
    else
        return L"";
}

OS_EXPORT JavaStorageEntry::JavaEntryColumnType JavaStorageEntry::entryType() const
{
    return mType;
}

OS_EXPORT bool JavaStorageEntry::operator< (const JavaStorageEntry& jse) const
{
    if (mName == jse.entryName())
    {
        // Comparison is done only by the name
        if (mValue == L"")
        {
            return mName < jse.entryName();
        }
        else  // By the name and value
        {
            return mValue < jse.entryValue();
        }
    }
    else
    {
        return mName < jse.entryName();
    }
}
