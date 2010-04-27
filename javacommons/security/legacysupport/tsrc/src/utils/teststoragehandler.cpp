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

using namespace std;
using namespace java::storage;

TestStorageHandler::TestStorageHandler()
{
}

TestStorageHandler::~TestStorageHandler()
{
}

void TestStorageHandler::cleanStorage(const std::string& aTableName, const java::util::Uid aAppUID)
{
    JavaStorage* javaStorage = JavaStorage::createInstance();
    javaStorage->open(JAVA_DATABASE_NAME);
    javaStorage->startTransaction();
    JavaStorageApplicationEntry_t data;
    JavaStorageEntry attr;
    attr.setEntry(ID , aAppUID.toString());
    data.insert(attr);
    javaStorage->remove(aTableName, data);
    javaStorage->commitTransaction();
    javaStorage->close();
    delete javaStorage;
    javaStorage = NULL;
}

void TestStorageHandler::populateApplicationPackageTable(const java::util::Uid aAppUID, const std::wstring aAppName, const std::wstring aAppVendor, const std::wstring aAppVersion)
{
    JavaStorage* javaStorage = JavaStorage::createInstance();
    javaStorage->open(JAVA_DATABASE_NAME);
    javaStorage->startTransaction();
    JavaStorageApplicationEntry_t data;
    JavaStorageEntry attr;
    attr.setEntry(ID , aAppUID.toString());
    data.insert(attr);
    if (aAppName.size() > 0)
    {
        attr.setEntry(PACKAGE_NAME, aAppName);
        data.insert(attr);
    }
    if (aAppVendor.size() > 0)
    {
        attr.setEntry(VENDOR, aAppVendor);
        data.insert(attr);
    }
    if (aAppVersion.size() > 0)
    {
        attr.setEntry(VERSION, aAppVersion);
        data.insert(attr);
    }
    javaStorage->write(APPLICATION_PACKAGE_TABLE, data);
    javaStorage->commitTransaction();
    javaStorage->close();
    delete javaStorage;
    javaStorage = NULL;
}

void TestStorageHandler::populateMIDPPackageTable(const java::util::Uid aAppUID, const std::wstring aAppDomain, const std::wstring aOnScreenKeypad)
{
    JavaStorage* javaStorage = JavaStorage::createInstance();
    javaStorage->open(JAVA_DATABASE_NAME);
    javaStorage->startTransaction();
    JavaStorageApplicationEntry_t data;
    JavaStorageEntry attr;
    attr.setEntry(ID , aAppUID.toString());
    data.insert(attr);
    if (aAppDomain.size() > 0)
    {
        attr.setEntry(SECURITY_DOMAIN_CATEGORY , aAppDomain);
        data.insert(attr);
    }
    if (aOnScreenKeypad.size() > 0)
    {
        attr.setEntry(ON_SCREEN_KEYPAD, aOnScreenKeypad);
        data.insert(attr);
    }
    javaStorage->write(MIDP_PACKAGE_TABLE, data);
    javaStorage->commitTransaction();
    javaStorage->close();
    delete javaStorage;
    javaStorage = NULL;
}

void TestStorageHandler::populateSecuritySettingsTable(const java::util::Uid aAppUID, const std::wstring aSettingsName, const std::wstring aCurrentInteractionMode, const std::wstring aAllowedInteractionModes)
{
    JavaStorage* javaStorage = JavaStorage::createInstance();
    javaStorage->open(JAVA_DATABASE_NAME);
    javaStorage->startTransaction();
    JavaStorageApplicationEntry_t data;
    JavaStorageEntry attr;
    attr.setEntry(ID , aAppUID.toString());
    data.insert(attr);
    if (aSettingsName.size() > 0)
    {
        attr.setEntry(FUNCTION_GROUP , aSettingsName);
        data.insert(attr);
    }
    if (aCurrentInteractionMode.size() > 0)
    {
        attr.setEntry(CURRENT_SETTING , aCurrentInteractionMode);
        data.insert(attr);
    }
    if (aAllowedInteractionModes.size() > 0)
    {
        attr.setEntry(ALLOWED_SETTINGS , aAllowedInteractionModes);
        data.insert(attr);
    }
    javaStorage->write(MIDP_FUNC_GRP_SETTINGS_TABLE, data);
    javaStorage->commitTransaction();
    javaStorage->close();
    delete javaStorage;
    javaStorage = NULL;
}

void TestStorageHandler::readAttribute(const std::string& aTableName, const java::util::Uid aAppUID, const std::wstring& aAttributeName, std::wstring& aAttributeValue)
{
    JavaStorage* javaStorage = JavaStorage::createInstance();
    javaStorage->open(JAVA_DATABASE_NAME);
    javaStorage->startTransaction();
    JavaStorageApplicationEntry_t query;
    JavaStorageApplicationList_t queryResult;
    JavaStorageEntry attr;
    attr.setEntry(ID , aAppUID.toString());
    query.insert(attr);
    attr.setEntry(aAttributeName , L"");
    query.insert(attr);
    javaStorage->search(aTableName, query, queryResult);
    findEntry(queryResult, aAttributeName, aAttributeValue);
    javaStorage->commitTransaction();
    javaStorage->close();
    delete javaStorage;
    javaStorage = NULL;
}

void TestStorageHandler::readBlanketPrompt(const java::util::Uid aAppUID, const std::wstring& aSettingsName, std::wstring& aAttributeValue)
{
    JavaStorage* javaStorage = JavaStorage::createInstance();
    javaStorage->open(JAVA_DATABASE_NAME);
    javaStorage->startTransaction();
    JavaStorageApplicationEntry_t query;
    JavaStorageApplicationList_t queryResult;
    JavaStorageEntry attr;
    attr.setEntry(ID , aAppUID.toString());
    query.insert(attr);
    attr.setEntry(FUNCTION_GROUP , aSettingsName);
    query.insert(attr);
    attr.setEntry(BLANKET_PROMPT , L"");
    query.insert(attr);
    javaStorage->search(MIDP_FUNC_GRP_SETTINGS_TABLE, query, queryResult);
    findEntry(queryResult, BLANKET_PROMPT, aAttributeValue);
    javaStorage->commitTransaction();
    javaStorage->close();
    delete javaStorage;
    javaStorage = NULL;
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


