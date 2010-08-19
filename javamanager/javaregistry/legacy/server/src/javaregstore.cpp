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
* Description:  javaregstore implementation
*
*/


#include <s32file.h>
#include <memory>
#include "driveutilities.h"
#include "javaattributes.h"
#include "javacommonutils.h"
#include "javapropertyarray.h"
#include "javaregconverter.h"
#include "javaregstore.h"
#include "javaregproperty.h"
#include "javaregistrypropertyids.h"
#include "javareguidarrayconv.h"
#include "javastorageexception.h"
#include "javastoragenames.h"
#include "javasymbianoslayer.h"
#include "javauid.h"
#include "logger.h"
#include "midpregistrypropertyids.h"
#include "javareguidarrayconv.h"

using namespace Java::Manager::Registry;
using namespace java::storage;
using namespace java::util;
using namespace std;

// ---------------------------------------------------------------------------
// Constructs a java registry store object.
// ---------------------------------------------------------------------------
CJavaRegStore* CJavaRegStore::NewL()
{
    CJavaRegStore* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}


// ---------------------------------------------------------------------------
// Constructs a java registry store object.
// ---------------------------------------------------------------------------
CJavaRegStore* CJavaRegStore::NewLC()
{
    CJavaRegStore* self = new(ELeave) CJavaRegStore();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}


// ---------------------------------------------------------------------------
// Reads properties
// ---------------------------------------------------------------------------
void CJavaRegStore::ReadPropertiesL(
    TUid aUid, CJavaPropertyArray*& aProperties, TBool /*aAllEntries*/)
{
    JELOG(EJavaStorage, "CJavaRegStore::ReadPropertiesL()");
    Uid uid;
    TUidToUid(aUid, uid);

    JavaStorageApplicationEntry_t attributes;
    Java::TJavaRegistryEntryType appType = EGeneralApplication;

    EntryAttributesL(uid, appType, attributes);

    if (0 == attributes.size())
    {
        return;
    }

    PopulatePropertiesL(aProperties, attributes);
    attributes.clear();

    PopulateAppTypePropertiesL(uid, appType, aProperties);
}


// ---------------------------------------------------------------------------
// Get the stored uids for all drives.
// ---------------------------------------------------------------------------
void CJavaRegStore::GetUidsL(RArray<TUid>& aUids)
{
    JELOG(EJavaStorage, "CJavaRegStore::GetUidsL()");
    JavaStorageApplicationList_t apps;
    JavaStorageApplicationEntry_t searchQuery;
    JavaStorageEntry entry;
    entry.setEntry(ID, L"");
    searchQuery.insert(entry);

    entry.setEntry(PACKAGE_ID, L"");
    searchQuery.insert(entry);

    SearchL(APPLICATION_TABLE, searchQuery, apps);

    JavaStorageEntry attribute;
    attribute.setEntry(ID, L"");

    // Add all applications.
    AddUidsL(apps, attribute, aUids);

    attribute.setEntry(PACKAGE_ID, L"");

    // Add all application suites.
    AddUidsL(apps, attribute, aUids);
}


// ---------------------------------------------------------------------------
// CJavaRegStore::FindEntriesL
// ---------------------------------------------------------------------------
void CJavaRegStore::FindEntriesL(
    CJavaPropertyArray& aProperties,
    RArray<TUid>& aFoundUids, TBool /*aAllEntries*/)
{
    JELOG(EJavaStorage, "CJavaRegStore::FindEntriesL()");
    JavaStorageApplicationList_t apps;
    JavaStorageApplicationEntry_t searchQuery;
    JavaStorageEntry attribute;

    for (TInt i = 0; i < aProperties.Count(); i++)
    {
        if (EPropType == aProperties.At(i)->Id())
        {
            TInt32 value = JavaRegConverter::GetTInt(
                               aProperties.At(i)->Value(), 0);

            switch (value)
            {
            case EGeneralPackage:
            case EMidp2MidletSuite:
            {
                attribute.setEntry(PACKAGE_ID, L"");
                searchQuery.insert(attribute);

                SearchL(APPLICATION_TABLE, searchQuery, apps);
                searchQuery.clear();
                AddUidsL(apps, attribute, aFoundUids);
                break;
            }
            case EGeneralApplication:
            case EMidp2Midlet:
            {
                // To check if this is present.
                attribute.setEntry(PACKAGE_ID, L"");
                searchQuery.insert(attribute);

                attribute.setEntry(ID, L"");
                searchQuery.insert(attribute);

                SearchL(APPLICATION_TABLE, searchQuery, apps);
                searchQuery.clear();

                AddUidsL(apps, attribute, aFoundUids);
                break;
            }
            default:
            {
                LOG1(EJavaStorage,
                     EInfo,
                     "Unsupported app type: %d",
                     value);
                break;
            }
            }
        }
        else if (EPropName == aProperties.At(i)->Id()
                 && aProperties.At(i)->Value() != KNullDesC)
        {
            // This is search by NAME and VENDOR pair.
            auto_ptr<HBufC16> tempBuf(HBufC16::NewL(
                                          aProperties.At(i)->Value().Size() + 1));

            TPtr16 tempBufPtr(tempBuf->Des());
            tempBufPtr.Append(aProperties.At(i)->Value());
            wstring propName(desToWstring(tempBufPtr));
            tempBuf.reset(0);

            attribute.setEntry(PACKAGE_NAME, propName);
            searchQuery.insert(attribute);

            TBool found = EFalse;
            TInt index = 0;
            // Search EPropVendor from props.
            for (; index < aProperties.Count(); index++)
            {
                if (aProperties.At(index)->Id() == EPropVendor)
                {
                    found = ETrue;
                    break;
                }
            }

            if (found && aProperties.At(index)->Value() != KNullDesC)
            {
                auto_ptr<HBufC16>tempBuf2(HBufC16::NewL(
                                              aProperties.At(index)->Value().Size() + 1));

                TPtr16 tempBufPtr2(tempBuf2->Des());
                tempBufPtr2.Append(aProperties.At(index)->Value());
                wstring vendorProp(desToWstring(tempBufPtr2));
                tempBuf2.reset(0);

                attribute.setEntry(VENDOR, vendorProp);
                searchQuery.insert(attribute);

                attribute.setEntry(ID, L"");
                searchQuery.insert(attribute);

                SearchL(APPLICATION_PACKAGE_TABLE, searchQuery, apps);
                searchQuery.clear();
                AddUidsL(apps, attribute, aFoundUids);
            }
        }
        else
        {
            LOG1(EJavaStorage,
                 EInfo,
                 "Find by unsupported property: %d",
                 aProperties.At(i)->Id());
        }
    }
}


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CJavaRegStore::~CJavaRegStore()
{
    if (iStorage)
    {
        try
        {
            iStorage->close();
        }
        catch (JavaStorageException& jse)
        {
            ELOG1(EJavaStorage, "Storage close failed %s ",
                  jse.toString().c_str());
        }
        delete iStorage;
        iStorage = NULL;
    }
}


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CJavaRegStore::CJavaRegStore()
{
}


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CJavaRegStore::ConstructL()
{
    JELOG(EJavaStorage, "CJavaRegStore::ConstructL()");
    try
    {
        iStorage = JavaStorage::createInstance();
        iStorage->open();
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaStorage,
              "Cannot create storage: %s", aJse.toString().c_str());
        User::Leave(aJse.mStatus);
    }
}


// ---------------------------------------------------------------------------
// CJavaRegStore::MatchUid
// ---------------------------------------------------------------------------
//
TBool CJavaRegStore::MatchUidL(
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

    for (appIter = aApps.begin(); appIter != aApps.end(); ++appIter)
    {
        JavaStorageApplicationEntry_t::const_iterator findIterator
        = (*appIter).find(idQueryEntry);

        if (findIterator != (*appIter).end())
        {
            aType = EMidp2Midlet;
            match = IsPresentL((*appIter));
            break;
        }

        findIterator = (*appIter).find(packageIdQueryEntry);
        if (findIterator != (*appIter).end())
        {
            aType = EMidp2MidletSuite;
            match = IsPresentL((*appIter));
            break;
        }
    }
    return match;
}

// ---------------------------------------------------------------------------
// CJavaRegStore::StorageEntry
// ---------------------------------------------------------------------------
//
void CJavaRegStore::StorageEntryL(
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
        User::Leave(jse.mStatus);
    }
}

// ---------------------------------------------------------------------------
// CJavaRegStore::PopulatePropertiesL
// ---------------------------------------------------------------------------
//
void CJavaRegStore::PopulatePropertiesL(
    CJavaPropertyArray*& aProperties,
    const JavaStorageApplicationEntry_t& aAttrs) const
{
    aProperties = CJavaPropertyArray::NewLC();

    JavaStorageApplicationEntry_t::const_iterator iter;

    for (iter = aAttrs.begin(); iter != aAttrs.end(); ++iter)
    {
        // These are deleted after response is written to the client.
        CJavaProperty* prop = CreatePropertyL(
                                  (*iter).entryName(), (*iter).entryValue());

        if (prop)
        {
            aProperties->Append(prop);
        }
    }
    CleanupStack::Pop(aProperties);
}

// ---------------------------------------------------------------------------
// CJavaRegStore::CreatePropertyL
// ---------------------------------------------------------------------------
//
CJavaProperty* CJavaRegStore::CreatePropertyL(const wstring& aName,
        const wstring& aValue) const
{
    TInt id = KErrNotFound;

    CJavaProperty* prop = NULL;

    if (NAME == aName || PACKAGE_NAME == aName)
    {
        id = EPropName;
    }
    else if (PACKAGE_ID == aName)
    {
        TUid uid;
        java::util::Uid omjUid(aValue);
        TInt err = uidToTUid(omjUid, uid);

        if (KErrNone == err)
        {
            HBufC* intBuf = NULL;  // This is allocated at StoreTIntL
            JavaRegConverter::StoreTIntL(uid.iUid, intBuf);
            CleanupStack::PushL(intBuf);
            prop = CJavaProperty::NewL(EPropInstallPackage, intBuf->Des());
            CleanupStack::PopAndDestroy(intBuf);
        }
        else
        {
            ELOG1(EJavaStorage, "UidToTUid conversion err: %d", err);
        }
        id = KErrNotFound;
    }
    else if (VERSION == aName)
    {
        HBufC* versionBuf = NULL;  // This is allocated at StoreTIntL
        try
        {
            JavaRegConverter::StoreTAppVersionL(
                wstringToAppVersion(aValue), versionBuf);
            CleanupStack::PushL(versionBuf);
            prop = CJavaProperty::NewL(EPropVersion, versionBuf->Des());
            CleanupStack::PopAndDestroy(versionBuf);
        }
        catch (ExceptionBase& e)
        {
            ELOG1WSTR(EJavaStorage,
                      "Version UTF-8 conversion failed: '%s'", aValue);
        }
        id = KErrNotFound;
    }
    else if (VENDOR == aName)
    {
        id = EPropVendor;
    }
    else if (JAR_PATH == aName)
    {
        id = EPropJarPath;
    }
    else if (JAR_URL == aName)
    {
        id = EPropJarUrl;
    }
    else if (ROOT_PATH == aName)
    {
        try
        {
            wstring::size_type start = 0;
            wstring::size_type end = 1;

            char* driveLetter =  JavaCommonUtils::wstringToUtf8(
                                     aValue.substr(start, end));

            TInt driveNumber = -1;
            TInt err = RFs::CharToDrive(driveLetter[0], driveNumber);

            if (KErrNone != err)
            {
                WLOG2(EJavaStorage,
                      "Drive number conversion error: %d, Drive letter: %c",
                      err, driveLetter[0]);
                driveNumber = EDriveC;
            }

            delete[] driveLetter;

            HBufC* intBuf = NULL;  // This is allocated at StoreTIntL
            JavaRegConverter::StoreTIntL(driveNumber, intBuf);
            CleanupStack::PushL(intBuf);
            prop = CJavaProperty::NewL(EPropDrive, intBuf->Des());
            CleanupStack::PopAndDestroy(intBuf);
        }
        catch (ExceptionBase& e)
        {
            ELOG1WSTR(EJavaStorage, "ROOT_PATH utf-8 conversion failed: '%s'", aValue);
        }
        id = KErrNotFound;
    }
    else if (JAD_PATH == aName)
    {
        id = EPropJadPath;
    }
    else if (JAD_URL == aName)
    {
        id = EPropJadUrl;
    }
    else if (INITIAL_SIZE == aName)
    {
        TInt initialSize = JavaCommonUtils::wstringToInt(aValue);
        HBufC* intBuf = NULL;  // This is allocated at StoreTIntL
        JavaRegConverter::StoreTIntL(initialSize, intBuf);
        CleanupStack::PushL(intBuf);
        prop = CJavaProperty::NewL(EPropSize, intBuf->Des());
        CleanupStack::PopAndDestroy(intBuf);
        id = KErrNotFound;
    }

    if (KErrNotFound != id)
    {
        auto_ptr<HBufC> valueBuf(wstringToBuf(aValue));
        prop = CJavaProperty::NewL(id, valueBuf->Des());
    }

    // Ownership is transfered.
    return prop;
}

// ---------------------------------------------------------------------------
// CJavaRegStore::AddUidsL
// ---------------------------------------------------------------------------
//
void CJavaRegStore::AddUidsL(
    const JavaStorageApplicationList_t& aApps,
    const JavaStorageEntry& aEntry,
    RArray<TUid>& aUids) const
{
    JavaStorageApplicationList_t::const_iterator iter;

    for (iter = aApps.begin(); iter != aApps.end(); ++iter)
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
                    if (IsPresentL((*iter)))
                    {
                        aUids.AppendL(uid);
                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CJavaRegStore::Search
// ---------------------------------------------------------------------------
//
void CJavaRegStore::SearchL(const string& aTableName,
                            const JavaStorageApplicationEntry_t& aQuery,
                            JavaStorageApplicationList_t& aFoundApps) const
{
    try
    {
        iStorage->search(aTableName, aQuery, aFoundApps);
    }
    catch (JavaStorageException& jse)
    {
        ELOG1(EJavaStorage, "Search failed: %s ",
              jse.toString().c_str());
        User::Leave(jse.mStatus);
    }
}

// ---------------------------------------------------------------------------
// CJavaRegStore::ComplementaryInformationL
// ---------------------------------------------------------------------------
//
void CJavaRegStore::ComplementaryInformationL(const Uid& aUid,
        JavaStorageApplicationEntry_t& aAttrs) const
{
    JavaStorageApplicationEntry_t storeAttrs = aAttrs;
    StorageEntryL(aUid, APPLICATION_PACKAGE_TABLE, aAttrs);

    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_NAME, L"");

    JavaStorageApplicationEntry_t::const_iterator finder =
        aAttrs.find(attr);

    if (finder != aAttrs.end())
    {
        aAttrs.erase((*finder));
    }

    // Restore original attrs.
    for (finder = storeAttrs.begin(); finder != storeAttrs.end(); ++finder)
    {
        aAttrs.insert((*finder));
    }
    storeAttrs.clear();
}

// ---------------------------------------------------------------------------
// CJavaRegStore::PopulateManifestAttributes
// ---------------------------------------------------------------------------
//
void CJavaRegStore::PopulateManifestAttributesL(
    const Uid& /*aUid*/,
    CJavaPropertyArray*& /*aProperties*/) const
{
    // Considered as not needed. Left as placeholder.
}

// ---------------------------------------------------------------------------
// CJavaRegStore::EntryAttributeValue
// ---------------------------------------------------------------------------
//
void CJavaRegStore::EntryAttributeValue(
    const JavaStorageApplicationEntry_t& aEntry,
    const wstring& aName,
    wstring& aValue) const
{
    JavaStorageEntry attr;
    attr.setEntry(aName, L"");
    JavaStorageApplicationEntry_t::const_iterator finder = aEntry.find(attr);

    if (finder != aEntry.end())
    {
        aValue = (*finder).entryValue();
    }
    else
    {
        LOG(EJavaStorage, EInfo, "Entry attribute value not found");
        aValue = L"";
    }
}


// ---------------------------------------------------------------------------
// CJavaRegStore::EntryAttributesL
// ---------------------------------------------------------------------------
//
void CJavaRegStore::EntryAttributesL(
    Uid& aUid,
    Java::TJavaRegistryEntryType& aAppType,
    JavaStorageApplicationEntry_t& aAttributes) const
{
    aAttributes.clear();
    JavaStorageEntry entry;
    JavaStorageApplicationList_t apps;

    entry.setEntry(ID, L"");
    aAttributes.insert(entry);
    entry.setEntry(PACKAGE_ID, L"");
    aAttributes.insert(entry);

    SearchL(APPLICATION_TABLE, aAttributes, apps);
    aAttributes.clear();

    TBool match = MatchUidL(apps, aUid, aAppType);
    apps.clear();

    if (!match)
    {
        return;
    }

    if ((EMidp2Midlet == aAppType))
    {
        StorageEntryL(aUid, APPLICATION_TABLE, aAttributes);

        if (0 == aAttributes.size())
        {
            WLOG(EJavaStorage, "Entry does not exists");
            return;
        }

        // Get suite ID
        entry.setEntry(PACKAGE_ID, L"");

        wstring value = L"";
        EntryAttributeValue(aAttributes, PACKAGE_ID, value);
        Uid suiteUid(value);
        aUid = suiteUid;    // Used to get manifest attributes.

        // JavaRegistryEntry requires also attributes from suite.
        // So they must be read for appliction as well e.g. drive.
        ComplementaryInformationL(aUid, aAttributes);
    }
    // Already matched so it must be suite.
    else // EGeneralPackage.
    {
        StorageEntryL(aUid, APPLICATION_PACKAGE_TABLE, aAttributes);

        if (0 == aAttributes.size())
        {
            WLOG(EJavaStorage, "Entry does not exists");
        }
    }
}


// ---------------------------------------------------------------------------
// CJavaRegStore::PopulateAppTypeProperties
// ---------------------------------------------------------------------------
//
void CJavaRegStore::PopulateAppTypePropertiesL(
    const java::util::Uid& aUid,
    const Java::TJavaRegistryEntryType aAppType,
    CJavaPropertyArray*& aProperties) const
{

    if (aProperties)
    {
        // Add type that entry can be identified on client side.
        HBufC* intBuf = NULL;  // This is allocated at StoreTIntL
        JavaRegConverter::StoreTIntL(aAppType, intBuf);
        CleanupStack::PushL(intBuf);
        // This is deleted after response is sent to client.
        CJavaProperty* typeProp = CJavaProperty::NewL(EPropType, intBuf->Des());
        aProperties->Append(typeProp);
        CleanupStack::PopAndDestroy(intBuf);
    }
    else
    {
        return;
    }

    JavaStorageEntry entry;
    JavaStorageApplicationEntry_t attributes;
    JavaStorageApplicationList_t apps;

    if (EMidp2MidletSuite == aAppType)
    {
        // Get embedded entries
        entry.setEntry(ID, L"");
        attributes.insert(entry);
        entry.setEntry(PACKAGE_ID, aUid.toString());
        attributes.insert(entry);

        SearchL(APPLICATION_TABLE, attributes, apps);
        attributes.clear();
        RArray<TUid> uids;

        if (apps.size() != 0)
        {
            JavaStorageEntry attr;
            attr.setEntry(ID, L"");
            JavaStorageApplicationList_t::const_iterator appsIter;

            for (appsIter = apps.begin(); appsIter != apps.end(); appsIter++)
            {
                JavaStorageApplicationEntry_t::const_iterator entryIter
                = (*appsIter).find(attr);

                if (entryIter != (*appsIter).end())
                {
                    wstring attrValue = L"";
                    EntryAttributeValue((*appsIter), ID, attrValue);

                    if (attrValue.size() > 0)
                    {
                        java::util::Uid appUid(attrValue);
                        TUid converted;
                        TInt err = uidToTUid(appUid, converted);
                        if (KErrNone == err)
                        {
                            uids.AppendL(converted);
                        }
                        else
                        {
                            ELOG1(EJavaStorage, "Uid TUid conversion err: %d",
                                  err);
                        }
                    }
                }
            }
        }
        apps.clear();
        if (uids.Count() > 0)
        {
            HBufC* serUids = NULL;
            JavaRegUidArrayConverter::StoreTUidsL(uids, serUids);
            CleanupStack::PushL(serUids);
            CJavaProperty* uidsProp =
                CJavaProperty::NewL(EPropApplicationUids, serUids->Des());
            aProperties->Append(uidsProp);
            CleanupStack::PopAndDestroy(serUids);
        }
    }
    else if (EMidp2Midlet == aAppType)
    {
        HBufC* trueValue = NULL;
        JavaRegConverter::StoreTBoolL(ETrue, trueValue);
        CleanupStack::PushL(trueValue);

        CJavaProperty* startableProp
        = CJavaProperty::NewL(EPropStartable, trueValue->Des());
        aProperties->Append(startableProp);

        CJavaProperty* visibleProp
        = CJavaProperty::NewL(EPropVisible, trueValue->Des());
        aProperties->Append(visibleProp);

        CJavaProperty* residentProp
        = CJavaProperty::NewL(EPropResident, trueValue->Des());
        aProperties->Append(residentProp);
        CleanupStack::PopAndDestroy(trueValue);
    }
}

// ---------------------------------------------------------------------------
// CJavaRegStore::IsPresentL
// ---------------------------------------------------------------------------
//
TBool CJavaRegStore::IsPresentL(
    const JavaStorageApplicationEntry_t& aEntry) const
{
    wstring value = L"";
    EntryAttributeValue(aEntry, PACKAGE_ID, value);

    if (value.size() == 0)
    {
        EntryAttributeValue(aEntry, ID, value);
    }

    java::util::Uid uid(value);
    value.clear();

    JavaStorageApplicationEntry_t appEntry;
    StorageEntryL(uid, APPLICATION_PACKAGE_TABLE, appEntry);
    EntryAttributeValue(appEntry, MEDIA_ID, value);

    try
    {
        int sourceMediaId = JavaCommonUtils::wstringToInt(value);

        java::fileutils::driveInfos drives;
        java::fileutils::DriveUtilities::getAccesibleDrives(drives);

        java::fileutils::driveInfos::const_iterator iter;

        for (iter = drives.begin(); iter != drives.end(); iter++)
        {
            if ((*iter).iId == sourceMediaId)
            {
                return ETrue;
            }
        }
    }
    catch (ExceptionBase)
    {
        ELOG1WSTR(EJavaStorage, "MediaId conversion failed: '%s'", value);
    }

    return EFalse;
}
