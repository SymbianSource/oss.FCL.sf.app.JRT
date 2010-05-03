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
* Description:  writeablejavaregistrypackageentry implementation
*
*/


// INCLUDES

#include <memory>
#include <e32base.h>
#include <appversion.h>

#include "writeablejavaregistrypackageentry.h"
#include "securitystoragedatadefs.h"
#include "javacommonutils.h"
#include "javastoragenames.h"
#include "javasizehelpclient.h"
#include "javasymbianoslayer.h"
#include "logger.h"

using namespace Java::Manager::Registry;
using namespace java::storage;
using namespace java::util;
using namespace std;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CJavaRegistryPackageEntry::CJavaRegistryPackageEntry
// ---------------------------------------------------------------------------
//
CWriteableJavaRegistryPackageEntry::CWriteableJavaRegistryPackageEntry
(JavaStorageApplicationEntry_t& aEntry, TJavaRegistryEntryType aType) :
        CWriteableJavaRegistryEntry(aEntry , aType)
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::~CWriteableJavaRegistryPackageEntry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryPackageEntry::
~CWriteableJavaRegistryPackageEntry()
{
    if (iVendor)
    {
        delete iVendor;
        iVendor = NULL;
    }
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::Version
// ---------------------------------------------------------------------------
//
EXPORT_C TAppVersion CWriteableJavaRegistryPackageEntry::Version() const
{
    JELOG2(EJavaStorage);

    wstring value = L"";
    EntryAttributeValue(iEntry, VERSION, value);
    return wstringToAppVersion(value);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::Vendor
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CWriteableJavaRegistryPackageEntry::Vendor() const
{
    JELOG2(EJavaStorage);

    if (!iVendor)
    {
        wstring value = L"";
        EntryAttributeValue(iEntry, VENDOR, value);

        if (value == L"")
        {
            return KNullDesC;
        }
        else
        {
            iVendor = wstringToBuf(value);
        }
    }
    return *iVendor;
}


// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::IsUninstallable
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CWriteableJavaRegistryPackageEntry::IsUninstallable() const
{
    JELOG2(EJavaStorage);

    TBool result = ETrue;

    _LIT(KBlockUninstall, "Nokia-MIDlet-Block-Uninstall");
    const MJavaAttribute* blockUninstall = NULL;
    TRAPD(err, blockUninstall = AttributeL(KBlockUninstall));

    if (KErrNone != err)
    {
        ELOG1(EJavaStorage, "Cannot read Block uninstall attribute: %d", err);
        return ETrue;
    }

    _LIT(KValueTrue, "true");

    // If attribute is present and value is set to true.
    if (blockUninstall && blockUninstall->Value().CompareF(KValueTrue) == 0)
    {
        // Check it is authorized i.e. belongs to manufacturer or operator
        // domain
        wstring value = L"";
        EntryAttributeValue(iEntry, ID, value);

        java::util::Uid suiteUid(value);
        JavaStorageApplicationEntry_t suiteEntry;

        StorageEntry(suiteUid, MIDP_PACKAGE_TABLE, suiteEntry);
        value.clear();

        EntryAttributeValue(suiteEntry, SECURITY_DOMAIN_CATEGORY, value);

        if (value.size() > 0)
        {
            try
            {
                if (value == OPERATOR_DOMAIN_CATEGORY
                        || value == MANUFACTURER_DOMAIN_CATEGORY)

                {
                    result = EFalse;
                }
            }
            catch (ExceptionBase& e)
            {
                ELOG1WSTR(EJavaStorage,
                          "Security domain UTF-8 conversion failed: %s", value);
            }
        }
    }

    if (blockUninstall)
    {
        delete blockUninstall;
        blockUninstall = NULL;
    }

    return result;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::GetEmbeddedEntries
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryPackageEntry::
GetEmbeddedEntries(RArray<TUid>& aUids) const
{
    JELOG2(EJavaStorage);

    wstring value = L"";
    EntryAttributeValue(iEntry, ID, value);

    JavaStorageApplicationList_t apps;
    java::util::Uid uid(value);
    SuiteEntries(uid, apps);

    if (apps.size() != 0)
    {
        JavaStorageEntry attr;
        attr.setEntry(ID, L"");
        JavaStorageApplicationList_t::const_iterator appsIter;

        for (appsIter = apps.begin(); appsIter != apps.end(); ++appsIter)
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
                        aUids.Append(converted);
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
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::NumberOfEmbeddedEntries
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CWriteableJavaRegistryPackageEntry::
NumberOfEmbeddedEntries() const
{
    JELOG2(EJavaStorage);

    wstring value = L"";
    EntryAttributeValue(iEntry, ID, value);

    JavaStorageApplicationList_t apps;
    java::util::Uid uid(value);
    SuiteEntries(uid, apps);

    return apps.size();
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::EmbeddedEntryByUidL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryEntry*
CWriteableJavaRegistryPackageEntry::
EmbeddedEntryByUidL(const TUid& aEntryUid) const
{
    JELOG2(EJavaStorage);

    JavaStorageApplicationEntry_t attributes;
    java::util::Uid entryUid;

    StorageEntry(
        TUidToUid(aEntryUid, entryUid), APPLICATION_TABLE, attributes);

    if (0 == attributes.size())
    {
        WLOG(EJavaStorage, "Entry does not exists");
        return NULL;
    }

    return new CWriteableJavaRegistryEntry(attributes, EMidp2Midlet);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::EmbeddedEntryByNumberL
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryEntry*
CWriteableJavaRegistryPackageEntry::
EmbeddedEntryByNumberL(TInt aEntryNum) const
{
    JELOG2(EJavaStorage);

    if (aEntryNum < 0)
    {
        User::Leave(KErrArgument);
    }

    wstring value = L"";
    EntryAttributeValue(iEntry, ID, value);

    JavaStorageApplicationList_t apps;
    java::util::Uid uid(value);
    SuiteEntries(uid, apps);

    // aEntryNum is zero based whereas apps is one based.
    if (aEntryNum >= apps.size())
    {
        return NULL;
    }

    JavaStorageApplicationList_t::const_iterator iter = apps.begin();

    for (int i = 0; i < aEntryNum; i++)
    {
        ++iter;
    }

    value.clear();

    EntryAttributeValue((*iter), ID, value);

    java::util::Uid appUid(value);
    JavaStorageApplicationEntry_t attributes;

    StorageEntry(appUid, APPLICATION_TABLE, attributes);

    if (attributes.size() == 0)
    {
        ELOG1WSTR(EJavaStorage,
                  "No attributes for embedded entry: %s",
                  appUid.toString());
        return NULL;
    }
    else
    {
        return new CWriteableJavaRegistryEntry(attributes, EMidp2Midlet);
    }
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::AttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C const MJavaAttribute* CWriteableJavaRegistryPackageEntry::
AttributeL(const TDesC& aName) const
{
    JELOG2(EJavaStorage);
    return CWriteableJavaRegistryEntry::AttributeL(aName);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::AttributesL
// ---------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<MJavaAttribute>&
CWriteableJavaRegistryPackageEntry::AttributesL() const
{
    JELOG2(EJavaStorage);
    return CWriteableJavaRegistryEntry::AttributesL();
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::UsedUserDiskSpace
// ---------------------------------------------------------------------------
EXPORT_C TInt64 CWriteableJavaRegistryPackageEntry::UsedUserDiskSpace() const
{
    JELOG(EJavaStorage, "CJavaRegistryPackageEntry::UsedUserDiskSpace()");

    wstring rootPath = L"";
    EntryAttributeValue(iEntry, ROOT_PATH, rootPath);

    auto_ptr<HBufC> suitePath(HBufC16::New(rootPath.size() + 1));

    if (!suitePath.get())
    {
        ELOG(EJavaStorage, "Cannot allocate suite path.");
        return 0;
    }

    TPtr16 suitePathPtr(suitePath->Des());
    suitePathPtr = (const TUint16*)rootPath.c_str();

    RJavaSizeHelpClient session;
    TInt err = session.Connect();

    TInt r = 0;

    if (KErrNone == err)
    {
        r = session.GetUserUsedDiskSpace(suitePathPtr);
        session.Close();
    }
    wstring jarPath = L"";
    EntryAttributeValue(iEntry, JAR_PATH, jarPath);

    wstring::size_type startIdx = 0;

    if (jarPath.find(rootPath, startIdx) == string::npos)
    {
        // As ROOT_PATH is not parth of the JAR_PATH this application suite
        // is preinstalled and its JAR size is added to ROOT_PATH count.
        wstring value = L"";
        EntryAttributeValue(iEntry, INITIAL_SIZE, value);
        r += JavaCommonUtils::wstringToInt(value);
    }
    return r;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::IsPreinstalled
// ---------------------------------------------------------------------------
EXPORT_C TBool CWriteableJavaRegistryPackageEntry::IsPreinstalled() const
{
    JELOG2(EJavaStorage);
    TBool isPreinstalled = EFalse;
    wstring name = L"";
    wstring vendor = L"";

    EntryAttributeValue(iEntry, PACKAGE_NAME, name);
    EntryAttributeValue(iEntry, VENDOR, vendor);

    JavaStorageApplicationList_t apps;
    JavaStorageApplicationEntry_t searchQuery;
    JavaStorageEntry attr;
    attr.setEntry(NAME, name);
    searchQuery.insert(attr);
    attr.setEntry(VENDOR, vendor);
    searchQuery.insert(attr);

    try
    {
        auto_ptr<JavaStorage> js(JavaStorage::createInstance());
        js->open();
        js->search(PREINSTALL_TABLE, searchQuery, apps);
        js->close();
        js.reset(0);
    }
    catch (JavaStorageException& jse)
    {
        ELOG1(EJavaStorage, "IsPreinstalled: Search failed: %s ",
              jse.toString().c_str());
    }
    searchQuery.clear();

    if (apps.size() > 0)
    {
        wstring installState = L"";
        EntryAttributeValue(apps.front(), INSTALL_STATE, installState);

        // INSTALL_STATE 1 indicates preinstallation.
        if (installState == L"1")
        {
            isPreinstalled = ETrue;
        }
    }

    return isPreinstalled;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryPackageEntry::SuiteEntries
// ---------------------------------------------------------------------------
//
void CWriteableJavaRegistryPackageEntry::SuiteEntries(
    const java::util::Uid& aUid,
    JavaStorageApplicationList_t& aApps) const
{
    JavaStorageApplicationEntry_t searchQuery;
    JavaStorageEntry attr;
    attr.setEntry(PACKAGE_ID, aUid.toString());
    searchQuery.insert(attr);
    attr.setEntry(ID, L"");
    searchQuery.insert(attr);

    try
    {
        auto_ptr<JavaStorage> js(JavaStorage::createInstance());
        js->open();
        js->search(APPLICATION_TABLE, searchQuery, aApps);
        js->close();
        js.reset(0);
    }
    catch (JavaStorageException& jse)
    {
        ELOG1(EJavaStorage, "Search failed: %s ",
              jse.toString().c_str());
    }
    searchQuery.clear();
}
