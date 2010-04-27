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
#ifndef TESTSTORAGEHANDLER_H
#define TESTSTORAGEHANDLER_H

#include <e32base.h>
#include <d32dbms.h>
#include <vector>
#include <string>
#include "javastorage.h"

class SecuritySettings
{
public:
    SecuritySettings(const std::wstring& aSettingsName,
                     const std::wstring& aCurrentInteractionMode);
    SecuritySettings &operator=(const SecuritySettings&);
    SecuritySettings();

    std::wstring iSettingsName;
    std::wstring iCurrentInteractionMode;
};

class TestStorageHandler : public CBase
{
public:

    static TestStorageHandler* NewL();
    virtual ~TestStorageHandler();
    void cleanup(const TUint32 aId, const std::wstring& aMidletSuiteUid);
    void populateLegacyStorageL(TUint32 aId, const TDesC& aName, const TDesC& aVendor, const TDesC& aVersion, const TDesC8& aSecurityPrefs);
    void populateLegacyStorageL(TUint32 aId, const TDesC8& aCustomAttributes);
    void populateOmjStorageL(const std::wstring& aMidletSuiteUid, const std::wstring& aMidletSuiteName, const std::wstring& aMidletSuiteVendor, const std::wstring& aMidletSuiteVersion, const std::wstring& aSecuritySettingsName, const std::wstring& aCurrentInteractionMode, const std::wstring& aAllowedInteractionModes = L"");
    void populateOmjStorageL(const std::wstring& aMidletSuiteUid, const std::wstring& aOnScreenKeypad);
    void retrieveSecuritySettings(const std::wstring& aMidletSuiteName, const std::wstring& aMidletSuiteVendor, const std::wstring& aMidletSuiteVersion, std::vector<SecuritySettings*>& aSecuritySettings);
    void retrieveOnScreenKeypad(const std::wstring& aMidletSuiteUid, std::wstring& aOnScreenKeypad);

private:
    TestStorageHandler();
    void ConstructL();
    void findColumn(const java::storage::JavaStorageApplicationEntry_t& aEntry, const std::wstring& aColName, std::wstring& aColValue);
    void findEntry(const java::storage::JavaStorageApplicationList_t&, const std::wstring&, std::wstring& eValue);
    RDbs iDbs;
    RDbNamedDatabase iUserDb;
    RDbTable  iSecurityPrefsTable;
    RDbTable  iCustomAttributesTable;
    TInt iMSIdColId;
    TInt iMSNameColId;
    TInt iMSVendorColId;
    TInt iMSVersionColId;
    TInt iPreferencesColId;
    TInt iCAIdColId;
    TInt iCAValueColId;
    java::storage::JavaStorage* iStorage;
};

#endif // TESTSTORAGEHANDLER_H

