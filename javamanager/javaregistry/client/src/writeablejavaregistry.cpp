/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  writeablejavaregistry implementation
*
*/


// INCLUDES
#include "javacommonutils.h"
#include "driveutilities.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javasymbianoslayer.h"
#include "javauid.h"
#include "logger.h"
#include "writeablejavaregistry.h"
#include "writeablejavaregistryapplicationentry.h"
#include "writeablejavaregistrypackageentry.h"

using namespace Java;
using namespace java::fileutils;
using namespace Java::Manager::Registry;
using namespace java::storage;
using namespace java::util;
using namespace std;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistry* CWriteableJavaRegistry::
NewL(TBool aUseIntegrity)
{
    CWriteableJavaRegistry* self
    = CWriteableJavaRegistry::NewLC(aUseIntegrity);

    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistry* CWriteableJavaRegistry::
NewLC(TBool aUseIntegrity)
{
    CWriteableJavaRegistry* self = new(ELeave) CWriteableJavaRegistry();
    CleanupStack::PushL(self);
    self->ConstructL(aUseIntegrity, EFalse);
    return self;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::~CWriteableJavaRegistry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistry::~CWriteableJavaRegistry()
{
    if (iStorage)
    {
        try
        {
            iStorage->close();
        }
        catch (JavaStorageException& jse)
        {
            // No can do
            ELOG1(EJavaStorage, "Storage close failed %s ",
                  jse.toString().c_str());
        }
        delete iStorage;
        iStorage = NULL;
    }
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::RegistryEntryExistsL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CWriteableJavaRegistry::
RegistryEntryExistsL(const TUid& aUid) const
{
    JELOG2(EJavaStorage);

    if (aUid == TUid::Null())
    {
        User::Leave(KErrArgument);
    }

    JavaStorageApplicationList_t foundApps;
    ApplicationUids(foundApps);

    TJavaRegistryEntryType appType;
    Uid entryUid;

    return MatchUid(foundApps, TUidToUid(aUid, entryUid), appType);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::GetRegistryEntryUidsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistry::
GetRegistryEntryUidsL(RArray<TUid>& aUids) const
{
    GetRegistryEntryUidsL(EGeneralPackage, aUids);
    GetRegistryEntryUidsL(EGeneralApplication, aUids);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::GetRegistryEntryUidsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistry::
GetRegistryEntryUidsL(TJavaRegistryEntryType aType,
                      RArray<TUid>& aUids,
                      TBool /*aAllEntries*/) const
{
    JavaStorageApplicationList_t apps;
    ApplicationUids(apps);

    if (apps.size() > 0)
    {
        if (EGeneralApplication == aType || EMidp2Midlet == aType)
        {
            JavaStorageEntry idQueryEntry;
            idQueryEntry.setEntry(ID, L"");

            AddUids(apps, idQueryEntry, aUids);
        }
        else if (EGeneralPackage == aType || EMidp2MidletSuite == aType)
        {
            JavaStorageEntry packageIdQueryEntry;
            packageIdQueryEntry.setEntry(PACKAGE_ID, L"");

            AddUids(apps, packageIdQueryEntry, aUids);
        }
        else
        {
            WLOG1(EJavaStorage, "Unknown entry type: %d", aType);
        }
    }
    apps.clear();
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::RegistryEntryL
// It calls DecideEntryTypeAndCreateL to create the suitable subclass of
// CWriteableJavaRegistryEntry.
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryEntry* CWriteableJavaRegistry::
RegistryEntryL(const TUid& aUid,
               TBool /*aAllEntries*/) const
{
    JavaStorageApplicationList_t foundApps;
    ApplicationUids(foundApps);

    TJavaRegistryEntryType appType;
    Uid entryUid;

    TBool match = MatchUid(foundApps, TUidToUid(aUid, entryUid), appType);
    foundApps.clear();

    // Original implementation returns NULL if entry does not exists.
    CWriteableJavaRegistryEntry* retEntry = NULL;

    if (match)
    {
        JavaStorageApplicationEntry_t attributes;

        if ((EGeneralApplication == appType))
        {
            StorageEntry(entryUid, APPLICATION_TABLE, attributes);

            if (attributes.size() > 0)
            {
                retEntry = new CWriteableJavaRegistryApplicationEntry(
                    attributes, EMidp2Midlet);
            }
        }
        // Already matched so it must be suite.
        else // EGeneralPackage.
        {
            StorageEntry(entryUid, APPLICATION_PACKAGE_TABLE, attributes);

            if (attributes.size() > 0)
            {
                retEntry = new CWriteableJavaRegistryPackageEntry(
                    attributes, EMidp2MidletSuite);
            }
        }
    }
    return retEntry;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::CWriteableJavaRegistry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistry::CWriteableJavaRegistry()
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistry::
ConstructL(TBool /*aUseIntegrity*/, TBool /*aLegacy*/)
{
    try
    {
        iStorage = JavaStorage::createInstance();
        iStorage->open();
    }
    catch (JavaStorageException& jse)
    {
        ELOG1(EJavaStorage, "Storage instance creation failed %s ",
              jse.toString().c_str());
        User::Leave(jse.mStatus);
    }
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::ApplicationUids
// ---------------------------------------------------------------------------
//
void CWriteableJavaRegistry::ApplicationUids(
    JavaStorageApplicationList_t& aFoundApps) const
{
    JavaStorageApplicationEntry_t searchQuery;
    JavaStorageEntry entry;
    entry.setEntry(ID, L"");
    searchQuery.insert(entry);

    entry.setEntry(PACKAGE_ID, L"");
    searchQuery.insert(entry);

    try
    {
        iStorage->search(APPLICATION_TABLE, searchQuery, aFoundApps);
    }
    catch (JavaStorageException& jse)
    {
        ELOG1(EJavaStorage, "Search failed: %s ",
              jse.toString().c_str());
    }
    searchQuery.clear();
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::MatchUid
// ---------------------------------------------------------------------------
//
TBool CWriteableJavaRegistry::MatchUid(
    const JavaStorageApplicationList_t& aApps,
    const Uid& aUid,
    Java::TJavaRegistryEntryType& aType) const
{
    JELOG2(EJavaStorage);

    TBool match = EFalse;

    // No applications nor suites to match.
    if (aApps.size()== 0)
    {
        LOG(EJavaStorage, EInfo, "No apps to match");
        return EFalse;
    }

    JavaStorageEntry idQueryEntry;
    idQueryEntry.setEntry(ID, aUid.toString());

    JavaStorageEntry packageIdQueryEntry;
    packageIdQueryEntry.setEntry(PACKAGE_ID, aUid.toString());

    JavaStorageApplicationList_t::const_iterator appIter;

    for (appIter = aApps.begin(); appIter != aApps.end(); appIter++)
    {
        JavaStorageApplicationEntry_t::const_iterator findIterator
        = (*appIter).find(idQueryEntry);

        if (findIterator != (*appIter).end())
        {
            aType = Java::EGeneralApplication;
            match = IsPresent((*appIter));
            break;
        }

        findIterator = (*appIter).find(packageIdQueryEntry);
        if (findIterator != (*appIter).end())
        {
            aType = Java::EGeneralPackage;
            match = IsPresent((*appIter));
            break;
        }
    }
    return match;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::StorageEntry
// ---------------------------------------------------------------------------
//
void CWriteableJavaRegistry::StorageEntry(
    const Uid& aUid,
    const string& aTableName,
    JavaStorageApplicationEntry_t& aAppEntry) const
{
    try
    {
        iStorage->read(aTableName, aUid, aAppEntry);
    }
    catch (JavaStorageException& jse)
    {
        ELOG1(EJavaStorage, "Read failed: %s ",
              jse.toString().c_str());

    }
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::AddUids
// ---------------------------------------------------------------------------
//
void CWriteableJavaRegistry::AddUids(
    const JavaStorageApplicationList_t& aApps,
    const JavaStorageEntry& aEntry,
    RArray<TUid>& aUids) const
{
    JavaStorageApplicationList_t::const_iterator iter;

    for (iter = aApps.begin(); iter != aApps.end(); iter++)
    {
        JavaStorageApplicationEntry_t::const_iterator finder
        = (*iter).find(aEntry);

        if (finder != (*iter).end())
        {
            Uid appUid((*finder).entryValue());
            TUid uid;
            TInt err = uidToTUid(appUid, uid);

            if (KErrNone == err)
            {
                err = aUids.Find(uid);

                if (KErrNotFound == err)
                {
                    // reset the error flag
                    err = KErrNone;
                    if (IsPresent((*iter)))
                    {
                        err = aUids.Append(uid);
                    }
                    if (err)
                    {
                        ELOG1(EJavaStorage, "Failed to add Uid to container. "
                              "( error code = %d )", err);

                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::IsPresent
// ---------------------------------------------------------------------------
//
TBool CWriteableJavaRegistry::IsPresent(
    const JavaStorageApplicationEntry_t& aEntry) const
{
    wstring value = L"";
    EntryAttributeValue(aEntry, PACKAGE_ID, value);

    java::util::Uid uid(value);
    value.clear();

    JavaStorageApplicationEntry_t appEntry;
    StorageEntry(uid, APPLICATION_PACKAGE_TABLE, appEntry);
    EntryAttributeValue(appEntry, MEDIA_ID, value);

    if (value == L"0")
    {
        // App is in internal c drive or internal mass memory.
        return true;
    }

    // App is in removable media.
    try
    {
        int sourceMediaId = JavaCommonUtils::wstringToInt(value);

        driveInfos drives;
        DriveUtilities::getAccesibleDrives(drives);

        driveInfos::const_iterator iter;

        for (iter = drives.begin(); iter != drives.end(); iter++)
        {
            if ((*iter).iId == sourceMediaId)
            {
                return ETrue;
            }
        }
    }
    catch (ExceptionBase& ee)
    {
        ELOG1WSTR(EJavaStorage, "MediaId conversion failed: '%s'", value);
    }
    return EFalse;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistry::EntryAttributeValue
// ---------------------------------------------------------------------------
//
void CWriteableJavaRegistry::EntryAttributeValue(
    const JavaStorageApplicationEntry_t& aEntry,
    const wstring& aName,
    wstring& aValue) const
{
    JavaStorageEntry attr;
    attr.setEntry(aName, L"");

    JavaStorageApplicationEntry_t::const_iterator findIterator = aEntry.find(attr);

    if (findIterator != aEntry.end())
    {
        aValue = (*findIterator).entryValue();
    }
    else
    {
        LOG(EJavaStorage, EInfo, "Entry attribute value not found");
        aValue = L"";
    }
}
