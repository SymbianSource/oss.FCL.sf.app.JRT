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

#include "teststoragehandler.h"
#include "javastoragenames.h"
#include "javastorage.h"
#include "javastorageexception.h"
#include "javasymbianoslayer.h"
#include "javacommonutils.h"

_LIT(KFlags,            "Flags");
_LIT(KMSId,             "MSId");
_LIT(KMSName,           "MSName");
_LIT(KMSVendor,         "MSVendor");
_LIT(KMSVersion,        "MSVersion");
_LIT(KProtectionDomain, "ProtectionDomain");
_LIT(KMSSecurityPrefs,  "MSSecurityPrefs");
_LIT(KCustomAttribsValue,            "Value");

using namespace java::storage;

TestStorageHandler* TestStorageHandler::NewL()
{
    TestStorageHandler* self = new(ELeave) TestStorageHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void TestStorageHandler::ConstructL()
{
    User::LeaveIfError(iDbs.Connect());
    TInt err = iUserDb.Open(iDbs, _L("c:MIDP2SystemAMSUserV2"), _L("secure[102045FE]"));
    if (err==KErrNotFound)
    {
        const TUid KUidSystemAMS={0x102045FE};
        iUserDb.Close();
        iUserDb.Create(iDbs, _L("c:MIDP2SystemAMSUserV2"), _L("secure[102045FE]"));
        iUserDb.Open(iDbs, _L("c:MIDP2SystemAMSUserV2"), _L("secure[102045FE]"));
    }
    else
    {
        User::LeaveIfError(err);
    }
    err = iSecurityPrefsTable.Open(iUserDb, _L("UserMSSecurityPreferences"));
    if (err==KErrNotFound)
    {
        CDbColSet* colSet = CDbColSet::NewLC();
        colSet->AddL(TDbCol(KFlags,             EDbColUint32));
        colSet->AddL(TDbCol(KMSId,              EDbColUint32));
        colSet->AddL(TDbCol(KMSName,            EDbColLongText));
        colSet->AddL(TDbCol(KMSVendor,          EDbColLongText));
        colSet->AddL(TDbCol(KMSVersion,         EDbColText));
        colSet->AddL(TDbCol(KProtectionDomain,  EDbColLongText));
        colSet->AddL(TDbCol(KMSSecurityPrefs,   EDbColBinary));
        User::LeaveIfError(iUserDb.CreateTable(_L("UserMSSecurityPreferences"), *colSet));
        CleanupStack::PopAndDestroy(colSet);
        CDbKey* key = CDbKey::NewLC();
        key->AddL(TDbKeyCol(KMSId));
        User::LeaveIfError(iUserDb.CreateIndex(_L("UserMSSecurityPreferencesIndex"), _L("UserMSSecurityPreferences"), *key));
        CleanupStack::PopAndDestroy(key);
        iSecurityPrefsTable.Open(iUserDb, _L("UserMSSecurityPreferences"));
    }
    else
    {
        User::LeaveIfError(err);
    }
    err = iCustomAttributesTable.Open(iUserDb, _L("MIDletProperties"));
    if (err==KErrNotFound)
    {
        _LIT(KOrdinal,          "Ordinal");
        _LIT(KComponentId,      "ComponentId");
        _LIT(KId,               "Id");
        _LIT(KValue,            "Value");
        CDbColSet* colSet = CDbColSet::NewLC();
        colSet->AddL(TDbCol(KMSId,          EDbColUint32));
        colSet->AddL(TDbCol(KOrdinal,       EDbColUint32));
        colSet->AddL(TDbCol(KFlags,         EDbColUint32));
        colSet->AddL(TDbCol(KComponentId,   EDbColUint32));
        colSet->AddL(TDbCol(KId,            EDbColUint32));
        colSet->AddL(TDbCol(KValue,         EDbColLongText8));
        User::LeaveIfError(iUserDb.CreateTable(_L("MIDletProperties"), *colSet));
        CleanupStack::PopAndDestroy(colSet);
        iCustomAttributesTable.Open(iUserDb, _L("MIDletProperties"));
    }
    else
    {
        User::LeaveIfError(err);
    }
    User::LeaveIfError(iSecurityPrefsTable.SetIndex(_L("UserMSSecurityPreferencesIndex")));
    CDbColSet* colSet = iSecurityPrefsTable.ColSetL();
    iMSIdColId              = colSet->ColNo(KMSId);
    iMSNameColId            = colSet->ColNo(KMSName);
    iMSVendorColId          = colSet->ColNo(KMSVendor);
    iMSVersionColId         = colSet->ColNo(KMSVersion);
    iPreferencesColId       = colSet->ColNo(KMSSecurityPrefs);
    colSet = iCustomAttributesTable.ColSetL();
    iCAIdColId              = colSet->ColNo(KMSId);
    iCAValueColId           = colSet->ColNo(KCustomAttribsValue);
    delete colSet;
    try
    {
        iStorage = JavaStorage::createInstance();
        iStorage->open(JAVA_DATABASE_NAME);
    }
    catch (JavaStorageException& aJse)
    {
        User::Leave(aJse.mStatus);
    }
}

TestStorageHandler::TestStorageHandler()
{
}

TestStorageHandler::~TestStorageHandler()
{
    iSecurityPrefsTable.Close();
    iCustomAttributesTable.Close();
    iUserDb.Close();
    iDbs.Close();
    if (iStorage)
    {
        iStorage->close();
        delete iStorage;
        iStorage = NULL;
    }
}

void TestStorageHandler::cleanup(const TUint32 aId, const std::wstring& aMidletSuiteUid)
{
    TDbSeekKey key((TUint)aId);
    if (iSecurityPrefsTable.SeekL(key))
    {
        iSecurityPrefsTable.DeleteL();
    }
    TUint32 cnt = iCustomAttributesTable.CountL();
    iCustomAttributesTable.BeginningL();
    for (int i=0; i<cnt; i++)
    {
        iCustomAttributesTable.NextL();
        iCustomAttributesTable.DeleteL();
    }
    try
    {
        JavaStorageApplicationEntry_t entry;
        JavaStorageEntry attr;
        attr.setEntry(ID, aMidletSuiteUid);
        entry.insert(attr);
        iStorage->remove(APPLICATION_PACKAGE_TABLE, entry);
        iStorage->remove(MIDP_FUNC_GRP_SETTINGS_TABLE, entry);
        iStorage->remove(MIDP_PACKAGE_TABLE, entry);
    }
    catch (JavaStorageException& aJse)
    {
    }
}

void TestStorageHandler::populateLegacyStorageL(TUint32 aId, const TDesC& aName, const TDesC& aVendor, const TDesC& aVersion, const TDesC8& aSecurityPrefs)
{
    iSecurityPrefsTable.EndL();
    iSecurityPrefsTable.InsertL();
    iSecurityPrefsTable.SetColL(iMSIdColId,       aId);
    iSecurityPrefsTable.SetColL(iMSNameColId,     aName);
    iSecurityPrefsTable.SetColL(iMSVendorColId,   aVendor);
    iSecurityPrefsTable.SetColL(iMSVersionColId,  aVersion);
    iSecurityPrefsTable.SetColL(iPreferencesColId,aSecurityPrefs);
    iSecurityPrefsTable.PutL();
}

void TestStorageHandler::populateLegacyStorageL(TUint32 aId, const TDesC8& aCustomAttributes)
{
    iCustomAttributesTable.EndL();
    iCustomAttributesTable.InsertL();
    iCustomAttributesTable.SetColL(iCAIdColId,       aId);
    iCustomAttributesTable.SetColL(iCAValueColId,     aCustomAttributes);
    iCustomAttributesTable.PutL();
}

void TestStorageHandler::populateOmjStorageL(const std::wstring& aMidletSuiteUid, const std::wstring& aMidletSuiteName, const std::wstring& aMidletSuiteVendor, const std::wstring& aMidletSuiteVersion, const std::wstring& aSecuritySettingsName, const std::wstring& aCurrentInteractionMode, const std::wstring& aAllowedInteractionModes)
{
    try
    {
        JavaStorageApplicationEntry_t entry;
        JavaStorageEntry attr;
        attr.setEntry(ID, aMidletSuiteUid);
        entry.insert(attr);
        attr.setEntry(PACKAGE_NAME, aMidletSuiteName);
        entry.insert(attr);
        attr.setEntry(VENDOR, aMidletSuiteVendor);
        entry.insert(attr);
        attr.setEntry(VERSION, aMidletSuiteVersion);
        entry.insert(attr);
        iStorage->write(APPLICATION_PACKAGE_TABLE, entry);
        entry.clear();
        attr.setEntry(ID, aMidletSuiteUid);
        entry.insert(attr);
        attr.setEntry(FUNCTION_GROUP, aSecuritySettingsName);
        entry.insert(attr);
        attr.setEntry(CURRENT_SETTING, aCurrentInteractionMode);
        entry.insert(attr);
        if (aAllowedInteractionModes.size() > 0)
        {
            attr.setEntry(ALLOWED_SETTINGS, aAllowedInteractionModes);
            entry.insert(attr);
        }
        iStorage->write(MIDP_FUNC_GRP_SETTINGS_TABLE, entry);
    }
    catch (JavaStorageException& aJse)
    {
    }
}

void TestStorageHandler::populateOmjStorageL(const std::wstring& aMidletSuiteUid, const std::wstring& aOnScreenKeypad)
{
    try
    {
        JavaStorageApplicationEntry_t entry;
        JavaStorageEntry attr;
        attr.setEntry(ID, aMidletSuiteUid);
        entry.insert(attr);
        attr.setEntry(ON_SCREEN_KEYPAD, aOnScreenKeypad);
        entry.insert(attr);
        iStorage->write(MIDP_PACKAGE_TABLE, entry);
    }
    catch (JavaStorageException& aJse)
    {
    }
}

void TestStorageHandler::retrieveOnScreenKeypad(const std::wstring& aMidletSuiteUid, std::wstring& aOnScreenKeypad)
{
    try
    {
        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        attr.setEntry(ID, aMidletSuiteUid);
        query.insert(attr);
        attr.setEntry(ON_SCREEN_KEYPAD, L"");
        query.insert(attr);
        iStorage->search(MIDP_PACKAGE_TABLE, query, queryResult);
        findEntry(queryResult, ON_SCREEN_KEYPAD, aOnScreenKeypad);
    }
    catch (JavaStorageException& aJse)
    {
    }
}

void TestStorageHandler::retrieveSecuritySettings(const std::wstring& aMidletSuiteName, const std::wstring& aMidletSuiteVendor, const std::wstring& aMidletSuiteVersion, std::vector<SecuritySettings*>& aSecuritySettings)
{
    try
    {
        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        std::wstring midletSuiteUid = L"";
        attr.setEntry(ID, L"");
        query.insert(attr);
        attr.setEntry(PACKAGE_NAME, aMidletSuiteName);
        query.insert(attr);
        attr.setEntry(VENDOR, aMidletSuiteVendor);
        query.insert(attr);
        attr.setEntry(VERSION, aMidletSuiteVersion);
        query.insert(attr);
        iStorage->search(APPLICATION_PACKAGE_TABLE, query, queryResult);
        findEntry(queryResult, ID, midletSuiteUid);
        if (midletSuiteUid.size() > 0)
        {
            query.clear();
            queryResult.clear();
            attr.setEntry(ID, midletSuiteUid);
            query.insert(attr);
            attr.setEntry(FUNCTION_GROUP, L"");
            query.insert(attr);
            iStorage->search(MIDP_FUNC_GRP_SETTINGS_TABLE, query, queryResult);
            JavaStorageApplicationList_t::const_iterator iterator;
            SecuritySettings* securitySettings;
            for (iterator = queryResult.begin(); iterator != queryResult.end(); iterator++)
            {
                JavaStorageApplicationEntry_t entry = (*iterator);
                std::wstring securitySettingsName = L"";
                findColumn(entry, FUNCTION_GROUP, securitySettingsName);
                if (securitySettingsName.size() > 0)
                {
                    query.clear();
                    JavaStorageApplicationList_t currentInteractionModeQueryResult;
                    attr.setEntry(ID, midletSuiteUid);
                    query.insert(attr);
                    attr.setEntry(FUNCTION_GROUP, securitySettingsName);
                    query.insert(attr);
                    attr.setEntry(CURRENT_SETTING, L"");
                    query.insert(attr);
                    iStorage->search(MIDP_FUNC_GRP_SETTINGS_TABLE, query, currentInteractionModeQueryResult);
                    if (currentInteractionModeQueryResult.size() > 0)
                    {
                        std::wstring currentInteractionMode = L"";
                        findEntry(currentInteractionModeQueryResult,CURRENT_SETTING , currentInteractionMode);
                        securitySettings = new SecuritySettings(securitySettingsName, currentInteractionMode);
                        aSecuritySettings.push_back(securitySettings);
                    }
                }
            }
        }
    }
    catch (JavaStorageException& aJse)
    {
    }
}

void TestStorageHandler::findEntry(const JavaStorageApplicationList_t& queryResult,
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

void TestStorageHandler::findColumn(const JavaStorageApplicationEntry_t& aEntry,
                                    const std::wstring& aColName, std::wstring& aColValue)
{
    JavaStorageEntry findPattern;
    findPattern.setEntry(aColName, L"");
    JavaStorageApplicationEntry_t::const_iterator findIterator =
        aEntry.find(findPattern);
    if (findIterator != aEntry.end())
    {
        aColValue = findIterator->entryValue();
    }
}

SecuritySettings::SecuritySettings(const std::wstring& aSettingsName,
                                   const std::wstring& aCurrentInteractionMode)
        : iSettingsName(aSettingsName), iCurrentInteractionMode(aCurrentInteractionMode)
{
}

SecuritySettings& SecuritySettings::operator=(const SecuritySettings& aSecuritySettings)
{
    iSettingsName = aSecuritySettings.iSettingsName;
    iCurrentInteractionMode = aSecuritySettings.iCurrentInteractionMode;
    return *this;
}

SecuritySettings::SecuritySettings()
        : iSettingsName(L""), iCurrentInteractionMode(L"")
{
}




