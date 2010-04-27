/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#include <centralrepository.h>
#include "storagehandler.h"
#include "javastoragenames.h"
#include "javastorage.h"
#include "javastorageexception.h"
#include "javasymbianoslayer.h"
#include "javauid.h"
#include <d32dbms.h>
#include "logger.h"
#include "j2me/midp2/security/MFunctionGroupBinding.h"
#include "j2me/midp2/security/MExtendedProtectionDomain.h"
#include "j2me/midp2/security/MFunctionGroup.h"
#include "j2me/midp2/security/MPermission.h"
#include "midletlist.h"
#include "connectionmanager.h"

_LIT(KUserDatabaseName,     "c:MIDP2SystemAMSUserV2");
_LIT(KMSId,             "MSId");
// security preferences table
_LIT(KSecurityPreferencesIndexName,        "UserMSSecurityPreferencesIndex");
_LIT(KSecurityPreferencesTableName,        "UserMSSecurityPreferences");
_LIT(KSecureUid,        "secure[102045FE]");
_LIT(KMSName,           "MSName");
_LIT(KMSVendor,         "MSVendor");
_LIT(KMSVersion,        "MSVersion");
_LIT(KMSDomain,         "ProtectionDomain");
_LIT(KMSSecurityPrefs,  "MSSecurityPrefs");
// custom attributes table
_LIT(KCustomAttributesTableName,    "MIDletProperties");
_LIT(KCustomAttribsValue,            "Value");

const TInt KSecurityPolicyFlags = 0x0200;
_LIT(KSecurityPolicy, "midp2_rp");

// This is in the private data cage of javausersettingsconfigurator
_LIT(KMidletImportDirectory, "C:\\private\\20022E7A\\data\\");

// Java access point CenRep Id
const TInt KJavaAPNRepositoryUid = 0x10206EC5;

// CenRep indicator to use the Java default APN
const TInt KUseJavaDefaultAPN = -2;

// CenRep key for Java default APN key-value pair
const TUint32 KJavaDefaultAPNKey = KMaxTUint32;

using namespace java::tools::usersettingsconfigurator;
using namespace java::storage;
using namespace java::util;
using namespace MIDP;

IMPORT_C MSecurityPolicyV2* GetSecurityPolicyL(TUint32 aFlags, const TDesC& aPolicyName);

StorageHandler* StorageHandler::NewL()
{
    StorageHandler* self = new(ELeave) StorageHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

StorageHandler::StorageHandler()
{
}

void StorageHandler::ConstructL()
{
    User::LeaveIfError(iDbs.Connect());
    User::LeaveIfError(iUserDb.Open(iDbs, KUserDatabaseName(), KSecureUid()));
    User::LeaveIfError(iSecurityPreferencesTable.Open(iUserDb, KSecurityPreferencesTableName));
    User::LeaveIfError(iSecurityPreferencesTable.SetIndex(KSecurityPreferencesIndexName));
    User::LeaveIfError(iCustomAttributesTable.Open(iUserDb, KCustomAttributesTableName));
    // security preferences column identifiers
    CDbColSet* colSet = iSecurityPreferencesTable.ColSetL();
    iSPNameColId            = colSet->ColNo(KMSName);
    iSPVendorColId          = colSet->ColNo(KMSVendor);
    iSPVersionColId         = colSet->ColNo(KMSVersion);
    iSPDomainColId          = colSet->ColNo(KMSDomain);
    iSPPreferencesColId     = colSet->ColNo(KMSSecurityPrefs);
    iSPIdColId              = colSet->ColNo(KMSId);
    // custom attributes column identifiers
    colSet = iCustomAttributesTable.ColSetL();
    iCAIdColId              = colSet->ColNo(KMSId);
    iCAValueColId           = colSet->ColNo(KCustomAttribsValue);
    delete colSet;
    try
    {
        iStorage = JavaStorage::createInstance();
        iStorage->open();
    }
    catch (JavaStorageException& aJse)
    {
        User::Leave(aJse.mStatus);
    }
    iSecurityPolicy = GetSecurityPolicyL(KSecurityPolicyFlags, KSecurityPolicy);
}

void StorageHandler::ReadSecuritySettingsL(std::vector<MidletSuiteSecuritySettings*>& aMidletSuitesSecuritySettings)
{
    TUint32 cnt = iSecurityPreferencesTable.CountL();
    iSecurityPreferencesTable.BeginningL();
    for (int i=0; i<cnt; i++)
    {
        iSecurityPreferencesTable.NextL();
        iSecurityPreferencesTable.GetL();

        // alloc the buffers
        HBufC* name = HBufC::NewLC(iSecurityPreferencesTable.ColLength(iSPNameColId));
        TPtr namePtr = name->Des();
        HBufC* vendor = HBufC::NewLC(iSecurityPreferencesTable.ColLength(iSPVendorColId));
        TPtr vendorPtr = vendor->Des();
        HBufC* version = HBufC::NewLC(iSecurityPreferencesTable.ColLength(iSPVersionColId));
        TPtr versionPtr = version->Des();
        HBufC* domain = HBufC::NewLC(iSecurityPreferencesTable.ColLength(iSPDomainColId));
        TPtr domainPtr = domain->Des();
        HBufC8* securityPrefs = HBufC8::NewLC(iSecurityPreferencesTable.ColLength(iSPPreferencesColId));
        TPtr8 securityPrefsPtr = securityPrefs->Des();
        TUint32 suiteUid = iSecurityPreferencesTable.ColUint32(iSPIdColId);

        // fill in the buffers
        RDbColReadStream in;
        in.OpenL(iSecurityPreferencesTable, iSPNameColId);
        in.ReadL(namePtr,  iSecurityPreferencesTable.ColLength(iSPNameColId));
        in.Close();
        in.OpenL(iSecurityPreferencesTable, iSPVendorColId);
        in.ReadL(vendorPtr,  iSecurityPreferencesTable.ColLength(iSPVendorColId));
        in.Close();
        in.OpenL(iSecurityPreferencesTable, iSPVersionColId);
        in.ReadL(versionPtr,  iSecurityPreferencesTable.ColLength(iSPVersionColId));
        in.Close();
        in.OpenL(iSecurityPreferencesTable, iSPDomainColId);
        in.ReadL(domainPtr,  iSecurityPreferencesTable.ColLength(iSPDomainColId));
        in.Close();
        in.OpenL(iSecurityPreferencesTable, iSPPreferencesColId);
        in.ReadL(securityPrefsPtr,  iSecurityPreferencesTable.ColLength(iSPPreferencesColId));
        in.Close();

        // process the buffers
        std::wstring suiteName((wchar_t*) name->Ptr(), name->Length());
        std::wstring suiteVendor((wchar_t*) vendor->Ptr(), vendor->Length());
        std::wstring suiteVersion((wchar_t*) version->Ptr(), version->Length());
        std::string prefs((char *) securityPrefs->Ptr(), securityPrefs->Length());
        std::wstring securitySettingsName = L"";
        std::wstring currentInteractionMode = L"";
        std::vector<SecuritySettings*> securitySettings;
        DecodeSecuritySettingsL(domainPtr, std::string((char *) securityPrefs->Ptr(), securityPrefs->Length()), securitySettings);
        if (securitySettings.size() > 0)
        {
            MidletSuiteSecuritySettings* suiteSecuritySettings =
                new MidletSuiteSecuritySettings(suiteUid, suiteName, suiteVendor, suiteVersion);
            suiteSecuritySettings->iSecuritySettings = securitySettings;
            aMidletSuitesSecuritySettings.push_back(suiteSecuritySettings);
        }

        // free the buffers
        CleanupStack::PopAndDestroy(securityPrefs);
        CleanupStack::PopAndDestroy(domain);
        CleanupStack::PopAndDestroy(version);
        CleanupStack::PopAndDestroy(vendor);
        CleanupStack::PopAndDestroy(name);
    }
}

void StorageHandler::WriteSecuritySettingsL(const std::vector<MidletSuiteSecuritySettings*>& aMidletSuitesSecuritySettings)
{
    for (int i=0; i<aMidletSuitesSecuritySettings.size(); i++)
    {
        try
        {
            // get the UID of the suite
            JavaStorageApplicationEntry_t query;
            JavaStorageApplicationList_t queryResult;
            JavaStorageEntry attr;
            std::wstring midletSuiteUid = L"";
            attr.setEntry(ID, L"");
            query.insert(attr);
            attr.setEntry(PACKAGE_NAME, aMidletSuitesSecuritySettings[i]->iName);
            query.insert(attr);
            attr.setEntry(VENDOR, aMidletSuitesSecuritySettings[i]->iVendor);
            query.insert(attr);
            attr.setEntry(VERSION, aMidletSuitesSecuritySettings[i]->iVersion);
            query.insert(attr);
            iStorage->search(APPLICATION_PACKAGE_TABLE, query, queryResult);
            findEntry(queryResult, ID, midletSuiteUid);
            if (midletSuiteUid.size() > 0)
            {
                // write the currentInteractionMode
                for (int j=0; j<aMidletSuitesSecuritySettings[i]->iSecuritySettings.size(); j++)
                {
                    // check that the function group exists and the current
                    // interaction mode is found among the allowed interaction modes
                    JavaStorageApplicationEntry_t fgQuery;
                    JavaStorageApplicationList_t fgQueryResult;
                    attr.setEntry(ID, midletSuiteUid);
                    fgQuery.insert(attr);
                    attr.setEntry(FUNCTION_GROUP, aMidletSuitesSecuritySettings[i]->iSecuritySettings[j]->iName);
                    fgQuery.insert(attr);
                    attr.setEntry(ALLOWED_SETTINGS, L"");
                    fgQuery.insert(attr);
                    std::wstring allowedInteractionModes = L"";
                    iStorage->search(MIDP_FUNC_GRP_SETTINGS_TABLE, fgQuery, fgQueryResult);
                    findEntry(fgQueryResult, ALLOWED_SETTINGS, allowedInteractionModes);
                    if (IsInteractionModeAllowed(aMidletSuitesSecuritySettings[i]->iSecuritySettings[j]->iCurrentInteractionMode,
                                                 allowedInteractionModes))
                    {
                        JavaStorageApplicationEntry_t newEntry;
                        JavaStorageApplicationEntry_t oldEntry;
                        attr.setEntry(ID, midletSuiteUid);
                        newEntry.insert(attr);
                        oldEntry.insert(attr);
                        attr.setEntry(FUNCTION_GROUP, aMidletSuitesSecuritySettings[i]->iSecuritySettings[j]->iName);
                        newEntry.insert(attr);
                        oldEntry.insert(attr);
                        attr.setEntry(CURRENT_SETTING, aMidletSuitesSecuritySettings[i]->iSecuritySettings[j]->iCurrentInteractionMode);
                        newEntry.insert(attr);
                        iStorage->update(MIDP_FUNC_GRP_SETTINGS_TABLE, newEntry, oldEntry);
                    }
                }
            }
        }
        catch (JavaStorageException& aJse)
        {
            ELOG1(EJavaConverters, "Can not write security settings to storage: %s", aJse.toString().c_str());
        }
    }
}

void StorageHandler::ReadCustomAttributesL(std::vector<MidletSuiteCustomAttributes*>& aMidletSuitesCustomAttributes)
{
    TUint32 cnt = iCustomAttributesTable.CountL();
    iCustomAttributesTable.BeginningL();
    for (int i=0; i<cnt; i++)
    {
        iCustomAttributesTable.NextL();
        iCustomAttributesTable.GetL();
        TUint32 suiteUid = iCustomAttributesTable.ColUint32(iCAIdColId);
        HBufC8* customAttribs = HBufC8::NewLC(iCustomAttributesTable.ColLength(iCAValueColId));
        TPtr8 customAttribsPtr = customAttribs->Des();
        RDbColReadStream in;
        in.OpenL(iCustomAttributesTable, iCAValueColId);
        in.ReadL(customAttribsPtr,  iCustomAttributesTable.ColLength(iCAValueColId));
        in.Close();

        // the custom attributes are stored in the following form:
        //       attrib_count attrib1_name_length attrib1_name attrib1_value_length attrib1_value ...
        // where attrib_count is 4 bytes
        //       attribX_name_length is 4 bytes
        //       attribX_name is multiple of 4
        //       attribX_value_length is 4 bytes
        //       attribX_value is multiple of 4 with first 4 bytes as a flag applicable to the value
        //                     and the rest of the bytes the actual value
        const TUint8* ptr     = customAttribs->Ptr() + 4;
        TUint32 attrib_count =  *((TUint32*)customAttribs->Ptr());

        for (TUint32 i = 0; i < attrib_count; i++)
        {
            TPtrC attrib_name;
            TPtrC attrib_value;

            // read the attrib_name_length and move the pointer further
            TUint32 attrib_name_length     = *((TUint32*)ptr);
            ptr += 4;
            // read the attrib_name and move the pointer further
            attrib_name.Set((const TUint16*)ptr, attrib_name_length);
            // skip everything until we hit the multiple of 4 bytes
            ptr += ((2 * attrib_name_length + (4 - ((2 * attrib_name_length) % 4))));

            // read the attrib_value_length and move the pointer further
            TUint32 attrib_value_length     = *((TUint32*)ptr);
            // skip the 8 bytes (4 bytes for the length and another 4 bytes for the the flag associated with the value)
            ptr += 8;
            // read the attrib_value and move the pointer further
            attrib_value.Set((const TUint16*)ptr, ((2*attrib_value_length) - 4)/2);
            // skip everything until we hit the multiple of 4 bytes
            ptr += ((2 * attrib_value_length + (4 - ((2 * attrib_value_length) % 4))));

            // map the value to the value known by omj
            CustomAttribute* customAttribute = NULL;
            if (attrib_value.Compare(_L("gameactions")) == 0)
            {
                customAttribute = new CustomAttribute(
                    std::wstring((wchar_t*)attrib_name.Ptr(),
                                 attrib_name.Length()),
                    L"1");
            }
            else if (attrib_value.Compare(_L("no")) == 0)
            {
                customAttribute = new CustomAttribute(
                    std::wstring((wchar_t*)attrib_name.Ptr(),
                                 attrib_name.Length()),
                    L"0");
            }
            else if (attrib_value.Compare(_L("navigationkeys")) == 0)
            {
                customAttribute = new CustomAttribute(
                    std::wstring((wchar_t*)attrib_name.Ptr(),
                                 attrib_name.Length()),
                    L"2");
            }
            else
            {
                // unknown value -> ignore
                continue;
            }

            std::vector<CustomAttribute*> customAttributes;
            customAttributes.push_back(customAttribute);
            MidletSuiteCustomAttributes* suiteCustomAttributes =
                new MidletSuiteCustomAttributes(suiteUid);
            suiteCustomAttributes->iCustomAttributes = customAttributes;
            aMidletSuitesCustomAttributes.push_back(suiteCustomAttributes);
        }

        CleanupStack::PopAndDestroy(customAttribs);
    }
}

void StorageHandler::WriteCustomAttributesL(const std::vector<MidletSuiteCustomAttributes*>& aMidletSuitesCustomAttributes)
{
    for (int i=0; i<aMidletSuitesCustomAttributes.size(); i++)
    {
        for (int j=0; j<aMidletSuitesCustomAttributes[i]->iCustomAttributes.size(); j++)
        {
            try
            {
                if (aMidletSuitesCustomAttributes[i]->iCustomAttributes[j]->iName
                        != L"Nokia-MIDlet-On-Screen-Keypad")
                {
                    continue;
                }
                // try to update the value
                TUid tuid;
                tuid.iUid = aMidletSuitesCustomAttributes[i]->iUid;
                Uid uid;
                TUidToUid(tuid, uid);
                JavaStorageApplicationEntry_t oldEntry;
                JavaStorageEntry attr;
                attr.setEntry(ID, uid.toString());
                oldEntry.insert(attr);
                JavaStorageApplicationEntry_t entry;
                attr.setEntry(ON_SCREEN_KEYPAD, aMidletSuitesCustomAttributes[i]->iCustomAttributes[j]->iValue);
                entry.insert(attr);
                iStorage->update(MIDP_PACKAGE_TABLE, entry, oldEntry);
            }
            catch (JavaStorageException& aJse)
            {
                ELOG1(EJavaConverters, "Can not write custom attributes to storage: %s", aJse.toString().c_str());
            }
        }
    }
}

void StorageHandler::readMidletSuitesPropertiesL(std::vector<MidletSuiteSecuritySettings*>& aMidletSuitesSecuritySettings, std::vector<MidletSuiteCustomAttributes*>& aMidletSuitesCustomAttributes)
{
    // read security settings
    ReadSecuritySettingsL(aMidletSuitesSecuritySettings);

    // read custom attributes
    ReadCustomAttributesL(aMidletSuitesCustomAttributes);
}

void StorageHandler::writeMidletSuitePropertiesL(const std::vector<MidletSuiteSecuritySettings*>& aMidletSuitesSecuritySettings, const std::vector<MidletSuiteCustomAttributes*>& aMidletSuitesCustomAttributes)
{
    // write security settings
    WriteSecuritySettingsL(aMidletSuitesSecuritySettings);

    // write custom attributes
    WriteCustomAttributesL(aMidletSuitesCustomAttributes);
}

void StorageHandler::convertAPNSettingsL() const
{
    TFileName dataFile(KMidletImportDirectory);
    dataFile.Append(KMidletExportDataFileName);
    TUint attributes;
    RFs fs;
    TInt err = fs.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters, "Cannot open RFs connection, err %d ", err);
        return;
    }

    err = fs.Att(dataFile, attributes);
    if (KErrNone != err)
    {
        ELOG1(EJavaConverters, "Cannot handle imported file: %d ", err);
        fs.Close();
        return;
    }

    TFileName importDirectory(KMidletImportDirectory);

    CMidletList* midlets = new(ELeave) CMidletList();
    TRAP(err, midlets->ImportListL(fs, importDirectory));

    if (err == KErrNone)
    {
        TUid repUid = TUid::Uid(KJavaAPNRepositoryUid);
        std::auto_ptr<CRepository> rep(CRepository::NewL(repUid));

        JavaStorageApplicationEntry_t matchEntry;
        JavaStorageApplicationEntry_t updateEntry;
        JavaStorageEntry attr;

        for (TInt i = 0; i < midlets->Count(); i++)
        {
            const CMidletInfo* oneApp = midlets->GetNext();
            TInt apnId = -1;
            TInt err = rep->Get(oneApp->GetSuiteId(), apnId);

            if (KErrNone != err)
            {
                continue;
            }

            if (KUseJavaDefaultAPN == apnId)
            {
                rep->Get(KJavaDefaultAPNKey, apnId);
            }

            matchEntry.clear();
            updateEntry.clear();

            try
            {
                java::util::Uid appUid;
                JavaStorageApplicationEntry_t foundEntry;

                iStorage->read(APPLICATION_TABLE,
                               TUidToUid(oneApp->GetMidletUid(), appUid), foundEntry);

                attr.setEntry(PACKAGE_ID, L"");
                JavaStorageApplicationEntry_t::const_iterator finder
                = foundEntry.find(attr);

                java::util::Uid suiteUid((*finder).entryValue());
                ConnectionManager::setApnIdL(suiteUid, apnId);
            }
            catch (JavaStorageException& jse)
            {
                ELOG2(EJavaConverters,
                      "Cannot write APN Id: %d of App: %d",
                      apnId,
                      oneApp->GetMidletUid().iUid);
            }
        }
    }

    midlets->ResetAndDestroy();
    fs.Delete(dataFile);
    fs.Close();

    LOG(EJavaConverters, EInfo, "APNs converted");
}

StorageHandler::~StorageHandler()
{
    iSecurityPreferencesTable.Close();
    iCustomAttributesTable.Close();
    iUserDb.Close();
    iDbs.Close();
    if (iStorage)
    {
        iStorage->close();
        delete iStorage;
        iStorage = NULL;
    }
    delete iSecurityPolicy;
}

void StorageHandler::DecodeSecuritySettingsL(const TDesC& aProtectionDomain, const std::string& aEncodedSecuritySettings, std::vector<SecuritySettings*>& aSecuritySettings)
{
    int BITS_PER_BYTE = 8;
    int BITS_PER_GROUP = 2;
    int GROUPS_PER_BYTE = (BITS_PER_BYTE/BITS_PER_GROUP);
    RPointerArray<const MFunctionGroupBinding>  bindings;
    if (aProtectionDomain.Length() > 0)
    {
        iSecurityPolicy->ProtectionDomainL(aProtectionDomain).FunctionGroupBindingsL(bindings);
    }
    else
    {
        iSecurityPolicy->UntrustedProtectionDomainL().FunctionGroupBindingsL(bindings);
    }
    TInt count = bindings.Count();
    for (TInt i = 0; i < count; i++)
    {
        const MFunctionGroupBinding& binding = *(bindings[i]);
        HBufC* name = binding.FunctionGroup().Name().AllocLC();
        std::wstring securitySettingsName = L"";
        std::wstring currentInteractionMode = L"";
        // mape the current interaction mode
        // try to read it from the encoded string -> if it fails, then go on with the default
        TInt    byteIndex = (i * BITS_PER_GROUP) / BITS_PER_BYTE;
        TInt    bitIndex  = (i - (byteIndex * GROUPS_PER_BYTE)) * BITS_PER_GROUP;
        if (byteIndex < aEncodedSecuritySettings.size())
        {
            switch (((aEncodedSecuritySettings[byteIndex] >> bitIndex) & 3))
            {
            case 1:
                // blanket
                currentInteractionMode = L"3";
                break;
            case 2:
                // oneshot
                currentInteractionMode = L"1";
                break;
            case 3:
                // session
                currentInteractionMode = L"2";
                break;
            default:
                // denied
                currentInteractionMode = L"4";
            }
        }
        if (currentInteractionMode.size() == 0)
        {
            // get the default one from the legacy policy
            switch (binding.DefaultInteractionMode())
            {
            case MPermission::EDenied:
                currentInteractionMode = L"4";
                break;
            case MPermission::EBlanket:
                currentInteractionMode = L"3";
                break;
            case MPermission::EOneshot:
                currentInteractionMode = L"1";
                break;
            case MPermission::ESession:
                currentInteractionMode = L"2";
                break;
            }
        }
        if (currentInteractionMode.size() > 0)
        {
            aSecuritySettings.push_back(new SecuritySettings(
                                            std::wstring((wchar_t*) name->Ptr(), name->Length()), currentInteractionMode));
        }
        CleanupStack::PopAndDestroy(name);
    }
    bindings.Close();
}

bool StorageHandler::IsInteractionModeAllowed(const std::wstring& aInteractionMode, const std::wstring& aAllowedInteractionModes)
{
    bool res = false;
    if (aAllowedInteractionModes.size() > 0)
    {
        int interactionMode = JavaCommonUtils::wstringToInt(aInteractionMode);
        // 1 = ONESHOT, 2 = SESSION, 3 = BLANKET, 4 = NO
        int allowedInteractionModes = JavaCommonUtils::wstringToInt(aAllowedInteractionModes);
        // allowedInteraction is a 4 bit number with the following representation:
        //     oneshot session blanket no
        // E.g. 1011 (=11 in decimal) means that oneshot, blanket and no are allowed
        bool oneshotAllowed = ((allowedInteractionModes & 0x8) == 0x8);
        bool sessionAllowed = ((allowedInteractionModes & 0x4) == 0x4);
        bool blanketAllowed = ((allowedInteractionModes & 0x2) == 0x2);
        bool noAllowed = ((allowedInteractionModes & 0x1) == 0x1);
        res = (interactionMode == 1 && oneshotAllowed)
              || (interactionMode == 2 && sessionAllowed)
              || (interactionMode == 3 && blanketAllowed)
              || (interactionMode == 4 && noAllowed);
    }
    return res;
}

void StorageHandler::findEntry(const JavaStorageApplicationList_t& queryResult,
                               const std::wstring& eName,
                               std::wstring& eValue)
{
    if (queryResult.size() > 0)
    {
        JavaStorageApplicationEntry_t entry = queryResult.front();
        JavaStorageEntry findPattern;
        findPattern.setEntry(eName, L"");
        JavaStorageApplicationEntry_t::const_iterator findIterator =
            entry.find(findPattern);
        if (findIterator != entry.end())
        {
            eValue = findIterator->entryValue();
        }
    }
}

MidletSuiteSecuritySettings::MidletSuiteSecuritySettings(const TUint32& aUid,
        const std::wstring& aName,
        const std::wstring& aVendor,
        const std::wstring& aVersion)
        : iUid(aUid), iName(aName), iVendor(aVendor), iVersion(aVersion)
{
}

SecuritySettings::SecuritySettings(const std::wstring& aName,
                                   const std::wstring& aCurrentInteractionMode)
        : iName(aName), iCurrentInteractionMode(aCurrentInteractionMode)
{
}

SecuritySettings& SecuritySettings::operator=(const SecuritySettings& aSecuritySettings)
{
    iName = aSecuritySettings.iName;
    iCurrentInteractionMode = aSecuritySettings.iCurrentInteractionMode;
    return *this;
}

SecuritySettings::SecuritySettings()
        : iName(L""), iCurrentInteractionMode(L"")
{
}

CustomAttribute::CustomAttribute(const std::wstring& aName,
                                 const std::wstring& aValue)
        : iName(aName), iValue(aValue)
{
}

CustomAttribute& CustomAttribute::operator=(const CustomAttribute& aCustomAttribute)
{
    iName = aCustomAttribute.iName;
    iValue = aCustomAttribute.iValue;
    return *this;
}

CustomAttribute::CustomAttribute()
        : iName(L""), iValue(L"")
{
}

MidletSuiteCustomAttributes::MidletSuiteCustomAttributes(const TUint32& aMidletSuiteUid)
        : iUid(aMidletSuiteUid)
{
}
