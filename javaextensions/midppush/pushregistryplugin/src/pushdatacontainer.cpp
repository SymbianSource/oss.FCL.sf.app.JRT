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


#include <utility>
#include <algorithm>
#include "pushdatacontainer.h"
#include "comms.h"
#include "logger.h"
#include "javacommonutils.h"
#include "pushdbhandler.h"
#include "pushexception.h"
#include "pusherrorcodes.h"
#include "javauid.h"
#include "javastoragenames.h"

using namespace java::push;
using namespace java::captain;
using namespace java::comms;
using namespace java::storage;
using namespace java::util;

/**
 *
 */
PushDataContainer::PushDataContainer(java::captain::CoreInterface& aCore,
                                     PushDBHandler& aDbHandler)
        : mCore(aCore),mPushDbHandler(aDbHandler),mPushRegsLoaded(false)
{
    JELOG2(EJavaPush);
}

/**
 *
 */
PushDataContainer::~PushDataContainer()
{
    JELOG2(EJavaPush);
    mPushRegistrations.clear();
    mDoNotLaunchList.clear();
    mMidletSuiteInfo.clear();
}

/**
 *
 */
void PushDataContainer::getIdsOfMidlets(std::set<Uid>& aUidList)
{
    JELOG2(EJavaPush);
    //Retrieving all push registrations from the db.
    loadPushRegs();

    PushRegsContainerIter_t iter;
    for (iter = mPushRegistrations.begin(); iter != mPushRegistrations.end(); ++iter)
    {
        aUidList.insert(iter->first);
    }//end for
}

/**
 *
 */
bool PushDataContainer::isPushConnections(const Uid& aUid)
{
    JELOG2(EJavaPush);
    loadPushRegs();

    std::pair<PushRegsContainerIter_t,PushRegsContainerIter_t> pushRegsOfMidletIter;
    pushRegsOfMidletIter = mPushRegistrations.equal_range(aUid);
    if (pushRegsOfMidletIter.first == pushRegsOfMidletIter.second)
        return false;

    return true;
}

/**
 *
 */
void PushDataContainer::storeDynamicPushRegistration(const Uid& aUid,const std::wstring& aUri,
        const std::wstring& aMidletName,
        const std::wstring& aFilter)
{
    JELOG2(EJavaPush);
    loadPushRegs();

    if (true == pushRegExists(aUri))
    {
        throw PushException(PUSH_CONNECTION_ALREADY_EXISTS,"Push connection already exists",
                            __FILE__,__FUNCTION__,__LINE__);
    }

    mPushDbHandler.storeDynamicPushRegistration(aUid,aUri,aMidletName,aFilter);

    PushRegistrationData data(aUri,aMidletName,aFilter,PushRegistrationData::DYNAMIC_REG);
    mPushRegistrations.insert(std::pair<Uid,PushRegistrationData>(aUid,data));
    //This situation MIDlet must be also removed from "do not launch" list.
    mDoNotLaunchList.erase(aUid);
}

/**
 *
 */
void PushDataContainer::unregisterDynamicPushRegistration(const Uid& aUid,const std::wstring& aUri)
{
    JELOG2(EJavaPush);
    loadPushRegs();

    //Deleting push reg from the storage. It has been checked in the java side that
    //type of the push reg is dynamic and it belongs to one MIDlet in the MIDlet suite.
    mPushDbHandler.unregisterDynamicPushRegistration(aUid,aUri);

    //Deleting push registration from the cache.
    std::pair<PushRegsContainerIter_t,PushRegsContainerIter_t> pushRegsOfMidletIter;
    pushRegsOfMidletIter = mPushRegistrations.equal_range(aUid);
    PushRegsContainerIter_t iter;
    for (iter = pushRegsOfMidletIter.first; iter != pushRegsOfMidletIter.second; ++iter)
    {
        if (0 == aUri.compare(iter->second.mUri))
        {
            mPushRegistrations.erase(iter);
            break;
        }
    }//end for

    //This situation MIDlet must be also removed from "do not launch" list.
    mDoNotLaunchList.erase(aUid);
}

/**
 *
 */
void PushDataContainer::setMidletDoNotLaunchList(const Uid& aUid)
{
    JELOG2(EJavaPush);
    mDoNotLaunchList.insert(aUid);
}

/**
 *
 */
void PushDataContainer::removeMidletFromDoNotLaunchList(const Uid& aMidletUid)
{
    JELOG2(EJavaPush);
    mDoNotLaunchList.erase(aMidletUid);
}

/**
 *
 */
bool PushDataContainer::isMidletInDoNotLaunchList(const Uid& aUid)
{
    JELOG2(EJavaPush);
    if (0 == mDoNotLaunchList.size())
        return false;
    DoNotLaunchListIter_t iter = mDoNotLaunchList.find(aUid);
    if (mDoNotLaunchList.end() != iter)
        return true;
    return false;
}

/**
 *
 */
void PushDataContainer::removeApp(const Uid& aMidletUid)
{
    JELOG2(EJavaPush);

    mPushRegistrations.erase(aMidletUid);
    removeMidletFromDoNotLaunchList(aMidletUid);

    MidletSuiteInfoContainerIter_t iter;
    for (iter = mMidletSuiteInfo.begin(); iter != mMidletSuiteInfo.end();)
    {
        if (aMidletUid == iter->second.mMidletUid)
        {
            mMidletSuiteInfo.erase(iter++);
            break;
        }
        else
        {
            ++iter;
        }
    }//end for
}

/**
 *
 */
void PushDataContainer::readAppDataFromDb(const Uid& aUid)
{
    JELOG2(EJavaPush);
    try
    {
        loadPushRegs();
        readPushRegs(aUid);
        mPushDbHandler.getMidletSuiteInfoByMidletUid(aUid,mMidletSuiteInfo);
        mDoNotLaunchList.erase(aUid);
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Exception caught in the PushDataContainer::readAppDataFromDb() operation");
    }
}

/**
 *
 */
void PushDataContainer::setPendingStatusOfConn(const Uid& aUid,const std::wstring& aUri,
        bool aMsgWaitsHandling)
{
    JELOG2(EJavaPush);

    LOG1(EJavaPush,EInfo,"aMsgWaitsHandling: %d",(int)aMsgWaitsHandling);

    PushRegistrationData* dataObj = getPushDataByMidletUidAndUri(aUid,aUri);
    dataObj->mPendingStatus = aMsgWaitsHandling;
}

/**
 *
 */
void PushDataContainer::clearPendingStatusesOfMidlet(const java::util::Uid& aMidletUid)
{
    JELOG2(EJavaPush);

    std::pair<PushRegsContainerIter_t,PushRegsContainerIter_t> pushRegsOfMidletIter;
    pushRegsOfMidletIter = mPushRegistrations.equal_range(aMidletUid);
    PushRegsContainerIter_t iter;
    for (iter = pushRegsOfMidletIter.first; iter != pushRegsOfMidletIter.second; ++iter)
    {
        iter->second.mPendingStatus = false;
    }
}

/**
 *
 */
void PushDataContainer::getPushRegsOfMidletSuite(const Uid& aMidletSuiteUid,
        bool aAllConnsFlag,
        std::list<std::wstring>& aListOfPushUris)
{
    JELOG2(EJavaPush);

    LOG1WSTR(EJavaPush,EInfo,"getPushRegsOfMidletSuite(): Value of the suite UID: %s",
             aMidletSuiteUid.toString());
    LOG1(EJavaPush,EInfo,"aAllConnsFlag: %d",(int)aAllConnsFlag);

    std::pair<MidletSuiteInfoContainerIter_t,MidletSuiteInfoContainerIter_t> suiteInfoIter;
    suiteInfoIter = mMidletSuiteInfo.equal_range(aMidletSuiteUid);
    MidletSuiteInfoContainerIter_t iter;
    for (iter = suiteInfoIter.first; iter != suiteInfoIter.second; ++iter)
    {
        fillConnectionsOfMidlet(iter->second.mMidletUid,aAllConnsFlag,aListOfPushUris);
    }//end  for
}

/**
 *
 */
const std::wstring& PushDataContainer::getFilterOfPushConn(const Uid& aSuiteUid,
        const std::wstring& aUri)
{
    JELOG2(EJavaPush);

    PushRegistrationData* dataObj = getPushConnOfSuite(aSuiteUid,aUri);
    return dataObj->mFilter;
}

/**
 *
 */
const std::wstring& PushDataContainer::getClassNameOfPushConn(const Uid& aSuiteUid,
        const std::wstring& aUri)
{
    JELOG2(EJavaPush);

    PushRegistrationData* dataObj = getPushConnOfSuite(aSuiteUid,aUri);
    return dataObj->mMidletClass;
}

/**
 *
 */
const Uid& PushDataContainer::getMidletUid(const Uid& aSuiteUid,
        const std::wstring& aClassName)
{
    JELOG2(EJavaPush);

    std::pair<MidletSuiteInfoContainerIter_t,MidletSuiteInfoContainerIter_t> suiteInfoIter;
    suiteInfoIter = mMidletSuiteInfo.equal_range(aSuiteUid);
    MidletSuiteInfoContainerIter_t iter;
    for (iter = suiteInfoIter.first; iter != suiteInfoIter.second; ++iter)
    {
        if (iter->second.mMidletClassName == aClassName)
        {
            return iter->second.mMidletUid;
        }
    }//end  for
    throw PushException(INCORRECT_APP_DATA,
                        "Class name does not match to any MIDlet in the suite",
                        __FILE__,__FUNCTION__,__LINE__);
}

/**
 *
 */
bool PushDataContainer::getSuiteUidByBaseUri(const std::wstring& aBaseUri,
        Uid& aOutputSuiteUid)
{
    JELOG2(EJavaPush);

    PushRegsContainerIter_t iter;
    std::string aBaseUriStr(aBaseUri.begin(),aBaseUri.end());
    for (iter = mPushRegistrations.begin(); iter != mPushRegistrations.end(); ++iter)
    {
        //size_t found = iter->second.mUri.find(aBaseUri);
        //if(0 == found)
        std::string fullUriStr(iter->second.mUri.begin(),iter->second.mUri.end());
        bool matchFlag = isUriStartingWith(fullUriStr,aBaseUriStr);
        if (true == matchFlag)
        {
            return getSuiteUidByMidletUid(iter->first,aOutputSuiteUid);
        }
    }//end for

    return false;
}

/**
 *
 */
int PushDataContainer::getMediaIdByMidletUid(const Uid& aMidletUid)
{
    JELOG2(EJavaPush);

    MidletSuiteInfoContainerIter_t iter;
    for (iter = mMidletSuiteInfo.begin(); iter != mMidletSuiteInfo.end(); ++iter)
    {
        if (aMidletUid == iter->second.mMidletUid)
        {
            return iter->second.mMediaId;
        }
    }
    return UNDEFINED_MEDIA_ID;
}

/**
 *
 */
void PushDataContainer::getMidletUidsByMediaId(unsigned int aMediaId,std::list<Uid>& aUidList)
{
    JELOG2(EJavaPush);

    MidletSuiteInfoContainerIter_t iter;
    for (iter = mMidletSuiteInfo.begin(); iter != mMidletSuiteInfo.end(); ++iter)
    {
        if (aMediaId == iter->second.mMediaId)
            aUidList.push_back(iter->second.mMidletUid);
    }
}

/**
 *
 */
void PushDataContainer::loadPushRegs()
{
    JELOG2(EJavaPush);

    if (true == mPushRegsLoaded)
        return;

    //Empty uid means that all push registrations are read from the db.
    java::util::Uid emptyUid;
    readPushRegs(emptyUid);
    //readMidletSuiteInfo(emptyUid);
    mPushDbHandler.getMidletSuiteInfo(emptyUid,mMidletSuiteInfo);

    mPushRegsLoaded = true;
}

/**
 *
 */
void PushDataContainer::readPushRegs(const java::util::Uid& aUid)
{
    JELOG2(EJavaPush);

    LOG1WSTR(EJavaPush,EInfo,"readPushRegs() Uid: %s",aUid.toString());

    JavaStorageApplicationList_t pushRegs;
    mPushDbHandler.getPushRegs(aUid,pushRegs);

    JavaStorageApplicationList_t::iterator iter;
    for (iter = pushRegs.begin(); iter != pushRegs.end(); ++iter)
    {
        insertPushRegToInternalContainer(iter);
    }//end for
}

/**
 *
 */
void PushDataContainer::insertPushRegToInternalContainer
(JavaStorageApplicationList_t::iterator& aIter)
{
    JavaStorageApplicationEntry_t::iterator multisetIter;
    Uid uid;
    std::wstring uri;
    std::wstring midletClass;
    std::wstring filter;
    PushRegistrationData::RegistrationType_t regType = PushRegistrationData::DYNAMIC_REG;
    int counter = 0;
    for (multisetIter = aIter->begin(); multisetIter != aIter->end(); ++multisetIter)
    {
        try
        {
            if (0 == multisetIter->entryName().compare(ID))
            {
                Uid tmpUid(multisetIter->entryValue());
                uid = tmpUid;
                counter++;
            }
            else if (0 == multisetIter->entryName().compare(URL))
            {
                uri = multisetIter->entryValue();
                counter++;
            }
            else if (0 == multisetIter->entryName().compare(NAME))
            {
                midletClass = multisetIter->entryValue();
                counter++;
            }
            else if (0 == multisetIter->entryName().compare(FILTER))
            {
                filter = multisetIter->entryValue();
                counter++;
            }
            else if (0 == multisetIter->entryName().compare(REGISTRATION_TYPE))
            {
                int tmpInt = JavaCommonUtils::wstringToInt(multisetIter->entryValue());
                if ((tmpInt == PushRegistrationData::DYNAMIC_REG)
                        || (tmpInt == PushRegistrationData::STATIC_REG))
                {
                    regType = (PushRegistrationData::RegistrationType_t) tmpInt;
                    counter++;
                }
            }
            if (5 == counter)
            {
                //All arguments reads successfully.
                PushRegistrationData data(uri,midletClass,filter,regType);
                mPushRegistrations.insert(std::pair<Uid,PushRegistrationData>(uid,data));
                counter = 0;
            }
            else if (5 > counter)
            {
                continue;
            }
            else
            {
                ELOG1(EJavaPush,"ERROR!!! All arguments wasn''t read successfully: %d",counter);
                counter = 0;
            }
        }
        catch (...)
        {
            ELOG(EJavaPush,"ERROR!!! Exception caught in the reading push regs");
        }
    }//end for
}

/**
 *
 */
void PushDataContainer::getPushRegsOfMidlet(const Uid& aUid,std::list<PushRegData>& aOutputList)
{
    JELOG2(EJavaPush);

    loadPushRegs();

    std::pair<PushRegsContainerIter_t,PushRegsContainerIter_t> pushRegsOfMidletIter;
    pushRegsOfMidletIter = mPushRegistrations.equal_range(aUid);
    PushRegsContainerIter_t iter;
    for (iter = pushRegsOfMidletIter.first; iter != pushRegsOfMidletIter.second; ++iter)
    {
        PushRegData pushData(iter->second.mUri,iter->second.mFilter);
        aOutputList.push_back(pushData);
    }//end for
}

/**
 *
 */
bool PushDataContainer::pushRegExists(const std::wstring& aUri)
{
    JELOG2(EJavaPush);

    for (PushRegsContainerIter_t iter = mPushRegistrations.begin();
            iter != mPushRegistrations.end(); ++iter)
    {
        if (0 == aUri.compare(iter->second.mUri))
            return true;
    }
    return false;
}

/**
 *
 */
void PushDataContainer::fillConnectionsOfMidlet(const java::util::Uid& aMidletUid,
        bool aAllConnsFlag,
        std::list<std::wstring>& aListOfPushUris)
{
    std::pair<PushRegsContainerIter_t,PushRegsContainerIter_t> pushRegsOfMidletIter;
    pushRegsOfMidletIter = mPushRegistrations.equal_range(aMidletUid);
    PushRegsContainerIter_t iter;
    for (iter = pushRegsOfMidletIter.first; iter != pushRegsOfMidletIter.second; ++iter)
    {
        if (false == aAllConnsFlag)
        {
            //Connection is skipped if it is not in the pending state.
            if (false == iter->second.mPendingStatus)
                continue;
        }
        aListOfPushUris.push_back(iter->second.mUri);
    }//end for
}

/**
 *
 */
bool PushDataContainer::isMidletPartOfSuite(const Uid& aSuiteUid,const Uid& aMidletUid)
{
    JELOG2(EJavaPush);

    std::pair<MidletSuiteInfoContainerIter_t,MidletSuiteInfoContainerIter_t> suiteInfoIter;
    suiteInfoIter = mMidletSuiteInfo.equal_range(aSuiteUid);
    MidletSuiteInfoContainerIter_t iter;
    bool midletFound = false;
    for (iter = suiteInfoIter.first; iter != suiteInfoIter.second; ++iter)
    {
        if (iter->second.mMidletUid == aMidletUid)
        {
            midletFound = true;
            break;
        }
    }//end  for
    return midletFound;
}

/**
 *
 */
PushRegistrationData* PushDataContainer::getPushConnOfSuite(const Uid& aSuiteUid,
        const std::wstring& aUri)
{
    JELOG2(EJavaPush);

    PushRegistrationDataWithUid dataObj = getPushDataByUri(aUri);
    if (0 == dataObj.mPushRegData)
    {
        throw PushException(SRV_CONN_NOT_FOUND,"MIDlet does not have push connection by given URI",
                            __FILE__,__FUNCTION__,__LINE__);
    }

    bool midletFound = isMidletPartOfSuite(aSuiteUid,dataObj.mUid);
    if (false == midletFound)
    {
        throw PushException(SRV_CONN_NOT_FOUND,"MIDlet does not belong to given MIDlet suite",
                            __FILE__,__FUNCTION__,__LINE__);
    }
    return dataObj.mPushRegData;
}

/**
 *
 */
PushRegistrationData*
PushDataContainer::getPushDataByMidletUidAndUri(const Uid& aMidletUid,
        const std::wstring& aUri)
{
    JELOG2(EJavaPush);

    LOG1WSTR(EJavaPush,EInfo,"getPushDataByMidletUidAndUri() URI: %s",aUri);

    std::pair<PushRegsContainerIter_t,PushRegsContainerIter_t> pushRegsOfMidletIter;
    pushRegsOfMidletIter = mPushRegistrations.equal_range(aMidletUid);
    PushRegsContainerIter_t iter;
    for (iter = pushRegsOfMidletIter.first; iter != pushRegsOfMidletIter.second; ++iter)
    {
        if (aUri == iter->second.mUri)
            return &iter->second;
    }
    return 0;
}

/**
 *
 */
PushRegistrationDataWithUid PushDataContainer::getPushDataByUri(const std::wstring& aUri)
{
    JELOG2(EJavaPush);

    PushRegsContainerIter_t iter;
    for (iter = mPushRegistrations.begin(); iter != mPushRegistrations.end(); ++iter)
    {
        if (aUri == iter->second.mUri)
            return PushRegistrationDataWithUid(iter->first,&iter->second);
    }//end for
    Uid emptyUid;
    return PushRegistrationDataWithUid(emptyUid,0);
}

/**
 *
 */
bool PushDataContainer::getSuiteUidByMidletUid(const java::util::Uid& aMidletUid,
        java::util::Uid& aOutputSuiteUid)
{
    JELOG2(EJavaPush);

    MidletSuiteInfoContainerIter_t iter;
    for (iter = mMidletSuiteInfo.begin(); iter != mMidletSuiteInfo.end(); ++iter)
    {
        if (aMidletUid == iter->second.mMidletUid)
        {
            aOutputSuiteUid = iter->first;
            return true;
        }
    }
    return false;
}

/**
 *
 */
bool PushDataContainer::isUriStartingWith(const std::string& aFullUri,
        const std::string& aBaseUri)
{
    if (aBaseUri.size() > aFullUri.size())
        return false;
    std::string::const_iterator iter = std::search(aFullUri.begin(),aFullUri.end(),
                                       aBaseUri.begin(),aBaseUri.end(),
                                       caseInsensitiveCompare);
    if (aFullUri.end() == iter)
        return false;
    if (0 == (iter - aFullUri.begin()))
        return true;
    return false;
}

/**
 *
 */
bool PushDataContainer::caseInsensitiveCompare(const char a1,const char a2)
{
    return toupper(a1) == toupper(a2);
}


