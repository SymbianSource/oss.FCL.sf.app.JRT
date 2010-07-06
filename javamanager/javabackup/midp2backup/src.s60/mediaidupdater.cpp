/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MediaIdUpdater
*
*/


#include <algorithm>
#include <memory>

#include "mediaidupdater.h"
#include "logger.h"
#include "javacommonutils.h"
#include "driveutilities.h"
#include "javastorageentry.h"
#include "javastorage.h"
#include "javastoragenames.h"

using namespace java::backup;
using namespace java::storage;
using namespace java::util;


MediaIdUpdater::MediaIdUpdater() : mStorage(0)
{
}

MediaIdUpdater::~MediaIdUpdater()
{
    close();
}

/**
* Ensures that java storage does not contain media ids that are not valid any more.
* Media id updating is done in following steps:
*   - get media ids from storage
*   - get present (valid) media ids
*   - get difference between storage and present ids (result is invalid ids that need to be updated)
*   - update invalid media ids with valid ids
* @param -
* @return -
*/
void MediaIdUpdater::update()
{
    try
    {
        open();
        std::set<std::wstring> storageIds = getMediaIdsFromStorage();
        std::set<std::wstring> presentIds = getPresentMediaIds();

        std::set<std::wstring> invalidIds = removePresentMediaIds(storageIds, presentIds);
        std::set<std::wstring> validIds = getPresentRemovableDriveMediaIds();

        if (isMediaIdUpdateNeeded(invalidIds, validIds))
        {
            updateMediaIds(invalidIds, validIds);
        }
        close();
    }
    catch (JavaStorageException jse)
    {
        ELOG1(EBackup, "MediaIdUpdater::update() failed: %S", jse.toString().c_str());
    }
}

/**
* Gets present media ids. Result includes phone, internal and mass memory drive ids if any
* @param -
* @return present media ids
*/
std::set<std::wstring> MediaIdUpdater::getPresentMediaIds()
{
    std::set<std::wstring> result;

    java::fileutils::driveInfos drives;
    java::fileutils::DriveUtilities::getAccesibleDrives(drives);

    PLOG1(EBackup, "getPresentMediaIds(count=%d)", drives.size());
    for (java::fileutils::driveInfos::const_iterator iter = drives.begin(); iter != drives.end(); iter++)
    {
        PLOG2(EBackup, "  %S - id=%d", (*iter).iRootPath.c_str(), (*iter).iId);
        std::wstring mediaId = JavaCommonUtils::intToWstring((*iter).iId);
        result.insert(mediaId);
    }

    result.insert(L"0"); // default set by javainstaller for internal drives
    return result;
}

/**
* Gets present removable media ids. Result mass memory drive ids if any
* @param -
* @return present removable media ids
*/
std::set<std::wstring> MediaIdUpdater::getPresentRemovableDriveMediaIds()
{
    std::set<std::wstring> result;

    java::fileutils::driveInfos drives;
    java::fileutils::DriveUtilities::getAccesibleDrives(drives);

    PLOG1(EBackup, "getPresentRemovableDriveMediaIds(count=%d)", drives.size());
    for (java::fileutils::driveInfos::const_iterator iter = drives.begin(); iter != drives.end(); iter++)
    {
        if ((*iter).iIsRemovable)
        {
            PLOG2(EBackup, "  %S - id=%d", (*iter).iRootPath.c_str(), (*iter).iId);
            std::wstring mediaId = JavaCommonUtils::intToWstring((*iter).iId);
            result.insert(mediaId);
        }
    }

    return result;
}

/**
* Filters present media ids from given set.
* @param[in] aStorageIds original media ids
* @param[in] aPresentIds present media ids
* @return set of media ids that are invalid i.e. not present at the moment
*/
std::set<std::wstring> MediaIdUpdater::removePresentMediaIds(const std::set<std::wstring>& aStorageIds,
                                           const std::set<std::wstring>& aPresentIds)
{
    std::set<std::wstring> result;

    std::set_difference(aStorageIds.begin(), aStorageIds.end(),
                        aPresentIds.begin(), aPresentIds.end(),
                        std::inserter(result, result.end()));

    return result;
}

/**
* Checks if there is a need for updating given media ids
* @param[in] aInvalidIds media ids to be updated
* @param[in] aValidIds valid ids used during update
* @return true, if media ids need to be updated, false otherwise
*/
bool MediaIdUpdater::isMediaIdUpdateNeeded(const std::set<std::wstring>& aInvalidIds,
                           const std::set<std::wstring>& aValidIds)
{
    if(aInvalidIds.size() > 0 && aValidIds.size() > 0)
    {
        return true;
    }
    return false;
}

/**
* Gets media ids that can be found from java storage.
* @param -
* @return media ids found from java storage
*/
std::set<std::wstring> MediaIdUpdater::getMediaIdsFromStorage()
{
    std::set<std::wstring> result;

    JavaStorageEntry attribute;
    attribute.setEntry(MEDIA_ID, L"");

    JavaStorageApplicationEntry_t findQuery;
    findQuery.insert(attribute);

    JavaStorageApplicationList_t foundEntries;

    mStorage->search(APPLICATION_PACKAGE_TABLE, findQuery, foundEntries);
    PLOG1(EBackup, "getMediaIdsFromStorage(count=%d)", foundEntries.size());
    for (JavaStorageApplicationList_t::const_iterator iter = foundEntries.begin(); iter != foundEntries.end(); iter++)
    {
        std::wstring id = (*iter).begin()->entryValue();
        PLOG1(EBackup, "  id=%S", id.c_str());
        result.insert(id);
    }

    return result;
}

/**
* Updates given media ids with new values and saves the result in java storage.
* @param[in] aInvalidIds media ids to be updated
* @param[in] aValidIds valid ids used during update
* @return -
*/
void MediaIdUpdater::updateMediaIds(const std::set<std::wstring>& aInvalidIds,
                    const std::set<std::wstring>& aValidIds)
{
    if (aValidIds.size() == 0)
    {
        return;
    }
    // use the first present media id for all
    std::wstring newId = *(aValidIds.begin());

    mStorage->startTransaction();

    PLOG1(EBackup, "updateMediaIds(count=%d)", aInvalidIds.size());
    for (std::set<std::wstring>::const_iterator iter = aInvalidIds.begin(); iter != aInvalidIds.end(); ++iter)
    {
        std::wstring origId = (*iter).c_str();
        PLOG2(EBackup, "  origId=%S - newId=%S", origId.c_str(), newId.c_str());

        JavaStorageEntry attribute;
        attribute.setEntry(MEDIA_ID, newId);

        JavaStorageApplicationEntry_t updateEntry;
        updateEntry.insert(attribute);

        JavaStorageApplicationEntry_t matchEntry;
        attribute.setEntry(MEDIA_ID, origId);
        matchEntry.insert(attribute);

        mStorage->update(APPLICATION_PACKAGE_TABLE, updateEntry, matchEntry);
    }

    mStorage->commitTransaction();
}

/**
* Opens java storage connection
* @param -
* @return -
*/
void MediaIdUpdater::open()
{
    close();
    mStorage = JavaStorage::createInstance();
    mStorage->open(JAVA_DATABASE_NAME);
}

/**
* Closes java storage connection
* @param -
* @return -
*/
void MediaIdUpdater::close()
{
    try
    {
        if (mStorage)
        {
            mStorage->close();
            delete mStorage;
            mStorage = 0;
        }
    }
    catch (JavaStorageException jse)
    {
        ELOG1(EBackup, "MediaIdUpdater::close() failed: %S", jse.toString().c_str());
    }
}

