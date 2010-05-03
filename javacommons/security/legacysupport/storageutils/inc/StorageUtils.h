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
#ifndef STORAGEUTILS_H
#define STORAGEUTILS_H

#include <e32base.h>
#include <appversion.h>
#include "javastorage.h"
#include <vector>
#include <string>

namespace java
{
namespace security
{
namespace legacysupport
{

class MidletSuiteInfo: public CBase
{
public:
    MidletSuiteInfo(const std::wstring& aUid,
                    const std::wstring& aName,
                    const std::wstring& aVendor,
                    const std::wstring& aVersion,
                    const std::wstring& aProtectionDomain);

    MidletSuiteInfo &operator=(const MidletSuiteInfo&);

    MidletSuiteInfo();

    std::wstring iUid;
    std::wstring iName;
    std::wstring iVendor;
    std::wstring iVersion;
    std::wstring iProtectionDomain;
};

class MidletSuiteSecuritySettings: public CBase
{
public:
    MidletSuiteSecuritySettings(const int aCurrentInteractionMode,
                                const int aAllowedInteractionModes);

    MidletSuiteSecuritySettings(const std::wstring& aSettingsName,
                                const int aCurrentInteractionMode,
                                const int aAllowedInteractionModes);

    MidletSuiteSecuritySettings(const MidletSuiteSecuritySettings& aMidletSuiteSecuritySettings);

    MidletSuiteSecuritySettings &operator=(const MidletSuiteSecuritySettings&);

    MidletSuiteSecuritySettings();

    std::wstring iSettingsName;
    int iCurrentInteractionMode;
    int iAllowedInteractionModes;
};

class StorageUtils: public CBase
{
public:

    static StorageUtils* NewL();
    void readMidletSuitesInfos(std::vector<MidletSuiteInfo*>& aMidletSuites);
    void readOnScreenKeyboardAttributeL(const std::wstring& aMidletSuiteUid, std::wstring& aValue);
    void writeOnScreenKeyboardAttribute(const std::wstring& aMidletSuiteUid, const std::wstring& aValue);
    void readMidletSuiteInfoL(const std::wstring& aMidletSuiteName, const std::wstring& aMidletSuiteVendor, const std::wstring& aMidletSuiteVersion, MidletSuiteInfo& aMidletSuiteInfo);
    int readMidletSuiteSecuritySettings(const std::wstring& aMidletSuiteUid, const std::wstring& aSettingsName, MidletSuiteSecuritySettings& aSecuritySettings);
    int readMidletSuiteSecuritySettings(const std::wstring& aMidletSuiteUid, std::vector<MidletSuiteSecuritySettings>& aMidletSuiteSecuritySettings);
    int writeMidletSuiteSecuritySettings(const std::wstring& aMidletSuiteUid, const std::wstring& aSettingsName, int aCurrentInteractionMode);
    int writeUserSecuritySettingsPromptFlag(const std::wstring& aMidletSuiteUid, const std::wstring& aSettingsName, bool aPromptFlag);
    ~StorageUtils();
private:
    void ConstructL();
    StorageUtils();
    void findColumn(const java::storage::JavaStorageApplicationEntry_t& aEntry, const std::wstring& aColName, std::wstring& aColValue);
    void findEntry(const java::storage::JavaStorageApplicationList_t&, const std::wstring&, std::wstring& eValue);
    TAppVersion wstringToAppVersion(const std::wstring& aVersionString);
    java::storage::JavaStorage* iStorage;
};

} //end namespace legacysupport
} //end namespace security
} //end namespace java

#endif // STORAGEUTILS_H
