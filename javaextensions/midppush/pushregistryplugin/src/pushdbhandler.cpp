/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "logger.h"
#include "pushdbhandler.h"
#include "javacommonutils.h"
#include "pusherrorcodes.h"
#include "pushexception.h"
#include "javastorageexception.h"
#include "javastoragenames.h"

using namespace java::push;
using namespace java::storage;
using namespace java::util;

/**
 *
 */
PushDBHandler::PushDBHandler()
        : mDbStorage(NULL)
{
    JELOG2(EJavaPush);
}

/**
 *
 */
PushDBHandler::~PushDBHandler()
{
    JELOG2(EJavaPush);
}

/**
 *
 */
void PushDBHandler::storeDynamicPushRegistration(const java::util::Uid& aUid,
        const std::wstring& aUri,
        const std::wstring& aMidletName,
        const std::wstring& aFilter)
{
    JELOG2(EJavaPush);
    try
    {
        JavaStorageApplicationEntry_t entries;
        JavaStorageEntry entry;
        entry.setEntry(URL,aUri,JavaStorageEntry::STRING);
        entries.insert(entry);
        entry.setEntry(NAME,aMidletName,JavaStorageEntry::STRING);
        entries.insert(entry);
        entry.setEntry(FILTER,aFilter,JavaStorageEntry::STRING);
        entries.insert(entry);
        entry.setEntry(REGISTRATION_TYPE,L"0",JavaStorageEntry::INT);
        entries.insert(entry);

        entry.setEntry(ID, aUid.toString());
        entries.insert(entry);

        //ScopedLock lockObj(mMutex);
        openDbStorage();
        mDbStorage->startTransaction();
        mDbStorage->write(PUSH_REGISTRATIONS_TABLE,entries);
        mDbStorage->commitTransaction();
        mDbStorage->close();
    }
    catch (JavaStorageException& ex)
    {
        if (0 != mDbStorage.get())
        {
            mDbStorage->rollbackTransaction();
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,ex.toString(),__FILE__,__FUNCTION__,__LINE__);
    }
    catch (...)
    {
        WLOG(EJavaPush,"ERROR!!! Unexpected error in storeDynamicPushRegistration() operation");
        if (0 != mDbStorage.get())
        {
            mDbStorage->rollbackTransaction();
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,"Unexpected exception occurred",__FILE__,__FUNCTION__,__LINE__);
    }
}

/**
 *
 */
void PushDBHandler::unregisterDynamicPushRegistration(const java::util::Uid& aUid,
        const std::wstring& aUri)
{
    JELOG2(EJavaPush);
    try
    {
        JavaStorageApplicationEntry_t entries;
        JavaStorageEntry entry;
        entry.setEntry(ID,aUid.toString(),JavaStorageEntry::STRING);
        entries.insert(entry);
        entry.setEntry(URL,aUri,JavaStorageEntry::STRING);
        entries.insert(entry);
        entry.setEntry(REGISTRATION_TYPE,L"0",JavaStorageEntry::INT);
        entries.insert(entry);

        //ScopedLock lockObj(mMutex);
        openDbStorage();

        mDbStorage->startTransaction();
        mDbStorage->remove(PUSH_REGISTRATIONS_TABLE,entries);
        mDbStorage->commitTransaction();
        mDbStorage->close();
    }
    catch (PushException& ex)
    {
        if (0 != mDbStorage.get())
        {
            mDbStorage->close();
        }
        throw;
    }
    catch (JavaStorageException& ex)
    {
        if (0 != mDbStorage.get())
        {
            mDbStorage->rollbackTransaction();
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,ex.toString(),__FILE__,__FUNCTION__,__LINE__);
    }
    catch (...)
    {
        WLOG(EJavaPush,"ERROR!!! Unexpected error in unregisterDynamicPushRegistration() operation");
        if (0 != mDbStorage.get())
        {
            mDbStorage->rollbackTransaction();
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,"Unexpected exception occurred",__FILE__,__FUNCTION__,__LINE__);
    }
}

/**
 *
 */
void PushDBHandler::storeAlarm(const Uid& aUid,const long long& aAlarmInMilliSecs)
{
    JELOG2(EJavaPush);
    try
    {
        JavaStorageApplicationEntry_t entries;
        JavaStorageEntry entry;

        std::wstring timeStr = JavaCommonUtils::longLongToWstring(aAlarmInMilliSecs);
        entry.setEntry(ALARM_TIME,timeStr,JavaStorageEntry::STRING);
        entries.insert(entry);
        entry.setEntry(ID, aUid.toString());
        entries.insert(entry);

        //ScopedLock lockObj(mMutex);
        openDbStorage();
        mDbStorage->startTransaction();
        mDbStorage->write(ALARM_REGISTRATIONS_TABLE,entries);
        mDbStorage->commitTransaction();
        mDbStorage->close();
    }
    catch (JavaStorageException& ex)
    {
        if (0 !=mDbStorage.get())
        {
            mDbStorage->rollbackTransaction();
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,ex.toString(),__FILE__,__FUNCTION__,__LINE__);
    }
    catch (...)
    {
        WLOG(EJavaPush,"ERROR!!! Unexpected error in storeAlarm() operation");
        if (0 != mDbStorage.get())
        {
            mDbStorage->rollbackTransaction();
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,"Unexpected exception occurred",__FILE__,__FUNCTION__,__LINE__);
    }
}

/**
 *
 */
void PushDBHandler::deleteAlarm(const Uid& aUid)
{
    JELOG2(EJavaPush);
    try
    {
        JavaStorageApplicationEntry_t entries;
        JavaStorageEntry entry;
        entry.setEntry(ID, aUid.toString());
        entries.insert(entry);

        //ScopedLock lockObj(mMutex);
        openDbStorage();
        mDbStorage->startTransaction();
        mDbStorage->remove(ALARM_REGISTRATIONS_TABLE,entries);
        mDbStorage->commitTransaction();
        mDbStorage->close();
    }
    catch (JavaStorageException& ex)
    {
        if (0 !=mDbStorage.get())
        {
            mDbStorage->rollbackTransaction();
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,ex.toString(),__FILE__,__FUNCTION__,__LINE__);
    }
    catch (...)
    {
        WLOG(EJavaPush,"ERROR!!! Unexpected error in deleteAlarm() operation");
        if (0 != mDbStorage.get())
        {
            mDbStorage->rollbackTransaction();
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,"Unexpected exception occurred",__FILE__,__FUNCTION__,__LINE__);
    }
}

/**
 *
 */
void PushDBHandler::getPushRegs(const java::util::Uid& aUid,
                                java::storage::JavaStorageApplicationList_t& aPushRegs)
{
    JELOG2(EJavaPush);
    try
    {
        JavaStorageApplicationEntry_t findPatterns;
        JavaStorageEntry findPattern;
        if (!aUid.isEmpty())
        {
            findPattern.setEntry(ID,aUid.toString(),JavaStorageEntry::STRING);
            findPatterns.insert(findPattern);
        }

        //ScopedLock lockObj(mMutex);
        openDbStorage();
        mDbStorage->search(PUSH_REGISTRATIONS_TABLE,findPatterns,aPushRegs);
        mDbStorage->close();
    }
    catch (JavaStorageException& ex)
    {
        if (0 !=mDbStorage.get())
        {
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,ex.toString(),__FILE__,__FUNCTION__,__LINE__);
    }
    catch (...)
    {
        WLOG(EJavaPush,"ERROR!!! Unexpected error in getPushRegs() operation");
        if (0 != mDbStorage.get())
        {
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,"Unexpected exception occurred",__FILE__,__FUNCTION__,__LINE__);
    }
}

/**
 *
 */
void PushDBHandler::getAlarms(const java::util::Uid& aUid,
                              std::list<DbAlarmData>& aAlarmList)
{
    JELOG2(EJavaPush);
    try
    {
        JavaStorageEntry findPattern;
        JavaStorageApplicationEntry_t findPatterns;
        JavaStorageApplicationList_t foundEntries;
        if (!aUid.isEmpty())
        {
            findPattern.setEntry(ID,aUid.toString(),JavaStorageEntry::STRING);
            findPatterns.insert(findPattern);
        }

        //ScopedLock lockObj(mMutex);
        openDbStorage();
        mDbStorage->search(ALARM_REGISTRATIONS_TABLE,findPatterns,foundEntries);
        fillAlarmData(aAlarmList,foundEntries);
        mDbStorage->close();
    }
    catch (JavaStorageException& ex)
    {
        if (0 !=mDbStorage.get())
        {
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,ex.toString(),__FILE__,__FUNCTION__,__LINE__);
    }
    catch (...)
    {
        WLOG(EJavaPush,"ERROR!!! Unexpected error in getAlarms() operation");
        if (0 != mDbStorage.get())
        {
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,"Unexpected exception occurred",__FILE__,__FUNCTION__,__LINE__);
    }
}

/**
 *
 */
void PushDBHandler::getMidletSuiteInfo(const java::util::Uid& aSuiteUid,
                                       std::multimap<Uid,
                                       MidletSuiteData>& aMidletSuiteInfo)
{
    JELOG2(EJavaPush);

    getMidletSuiteInfo(aSuiteUid,PACKAGE_ID,aMidletSuiteInfo);
}

/**
 *
 */
void PushDBHandler::getMidletSuiteInfoByMidletUid
(const Uid& aMidletUid,std::multimap<Uid,MidletSuiteData>& aMidletSuiteInfo)
{
    JELOG2(EJavaPush);

    getMidletSuiteInfo(aMidletUid,ID,aMidletSuiteInfo);
}

/**
 *
 */
void PushDBHandler::getMidletSuiteInfo(const java::util::Uid& aUid,
                                       const std::wstring& aSearchColumn,
                                       std::multimap<Uid,MidletSuiteData>& aMidletSuiteInfo)
{
    JELOG2(EJavaPush);

    try
    {
        JavaStorageEntry findPattern;
        JavaStorageApplicationEntry_t findPatterns;
        JavaStorageApplicationList_t foundEntries;
        if (!aUid.isEmpty())
        {
            //Search condition can be PACKAGE_ID or ID.
            findPattern.setEntry(aSearchColumn,aUid.toString(),JavaStorageEntry::STRING);
            findPatterns.insert(findPattern);
        }

        findPattern.setEntry(PACKAGE_ID,L"");
        findPatterns.insert(findPattern);
        findPattern.setEntry(ID,L"");
        findPatterns.insert(findPattern);
        findPattern.setEntry(MAIN_CLASS,L"");
        findPatterns.insert(findPattern);

        //ScopedLock lockObj(mMutex);
        openDbStorage();
        mDbStorage->search(APPLICATION_TABLE,findPatterns,foundEntries);
        fillMidletSuiteInfo(aMidletSuiteInfo,foundEntries);
        addMediaId(*mDbStorage.get(),aMidletSuiteInfo);
        mDbStorage->close();
    }
    catch (ExceptionBase& ex)
    {
        if (0 !=mDbStorage.get())
        {
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,ex.toString(),__FILE__,__FUNCTION__,__LINE__);
    }
    catch (...)
    {
        WLOG(EJavaPush,"ERROR!!! Unexpected error in getAlarms() operation");
        if (0 != mDbStorage.get())
        {
            mDbStorage->close();
        }
        throw PushException(DB_ERROR,"Unexpected exception occurred",__FILE__,__FUNCTION__,__LINE__);
    }
}

/**
 *
 */
void PushDBHandler::addMediaId(JavaStorage& aStorageObj,
                               std::multimap<Uid,MidletSuiteData>& aMidletSuiteInfo)
{
    JELOG2(EJavaPush);

    std::map<Uid,int> mediaIdContainer;
    std::map<Uid,int>::iterator mediaIdContainerIter;

    std::multimap<Uid,MidletSuiteData>::iterator iter;
    for (iter = aMidletSuiteInfo.begin(); iter != aMidletSuiteInfo.end(); ++iter)
    {
        if (UNDEFINED_MEDIA_ID == iter->second.mMediaId)
        {
            mediaIdContainerIter = mediaIdContainer.find(iter->first);
            if (mediaIdContainer.end() == mediaIdContainerIter)
            {
                getMediaId(aStorageObj,iter->first,mediaIdContainer);
                mediaIdContainerIter = mediaIdContainer.find(iter->first);
                if (mediaIdContainer.end() == mediaIdContainerIter)
                {
                    throw PushException(DB_ERROR,"media id by suite uid is not found",
                                        __FILE__,__FUNCTION__,__LINE__);
                }
            }
            iter->second.mMediaId = mediaIdContainerIter->second;
        }//end if(-1 == iter->second.mMediaId)
    }//end for

    mediaIdContainer.clear();
}

/**
 *
 */
void PushDBHandler::getMediaId(JavaStorage& aStorageObj,const Uid& aSuiteUid,
                               std::map<Uid,int>& aMediaIdContainer)
{
    JavaStorageEntry findPattern;
    JavaStorageApplicationEntry_t findPatterns;
    JavaStorageApplicationList_t foundEntries;

    findPattern.setEntry(ID,aSuiteUid.toString(),JavaStorageEntry::STRING);
    findPatterns.insert(findPattern);
    findPattern.setEntry(MEDIA_ID,L"");
    findPatterns.insert(findPattern);

    aStorageObj.search(APPLICATION_PACKAGE_TABLE,findPatterns,foundEntries);
    fillMidletId(aSuiteUid,aMediaIdContainer,foundEntries);
}

/**
 *
 */
void PushDBHandler::fillMidletId(const Uid& aSuiteUid,
                                 std::map<Uid,int>& aMediaIdContainer,
                                 JavaStorageApplicationList_t& aFoundEntries)
{
    JavaStorageEntry findPatternForMediaId;
    std::wstring empty = L"";
    findPatternForMediaId.setEntry(MEDIA_ID,empty);

    JavaStorageApplicationList_t::iterator iter;
    for (iter = aFoundEntries.begin(); iter != aFoundEntries.end(); ++iter)
    {
        int mediaId = 0;
        if ((true == getDbValueAsInt(iter,findPatternForMediaId,mediaId)))
            aMediaIdContainer.insert(std::pair<Uid,int>(aSuiteUid,mediaId));
    }//end for
}

/**
 *
 */
void PushDBHandler::fillAlarmData(std::list<DbAlarmData>& aAlarmList,
                                  JavaStorageApplicationList_t& aFoundEntries)
{
    JELOG2(EJavaPush);

    JavaStorageEntry findPatternForUid;
    std::wstring empty = L"";
    findPatternForUid.setEntry(ID,empty);

    JavaStorageEntry findPatternForTime;
    findPatternForTime.setEntry(ALARM_TIME,empty);

    JavaStorageApplicationList_t::iterator iter;
    for (iter = aFoundEntries.begin(); iter != aFoundEntries.end(); ++iter)
    {
        // Get application ID.
        Uid tmpUid;
        long long tmpTime = 0LL;
        if ((true == getDbValueAsUid(iter,findPatternForUid,tmpUid))
                && (true == getDbValueAsLongLong(iter,findPatternForTime,tmpTime)))
        {
            DbAlarmData alarmData(tmpUid,tmpTime,true);
            aAlarmList.push_back(alarmData);
        }
    }//end for
}

/**
 *
 */
void PushDBHandler::fillMidletSuiteInfo(std::multimap<Uid,MidletSuiteData>& aMidletSuiteInfo,
                                        JavaStorageApplicationList_t& aFoundEntries)
{
    JELOG2(EJavaPush);

    JavaStorageEntry findPatternForSuiteUid;
    std::wstring empty = L"";
    findPatternForSuiteUid.setEntry(PACKAGE_ID,empty);

    JavaStorageEntry findPatternForMidletUid;
    findPatternForMidletUid.setEntry(ID,empty);

    JavaStorageEntry findPatternForMainClass;
    findPatternForMainClass.setEntry(MAIN_CLASS,empty);

    JavaStorageApplicationList_t::iterator iter;
    for (iter = aFoundEntries.begin(); iter != aFoundEntries.end(); ++iter)
    {
        Uid tmpSuiteUid;
        Uid tmpMidletUid;
        std::wstring className;
        if ((true == getDbValueAsUid(iter,findPatternForSuiteUid,tmpSuiteUid))
                && (true == getDbValueAsUid(iter,findPatternForMidletUid,tmpMidletUid))
                && (true == getDbValueAsWStr(iter,findPatternForMainClass,className)))
        {
            MidletSuiteData dataObj(tmpMidletUid,className);
            aMidletSuiteInfo.insert(std::pair<Uid,MidletSuiteData>(tmpSuiteUid,dataObj));
        }
    }//end for
}

/**
 *
 */
bool PushDBHandler::getDbValueAsUid(JavaStorageApplicationList_t::iterator& aIter,
                                    JavaStorageEntry& aFindPattern,Uid& aOutput)
{
    JavaStorageApplicationEntry_t::iterator tmpIter = aIter->find(aFindPattern);
    if (tmpIter != aIter->end())
    {
        std::wstring uidAsStr = tmpIter->entryValue();
        if (0 != uidAsStr.compare(L""))
        {
            Uid tmpUid(uidAsStr);
            aOutput = uidAsStr;
            return true;
        }
    }//end if(tmpIter != aIter->end())
    return false;
}

/**
 *
 */
bool PushDBHandler::getDbValueAsLongLong(JavaStorageApplicationList_t::iterator& aIter,
        JavaStorageEntry& aFindPattern,long long& aOutput)
{
    JavaStorageApplicationEntry_t::iterator tmpIter = aIter->find(aFindPattern);
    if (tmpIter != aIter->end())
    {
        std::wstring longLongAsStr = tmpIter->entryValue();
        if (0 != longLongAsStr.compare(L""))
        {
            aOutput = JavaCommonUtils::wstringToLongLong(longLongAsStr);
            return true;
        }
    }//end if(tmpIter != aIter->end())
    return false;
}

/**
 *
 */
bool PushDBHandler::getDbValueAsInt(JavaStorageApplicationList_t::iterator& aIter,
                                    JavaStorageEntry& aFindPattern,int& aOutput)
{
    JavaStorageApplicationEntry_t::iterator tmpIter = aIter->find(aFindPattern);
    if (tmpIter != aIter->end())
    {
        std::wstring intAsStr = tmpIter->entryValue();
        if (0 != intAsStr.compare(L""))
        {
            aOutput = JavaCommonUtils::wstringToInt(intAsStr);
            return true;
        }
    }//end if(tmpIter != aIter->end())
    return false;
}

/**
 *
 */
bool PushDBHandler::getDbValueAsWStr(JavaStorageApplicationList_t::iterator& aIter,
                                     JavaStorageEntry& aFindPattern,std::wstring& aOutput)
{
    JavaStorageApplicationEntry_t::iterator tmpIter = aIter->find(aFindPattern);
    if (tmpIter != aIter->end())
    {
        aOutput = tmpIter->entryValue();
        if (0 != aOutput.compare(L""))
        {
            return true;
        }
    }//end if(tmpIter != aIter->end())
    return false;
}

/**
 *
 */
void PushDBHandler::openDbStorage()
{
    JELOG2(EJavaPush);

    if (0 == mDbStorage.get())
        mDbStorage.reset(JavaStorage::createInstance());

    mDbStorage->open("JavaStorage");
}

/**
 * IMPLEMENTATION OF DbAlarmData CLASS.
 */
DbAlarmData::DbAlarmData(const Uid& aUid,const long long& aTimeInMilliSecs,bool aValidateTime)
        : mUid(aUid),mTimeInMilliSecs(aTimeInMilliSecs),mValidateTime(aValidateTime)
{
}

DbAlarmData::~DbAlarmData()
{
}

DbAlarmData::DbAlarmData(const DbAlarmData& x)
{
    *this = x;
}

const Uid& DbAlarmData::getUid() const
{
    return mUid;
}

long long DbAlarmData::getTime() const
{
    return mTimeInMilliSecs;
}

bool DbAlarmData::validateAlarmTime() const
{
    return mValidateTime;
}

DbAlarmData& DbAlarmData::operator=(const DbAlarmData& x)
{
    mUid = x.mUid;
    mTimeInMilliSecs = x.mTimeInMilliSecs;
    mValidateTime = x.mValidateTime;
    return *this;
}



