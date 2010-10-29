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


#include <list>
#include <memory>
#include <string>
#include <time.h>
#include <algorithm>

#include "pushregistryhandler.h"
#include "coreinterface.h"
#include "rtcinterface.h"
#include "commsendpoint.h"
#include "commsmessage.h"
#include "pushregistrymessages.h"
#include "javacommonutils.h"
#include "logger.h"
#include "pushconstant.h"
#include "pushexception.h"
#include "pusherrorcodes.h"
#include "pushdbhandler.h"
#include "pushdatacontainer.h"
#include "pushalarmhandler.h"
#include "javauid.h"
#include "booteventprovidermessages.h"
#include "mmceventprovidermessages.h"

using namespace java::push;
using namespace java::captain;
using namespace java::comms;
using namespace java::util;
using namespace java::fileutils;
using namespace std;

/*extern "C"*/
#ifdef __SYMBIAN32__
ExtensionPluginInterface* getExtensionPlugin()
{
#else
extern "C" ExtensionPluginInterface* getExtensionPlugin()
{
#endif
    return new PushRegistryHandler();
}

/**
 *
 */
OS_EXPORT PushRegistryHandler::PushRegistryHandler()
        :mCore(NULL),mPushDbHandler(new PushDBHandler()),mPushDataContainer(NULL),
        mPushAlarmHandler(NULL),mConnCompletedTimerContainer(NULL)
{
    JELOG2(EJavaPush);
    //LOG1(EJavaPush,EInfo,"PushRegistryHandler(), this: %d",this);
}

/**
 *
 */
OS_EXPORT PushRegistryHandler::~PushRegistryHandler()
{
    JELOG2(EJavaPush);
    mDriveInfo.clear();
    //LOG1(EJavaPush,EInfo,"~PushRegistryHandler(), this: %d",this);
}

/**
 *
 */
void PushRegistryHandler::startPlugin(java::captain::CoreInterface* aCore)
{
    JELOG2(EJavaPush);

    try
    {
        mCore = aCore;
        mCore->getComms()->registerListener(PLUGIN_ID_PUSH_REGISTRY_C,this);
        mPushDataContainer.reset(new PushDataContainer(*mCore,*mPushDbHandler));
        mPushAlarmHandler.reset(new PushAlarmHandler(*mCore,*this,*mPushDbHandler));
        mConnCompletedTimerContainer.reset
        (new PushConnCompletedTimerContainer(*aCore->getTimerServer(),*this));
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Exception caught in the PushRegistryHandler::start() operation");
    }
}

/**
 *
 */
void PushRegistryHandler::stopPlugin()
{
    JELOG2(EJavaPush);

    mCore->getComms()->unregisterListener(PLUGIN_ID_PUSH_REGISTRY_C,this);
}

/**
 *
 */
ApplicationRuntimeEventsInterface* PushRegistryHandler::getApplicationRuntimeListener()
{
    return this;
}

/**
 *
 */
ApplicationManagementEventsInterface* PushRegistryHandler::getApplicationManagementListener()
{
    return this;
}

/**
 *
 */
EventConsumerInterface* PushRegistryHandler::getEventConsumer()
{
    return this;
}

/**
 *
 */
void PushRegistryHandler::processMessage(CommsMessage& aMessage)
{
    JELOG2(EJavaPush);

    switch (aMessage.getMessageId())
    {
    case IPC_PR_GET_ALL_PUSH_REGS:
    {
        handleGetAllPushRegsMsg(aMessage);
        break;
    }
    case IPC_PR_REGISTER_DYNAMIC_PUSH_CONN:
    {
        handleRegDynamicPushConnMsg(aMessage);
        break;
    }
    case IPC_PR_UNREGISTER_DYNAMIC_PUSH_CONN:
    {
        handleUnregDynamicPushConnMsg(aMessage);
        break;
    }
    case IPC_PR_REGISTER_ALARM:
    {
        handleRegisterAlarmMsg(aMessage);
        break;
    }
    case IPC_PR_ALL_CONNS_FAILED:
    {
        handleAllConnsFailedMsg(aMessage);
        break;
    }
    case IPC_PR_PENDING_STATUS:
    {
        handlePendingStatusMsg(aMessage);
        break;
    }
    case IPC_PR_LIST_CONNECTIONS:
    {
        handleListConnectionsMsg(aMessage);
        break;
    }
    case IPC_PR_DATA_OF_PUSH_CONN_QUERY:
    {
        handleDataOfPushConnQueryMsg(aMessage);
        break;
    }
    case IPC_PR_PUSH_URI_STARTS_WITH_QUERY:
    {
        handlePushUriStartsWithQueryMsg(aMessage);
        break;
    }
    case IPC_PR_CONN_COMPLETED:
    {
        handleConnCompletedMsg(aMessage);
        break;
    }

    default:
        ELOG1(EJavaPush, "ERROR!!! Unsupported method arrives to processMessage(): %d",
              aMessage.getMessageId());
    }//end switch
}

/**
 *
 */
void PushRegistryHandler::arLaunched(const Uid& /*aUID*/, const int& /*aRuntimeCommsAddress*/)
{
    JELOG2(EJavaPush);
    //Launch of the application does not interest push framework.
}

/**
 *
 */
void PushRegistryHandler::arTerminated(const Uid& aUID, const int& aExitCode)
{
    JELOG2(EJavaPush);

    LOG1WSTR(EJavaPush,EInfo,"arTerminated(), Uid: %s",aUID.toString());
    LOG1(EJavaPush,EInfo,"arTerminated(), aExitCode: %d",aExitCode);

    //MIDlet has been terminated so pending status of all its push connections
    //can be set to "not active" state.
    mPushDataContainer->clearPendingStatusesOfMidlet(aUID);

    bool flag = mPushDataContainer->isPushConnections(aUID);
    if (true == flag)
    {
        //aExitCode is non-zero if runtime has crashed. In that case MIDlet
        //is not launched as listening mode.
        if (0 != aExitCode)
        {
            ELOG1(EJavaPush,"ERROR!!! arTerminated(), aExitCode: %d",aExitCode);
            mPushDataContainer->setMidletDoNotLaunchList(aUID);
        }
        if (false == mPushDataContainer->isMidletInDoNotLaunchList(aUID))
        {
            //Launching an application.
            LOG(EJavaPush,EInfo,"arTerminated(), launching application in push mode");
            launchMidlet(aUID,java::captain::RTC_LAUNCH_TYPE_PUSH_C);
        }
    }//end if(true == flag)
}

/**
 *
 */
void PushRegistryHandler::amAdded(const uids_t& aUids)
{
    JELOG2(EJavaPush);

    //JavaCaptain calls this method when new application has been installed or
    //application's uid has been changed in the upgrade.
    for (uids_t::const_iterator iter = aUids.begin(); iter != aUids.end(); ++iter)
    {
        //Alarm case have to be checked in this situation because uid of the alarm might
        //be changed in the upgrade.
        readAppData((*iter),true);
    }//end for
}

/**
 *
 */
void PushRegistryHandler::amUpdated(const uids_t& aUids)
{
    JELOG2(EJavaPush);

    //JavaCaptain calls this operation when application's data has been updated
    //by Installer. In this situation uid of the appplication has not been updated.
    for (uids_t::const_iterator iter = aUids.begin(); iter != aUids.end(); ++iter)
    {
        deleteAppData((*iter),false);
        //Alarm does not have to be checked in this situation because uid of the
        //application has not been changed and Installer has not changed alarm.
        readAppData((*iter),false);
    }
}

/**
 *
 */
void PushRegistryHandler::amDeleted(const uids_t& aUids)
{
    JELOG2(EJavaPush);

    //JavaCaptain calls this method when application has been uninstalled.
    //Note: This is called also in the upgrade of the application if application's
    //uid has been changed.
    for (uids_t::const_iterator iter = aUids.begin(); iter != aUids.end(); ++iter)
    {
        deleteAppData((*iter),true);
    }//end for
}

/**
 *
 */
void PushRegistryHandler::event(const std::string& eventProvider,
                                java::comms::CommsMessage& aMsg)
{
    JELOG2(EJavaPush);
    try
    {
        if (eventProvider == BOOT_EVENT_PROVIDER)
        {
            LOG(EJavaPush, EInfo, "PushRegistryHandler::event() boot event received");
            int bootType = 0;
            getBootMessageParams(aMsg, bootType);
            //Read initial drive status. After that drive info status is updated
            //when mmc is inserted/deleted. JavaCaptain sends MMC_EVENT_PROVIDER event when mmc
            //status changes.
            mDriveInfo.clear();
            DriveUtilities::getAccesibleDrives(mDriveInfo);
            startPushMidlets();
            mPushAlarmHandler->readAndStartAlarms();
        }
        else if (eventProvider == MMC_EVENT_PROVIDER)
        {
            int operation = 0;
            driveInfo di;
            getMmcChangedMessageParams(aMsg, operation, di);
            LOG1(
                EJavaCaptain,
                EInfo,
                "PushRegistryHandler::event() mmc event received (operation=%d)",
                operation);

            switch (operation)
            {
            case DriveListenerInterface::REMOVABLE_MEDIA_INSERTED_C:
                manageDriveInsertion(di);
                break;
            case DriveListenerInterface::REMOVABLE_MEDIA_REMOVED_C:
                manageDriveRemoval(di);
                break;
            }
        }
    }
    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaPush,"ERROR!!! PushRegistryHandler::event(): %s",ex.toString().c_str());
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Exception caught in the PushRegistryHandler::event() operation");
    }
}

/**
 *
 */
void PushRegistryHandler::connCompletedTimerExpired(const java::comms::CommsMessage& aMsg)
{
    JELOG2(EJavaPush);

    try
    {
        CommsMessage unRegCompleted;
        unRegCompleted.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
        unRegCompleted.setReceiver(aMsg.getSender());
        unRegCompleted.setModuleId(PLUGIN_ID_PUSH_CONTROLLER_C);
        unRegCompleted.setMessageId(IPC_PR_CONN_MANAGED_BY_OTHER_MIDLET);
        int retValue = mCore->getComms()->send(unRegCompleted);
        if (0 != retValue)
        {
            //Exception is not thrown from this situation.
            std::string errTxt("Sending IPC_PR_CONN_MANAGED_BY_OTHER_MIDLET comms request failed: ");
            errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
            ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        }
    }
    catch (...)
    {
        ELOG(EJavaPush,
             "ERROR!!! Exception caught in the PushRegistryHandler::connCompletedTimerExpired() operation");
    }
}

/**
 *
 */
void PushRegistryHandler::handleGetAllPushRegsMsg(CommsMessage& aMsg)
{
    JELOG2(EJavaPush);

    Uid uid;
    try
    {
        readUidArg(aMsg,"IPC_PR_GET_ALL_PUSH_REGS",uid);
        std::list<PushRegData> listOfPushRegs;
        mPushDataContainer->getPushRegsOfMidlet(uid,listOfPushRegs);

        CommsMessage listOfPushRegsMsg;
        listOfPushRegsMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
        listOfPushRegsMsg.setReceiver(aMsg.getSender());
        listOfPushRegsMsg.setModuleId(PLUGIN_ID_PUSH_CONTROLLER_C);
        listOfPushRegsMsg.setMessageId(IPC_PR_LIST_OF_ALL_PUSH_REGS);
        listOfPushRegsMsg.setMessageRef(aMsg.getMessageRef());

        listOfPushRegsMsg << uid;
        listOfPushRegsMsg << NO_ERROR;
        listOfPushRegsMsg << "";
        std::list<PushRegData>::iterator iter;
        for (iter = listOfPushRegs.begin(); iter != listOfPushRegs.end(); ++iter)
        {
            LOG1(EJavaPush,EInfo, "handleGetAllPushRegsMsg() URI: %S",iter->mUri.c_str());
            listOfPushRegsMsg << iter->mUri;
            listOfPushRegsMsg << iter->mFilter;
        }//end for

        int retValue = mCore->getComms()->send(listOfPushRegsMsg);
        if (0 != retValue)
        {
            //Exception is not thrown from this situation.
            std::string errTxt("Sending IPC_PR_LIST_OF_ALL_PUSH_REGS comms request failed: ");
            errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
            ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        }
    }
    catch (ExceptionBase& ex)
    {
        sendCommsResponseInErrorCase(ex,IPC_PR_LIST_OF_ALL_PUSH_REGS,uid,aMsg.getSender(),
                                     aMsg.getMessageRef());
    }
    catch (...)
    {
        ExceptionBase tmpBase(UNKNOWN_ERROR,"Unknown exception caught",
                              __FILE__,__FUNCTION__,__LINE__);
        sendCommsResponseInErrorCase(tmpBase,IPC_PR_LIST_OF_ALL_PUSH_REGS,uid,aMsg.getSender(),
                                     aMsg.getMessageRef());
    }
}

/**
 *
 */
void PushRegistryHandler::handleRegDynamicPushConnMsg(CommsMessage& aMsg)
{
    JELOG2(EJavaPush);
    Uid uid;
    try
    {
        readUidArg(aMsg,"IPC_PR_REGISTER_DYNAMIC_PUSH_CONN",uid);

        std::wstring pushUri;
        readWStrArg(aMsg,"IPC_PR_REGISTER_DYNAMIC_PUSH_CONN",pushUri);

        std::wstring midletName;
        readWStrArg(aMsg,"IPC_PR_REGISTER_DYNAMIC_PUSH_CONN",midletName);

        std::wstring filter;
        readWStrArg(aMsg,"IPC_PR_REGISTER_DYNAMIC_PUSH_CONN",filter);

        int isUidOfThisMidlet = readIntAsBooleanArg(aMsg,"IPC_PR_REGISTER_DYNAMIC_PUSH_CONN");

        mPushDataContainer->storeDynamicPushRegistration(uid,pushUri,midletName,filter);

        if (0 == isUidOfThisMidlet)
        {
            LOG1WSTR(EJavaPush,EInfo,"handleRegDynamicPushConnMsg() Uid: %s",uid.toString());
            mConnCompletedTimerContainer->setTimer(pushUri,aMsg);
            launchMidlet(uid,java::captain::RTC_LAUNCH_TYPE_PUSH_C);
        }

        sendCommsResponseInSuccessCase(IPC_PR_REGISTER_DYNAMIC_PUSH_CONN_REPLY,uid,aMsg.getSender(),
                                       aMsg.getMessageRef());
    }
    catch (ExceptionBase& ex)
    {
        sendCommsResponseInErrorCase(ex,IPC_PR_REGISTER_DYNAMIC_PUSH_CONN_REPLY,uid,aMsg.getSender(),
                                     aMsg.getMessageRef());
    }
    catch (...)
    {
        ExceptionBase tmpBase(UNKNOWN_ERROR,"Unknown exception caught",
                              __FILE__,__FUNCTION__,__LINE__);
        sendCommsResponseInErrorCase(tmpBase,IPC_PR_REGISTER_DYNAMIC_PUSH_CONN_REPLY,uid,aMsg.getSender(),
                                     aMsg.getMessageRef());
    }
}

/**
 *
 */
void PushRegistryHandler::handleUnregDynamicPushConnMsg(CommsMessage& aMsg)
{
    JELOG2(EJavaPush);
    Uid uid;
    try
    {
        readUidArg(aMsg,"IPC_PR_UNREGISTER_DYNAMIC_PUSH_CONN",uid);
        std::wstring pushUri;
        readWStrArg(aMsg,"IPC_PR_UNREGISTER_DYNAMIC_PUSH_CONN",pushUri);
        int isUidOfThisMidlet = readIntAsBooleanArg(aMsg,"IPC_PR_UNREGISTER_DYNAMIC_PUSH_CONN");

        mPushDataContainer->unregisterDynamicPushRegistration(uid,pushUri);

        if (0 == isUidOfThisMidlet)
        {
            LOG1WSTR(EJavaPush,EInfo,"handleUnregDynamicPushConnMsg() Uid: %s",uid.toString());
            mConnCompletedTimerContainer->setTimer(pushUri,aMsg);
            launchMidlet(uid,java::captain::RTC_LAUNCH_TYPE_PUSH_C);
        }

        sendCommsResponseInSuccessCase(IPC_PR_UNREGISTER_DYNAMIC_PUSH_CONN_REPLY,uid,aMsg.getSender(),
                                       aMsg.getMessageRef());
    }
    catch (ExceptionBase& ex)
    {
        sendCommsResponseInErrorCase(ex,IPC_PR_UNREGISTER_DYNAMIC_PUSH_CONN_REPLY,uid,aMsg.getSender(),
                                     aMsg.getMessageRef());
    }
    catch (...)
    {
        ExceptionBase tmpBase(UNKNOWN_ERROR,"Unknown exception caught",
                              __FILE__,__FUNCTION__,__LINE__);
        sendCommsResponseInErrorCase(tmpBase,IPC_PR_UNREGISTER_DYNAMIC_PUSH_CONN_REPLY,uid,aMsg.getSender(),
                                     aMsg.getMessageRef());
    }
}

/**
 *
 */
void PushRegistryHandler::handleRegisterAlarmMsg(CommsMessage& aMsg)
{
    JELOG2(EJavaPush);
    Uid suiteUid;
    long long retWakeupTime = -1LL;
    try
    {
        readUidArg(aMsg,"IPC_PR_REGISTER_ALARM",suiteUid);

        long long alarmTime = readLongLongArg(aMsg,"IPC_PR_REGISTER_ALARM");

        std::wstring classNameOfMidlet;
        readWStrArg(aMsg,"IPC_PR_REGISTER_ALARM",classNameOfMidlet);

        int isUidOfThisMidlet = readIntAsBooleanArg(aMsg,"IPC_PR_REGISTER_ALARM");
        bool isUidOfThisMidletAsBool = false;
        if (1 == isUidOfThisMidlet)
            isUidOfThisMidletAsBool = true;

        const java::util::Uid& midletUid = mPushDataContainer->getMidletUid(suiteUid,classNameOfMidlet);
        retWakeupTime = mPushAlarmHandler->setAlarm(midletUid,alarmTime,isUidOfThisMidletAsBool);
        sendRegisterAlarmReply(aMsg.getSender(),suiteUid,retWakeupTime,aMsg.getMessageRef());
    }
    catch (ExceptionBase& ex)
    {
        sendRegisterAlarmReplyInErrorCase(ex,aMsg.getSender(),suiteUid,aMsg.getMessageRef());
    }
    catch (...)
    {
        ExceptionBase tmpBase(UNKNOWN_ERROR,"Unknown exception caught",
                              __FILE__,__FUNCTION__,__LINE__);
        sendRegisterAlarmReplyInErrorCase(tmpBase,aMsg.getSender(),suiteUid,aMsg.getMessageRef());
    }
}

/**
 *
 */
void PushRegistryHandler::handleAllConnsFailedMsg(CommsMessage& aMsg)
{
    JELOG2(EJavaPush);

    try
    {
        Uid uid;
        readUidArg(aMsg,"IPC_PR_ALL_CONNS_FAILED",uid);
        mPushDataContainer->setMidletDoNotLaunchList(uid);
        RtcInterface* rtc = mCore->getRtc();
        rtc->terminate(uid);
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Exception occurs in the handling a IPC_PR_ALL_CONNS_FAILED comms msg");
    }
}

/**
 *
 */
void PushRegistryHandler::handlePendingStatusMsg(CommsMessage& aMsg)
{
    JELOG2(EJavaPush);

    try
    {
        Uid uid;
        readUidArg(aMsg,"IPC_PR_PENDING_STATUS",uid);

        std::wstring pushUri;
        readWStrArg(aMsg,"IPC_PR_PENDING_STATUS",pushUri);

        int msgWaitsHandling = readIntAsBooleanArg(aMsg,"IPC_PR_PENDING_STATUS");
        bool msgWaitsHandlingFlag = false;
        if (1 == msgWaitsHandling)
            msgWaitsHandlingFlag = true;

        mPushDataContainer->setPendingStatusOfConn(uid,pushUri,msgWaitsHandlingFlag);
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Exception occurs in the handling a IPC_PR_PENDING_STATUS comms msg");
    }
}

/**
 *
 */
void PushRegistryHandler::handleListConnectionsMsg(CommsMessage& aMsg)
{
    JELOG2(EJavaPush);

    Uid uid;
    try
    {
        readUidArg(aMsg,"IPC_PR_LIST_CONNECTIONS",uid);
        int availableConns = readIntAsBooleanArg(aMsg,"IPC_PR_LIST_CONNECTIONS");
        bool allConnsFlag = true;
        if (1 == availableConns)
            allConnsFlag = false;

        std::list<std::wstring> listOfPushUris;
        mPushDataContainer->getPushRegsOfMidletSuite(uid,allConnsFlag,listOfPushUris);

        CommsMessage listConnsReply;
        listConnsReply.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
        listConnsReply.setReceiver(aMsg.getSender());
        listConnsReply.setModuleId(PLUGIN_ID_PUSH_CONTROLLER_C);
        listConnsReply.setMessageId(IPC_PR_LIST_CONNECTIONS_REPLY);
        listConnsReply.setMessageRef(aMsg.getMessageRef());
        listConnsReply << uid;
        listConnsReply << NO_ERROR;
        listConnsReply << "";
        std::list<std::wstring>::iterator iter;
        for (iter = listOfPushUris.begin(); iter != listOfPushUris.end(); ++iter)
        {
            LOG1WSTR(EJavaPush,EInfo,"handleListConnectionsMsg() URI: %s",(*iter));
            listConnsReply << (*iter);
        }//end for

        int retValue = mCore->getComms()->send(listConnsReply);
        if (0 != retValue)
        {
            //Exception is not thrown from this situation.
            std::string errTxt("Sending IPC_PR_LIST_CONNECTIONS_REPLY comms request failed: ");
            errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
            ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        }
    }
    catch (ExceptionBase& ex)
    {
        sendCommsResponseInErrorCase(ex,IPC_PR_LIST_CONNECTIONS_REPLY,uid,aMsg.getSender(),
                                     aMsg.getMessageRef());
    }
    catch (...)
    {
        ExceptionBase tmpBase(UNKNOWN_ERROR,"Unknown exception caught",
                              __FILE__,__FUNCTION__,__LINE__);
        sendCommsResponseInErrorCase(tmpBase,IPC_PR_LIST_CONNECTIONS_REPLY,uid,aMsg.getSender(),
                                     aMsg.getMessageRef());
    }
}

/**
 *
 */
void PushRegistryHandler::handleDataOfPushConnQueryMsg(CommsMessage& aMsg)
{
    JELOG2(EJavaPush);

    Uid suiteUid;
    string nameOfMsg("IPC_PR_DATA_OF_PUSH_CONN_QUERY");
    try
    {
        readUidArg(aMsg,nameOfMsg,suiteUid);
        std::wstring uriAsStr;
        readWStrArg(aMsg,nameOfMsg,uriAsStr);
        int queryArg = readIntArg(aMsg,nameOfMsg);

        std::wstring retStr;
        if (FILTER_OF_PUSH_CONN == (EPushInfoQueryParams) queryArg)
            retStr = mPushDataContainer->getFilterOfPushConn(suiteUid,uriAsStr);
        else if (CLASSNAME_OF_PUSH_CONN == (EPushInfoQueryParams) queryArg)
            retStr = mPushDataContainer->getClassNameOfPushConn(suiteUid,uriAsStr);
        else
        {
            throw PushException(INCORRECT_COMMS_MSG,"Incorrect value in the query param",
                                __FILE__,__FUNCTION__,__LINE__);
        }

        CommsMessage replyMsg;
        replyMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
        replyMsg.setReceiver(aMsg.getSender());
        replyMsg.setModuleId(PLUGIN_ID_PUSH_CONTROLLER_C);
        replyMsg.setMessageId(IPC_PR_DATA_OF_PUSH_CONN_QUERY_REPLY);
        replyMsg.setMessageRef(aMsg.getMessageRef());
        replyMsg << suiteUid;
        replyMsg << NO_ERROR;
        replyMsg << "";
        replyMsg << retStr;

        int retValue = mCore->getComms()->send(replyMsg);
        if (0 != retValue)
        {
            //Exception is not thrown from this situation.
            std::string errTxt("Sending IPC_PR_DATA_OF_PUSH_CONN_QUERY_REPLY comms request failed: ");
            errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
            ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        }
    }
    catch (ExceptionBase& ex)
    {
        sendCommsResponseInErrorCase(ex,IPC_PR_DATA_OF_PUSH_CONN_QUERY_REPLY,suiteUid,
                                     aMsg.getSender(),aMsg.getMessageRef());
    }
    catch (...)
    {
        ExceptionBase tmpBase(UNKNOWN_ERROR,"Unknown exception caught",
                              __FILE__,__FUNCTION__,__LINE__);
        sendCommsResponseInErrorCase(tmpBase,IPC_PR_DATA_OF_PUSH_CONN_QUERY_REPLY,suiteUid,
                                     aMsg.getSender(),aMsg.getMessageRef());
    }
}

/**
 *
 */
void PushRegistryHandler::handlePushUriStartsWithQueryMsg(CommsMessage& aMsg)
{
    JELOG2(EJavaPush);

    try
    {
        std::wstring uriAsStr;
        readWStrArg(aMsg,"IPC_PR_PUSH_URI_STARTS_WITH_QUERY",uriAsStr);

        Uid suiteUid;
        bool uriFound = mPushDataContainer->getSuiteUidByBaseUri(uriAsStr,suiteUid);
        int status = -1;
        if (true == uriFound)
            status = 0;

        CommsMessage replyMsg;
        replyMsg.replyTo(aMsg);
        replyMsg.setMessageId(IPC_PR_PUSH_URI_STARTS_WITH_QUERY_REPLY);
        replyMsg << status;
        if (0 == status)
            replyMsg << suiteUid;
        int retValue = mCore->getComms()->send(replyMsg);
        if (0 != retValue)
        {
            //Exception is not thrown from this situation.
            std::string errTxt("Sending IPC_PR_PUSH_URI_STARTS_WITH_QUERY_REPLY comms request failed: ");
            errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
            ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        }
    }
    catch (...)
    {
        //Exceptions should not been thrown in this operation.
        sendPushUriStartsWithQueryReplyInErrorCase(aMsg);
    }
}

/**
 *
 */
void PushRegistryHandler::handleConnCompletedMsg(CommsMessage& aMsg)
{
    JELOG2(EJavaPush);

    std::wstring pushUri;
    readWStrArg(aMsg,"IPC_PR_UNREG_COMPLETED",pushUri);

    mConnCompletedTimerContainer->cancelTimer(pushUri);
}

/**
 *
 */
void PushRegistryHandler::sendCommsResponseInSuccessCase(int aMsgId,const Uid& aUid,
        int aSenderOfOrigMsg,int aMsgRef)
{
    JELOG2(EJavaPush);

    CommsMessage replyMsg;
    replyMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    replyMsg.setReceiver(aSenderOfOrigMsg);
    replyMsg.setModuleId(PLUGIN_ID_PUSH_CONTROLLER_C);
    replyMsg.setMessageId(aMsgId);
    replyMsg.setMessageRef(aMsgRef);

    replyMsg << aUid;
    replyMsg << NO_ERROR;
    replyMsg << "";
    int retValue = mCore->getComms()->send(replyMsg);
    if (0 != retValue)
    {
        std::string errTxt("Sending a comms request failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
        ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
    }
}

/**
 *
 */
void PushRegistryHandler::sendRegisterAlarmReply(int aSenderOfOrigMsg,const Uid& aUid,
        const long long& aTimeInMilliSecs,int aMsgRef)
{
    JELOG2(EJavaPush);

    CommsMessage replyMsg;
    replyMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    replyMsg.setReceiver(aSenderOfOrigMsg);
    replyMsg.setModuleId(PLUGIN_ID_PUSH_CONTROLLER_C);
    replyMsg.setMessageId(IPC_PR_REGISTER_ALARM_REPLY);
    replyMsg.setMessageRef(aMsgRef);

    replyMsg << aUid;
    replyMsg << NO_ERROR;
    replyMsg << "";
    replyMsg << aTimeInMilliSecs;
    int retValue = mCore->getComms()->send(replyMsg);
    if (0 != retValue)
    {
        std::string errTxt("Sending a comms request failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
        ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
    }
}

/**
 *
 */
void PushRegistryHandler::sendRegisterAlarmReplyInErrorCase(ExceptionBase& aException,
        int aSenderOfOrigMsg,
        const Uid& aUid,
        int aMsgRef)
{
    JELOG2(EJavaPush);
    std::string errTxt = aException.toString();
    int errCode = aException.mErrCode;

    CommsMessage replyMsg;
    replyMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    replyMsg.setReceiver(aSenderOfOrigMsg);
    replyMsg.setModuleId(PLUGIN_ID_PUSH_CONTROLLER_C);
    replyMsg.setMessageId(IPC_PR_REGISTER_ALARM_REPLY);
    replyMsg.setMessageRef(aMsgRef);

    long long tmpTimeValue = 0LL;
    replyMsg << aUid;
    replyMsg << errCode;
    replyMsg << errTxt;
    replyMsg << tmpTimeValue;
    int retValue = mCore->getComms()->send(replyMsg);
    if (0 != retValue)
    {
        std::string errTxt("Sending an IPC_PR_REGISTER_ALARM_REPLY request failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
        ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
    }
}

/**
 *
 */
void PushRegistryHandler::sendCommsResponseInErrorCase(ExceptionBase& aException,
        int aMsgId,const Uid& aUid,
        int aSenderOfOrigMsg,
        int aMsgRef)
{
    JELOG2(EJavaPush);
    std::string errTxt = aException.toString();
    int errCode = aException.mErrCode;

    CommsMessage replyMsg;
    replyMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    replyMsg.setReceiver(aSenderOfOrigMsg);
    replyMsg.setModuleId(PLUGIN_ID_PUSH_CONTROLLER_C);
    replyMsg.setMessageId(aMsgId);
    replyMsg.setMessageRef(aMsgRef);

    replyMsg << aUid;
    replyMsg << errCode;
    replyMsg << errTxt;
    //This error is ignored because original error is wanted to show to the user.
    int retValue = mCore->getComms()->send(replyMsg);
    if (0 != retValue)
    {
        std::string errTxt("Sending a comms request failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
        ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
    }
}

/**
 *
 */
void PushRegistryHandler::sendPushUriStartsWithQueryReplyInErrorCase(CommsMessage& aMsg)
{
    JELOG2(EJavaPush);

    try
    {
        CommsMessage replyMsg;
        replyMsg.replyTo(aMsg);
        replyMsg.setMessageId(IPC_PR_PUSH_URI_STARTS_WITH_QUERY_REPLY);
        replyMsg << (int) -1;
        int retValue = mCore->getComms()->send(replyMsg);
        if (0 != retValue)
        {
            //Exception is not thrown from this situation.
            std::string errTxt("Sending IPC_PR_PUSH_URI_STARTS_WITH_QUERY_REPLY comms request failed: ");
            errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
            ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        }
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Unexpected exception caught in sendPushUriStartsWithQueryReplyInErrorCase() operation");
    }
}

/**
 *
 */
void PushRegistryHandler::startPushMidlets()
{
    JELOG2(EJavaPush);
    //Retrieving all push registrations from the db.
    std::set<Uid> uidList;
    mPushDataContainer->getIdsOfMidlets(uidList);
    std::set<Uid>::iterator iter;
    for (iter = uidList.begin(); iter != uidList.end(); ++iter)
    {
        LOG1WSTR(EJavaPush,EInfo,"startPushMidlets() Uid: %s",iter->toString());
        launchMidlet((*iter),java::captain::RTC_LAUNCH_TYPE_PUSH_C);
    }
}

/**
 *
 */
void PushRegistryHandler::deleteAppData(const java::util::Uid& aUid,const bool& aDeleteAlarm)
{
    JELOG2(EJavaPush);
    if (true == aDeleteAlarm)
    {
        //Cancelling pending alarm.
        mPushAlarmHandler->cancelAlarm(aUid);
    }
    //Removing push registrations of the application from the internal cache.
    mPushDataContainer->removeApp(aUid);
}

/**
 *
 */
void PushRegistryHandler::readAppData(const java::util::Uid& aUid,const bool& aUpdateAlarm)
{
    JELOG2(EJavaPush);
    if (true == aUpdateAlarm)
    {
        mPushAlarmHandler->readAndStartAlarms(aUid);
    }
    mPushDataContainer->readAppDataFromDb(aUid);
    bool isPushConns = mPushDataContainer->isPushConnections(aUid);
    if (true == isPushConns)
    {
        LOG1WSTR(EJavaPush,EInfo,"readAppData() Uid: %s",aUid.toString());
        launchMidlet(aUid,java::captain::RTC_LAUNCH_TYPE_PUSH_C);
    }
}

/**
 *
 */
void PushRegistryHandler::manageDriveInsertion(const driveInfo& aDriveInfo)
{
    JELOG2(EJavaPush);

    //On the safe side, first is checked whether a new drive already exists
    //in the mDriveInfo container. This should not never happened.
    removeDriveFromContainer(aDriveInfo);

    //Next a new drive is added to the mDriveInfo container.
    mDriveInfo.push_back(aDriveInfo);

    //Managing MIDlets which alarm was expired when added drive was not in the device.
    mPushAlarmHandler->handleDriveWasMissingMidlets(aDriveInfo.iId);

    //Launching all push MIDlets in the push mode which has been installed to a new drive.
    startOrKillMidlet(aDriveInfo.iId,false);
}

/**
 *
 */
void PushRegistryHandler::manageDriveRemoval(const driveInfo& aDriveInfo)
{
    JELOG2(EJavaPush);

    //First is removed drive from the mDriveInfo container.
    removeDriveFromContainer(aDriveInfo);

    //Killing all push MIDlets which are installed to the removed drive.
    startOrKillMidlet(aDriveInfo.iId,true);
}

/**
 *
 */
void PushRegistryHandler::startOrKillMidlet(const unsigned int aMediaId,
        bool aTerminateMidlet)
{
    JELOG2(EJavaPush);

    //Retrieving uids of MIDlets installed to the added drive.
    list<Uid> uidList;
    mPushDataContainer->getMidletUidsByMediaId(aMediaId,uidList);

    //Start push MIDlets in push listening mode.
    RtcInterface* rtc = mCore->getRtc();
    for (list<Uid>::const_iterator iter = uidList.begin(); iter != uidList.end(); ++iter)
    {
        if (false == aTerminateMidlet)
        {
            if (true == mPushDataContainer->isPushConnections((*iter)))
            {
                //MIDlet is removed from "do not launch" list also when
                //mmc(wherein MIDlet has been installed) is inserted to the device.
                mPushDataContainer->removeMidletFromDoNotLaunchList((*iter));
                LOG1WSTR(EJavaPush,EInfo,"startOrKillMidlet() launch Uid: %s",iter->toString());
                rtc->launch(rtcLaunchInfo((*iter),java::captain::RTC_LAUNCH_TYPE_PUSH_C));
            }
        }
        else
        {
            mPushDataContainer->clearPendingStatusesOfMidlet((*iter));
            LOG1WSTR(EJavaPush,EInfo,"startOrKillMidlet() terminate Uid: %s",iter->toString());
            rtc->terminate((*iter));
        }
    }
    uidList.clear();
}

/**
 *
 */
void PushRegistryHandler::removeDriveFromContainer(const driveInfo& aDriveInfo)
{
    //Root path is unique for every drive and it is always available
    //in the driveInfo so it is safe to compare equality of two driveInfo objects
    //with this value.
    //In theory, there might be duplicate driveInfo objects with same root path
    //in the vector, so it is better to loop through a whole vector.
    //Multiple objects can be removed from vector by stl's remove_if method.
    DriveInfoComparisonUtil searchObj(aDriveInfo);
    mDriveInfo.erase( remove_if(mDriveInfo.begin(),mDriveInfo.end(),searchObj) ,
                      mDriveInfo.end() );
}

/**
 *
 */
bool PushRegistryHandler::isDriveAvailable(int aMediaId)
{
    JELOG2(EJavaPush);

    driveInfos::iterator iter = mDriveInfo.begin();
    for (; iter != mDriveInfo.end(); ++iter)
    {
        if (aMediaId == iter->iId)
        {
            //iIsPresent should never be false when it is added to the mDriveInfo
            //container.
            if (true == iter->iIsPresent)
                return true;
        }
    }//end for
    return false;
}

/**
 *
 */
bool PushRegistryHandler::launchMidlet(const Uid& aMidletUid,
                                       const int& aLaunchType)
{
    JELOG2(EJavaPush);

    int mediaId = mPushDataContainer->getMediaIdByMidletUid(aMidletUid);

    bool isDriveAvailableFlag = false;
    //Media id 0 is reserved for non-removable drives (e.g. c:\ drive) so
    //we does not have to check drive's availability in that case.
    if (0 == mediaId)
        isDriveAvailableFlag = true;
    else
    {
        if (UNDEFINED_MEDIA_ID != mediaId)
            isDriveAvailableFlag = isDriveAvailable(mediaId);
    }

    if ((MEDIA_ID_IN_LINUX == mediaId) || (true == isDriveAvailableFlag))
    {
        LOG2(EJavaPush,EInfo,"MIDlet launched UID: %S, launch type: %d",
             aMidletUid.toString().c_str(),aLaunchType);
        RtcInterface* rtc = mCore->getRtc();
        rtc->launch(rtcLaunchInfo(aMidletUid,aLaunchType));
        return true;
    }
    return false;
}

/**
 *
 */
int PushRegistryHandler::getMediaIdByMidletUid(const Uid& aMidletUid)
{
    JELOG2(EJavaPush);

    return mPushDataContainer->getMediaIdByMidletUid(aMidletUid);
}

/**
 *
 */
void PushRegistryHandler::readUidArg(CommsMessage& aMsg,const std::string& aMsgName,Uid& aUid)
{
    if (!(aMsg >> aUid))
    {
        std::string errTxt("Cannot read UID from ");
        errTxt.append(aMsgName);
        errTxt.append(" msg");
        ELOG1(EJavaPush,"ERROR!!! %s",errTxt.c_str());
        throw PushException(INCORRECT_COMMS_MSG,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }
}

/**
 *
 */
void PushRegistryHandler::readWStrArg(CommsMessage& aMsg,const std::string& aMsgName,std::wstring& aStr)
{
    if (!(aMsg >> aStr))
    {
        std::string errTxt("Cannot read wstring from ");
        errTxt.append(aMsgName);
        errTxt.append(" msg");
        ELOG1(EJavaPush,"ERROR!!! %s",errTxt.c_str());
        throw PushException(INCORRECT_COMMS_MSG,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }
}

/**
 *
 */
int PushRegistryHandler::readIntAsBooleanArg(CommsMessage& aMsg,const std::string& aMsgName)
{
    int retValue = readIntArg(aMsg,aMsgName);

    if ((0 != retValue) && (1 != retValue))
    {
        std::string errTxt("Incorrect integer value in the ");
        errTxt.append(aMsgName);
        errTxt.append(" msg");
        ELOG1(EJavaPush,"ERROR!!! %s",errTxt.c_str());
        throw PushException(INCORRECT_COMMS_MSG,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }
    return retValue;
}

/**
 *
 */
int PushRegistryHandler::readIntArg(CommsMessage& aMsg,const std::string& aMsgName)
{
    int retValue = -1;
    if (!(aMsg >> retValue))
    {
        std::string errTxt("Cannot read int from ");
        errTxt.append(aMsgName);
        errTxt.append(" msg");
        ELOG1(EJavaPush,"ERROR!!! %s",errTxt.c_str());
        throw PushException(INCORRECT_COMMS_MSG,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }
    return retValue;
}

/**
 *
 */
long long PushRegistryHandler::readLongLongArg(CommsMessage& aMsg,const std::string& aMsgName)
{
    long long retValue = -1;
    if (!(aMsg >> retValue))
    {
        std::string errTxt("Cannot read long long value from ");
        errTxt.append(aMsgName);
        errTxt.append(" msg");
        ELOG1(EJavaPush,"ERROR!!! %s",errTxt.c_str());
        throw PushException(INCORRECT_COMMS_MSG,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }
    return retValue;
}


