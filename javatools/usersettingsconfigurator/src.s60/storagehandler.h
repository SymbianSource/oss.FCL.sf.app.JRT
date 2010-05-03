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
#ifndef STORAGEHANDLER_H
#define STORAGEHANDLER_H

#include <vector>
#include <string>
#include <e32base.h>
#include <d32dbms.h>
#include "javastorage.h"
#include "javacommonutils.h"
#include "j2me/midp2/security/MSecurityPolicyV2.h"

using namespace java::util;
using namespace java::storage;

namespace java
{
namespace tools
{
namespace usersettingsconfigurator
{

class SecuritySettings
{
public:
    SecuritySettings(const std::wstring& aSettingsName,
                     const std::wstring& aCurrentInteractionMode);

    SecuritySettings &operator=(const SecuritySettings&);
    SecuritySettings();

    std::wstring iName;
    std::wstring iCurrentInteractionMode;
};

class CustomAttribute
{
public:
    CustomAttribute(const std::wstring& aAttributeName,
                    const std::wstring& aAttributeValue);

    CustomAttribute &operator=(const CustomAttribute&);
    CustomAttribute();

    std::wstring iName;
    std::wstring iValue;
};

class MidletSuiteCustomAttributes
{
public:
    MidletSuiteCustomAttributes(const TUint32& aMidletSuiteUid);

    TUint32 iUid;
    std::vector<CustomAttribute*> iCustomAttributes;
};


class MidletSuiteSecuritySettings
{
public:
    MidletSuiteSecuritySettings(const TUint32& aMidletSuiteUid,
                                const std::wstring& aMidletSuiteName,
                                const std::wstring& aMidletSuiteVendor,
                                const std::wstring& aMidletSuiteVersion);
    TUint32 iUid;
    std::wstring iName;
    std::wstring iVendor;
    std::wstring iVersion;
    std::vector<SecuritySettings*> iSecuritySettings;
};

class StorageHandler: public CBase
{
public:
    static StorageHandler* NewL();
    void readMidletSuitesPropertiesL(std::vector<MidletSuiteSecuritySettings*>& aMidletSuitesSecuritySettings, std::vector<MidletSuiteCustomAttributes*>& aMidletSuitesCustomAttributes);
    void writeMidletSuitePropertiesL(const std::vector<MidletSuiteSecuritySettings*>& aMidletSuitesSecuritySettings, const std::vector<MidletSuiteCustomAttributes*>& aMidletSuitesCustomAttributes);
    void convertAPNSettingsL() const;
    virtual ~StorageHandler();
private:
    void ConstructL();
    StorageHandler();
    void DecodeSecuritySettingsL(const TDesC& aProtectionDomain, const std::string& aEncodedSecuritySettings, std::vector<SecuritySettings*>& aSecuritySettings);
    void ReadSecuritySettingsL(std::vector<MidletSuiteSecuritySettings*>& aMidletSuitesSecuritySettings);
    void ReadCustomAttributesL(std::vector<MidletSuiteCustomAttributes*>& aMidletSuitesCustomAttributes);
    void WriteSecuritySettingsL(const std::vector<MidletSuiteSecuritySettings*>& aMidletSuitesSecuritySettings);
    void WriteCustomAttributesL(const std::vector<MidletSuiteCustomAttributes*>& aMidletSuitesCustomAttributes);
    bool IsInteractionModeAllowed(const std::wstring& aInteractionMode, const std::wstring& aAllowedInteractionModes);
    void findEntry(const JavaStorageApplicationList_t&, const std::wstring&, std::wstring& eValue);

private:
    RDbs iDbs;
    RDbNamedDatabase iUserDb;
    RDbTable  iSecurityPreferencesTable;
    RDbTable  iCustomAttributesTable;
    // security preferences columns
    TInt iSPNameColId;
    TInt iSPVendorColId;
    TInt iSPVersionColId;
    TInt iSPDomainColId;
    TInt iSPPreferencesColId;
    TInt iSPIdColId;
    // custom attributes columns
    TInt iCAIdColId;
    TInt iCAValueColId;
    JavaStorage* iStorage;
    MIDP::MSecurityPolicyV2* iSecurityPolicy;
};

} //end namespace usersettingsconfigurator
} //end namespace tools
} // end namespace java
#endif // STORAGEHANDLER_H

