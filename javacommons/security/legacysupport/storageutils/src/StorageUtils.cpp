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
#include "StorageUtils.h"
#include "javastoragenames.h"
#include "javastorage.h"
#include "javastorageexception.h"
#include "javasymbianoslayer.h"
#include "javacommonutils.h"

using namespace std;
using namespace java::storage;
using namespace java::security::legacysupport;
using namespace java::util;

StorageUtils* StorageUtils::NewL()
{
    StorageUtils* self = new(ELeave) StorageUtils();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

StorageUtils::StorageUtils()
{
}

void StorageUtils::ConstructL()
{
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

void StorageUtils::readOnScreenKeyboardAttributeL(const std::wstring& aMidletSuiteUid, wstring& aValue)
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
        wstring value = L"";
        findEntry(queryResult, ON_SCREEN_KEYPAD, aValue);
        if (aValue.size() == 0)
        {
            User::Leave(KErrNotFound);
        }
    }
    catch (JavaStorageException& aJse)
    {
        User::Leave(KErrNotFound);
    }
}

void StorageUtils::writeOnScreenKeyboardAttribute(const std::wstring& aMidletSuiteUid, const wstring& aValue)
{
    try
    {
        JavaStorageApplicationEntry_t oldEntry;
        JavaStorageEntry attr;
        attr.setEntry(ID, aMidletSuiteUid);
        oldEntry.insert(attr);
        JavaStorageApplicationEntry_t entry;
        attr.setEntry(ON_SCREEN_KEYPAD, aValue);
        entry.insert(attr);
        iStorage->update(MIDP_PACKAGE_TABLE, entry, oldEntry);
    }
    catch (JavaStorageException& aJse)
    {
    }
}

void StorageUtils::readMidletSuitesInfos(vector<MidletSuiteInfo*>& aMidletSuites)
{
    try
    {
        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        attr.setEntry(ID, L"");
        query.insert(attr);
        attr.setEntry(PACKAGE_NAME, L"");
        query.insert(attr);
        attr.setEntry(VENDOR, L"");
        query.insert(attr);
        attr.setEntry(VERSION, L"");
        query.insert(attr);
        iStorage->search(APPLICATION_PACKAGE_TABLE, query, queryResult);
        JavaStorageApplicationList_t::const_iterator iterator;
        JavaStorageApplicationList_t domainQueryResult;
        MidletSuiteInfo* info;
        for (iterator = queryResult.begin(); iterator != queryResult.end(); iterator++)
        {
            std::wstring suiteUid = L"";
            std::wstring suiteName = L"";
            std::wstring suiteVendor = L"";
            std::wstring suiteVersion = L"";
            std::wstring suiteProtectionDomain = L"";
            JavaStorageApplicationEntry_t entry = (*iterator);
            findColumn(entry, ID, suiteUid);
            findColumn(entry, PACKAGE_NAME, suiteName);
            findColumn(entry, VENDOR, suiteVendor);
            findColumn(entry, VERSION, suiteVersion);
            entry.clear();
            query.clear();
            attr.setEntry(ID, suiteUid);
            query.insert(attr);
            attr.setEntry(SECURITY_DOMAIN_CATEGORY, L"");
            query.insert(attr);
            domainQueryResult.clear();
            iStorage->search(MIDP_PACKAGE_TABLE, query, domainQueryResult);
            findEntry(queryResult,SECURITY_DOMAIN_CATEGORY , suiteProtectionDomain);
            info = new MidletSuiteInfo(suiteUid, suiteName, suiteVendor, suiteVersion, suiteProtectionDomain);
            aMidletSuites.push_back(info);
        }
    }
    catch (JavaStorageException& aJse)
    {
    }
}

void StorageUtils::readMidletSuiteInfoL(const std::wstring& aMidletSuiteName, const std::wstring& aMidletSuiteVendor, const std::wstring& aMidletSuiteVersion, MidletSuiteInfo& aMidletSuiteInfo)
{
    if (aMidletSuiteName.size() == 0
            || aMidletSuiteVendor.size() == 0
            || aMidletSuiteVersion.size() == 0)
    {
        User::Leave(KErrNotFound);
    }
    try
    {
        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        wstring midletSuiteUid = L"";
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
        wstring midletSuiteVersion = aMidletSuiteVersion;
        if (midletSuiteUid.size() == 0)
        {
            TAppVersion appVersion = wstringToAppVersion(aMidletSuiteVersion);
            midletSuiteVersion = JavaCommonUtils::intToWstring(appVersion.iMajor);
            midletSuiteVersion += L".";
            midletSuiteVersion += JavaCommonUtils::intToWstring(appVersion.iMinor);
            query.clear();
            queryResult.clear();
            attr.setEntry(ID, L"");
            query.insert(attr);
            attr.setEntry(PACKAGE_NAME, aMidletSuiteName);
            query.insert(attr);
            attr.setEntry(VENDOR, aMidletSuiteVendor);
            query.insert(attr);
            attr.setEntry(VERSION, midletSuiteVersion);
            query.insert(attr);
            iStorage->search(APPLICATION_PACKAGE_TABLE, query, queryResult);
            findEntry(queryResult, ID, midletSuiteUid);
            if (midletSuiteUid.size() == 0)
            {
                // one more try: the long version
                midletSuiteVersion += L".";
                midletSuiteVersion += JavaCommonUtils::intToWstring(appVersion.iBuild);
                query.clear();
                queryResult.clear();
                attr.setEntry(ID, L"");
                query.insert(attr);
                attr.setEntry(PACKAGE_NAME, aMidletSuiteName);
                query.insert(attr);
                attr.setEntry(VENDOR, aMidletSuiteVendor);
                query.insert(attr);
                attr.setEntry(VERSION, midletSuiteVersion);
                query.insert(attr);
                iStorage->search(APPLICATION_PACKAGE_TABLE, query, queryResult);
                findEntry(queryResult, ID, midletSuiteUid);
                if (midletSuiteUid.size() == 0)
                {
                    User::Leave(KErrNotFound);
                }
            }
        }
        query.clear();
        attr.setEntry(ID, midletSuiteUid);
        query.insert(attr);
        attr.setEntry(SECURITY_DOMAIN_CATEGORY, L"");
        query.insert(attr);
        queryResult.clear();
        iStorage->search(MIDP_PACKAGE_TABLE, query, queryResult);
        wstring midletSuiteDomain = L"";
        findEntry(queryResult,SECURITY_DOMAIN_CATEGORY , midletSuiteDomain);
        if (midletSuiteDomain.size() == 0)
        {
            User::Leave(KErrNotFound);
        }
        MidletSuiteInfo midletSuiteInfo(midletSuiteUid,
                                        aMidletSuiteName,
                                        aMidletSuiteVendor,
                                        midletSuiteVersion,
                                        midletSuiteDomain);
        aMidletSuiteInfo = midletSuiteInfo;
    }
    catch (JavaStorageException& aJse)
    {
        User::Leave(KErrNotFound);
    }
}

int StorageUtils::readMidletSuiteSecuritySettings(const std::wstring& aMidletSuiteUid, std::vector<MidletSuiteSecuritySettings>& aMidletSuiteSecuritySettings)
{
    try
    {
        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        attr.setEntry(ID, aMidletSuiteUid);
        query.insert(attr);
        attr.setEntry(FUNCTION_GROUP, L"");
        query.insert(attr);
        iStorage->search(MIDP_FUNC_GRP_SETTINGS_TABLE, query, queryResult);
        JavaStorageApplicationList_t::const_iterator iterator;
        JavaStorageApplicationList_t settingsQueryResult;
        MidletSuiteSecuritySettings settings;
        for (iterator = queryResult.begin(); iterator != queryResult.end(); iterator++)
        {
            std::wstring settingsName = L"";
            JavaStorageApplicationEntry_t entry = (*iterator);
            findColumn(entry, FUNCTION_GROUP, settingsName);
            entry.clear();
            query.clear();
            attr.setEntry(ID, aMidletSuiteUid);
            query.insert(attr);
            attr.setEntry(FUNCTION_GROUP, settingsName);
            query.insert(attr);
            attr.setEntry(CURRENT_SETTING, L"");
            query.insert(attr);
            attr.setEntry(ALLOWED_SETTINGS , L"");
            query.insert(attr);
            settingsQueryResult.clear();
            iStorage->search(MIDP_FUNC_GRP_SETTINGS_TABLE, query, settingsQueryResult);
            if (settingsQueryResult.size() > 0)
            {
                wstring currentInteractionMode = L"";
                wstring allowedInteractionModes = L"";
                findEntry(settingsQueryResult,CURRENT_SETTING , currentInteractionMode);
                findEntry(settingsQueryResult,ALLOWED_SETTINGS , allowedInteractionModes);
                if (currentInteractionMode.size() > 0 && allowedInteractionModes.size() > 0)
                {
                    settings = MidletSuiteSecuritySettings(
                                   settingsName,
                                   JavaCommonUtils::wstringToInt(currentInteractionMode),
                                   JavaCommonUtils::wstringToInt(allowedInteractionModes));
                }
            }
            aMidletSuiteSecuritySettings.push_back(settings);
        }
    }
    catch (JavaStorageException& aJse)
    {
        return KErrNotFound;
    }
    return KErrNone;
}

int StorageUtils::readMidletSuiteSecuritySettings(const std::wstring& aMidletSuiteUid, const std::wstring& aSettingsName, MidletSuiteSecuritySettings& aSecuritySettings)
{
    try
    {
        JavaStorageApplicationEntry_t query;
        JavaStorageApplicationList_t queryResult;
        JavaStorageEntry attr;
        attr.setEntry(ID, aMidletSuiteUid);
        query.insert(attr);
        attr.setEntry(FUNCTION_GROUP, aSettingsName);
        query.insert(attr);
        iStorage->search(MIDP_FUNC_GRP_SETTINGS_TABLE, query, queryResult);
        if (queryResult.size() == 0)
        {
            return KErrNotFound;
        }
        MidletSuiteSecuritySettings settings;
        queryResult.clear();
        attr.setEntry(CURRENT_SETTING, L"");
        query.insert(attr);
        attr.setEntry(ALLOWED_SETTINGS , L"");
        query.insert(attr);
        iStorage->search(MIDP_FUNC_GRP_SETTINGS_TABLE, query, queryResult);
        if (queryResult.size() > 0)
        {
            wstring currentInteractionMode = L"";
            wstring allowedInteractionModes = L"";
            findEntry(queryResult,CURRENT_SETTING , currentInteractionMode);
            findEntry(queryResult,ALLOWED_SETTINGS , allowedInteractionModes);
            if (currentInteractionMode.size() > 0 && allowedInteractionModes.size() > 0)
            {
                settings = MidletSuiteSecuritySettings(
                               JavaCommonUtils::wstringToInt(currentInteractionMode),
                               JavaCommonUtils::wstringToInt(allowedInteractionModes));
            }
        }
        aSecuritySettings = settings;
    }
    catch (JavaStorageException& aJse)
    {
        return KErrNotFound;
    }
    return KErrNone;
}

int StorageUtils::writeMidletSuiteSecuritySettings(const std::wstring& aMidletSuiteUid, const std::wstring& aSettingsName, int aCurrentInteractionMode)
{
    try
    {
        JavaStorageApplicationEntry_t entry;
        JavaStorageApplicationEntry_t oldEntry;
        JavaStorageEntry attr;
        attr.setEntry(ID, aMidletSuiteUid);
        oldEntry.insert(attr);
        attr.setEntry(FUNCTION_GROUP, aSettingsName);
        oldEntry.insert(attr);
        attr.setEntry(CURRENT_SETTING, JavaCommonUtils::intToWstring(aCurrentInteractionMode));
        entry.insert(attr);
        iStorage->update(MIDP_FUNC_GRP_SETTINGS_TABLE, entry, oldEntry);
    }
    catch (JavaStorageException& aJse)
    {
        return KErrNotFound;
    }
    return KErrNone;
}

int StorageUtils::writeUserSecuritySettingsPromptFlag(const std::wstring& aMidletSuiteUid, const std::wstring& aSettingsName, bool aPromptFlag)
{
    try
    {
        JavaStorageApplicationEntry_t entry;
        JavaStorageApplicationEntry_t oldEntry;
        JavaStorageEntry attr;
        attr.setEntry(ID, aMidletSuiteUid);
        oldEntry.insert(attr);
        attr.setEntry(FUNCTION_GROUP, aSettingsName);
        oldEntry.insert(attr);
        std::wstring blanketPrompt;
        if (aPromptFlag)
        {
            blanketPrompt = L"1";
        }
        else
        {
            blanketPrompt = L"0";
        }
        attr.setEntry(BLANKET_PROMPT, blanketPrompt);
        entry.insert(attr);
        iStorage->update(MIDP_FUNC_GRP_SETTINGS_TABLE, entry, oldEntry);
    }
    catch (JavaStorageException& aJse)
    {
        return KErrNotFound;
    }
    return KErrNone;
}

StorageUtils::~StorageUtils()
{
    if (iStorage != NULL)
    {
        iStorage->close();
        delete iStorage;
        iStorage = NULL;
    }
}

void StorageUtils::findEntry(const JavaStorageApplicationList_t& queryResult,
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

void StorageUtils::findColumn(const JavaStorageApplicationEntry_t& aEntry,
                              const wstring& aColName, wstring& aColValue)
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

// Converts a string from either xxx.yy(zzzzz) or xxx.yy.zzzzzz
// into a TAppVersion representation
// where xx is the major, yy is the minor and zzzzz is the build
TAppVersion StorageUtils::wstringToAppVersion(const std::wstring& aVersionString)
{
    TAppVersion appVersion;
    char* temp = JavaCommonUtils::wstringToUtf8(aVersionString);
    std::string versionStr = temp;

    std::string::size_type idx = 0;
    idx = versionStr.find(".", idx);

    if (idx != std::string::npos)
    {
        versionStr.replace(idx, 1, " ");
    }

    std::string::size_type tmp = 0;
    tmp = versionStr.find("(", idx + 1);

    if (tmp != std::string::npos)
    {
        idx = tmp;
        versionStr.replace(idx, 1, " ");
    }
    else
    {
        tmp = versionStr.find(".", idx + 1);
        idx = tmp;
        if (tmp != std::string::npos)
        {
            versionStr.replace(idx, 1, " ");
        }
    }

    tmp = versionStr.find(")", idx + 1);

    if (tmp != std::string::npos)
    {
        idx = tmp;
        versionStr.replace(idx, 1, " ");
    }
    else
    {
        tmp = versionStr.find(".", idx + 1);
        idx = tmp;
        if (tmp != std::string::npos)
        {
            versionStr.replace(idx, 1, " ");
        }
    }

    int major = 0;
    int minor = 0;
    int build = 0;
    int result = sscanf(
                     versionStr.c_str(), "%d %d %d", &major, &minor, &build);

    if (major > 0)
    {
        appVersion.iMajor = major;
    }

    if (minor > 0)
    {
        appVersion.iMinor = minor;
    }

    if (build > 0)
    {
        appVersion.iBuild = build;
    }
    delete[] temp;

    return appVersion;
}


MidletSuiteInfo::MidletSuiteInfo(const std::wstring& aUid,
                                 const std::wstring& aName,
                                 const std::wstring& aVendor,
                                 const std::wstring& aVersion,
                                 const std::wstring& aProtectionDomain)
        : iUid(aUid), iName(aName), iVendor(aVendor), iVersion(aVersion), iProtectionDomain(aProtectionDomain)
{
}

MidletSuiteInfo& MidletSuiteInfo::operator=(const MidletSuiteInfo& aMidletSuiteInfo)
{
    iUid = aMidletSuiteInfo.iUid;
    iName = aMidletSuiteInfo.iName;
    iVendor = aMidletSuiteInfo.iVendor;
    iVersion = aMidletSuiteInfo.iVersion;
    iProtectionDomain = aMidletSuiteInfo.iProtectionDomain;
    return *this;
}

MidletSuiteInfo::MidletSuiteInfo()
        : iUid(L""), iName(L""), iVendor(L""), iVersion(L""), iProtectionDomain(L"")
{
}

MidletSuiteSecuritySettings::MidletSuiteSecuritySettings(const int aCurrentInteractionMode,
        const int aAllowedInteractionModes)
        : iSettingsName(L""), iCurrentInteractionMode(aCurrentInteractionMode), iAllowedInteractionModes(aAllowedInteractionModes)
{
}

MidletSuiteSecuritySettings::MidletSuiteSecuritySettings(const std::wstring& aSettingsName,
        const int aCurrentInteractionMode,
        const int aAllowedInteractionModes)
        : iSettingsName(aSettingsName), iCurrentInteractionMode(aCurrentInteractionMode), iAllowedInteractionModes(aAllowedInteractionModes)
{
}

MidletSuiteSecuritySettings& MidletSuiteSecuritySettings::operator=(const MidletSuiteSecuritySettings& aMidletSuiteSecuritySettings)
{
    iSettingsName = aMidletSuiteSecuritySettings.iSettingsName;
    iCurrentInteractionMode = aMidletSuiteSecuritySettings.iCurrentInteractionMode;
    iAllowedInteractionModes = aMidletSuiteSecuritySettings.iAllowedInteractionModes;
    return *this;
}

MidletSuiteSecuritySettings::MidletSuiteSecuritySettings(const MidletSuiteSecuritySettings& aMidletSuiteSecuritySettings)
{
    iSettingsName = aMidletSuiteSecuritySettings.iSettingsName;
    iCurrentInteractionMode = aMidletSuiteSecuritySettings.iCurrentInteractionMode;
    iAllowedInteractionModes = aMidletSuiteSecuritySettings.iAllowedInteractionModes;
}

MidletSuiteSecuritySettings::MidletSuiteSecuritySettings()
        : iSettingsName(L""), iCurrentInteractionMode(-1), iAllowedInteractionModes(-1)
{
}
