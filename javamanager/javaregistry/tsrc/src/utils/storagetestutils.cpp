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

#include "storagetestutils.h"
#include "javastorageexception.h"
#include "javacommonutils.h"
#include "javaoslayer.h"

#include "javastorage.h"
#include "javastorageentry.h"
#include "javastoragenames.h"
#include "javauid.h"

using namespace java::storage;
using namespace java::util;
using namespace std;

OS_EXPORT JavaStorageTestUtils::JavaStorageTestUtils()
{
}

OS_EXPORT JavaStorageTestUtils::~JavaStorageTestUtils()
{
}

/**
 * Compare two JavaStorageEntry to each other.
 *
 * @return true if match.
 */
OS_EXPORT bool JavaStorageTestUtils::compareEntry(
    const JavaStorageEntry& aRef,
    const JavaStorageEntry& aSource)
{
    bool equals = true;
    if (aRef.entryName() != aSource.entryName())
    {
        equals = false;
        printf("[JTU]: Entry name does not match\n");
        char* sqlInUtf = JavaCommonUtils::wstringToUtf8(aRef.entryName());
        printf("[JTU]: Entry Ref Name: '%s'", sqlInUtf);
        delete[] sqlInUtf;

        sqlInUtf = JavaCommonUtils::wstringToUtf8(aSource.entryName());
        printf(" Source Name: '%s'\n", sqlInUtf);
        delete[] sqlInUtf;
    }
    else if (aRef.entryValue() != aSource.entryValue())
    {
        equals = false;
        printf("[JTU]: Entry value does not match\n");
        char* sqlInUtf = JavaCommonUtils::wstringToUtf8(aRef.entryValue());
        printf("[JTU]: Entry Ref val: '%s'", sqlInUtf);
        delete[] sqlInUtf;

        sqlInUtf = JavaCommonUtils::wstringToUtf8(aSource.entryValue());
        printf(" Source val: '%s'\n", sqlInUtf);
        delete[] sqlInUtf;
    }

    return equals;
}

/**
 * Compare two ApplicationEntry to each other.
 *
 * @return true if match.
 */
OS_EXPORT bool JavaStorageTestUtils::compareEntries(
    const JavaStorageApplicationEntry_t& aRef,
    const JavaStorageApplicationEntry_t& aSource)
{
    bool equals = false;

    if (aRef.size() != aSource.size())
    {
        printf("[JTU]: Size does not match. RefSize: %d", aRef.size());
        printf(" SourceSize: %d\n", aSource.size());
        printf("Printing ref entry\n");
        printEntry(aRef);
        printf("Printing source entry\n");
        printEntry(aSource);

        return equals;
    }

    JavaStorageApplicationEntry_t::const_iterator refIter;

    // Go through all reference values.
    for (refIter = aRef.begin(); refIter != aRef.end(); refIter++)
    {
        // printf("[JTU]: Comparing entry\n");

        JavaStorageEntry refEntry = (*refIter);

        // Check source contains same entry.
        JavaStorageApplicationEntry_t::const_iterator sourceIterator =
            aSource.find(refEntry);

        if (sourceIterator != aSource.end())
        {
            JavaStorageEntry sourceEntry = (*sourceIterator);
            // Make sure it definitely is same.
            bool result = compareEntry(refEntry, sourceEntry);
            if (!result)
            {
                printf("[JTU]: Entry did not match\n");
                return equals;
            }
        }
        else
        {
            printf("[JTU]: No similar source entry found\n");
            return equals;
        }
    }
    return true;
}

OS_EXPORT bool JavaStorageTestUtils::populate(
    JavaStorage& aStorage,
    const string& aTableName,
    const JavaStorageApplicationEntry_t& aRef)
{
    bool success = false;

    try
    {
        aStorage.write(aTableName, aRef);
        success = true;
    }
    catch (JavaStorageException& jse)
    {
        printf("[JTU]: JSE while populating: %s\n", jse.toString().c_str());
    }
    catch (...)
    {
        printf("[JTU]: Unexcpedted exception thrown while populating\n");
    }

    return success;
}


OS_EXPORT bool JavaStorageTestUtils::update(
    JavaStorage& aStorage,
    const string& aTableName,
    const JavaStorageApplicationEntry_t& aUpdate,
    const JavaStorageApplicationEntry_t& aMatch)
{
    bool success = false;

    try
    {
        aStorage.update(aTableName, aUpdate, aMatch);
        success = true;
    }
    catch (JavaStorageException& jse)
    {
        printf("[JTU]: JSE while updating: %s\n", jse.toString().c_str());
    }
    catch (...)
    {
        printf("[JTU]: Unexcpedted exception thrown while updating\n");
    }

    return success;
}

OS_EXPORT bool JavaStorageTestUtils::checkParams(
    JavaStorage& aStorage,
    const string& aTableName,
    const JavaStorageApplicationEntry_t& aRef,
    int aFoundApps)
{
    bool equals = false;

    try
    {
        JavaStorageApplicationList_t foundApps;

        aStorage.search(aTableName, aRef, foundApps);

        if (foundApps.size() != aFoundApps)
        {
            printf("[JTU]: Invalid amount of applications received: ");
            printf("Expected: %d", aFoundApps);
            printf(" Found: %d\n", foundApps.size());

            /*            // ################################

                        JavaStorageApplicationList_t::const_iterator appIter;
                        appIter = foundApps.begin();

                        if (foundApps.size() > 0)
                        {
                            printEntry((*appIter));
                            appIter++;
                        }
                        if (foundApps.size() > 1)
                        {
                            printEntry((*appIter));
                        }

                        // ##################################
            */
            return false;
        }
        else
        {
            JavaStorageApplicationList_t::const_iterator appIter;

            // Go through all entries
            for (appIter = foundApps.begin(); appIter != foundApps.end(); appIter++)
            {
                // printf("Comparing application\n");
                JavaStorageApplicationEntry_t app = (*appIter);

                equals = compareEntries(aRef, app);
                app.clear();
                if (!equals)
                {
                    printf("Applications did not match\n");
                    return false;
                }
            }
        }
        foundApps.clear();

        equals = true;
    }
    catch (JavaStorageException& jse)
    {
        printf("[JTU]: JSE while checking: %s\n", jse.toString().c_str());
    }
    catch (...)
    {
        printf("[JTU]: Unexcpedted exception thrown while checking\n");
    }

    return equals;
}

OS_EXPORT bool JavaStorageTestUtils::remove(
    JavaStorage& aStorage,
    const string& aTableName,
    const JavaStorageApplicationEntry_t& aRef)
{
    bool success = false;

    try
    {
        aStorage.remove(aTableName, aRef);

        JavaStorageApplicationList_t foundApps;

        // Verify removal
        aStorage.search(aTableName, aRef, foundApps);
        // printf("[JTU]: Entries found: %d\n", foundApps.size());
        if (foundApps.size() == 0)
        {
            success = true;
        }
    }
    catch (JavaStorageException& jse)
    {
        printf("[JTU]: JSE while removing: %s\n", jse.toString().c_str());
    }
    catch (...)
    {
        printf("[JTU]: Unexcpedted exception thrown while removing\n");
    }

    return success;
}

OS_EXPORT void JavaStorageTestUtils::createAppDataEntry(
    JavaStorageApplicationEntry_t& aEntry)
{
    wstring name = PACKAGE_NAME;
    wstring nameValue = L"MyDefaultMIDlet";
    JavaStorageEntry entry;

    entry.setEntry(name, nameValue);
    aEntry.insert(entry);

    name = VENDOR;
    nameValue = L"MyDefaultVendor";
    entry.setEntry(name, nameValue);
    aEntry.insert(entry);

    name = VERSION;
    nameValue = L"12.12.12";
    entry.setEntry(name, nameValue);
    aEntry.insert(entry);

    name = ROOT_PATH;
    nameValue = L"/home/default";
    entry.setEntry(name, nameValue);
    aEntry.insert(entry);

    name = MEDIA_ID;
    nameValue = L"/mnt/DefaultMMC";
    entry.setEntry(name, nameValue);
    aEntry.insert(entry);

    name = INITIAL_SIZE;
    nameValue = L"1234";
    entry.setEntry(name, nameValue, JavaStorageEntry::INT);
    aEntry.insert(entry);

    name = JAD_PATH;
    nameValue = L"/usr/local/defaultApp/JAD";
    entry.setEntry(name, nameValue);
    aEntry.insert(entry);

    name = JAR_PATH;
    nameValue = L"/usr/local/defaultApp/JAR";
    entry.setEntry(name, nameValue);
    aEntry.insert(entry);

    name = ACCESS_POINT;
    nameValue = L"Default Access Point";
    entry.setEntry(name, nameValue);
    aEntry.insert(entry);
}

OS_EXPORT void JavaStorageTestUtils::createDefaultEntry(
    JavaStorageApplicationEntry_t& aEntry)
{
    JavaStorageEntry entry;
    entry.setEntry(PACKAGE_NAME, L"BasicGame");
    aEntry.insert(entry);

    entry.setEntry(VENDOR, L"GreatVendor");
    aEntry.insert(entry);

    entry.setEntry(VERSION, L"18.01.91");
    aEntry.insert(entry);
}

OS_EXPORT void JavaStorageTestUtils::printEntry(
    JavaStorageApplicationEntry_t aEntry)
{
    JavaStorageApplicationEntry_t::const_iterator iter;

    printf("    [JTU]: Printing entry\n");

    // Go through all reference values.
    for (iter = aEntry.begin(); iter != aEntry.end(); iter++)
    {
        JavaStorageEntry entry = (*iter);
        char* name = JavaCommonUtils::wstringToUtf8(entry.entryName());
        char* value = JavaCommonUtils::wstringToUtf8(entry.entryValue());
        printf("    Entry: %s=", name);
        printf("'%s'\n", value);

        delete[] name;
        delete[] value;
    }
    printf("    [JTU]: Printing entry DONE\n");

}
OS_EXPORT void JavaStorageTestUtils::databasePath(std::string& aDatabasePath)
{
    JavaOsLayer::getOsSpecificJavaRootPath(aDatabasePath);

    int endIdx = (aDatabasePath.size() - 1);

    if (aDatabasePath[endIdx] != '/' && aDatabasePath[endIdx] != '\\')
    {
        // Detect used slash char
        // This do not work if root path is without slashes. Then windows
        // path is expected.
        int idx = aDatabasePath.find("/");

        if (idx != -1)
        {
            aDatabasePath.append("/bin/");
        }
        else
        {
            aDatabasePath.append("\\bin\\");
        }
    }
}

