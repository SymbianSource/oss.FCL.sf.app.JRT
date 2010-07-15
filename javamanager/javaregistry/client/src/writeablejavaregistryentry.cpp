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
* Description:  writeablejavaregistryentry implementation
*
*/


// INCLUDES

#include <memory>

#include "javacommonutils.h"
#include "javaregistryentry.h"
#include "javastoragenames.h"
#include "javasymbianoslayer.h"
#include "javauid.h"
#include "logger.h"
#include "midpauthenticationmodule.h"
#include "writeablejavaregistryentry.h"
#include "writeablejavaregistrypackageentry.h"

using namespace Java;
using namespace Java::Manager::Registry;
using namespace java::storage;
using namespace java::util;
using namespace std;

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::CWriteableJavaRegistryEntry
// ---------------------------------------------------------------------------
//
CWriteableJavaRegistryEntry::CWriteableJavaRegistryEntry
(JavaStorageApplicationEntry_t& aEntry, TJavaRegistryEntryType aType) :
        iEntry(aEntry), iType(aType)
{
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::~CWriteableJavaRegistryEntry
// ---------------------------------------------------------------------------
//
EXPORT_C CWriteableJavaRegistryEntry::~CWriteableJavaRegistryEntry()
{
    iAttributes.ResetAndDestroy();
    iCertificates.ResetAndDestroy();

    if (iName)
    {
        delete iName;
        iName = NULL;
    }

    if (iDrmContentId)
    {
        delete iDrmContentId;
        iDrmContentId = NULL;
    }
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::Uid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CWriteableJavaRegistryEntry::Uid() const
{
    JELOG2(EJavaStorage);
    TUid uid;
    wstring value;
    EntryAttributeValue(iEntry, ID, value);
    if (value.size() > 0)
    {
        java::util::Uid omjUid(value);
        TInt err = uidToTUid(omjUid, uid);
        if (KErrNone != err)
        {
            ELOG1(EJavaStorage, "Uid to TUid conversion failed: %d", err);
            uid = TUid::Null();
        }
    }
    else
    {
        uid = TUid::Null();
    }
    return uid;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::Name
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CWriteableJavaRegistryEntry::Name() const
{
    JELOG2(EJavaStorage);

    if (!iName)
    {
        wstring value = L"";

        if (EMidp2MidletSuite == iType)
        {
            EntryAttributeValue(iEntry, PACKAGE_NAME, value);
        }
        else  // Application
        {
            EntryAttributeValue(iEntry, NAME, value);
        }

        if (value.size() > 0)
        {
            iName = wstringToBuf(value);
        }
        else
        {
            return KNullDesC;
        }
    }
    return *iName;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::Type
// ---------------------------------------------------------------------------
//
EXPORT_C Java::TJavaRegistryEntryType CWriteableJavaRegistryEntry::
Type() const
{
    return iType;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::Drive
// ---------------------------------------------------------------------------
//
EXPORT_C TDriveNumber CWriteableJavaRegistryEntry::Drive() const
{
    JELOG2(EJavaStorage);

    wstring value = L"";

    if (EMidp2MidletSuite == iType)
    {
        EntryAttributeValue(iEntry, ROOT_PATH, value);
    }
    else  // Application
    {
        EntryAttributeValue(iEntry, PACKAGE_ID, value);

        java::util::Uid suiteUid(value);
        JavaStorageApplicationEntry_t suiteEntry;

        StorageEntry(suiteUid, APPLICATION_PACKAGE_TABLE, suiteEntry);
        EntryAttributeValue(suiteEntry, ROOT_PATH, value);

        suiteEntry.clear();
    }

    // Original implementation returns EDriveC in a case of failure.
    TInt driveNumber = EDriveC;

    if (value.size() > 0)
    {
        wstring::size_type start = 0;
        wstring::size_type end = 1;

        char* driveLetter = 0;

        try
        {
            driveLetter =  JavaCommonUtils::wstringToUtf8(
                               value.substr(start, end));

            TInt err = RFs::CharToDrive(driveLetter[0], driveNumber);

            if (KErrNone != err)
            {
                WLOG2(EJavaStorage,
                      "Drive number conversion fault: %d. Letter: %c",
                      err, driveLetter[0]);
            }

            delete[] driveLetter;
        }
        catch (ExceptionBase& e)
        {
            ELOG1WSTR(
                EJavaStorage,
                "Drive letter UTF-8 conversion failed for string: %s",
                value.substr(start, end));
        }
    }

    return (TDriveNumber) driveNumber;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::NumberOfCertificateChains
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CWriteableJavaRegistryEntry::NumberOfCertificateChains() const
{
    JELOG2(EJavaStorage);

    if (iCertificates.Count() == 0)
    {
        PopulateCertChains();
    }

    return iCertificates.Count();
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::CertificateChain
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CWriteableJavaRegistryEntry::
CertificateChain(TInt aIndex) const
{
    JELOG2(EJavaStorage);

    if (iCertificates.Count() == 0)
    {
        PopulateCertChains();
    }

    if (aIndex < 0 || iCertificates.Count() <= aIndex)
    {
        return KNullDesC8;
    }
    return *(iCertificates[aIndex]);
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::DrmContentId
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CWriteableJavaRegistryEntry::DrmContentId() const
{
    JELOG2(EJavaStorage);

    if (!iDrmContentId)
    {
        wstring value = L"";

        if (EMidp2MidletSuite == iType)
        {
            EntryAttributeValue(iEntry, CONTENT_ID, value);
        }
        else  // Application
        {
            EntryAttributeValue(iEntry, PACKAGE_ID, value);
            java::util::Uid suiteUid(value);
            JavaStorageApplicationEntry_t suiteEntry;

            StorageEntry(suiteUid, APPLICATION_PACKAGE_TABLE, suiteEntry);
            EntryAttributeValue(suiteEntry, CONTENT_ID, value);
            suiteEntry.clear();
        }

        if (value.size() > 0)
        {
            iDrmContentId = wstringToBuf(value);
        }
        else
        {
            return KNullDesC;
        }
    }
    return *iDrmContentId;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::AttributeL
// ---------------------------------------------------------------------------
//
EXPORT_C const MJavaAttribute* CWriteableJavaRegistryEntry::
AttributeL(const TDesC& aName) const
{
    JELOG2(EJavaStorage);

    if (0 == aName.Size())
    {
        ELOG(EJavaStorage, "Empty attribute name");
        User::Leave(KErrArgument);
    }

    // If attributes are not already read. These are manifest attributes
    // and they are not updated unless application suite is re-installed.
    if (iAttributes.Count() == 0)
    {
        AttributesL();
    }


    for (TInt i = 0; i < iAttributes.Count(); i++)
    {
        if (0 == (iAttributes[i])->Name().Compare(aName))
        {
            return iAttributes[i];
        }
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::AttributesL
// ---------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<MJavaAttribute>&
CWriteableJavaRegistryEntry::AttributesL() const
{
    JELOG2(EJavaStorage);

    // If attributes are already read. These are manifest attributes and they
    // are not updated unless application suite is reinstalled.
    if (iAttributes.Count() > 0)
    {

        return iAttributes;
    }

    wstring value = L"";
    JavaStorageApplicationList_t attributeList;

    if (EMidp2MidletSuite == iType)
    {
        EntryAttributeValue(iEntry, ID, value);
    }
    else  // Application
    {
        // Get suiteID where application belongs to.
        EntryAttributeValue(iEntry, PACKAGE_ID, value);
    }

    ReadAttributesL(value, attributeList);

    if (attributeList.size() != 0)
    {
        // Populate attributes
        JavaStorageApplicationList_t::const_iterator attrIter;
        JavaStorageEntry attr;
        CJavaAttribute* regAttr = NULL;

        wstring attrName = L"";
        wstring attrValue = L"";
        wstring attrTrusted = L"";

        for (attrIter = attributeList.begin();
                attrIter != attributeList.end();
                ++attrIter)
        {
            EntryAttributeValue((*attrIter), NAME, attrName);
            EntryAttributeValue((*attrIter), VALUE, attrValue);
            EntryAttributeValue((*attrIter), TRUSTED, attrTrusted);

            if (attrValue.size() > 0 && attrName.size() > 0)
            {
                auto_ptr<HBufC> nameBuf(wstringToBuf(attrName));
                auto_ptr<HBufC> valueBuf(wstringToBuf(attrValue));

                TBool trusted = EFalse;
                if (attrTrusted == L"1")
                {
                    trusted = ETrue;
                }

                regAttr = CJavaAttribute::NewL(
                              nameBuf->Des(), valueBuf->Des(), trusted);

                iAttributes.Append(regAttr);

                nameBuf.reset(0);
                valueBuf.reset(0);
                attrName.clear();
                attrValue.clear();
                attrTrusted.clear();
            }
            // else CJavaAttribute cannot be constructed with empty values.
            // Using same logic it does not exists if value is not set.
        }
    }
    return iAttributes;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CWriteableJavaRegistryEntry::
ConstructL(Java::TJavaRegistryEntryType aType)
{
    JELOG2(EJavaStorage);
    iType = aType;
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::ReadAttributesL
// ---------------------------------------------------------------------------
//
void CWriteableJavaRegistryEntry::ReadAttributesL(const wstring& aUid,
        JavaStorageApplicationList_t& aAttributes) const
{
    JavaStorageApplicationEntry_t searchQuery;
    JavaStorageEntry attr;

    attr.setEntry(ID, aUid);
    searchQuery.insert(attr);

    try
    {
        auto_ptr<JavaStorage> js(JavaStorage::createInstance());
        js->open();
        js->search(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, searchQuery, aAttributes);
        js->close();
        js.reset(0);
        searchQuery.clear();
    }
    catch (JavaStorageException& aJse)
    {
        ELOG1(EJavaStorage, "Attributes read failed: %d", aJse.mStatus);
        User::Leave(aJse.mStatus);
    }
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::EntryAttributeValue
// ---------------------------------------------------------------------------
//
void CWriteableJavaRegistryEntry::EntryAttributeValue(
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
// CWriteableJavaRegistryEntry::StorageEntry
// ---------------------------------------------------------------------------
//
void CWriteableJavaRegistryEntry::StorageEntry(
    const java::util::Uid& aUid,
    const string& aTableName,
    JavaStorageApplicationEntry_t& aStorageEntry) const
{
    try
    {
        auto_ptr<JavaStorage> js(JavaStorage::createInstance());
        js->open();
        js->read(aTableName, aUid, aStorageEntry);
        js->close();
        js.reset(0);
    }
    catch (JavaStorageException& jse)
    {
        ELOG1(EJavaStorage, "Read failed: %s ",
              jse.toString().c_str());
    }
}

// ---------------------------------------------------------------------------
// CWriteableJavaRegistryEntry::PopulateCertChains
// ---------------------------------------------------------------------------
//
void CWriteableJavaRegistryEntry::PopulateCertChains() const
{
    auto_ptr<java::security::MIDPAuthenticationModule>authModule(
        java::security::MIDPAuthenticationModule::createInstance());

    wstring value = L"";

    if (EMidp2MidletSuite == iType)
    {
        EntryAttributeValue(iEntry, ID, value);
    }
    else  // Application
    {
        EntryAttributeValue(iEntry, PACKAGE_ID, value);
    }

    java::util::Uid suiteUid(value);
    std::list<std::string> chains;

    authModule->getCertChains(suiteUid, chains);

    list<string>::const_iterator iter;

    for (iter = chains.begin(); iter != chains.end(); ++iter)
    {
        if ((*iter).size() > 0)
        {
            string decoded = (*iter);

            HBufC8* decodedBuf = HBufC8::New(decoded.size());
            TPtr8 decodedBufPtr(decodedBuf->Des());
            decodedBufPtr.Append(
                ((const TUint8*)decoded.c_str()), decoded.size());
            iCertificates.Append(decodedBuf);
        }
    }
    chains.clear();
}
