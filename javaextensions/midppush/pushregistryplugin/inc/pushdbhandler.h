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


#ifndef PUSHDBHANDLER_H
#define PUSHDBHANDLER_H

#include <memory>
#include <string>
#include <set>
#include <map>
#include "javastorage.h"
#include "scopedlocks.h"
#include "javauid.h"
#include "pushconstant.h"

namespace java
{
namespace push
{

/**
 * This class contains alarm information read from the Java Storage.
 */
class DbAlarmData
{
public:

    /**
     * @param aUid              UID of the application.
     * @param aTimeInMilliSecs  Alarm time in milliseconds.
     * @param aValidateTime     Indicates whether alarm time must be validate. In practice
     *                          this means check weather alarm has expired.
     */
    DbAlarmData(const java::util::Uid& aUid,
                const long long& aTimeInMilliSecs,bool aValidateTime);

    virtual ~DbAlarmData();

    DbAlarmData(const DbAlarmData&);

    const java::util::Uid& getUid() const;

    long long getTime() const;

    bool validateAlarmTime() const;

    DbAlarmData &operator=(const DbAlarmData&);

private:

    //Datamembers.
    java::util::Uid mUid;
    long long mTimeInMilliSecs;
    bool mValidateTime;
};

class MidletSuiteData
{
public:

    MidletSuiteData(const java::util::Uid& aMidletUid,const std::wstring& aMidletClassName)
            : mMidletUid(aMidletUid),mMidletClassName(aMidletClassName),mMediaId(UNDEFINED_MEDIA_ID) {}

    virtual ~MidletSuiteData() {}

    MidletSuiteData &operator=(const MidletSuiteData &x)
    {
        mMidletUid = x.mMidletUid;
        mMidletClassName = x.mMidletClassName;
        mMediaId = x.mMediaId;
        return *this;
    }

    MidletSuiteData(const MidletSuiteData& x)
    {
        *this = x;
    }

    java::util::Uid mMidletUid;
    std::wstring mMidletClassName;
    int mMediaId;
};

/**
 * This class manages reading/writing/deleting data from Java Storage.
 */
class PushDBHandler
{
public:

    PushDBHandler();

    virtual ~PushDBHandler();

    /**
     * This operation stores dynamic push connnection into database. This operation assumes
     * that validation of arguments has been done by user.
     * @param aUid          UID of the MIDlet.
     * @param aUri          Push connection URI.
     * @param aMidletName   Class name of the MIdlet.
     * @param aFilter       Connection URL indicating which senders are allowed to cause
       *                      the MIDlet to be launched.
       * @throws              PushException with following error code:
       *                      DB_ERROR:
       *                      Inserting dynamic push connection to the database failed.
     */
    void storeDynamicPushRegistration(const java::util::Uid& aUid,
                                      const std::wstring& aUri,
                                      const std::wstring& aMidletName,
                                      const std::wstring& aFilter);

    /**
     * This operation deletes dynamic push registration from the db.
     * @param aUid    MIDlet's UID.
     * @param aUri    Push connection URI.
     * @throws        PushException with following error code:
     *                DB_ERROR:
     *                Deleting push registration failed to the db error.
     */
    void unregisterDynamicPushRegistration(const java::util::Uid& aUid,
                                           const std::wstring& aUri);

    /**
     * This operation stores MIDlet's wake-up time to the db.
     * @param aUid              MIDlet's UID.
     * @param aAlarmInMilliSecs Wake-up time in milliseconds.
     * @throws                  PushException with following error code:
     *                          DB_ERROR:
       *                          Deleting push registration failed to the db error.
     */
    void storeAlarm(const java::util::Uid& aUid, const long long& aAlarmInMilliSecs);

    /**
     * Deletes MIDlet's wake-up time from the db.
     * @param aUid  MIDlet's UID.
     * @throws      PushException with following error code:
     *              DB_ERROR:
       *              Deleting push registration failed to the db error.
     */
    void deleteAlarm(const java::util::Uid& aUid);

    /**
     * This operation returns push registrations by Uid.
     * @param aUid        All push registrations are read from the db if value of the uid is empty.
     *                    Otherwise is returned push registrations by uid.
     * @param aPushRegs   Output parameter. Contains all push registrations of all MIDlets.
     *                    Empty list is returned if phone does not have any
     *                    push registration.
     * @throws            PushException with following error code:
     *                    DB_ERROR:
       *                    Deleting push registration failed to the db error.
     */
    void getPushRegs(const java::util::Uid& aUid,
                     java::storage::JavaStorageApplicationList_t& aPushRegs);

    /**
     * This operation returns alarm times by Uid.
     * @param aUid        All alarms are read from the db if value of the uid is empty.
     *                    Otherwise is returned alarms by uid.
     * @param aAlarmList  Output parameter. List contains all MIDlets' alarm time and
     *                    UID. This operation does not do anything if no alarms has been defined
     *                    to the phone.
     */
    void getAlarms(const java::util::Uid& aUid,std::list<DbAlarmData>& aAlarmList);

    /**
     * This operation returns container which contains <MIDlet suite uid,MIDlet uid> value pairs.
     * @param aSuiteUid MIDlet suite UID of search condition. This can be also empty. All MIDlet suites
     *        are returned in that case.
     * @param aMidletSuiteInfo Output parameter. <MIDlet suite uid,MIDlet uid> value pairs.
     * @throws            PushException with following error code:
     *                    DB_ERROR:
       *                    Retrieving MIDlet Suite info failed.
     */
    void getMidletSuiteInfo(const java::util::Uid& aSuiteUid,
                            std::multimap<java::util::Uid,MidletSuiteData>& aMidletSuiteInfo);

    /**
     * This operation returns MIDlet Suite info by MIDlet uid.
     * @param aMidletUid MIDlet UID of search condition.
     * @param aMidletSuiteInfo Output parameter. <MIDlet suite uid,MIDlet uid> value pairs.
     * @throws            PushException with following error code:
     *                    DB_ERROR:
       *                    Retrieving MIDlet Suite info failed.
     */
    void getMidletSuiteInfoByMidletUid(const java::util::Uid& aMidletUid,
                                       std::multimap<java::util::Uid,MidletSuiteData>& aMidletSuiteInfo);

private:

    //Datamembers.
    std::auto_ptr<java::storage::JavaStorage> mDbStorage;
    //java::util::ScopedMutex mMutex;

    //Internal operations.
    void getMidletSuiteInfo(const java::util::Uid& aUid,
                            const std::wstring& aSearchColumn,
                            std::multimap<java::util::Uid,MidletSuiteData>& aMidletSuiteInfo);
    void addMediaId(java::storage::JavaStorage& aStorageObj,
                    std::multimap<java::util::Uid,MidletSuiteData>& aMidletSuiteInfo);
    void getMediaId(java::storage::JavaStorage& aStorageObj,const java::util::Uid& aSuiteUid,
                    std::map<java::util::Uid,int>& aMediaIdContainer);
    void fillMidletId(const java::util::Uid& aSuiteUid,
                      std::map<java::util::Uid,int>& aMediaIdContainer,
                      java::storage::JavaStorageApplicationList_t& aFoundEntries);
    void fillAlarmData(std::list<DbAlarmData>& aAlarmList,
                       java::storage::JavaStorageApplicationList_t& aFoundEntries);
    void fillMidletSuiteInfo(std::multimap<java::util::Uid,MidletSuiteData>& aMidletSuiteInfo,
                             java::storage::JavaStorageApplicationList_t& aFoundEntries);
    bool getDbValueAsUid(java::storage::JavaStorageApplicationList_t::iterator& aIter,
                         java::storage::JavaStorageEntry& aFindPattern,java::util::Uid& aOutput);
    bool getDbValueAsLongLong(java::storage::JavaStorageApplicationList_t::iterator& aIter,
                              java::storage::JavaStorageEntry& aFindPattern,long long& aOutput);
    bool getDbValueAsInt(java::storage::JavaStorageApplicationList_t::iterator& aIter,
                         java::storage::JavaStorageEntry& aFindPattern,int& aOutput);
    bool getDbValueAsWStr(java::storage::JavaStorageApplicationList_t::iterator& aIter,
                          java::storage::JavaStorageEntry& aFindPattern,std::wstring& aOutput);
    void openDbStorage();

    //Not implemented.
    PushDBHandler(const PushDBHandler&);
    PushDBHandler& operator= (const PushDBHandler&);
};

}//end namespace push
}//end namespace java

#endif // PUSHDBHANDLER_H
