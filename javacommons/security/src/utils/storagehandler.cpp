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
#include "storagehandler.h"
#include "javacommonutils.h"
#include "javastoragenames.h"
#include "javastorage.h"
#include "logger.h"

using namespace std;
using namespace java::storage;
using namespace java::security;
using namespace java::util;

StorageHandler::StorageHandler()
{
    iStorage = JavaStorage::createInstance();
    iStorage->open(JAVA_DATABASE_NAME);
}

void StorageHandler::readValidCerts(const Uid& aUid, std::list<int>& aCerts)
{
    JavaStorageEntry attr;
    JavaStorageApplicationEntry_t entry;

    attr.setEntry(ID, aUid.toString());
    entry.insert(attr);

    attr.setEntry(VALID_CERTS, L"");
    entry.insert(attr);

    JavaStorageApplicationList_t foundApps;

    iStorage->search(MIDP_PACKAGE_TABLE, entry, foundApps);

    wstring certIndexes = L"";
    findEntry(foundApps, VALID_CERTS, certIndexes);

    wstring::size_type startIdx = 0;
    wstring::size_type endIdx = 0;
    const char delim = ',';

    while (wstring::npos != startIdx || wstring::npos != endIdx)
    {
        endIdx = certIndexes.find(delim, startIdx);
        if (endIdx == wstring::npos) break;    // Not found

        if (startIdx != endIdx)  // Skip 1,,2 case
        {
            addToValidCerts(certIndexes.substr(
                                startIdx, (endIdx - startIdx)), aCerts);
        }
        startIdx = endIdx + 1;
    }

    // Read last one
    if ((startIdx = certIndexes.find_last_of(delim)) != wstring::npos)
    {
        addToValidCerts(certIndexes.substr(startIdx + 1,
                                           (certIndexes.size() - startIdx)), aCerts);
    }

    // If only one cert index without delimiter
    if (aCerts.size() == 0 && certIndexes.size() > 0)
    {
        addToValidCerts(certIndexes, aCerts);
    }
}

void StorageHandler::getChainFromIndex(
    const JavaStorageApplicationList_t& aAttributes,
    const int aIndex,
    std::string& aChain)
{
    int i = 1;
    bool foundPart = true;
    const wstring attrPrefix = L"MIDlet-Certificate-";
    JavaStorageEntry attr;

    do
    {
        wstring indexedAttrName = attrPrefix;
        indexedAttrName.append(JavaCommonUtils::intToWstring(aIndex))
        .append(L"-").append(JavaCommonUtils::intToWstring(i));

        attr.setEntry(NAME, indexedAttrName);

        JavaStorageApplicationEntry_t findEntry;
        findEntry.insert(attr);

        bool found = false;
        JavaStorageApplicationList_t::const_iterator appIter;

        for (appIter = aAttributes.begin()
                       ; appIter != aAttributes.end()
                ; appIter++)
        {
            JavaStorageApplicationEntry_t::const_iterator entryFinder
            = (*appIter).find(attr);

            if (entryFinder != (*appIter).end()) // Correct one found.
            {
                attr.setEntry(VALUE, L"");
                entryFinder = (*appIter).find(attr); // Get its value.

                if (entryFinder != (*appIter).end())
                {
                    char* tmp = JavaCommonUtils::wstringToUtf8((*entryFinder).entryValue());
                    aChain.append(JavaCommonUtils::base64decode(tmp));
                    delete [] tmp;
                }

                found = true;
                break;
            }
        }

        if (!found)
        {
            foundPart = false;
        }

        i++;
    }
    while (foundPart);
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

void StorageHandler::addToValidCerts(
    const wstring& aValue, list<int>& aCerts)
{
    try
    {
        aCerts.push_back(JavaCommonUtils::wstringToInt(aValue));
    }
    catch (ExceptionBase& eb)
    {
        WLOG1WSTR(EJavaStorage,
                  "Invalid cert index skipped: %s", aValue);
    }
}

StorageHandler::~StorageHandler()
{
    if (NULL != iStorage)
    {
        iStorage->close();
        delete iStorage;
        iStorage = NULL;
    }
}
