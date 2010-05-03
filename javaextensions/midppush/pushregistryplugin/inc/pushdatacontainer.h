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


#ifndef PUSHDATACONTAINER_H
#define PUSHDATACONTAINER_H

#include <memory>
#include <map>
#include <set>
#include "pushregdata.h"
#include "javastorage.h"
#include "javauid.h"
#include "pushdbhandler.h"

namespace java
{
namespace captain
{
class CoreInterface;
}
namespace push
{

class PushDBHandler;

/**
 * This is internal class used by PushDataContainer class.
 */
class PushRegistrationData
{
public:

    enum RegistrationType_t {DYNAMIC_REG = 0,STATIC_REG = 1};

    PushRegistrationData(const std::wstring& aUri,const std::wstring& aMidletClass,
                         const std::wstring& aFilter,RegistrationType_t aType)
            : mUri(aUri),mMidletClass(aMidletClass),mFilter(aFilter),mType(aType),
            mPendingStatus(false) {}

    virtual ~PushRegistrationData() {}

    PushRegistrationData &operator=(const PushRegistrationData &x)
    {
        mUri = x.mUri;
        mMidletClass = x.mMidletClass;
        mFilter = x.mFilter;
        mType = x.mType;
        mPendingStatus = x.mPendingStatus;
        return *this;
    }

    PushRegistrationData(const PushRegistrationData& x)
    {
        *this = x;
    }

    std::wstring mUri;
    std::wstring mMidletClass;
    std::wstring mFilter;
    RegistrationType_t mType;
    //true' means that message has arrived to the connection.
    //'false' means that application has started to handle arrived msg of the push connection
    //        or msg has not been arrived to the connection.
    bool mPendingStatus;

};

class PushRegistrationDataWithUid
{
public:

    //This class does not take ownership of the aData argument.
    PushRegistrationDataWithUid(const java::util::Uid& aUid,
                                PushRegistrationData* aData)
            : mUid(aUid),mPushRegData(aData) {}

    virtual ~PushRegistrationDataWithUid() {}

    PushRegistrationDataWithUid &operator=(const PushRegistrationDataWithUid &x)
    {
        mUid = x.mUid;
        mPushRegData = x.mPushRegData;
        return *this;
    }

    PushRegistrationDataWithUid(const PushRegistrationDataWithUid& x)
    {
        *this = x;
    }

    //MIDlet's uid.
    java::util::Uid mUid;
    PushRegistrationData* mPushRegData;
};

/**
 * This class manages push registrations in the Java Captain process.
 * This class reads all push registrations from the Java Storage and caches
 * push registrations. Dynamic puush registrations can be register/unregister
 * via this class.
 */

class PushDataContainer
{
public:

    PushDataContainer(java::captain::CoreInterface& aCore,
                      PushDBHandler& aDbHandler);

    virtual ~PushDataContainer();

    void getIdsOfMidlets(std::set<java::util::Uid>& aUidList);

    /**
     * Informs whether MIDlet has push registrations.
     * @param aUid UID of the MIDlet.
     * @return 'true' if MIDlet has static and/or dynamic push connedction(s).
     */
    bool isPushConnections(const java::util::Uid& aUid);

    /**
     * Returns push registrations of the application.
     * @param aUid UID of the application.
     * @param aOutputList Output parameter. Contains push registrations of the application.
     */
    void getPushRegsOfMidlet(const java::util::Uid& aUid,std::list<PushRegData>& aOutputList);

    /**
     * This operation stores dynamic push registration to the java storage.
     * @param aUid          UID of the MIDlet.
     * @param aUri          Push connection URI.
     * @param aMidletName   Class name of the MIdlet.
     * @param aFilter       Connection URL indicating which senders are allowed to cause
       *                      the MIDlet to be launched.
     * @throws              PushException with following error codes:
     *                      PUSH_CONNECTION_ALREADY_EXISTS:
     *                      Push connection already exists.
     *                      DB_ERROR:
       *                      Inserting dynamic push connection to the database failed.
     */
    void storeDynamicPushRegistration(const java::util::Uid& aUid,const std::wstring& aUri,
                                      const std::wstring& aMidletName,
                                      const std::wstring& aFilter);

    /**
     * This operation removes dynamic push connection from the MIDlet.
     * @throws  PushException with following error codes:
     *          NOT_DYNAMIC_PUSH_URI:
     *          URI is static push URI.
     *          URI_BELONGS_TO_OTHER_MIDLET:
     *          Push URI is reserved for other MIDlet.
     *          SRV_CONN_NOT_FOUND:
     *          Server connection is not found from the push registry db.
     *          DB_ERROR:
       *          Database error occurred.
     */
    void unregisterDynamicPushRegistration(const java::util::Uid& aUid,const std::wstring& aUri);

    /**
     * This method sets MIDlet to the "do not launch" list. This means that MIDlet
     * is not try to re-launch in the listening until device has booted next time.
     * This operation does not throw exceptions.
     * @param aUid UID of the MIDlet.
     */
    void setMidletDoNotLaunchList(const java::util::Uid& aUid);

    /**
     * This method removes MIDlet to the "do not launch" list.
     * This operation does not throw exceptions.
     * @param aUid UID of the MIDlet.
     */
    void removeMidletFromDoNotLaunchList(const java::util::Uid& aUid);

    /**
     * This operation does not throw exceptions.
     * @return 'true' if MIDlet is not in the "do not launch" list.
     */
    bool isMidletInDoNotLaunchList(const java::util::Uid& aUid);

    /**
     * This operation removes application from all containers.
     */
    void removeApp(const java::util::Uid& aUid);

    /**
     * This operation reads application's push registrations from the db and adds
     * those ones into the internal cache. It also removes application from the
     * 'do not launch in the push listening mode' list.
     * @param aUid  UID of the application.
     * @throws This operation does not throw exceptions.
     */
    void readAppDataFromDb(const java::util::Uid& aUid);

    /**
     * This method updates pending status of the push connection. Pending status is needed
     * by PushRegistry.listConnections() operation when this operation is called with 'true'
     * argument.
     * @param aUid UID of the application.
     * @param aUri URI of the push connection.
     * @param aMsgWaitsHandling status flag of push connection.
     *        - 'true' means that message has arrived to the connection.
     *        - 'false' means that application has started to handle arrived msg
     *          of the push connection.
     * @throws This operation does not throw exceptions.
     */
    void setPendingStatusOfConn(const java::util::Uid& aUid,const std::wstring& aUri,
                                bool aMsgWaitsHandling);

    /**
     * Sets pending status of MIDlet's all push connections to "not active".
     * @param aMidletUid UID of the application.
     * @throws This operation does not throw exceptions.
     */
    void clearPendingStatusesOfMidlet(const java::util::Uid& aMidletUid);

    /**
     * This method returns push connections of all MIDlets' in the MIDlet suite.
     * @param aMidletSuiteUid MIDlet suite UID.
     * @param aAllConnsFlag this flag indicates whether only "pending conns"
     *        are returned:
     *        'true'  => all MIDlets' push connections are returned.
     *        'false' => only push connections which waiting application's handling.
     * @param listOfPushUris URIs of push connections.
     * @throws This operation does not throw exceptions.
     */
    void getPushRegsOfMidletSuite(const java::util::Uid& aMidletSuiteUid,bool aAllConnsFlag,
                                  std::list<std::wstring>& aListOfPushUris);

    /**
     * Returns filter argument of the push registration.
     * @param aSuiteUid   MIDlet suite UID.
     * @param aUri        push URI.
     * @throws            PushException with following error codes:
     *                    SRV_CONN_NOT_FOUND:
     *                    - Push connection by URI and suite uid is not found.
     */
    const std::wstring& getFilterOfPushConn(const java::util::Uid& aSuiteUid,
                                            const std::wstring& aUri);

    /**
     * Returns class name of the push registration.
     * @param aSuiteUid   MIDlet suite UID.
     * @param aUri        push URI.
     * @throws            PushException with following error codes:
     *                    SRV_CONN_NOT_FOUND:
     *                    - Push connection by URI and suite uid is not found.
     */
    const std::wstring& getClassNameOfPushConn(const java::util::Uid& aSuiteUid,
            const std::wstring& aUri);

    /**
     * This operation returns MIDlet's uid. MIDlet is identified by suite uid and
     * class name of the MIDlet.
     * @param aSuiteUid   MIDlet suite UID.
     * @param aClassName  Class name of the MIDlet.
     * @throws            PushException with following error code:
     *                    INCORRECT_APP_DATA:
     *                    - aClassName does not match to any MIDlet in the suite.
     */
    const java::util::Uid& getMidletUid(const java::util::Uid& aSuiteUid,
                                        const std::wstring& aClassName);


    /**
     * This operation returns suite uid of the push connection retrieved by
     * base URI. Base URI means "starts with" URI in this context.
     * Comparison is case insensitive and it assumes that URI contains
     * only us-ascii characters.
     * E.g. Match occurs if full URI is "socket://:1234?param1=value1"
     * and base URI is "socket://:1234".
     * @param aBaseUri "starts with" URI.
     * @param aOutputSuiteUid Output parameter. Suite uid of the MIDlet.
     * @return true if aBaseUri argument match to one push connection URI.
     */
    bool getSuiteUidByBaseUri(const std::wstring& aBaseUri,java::util::Uid& aOutputSuiteUid);

    /**
     * This operation returns MIDlet's media uid.
     * @return MIDlet's media id. Returns -2 if MIDlet by uid is not found.
     *         Returns -1 if "is mmc available" check is not needed to do.
     */
    int getMediaIdByMidletUid(const java::util::Uid& aMidletUid);


    /**
     * This operation returns list of MIDlet's uids by media id.
     * @param aMediaId media id of the drive.
     * @param aUidList Output argument. List of MIDlet's uids installed to the certain drive.
     * @throws This operation does not throw exceptions.
     */
    void getMidletUidsByMediaId(unsigned int aMediaId,std::list<java::util::Uid>& aUidList);

private:

    //Datamembers.
    java::captain::CoreInterface& mCore;
    PushDBHandler& mPushDbHandler;

    //Container for push registrations.
    typedef std::multimap<java::util::Uid,PushRegistrationData> PushRegsContainer_t;
    typedef std::multimap<java::util::Uid,PushRegistrationData>::iterator PushRegsContainerIter_t;
    PushRegsContainer_t mPushRegistrations;
    bool mPushRegsLoaded;

    typedef std::set<java::util::Uid> DoNotLaunchList_t;
    typedef std::set<java::util::Uid>::iterator DoNotLaunchListIter_t;
    DoNotLaunchList_t mDoNotLaunchList;

    //Container for MIDlet suite info.
    //Uid is reserved for the suite uid.
    typedef std::multimap<java::util::Uid,MidletSuiteData> MidletSuiteInfoContainer_t;
    typedef std::multimap<java::util::Uid,MidletSuiteData>::iterator MidletSuiteInfoContainerIter_t;
    MidletSuiteInfoContainer_t mMidletSuiteInfo;

    //Internal operations.
    void loadPushRegs();
    void readPushRegs(const java::util::Uid& aUid);
    void insertPushRegToInternalContainer(java::storage::JavaStorageApplicationList_t::iterator& aIter);
    bool pushRegExists(const std::wstring& aUri);
    void fillConnectionsOfMidlet(const java::util::Uid& aMidletUid,bool aAllConnsFlag,
                                 std::list<std::wstring>& aListOfPushUris);
    bool isMidletPartOfSuite(const java::util::Uid& aSuiteUid,
                             const java::util::Uid& aMidletUid);
    PushRegistrationData* getPushConnOfSuite(const java::util::Uid& aSuiteUid,
            const std::wstring& aUri);
    PushRegistrationData* getPushDataByMidletUidAndUri(const java::util::Uid& aMidletUid,
            const std::wstring& aUri);
    PushRegistrationDataWithUid getPushDataByUri(const std::wstring& aUri);

    bool getSuiteUidByMidletUid(const java::util::Uid& aMidletUid,
                                java::util::Uid& aOutputSuiteUid);
    /**
     * This operation compares whether aFullUri starts with value of the aBaseUri
     * argument. Comparison is case insensitive and both URIs can contains only
     * us-ascii characters.
     */
    bool isUriStartingWith(const std::string& aFullUri,const std::string& aBaseUri);

    /**
     * This operation compares whether two chars are equals.
     * Comparison is case insensitive.
     */
    static bool caseInsensitiveCompare(const char a1,const char a2);

    //Not implemented.
    PushDataContainer(const PushDataContainer &x);
    PushDataContainer &operator=(const PushDataContainer &x);
};

}//end namespace push
}//end namespace java

#endif // PUSHDATACONTAINER_H
