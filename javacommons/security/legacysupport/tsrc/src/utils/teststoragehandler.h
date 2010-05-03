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

#include "javastorage.h"
#include "javastorageexception.h"
#include "javauid.h"

class TestStorageHandler
{
public:

    TestStorageHandler();
    virtual ~TestStorageHandler();
    void cleanStorage(const std::string& aTableName, const java::util::Uid aAppUID);
    void populateApplicationPackageTable(const java::util::Uid aAppUID, const std::wstring aAppName = L"", const std::wstring aAppVendor = L"", const std::wstring aAppVersion = L"");
    void populateMIDPPackageTable(const java::util::Uid aAppUID, const std::wstring aAppDomain = L"", const std::wstring aOnScreenKeypad = L"");
    void populateSecuritySettingsTable(const java::util::Uid aAppUID, const std::wstring aSettingsName = L"", const std::wstring aCurrentInteractionMode = L"", const std::wstring aAllowedInteractionModes = L"");
    void readAttribute(const std::string& aTableName, const java::util::Uid aAppUID, const std::wstring& aAttributeName, std::wstring& aAttributeValue);
    void readBlanketPrompt(const java::util::Uid aAppUID, const std::wstring& aSettingsName, std::wstring& aAttributeValue);

private:
    void findEntry(const java::storage::JavaStorageApplicationList_t&, const std::wstring&, std::wstring& eValue);
};

#endif // TESTSTORAGEHANDLER_H

