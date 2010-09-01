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
* Description:  MidletAppInfo implementation
*
*/


#include "appmngr2midletstorageutil.h"
#include "exceptionbase.h"
#include "javastoragenames.h"
#include "javasymbianoslayer.h"
#include "javauid.h"
#include "logger.h"                          // LOG

using namespace java::storage;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::AppMngr2MidletStorageUtil()
// ---------------------------------------------------------------------------
//
AppMngr2MidletStorageUtil::AppMngr2MidletStorageUtil()
{
    mStorage.reset(JavaStorage::createInstance());
    mIsOpen = false;
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletAppInfo::~AppMngr2MidletStorageUtil()
// ---------------------------------------------------------------------------
//
AppMngr2MidletStorageUtil::~AppMngr2MidletStorageUtil()
{
    if (mIsOpen)
    {
        try
        {
            mStorage->close();
        }
        catch (JavaStorageException& aJse)
        {
            // No can do.
            //LOG(EJavaAppMngrPlugin, EInfo, "Storage connection close failed.");
        }
    }
}

void AppMngr2MidletStorageUtil::populateSuiteInformation(const TUid aUid)
{
    mPopulatedEntry.clear();

    checkConnection();
    java::util::Uid javaUid;

    try
    {
        mStorage->read(APPLICATION_PACKAGE_TABLE,
                       TUidToUid(aUid, javaUid),
                       mPopulatedEntry);
    }
    catch (JavaStorageException& aJse)
    {
        WLOG(EJavaAppMngrPlugin, "Suite information population failed.");
    }
}

HBufC* AppMngr2MidletStorageUtil::attribute(const TDesC& aName)
{
    std::auto_ptr<HBufC16> nameBuf(HBufC16::New(aName.Size() + 1));

    if (!nameBuf.get())
    {
        ELOG(EJavaAppMngrPlugin, "Cannot allocate name buffer.");
        throw java::util::ExceptionBase(
            "Out of memory", __FILE__, __FUNCTION__, __LINE__);
    }

    TPtr16 namePtr(nameBuf->Des());
    namePtr.Copy(aName);
    std::wstring name(desToWstring(namePtr));

    JavaStorageEntry attr;
    attr.setEntry(name, L"");

    JavaStorageApplicationEntry_t::const_iterator finder =
        mPopulatedEntry.find(attr);

    std::wstring value = L"";
    if (finder != mPopulatedEntry.end())
    {
        value = (*finder).entryValue();
    }
    else
    {
        ILOG1(EJavaAppMngrPlugin, "Attribute '%S' not found.", name.c_str());
        return NULL;
    }

    HBufC* valueBuf(HBufC16::New(value.size() + 1));

    if (!valueBuf)
    {
        ELOG(EJavaAppMngrPlugin, "Cannot allocate value buffer.");
        throw java::util::ExceptionBase(
            "Out of memory", __FILE__, __FUNCTION__, __LINE__);
    }

    TPtr16 valuePtr(valueBuf->Des());
    valuePtr = (const TUint16*)value.c_str();
    return valueBuf;    // Ownership is transfered.
}

void AppMngr2MidletStorageUtil::checkConnection()
{
    if (!mIsOpen)
    {
        try
        {
            mStorage->open();
            mIsOpen = true;
        }
        catch (JavaStorageException& aJse)
        {
            WLOG(EJavaAppMngrPlugin, "Cannot open storage connection.");
        }
    }
}
