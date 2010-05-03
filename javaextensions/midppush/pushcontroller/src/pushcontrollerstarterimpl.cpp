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


#include <errno.h>
#include <pthread.h>
#include <iostream>
#include <sstream>

#include "logger.h"

#include "dynamiclibloader.h"
#include "runtimeexception.h"
#include "javaosheaders.h"

#include "midpruntimepushsupport.h"

#include "pushcontrollerstarterimpl.h"
#include "serverconnectionfactory.h"
#include "serverconnection.h"
#include "pushregistrymessages.h"
#include "comms.h"
#include "commsclientendpoint.h"
#include "commsmessage.h"
#include "javacommonutils.h"
#include "pusherrorcodes.h"
#include "pushexception.h"
#include "pushconstant.h"
#include "javauid.h"
#include "libraryloaderexception.h"

using namespace java::push;
using namespace java::runtime;
using namespace java::comms;
using namespace java::util;
using namespace std;

#if defined(__SYMBIAN32__) && defined(__WINSCW__)
#include <pls.h>
#else
static PushControllerStarterImpl* sPushContStarter = 0;
#endif

#ifdef __SYMBIAN32__
PushControllerStarter& getPushControllerStarter()
{
#else
extern "C" PushControllerStarter& getPushControllerStarter()
{
#endif

    JELOG2(EJavaPush);

#if defined(__SYMBIAN32__) && defined(__WINSCW__)
    TUid uid = TUid::Uid(0xE0000050);
    PushControllerStarterImpl* retObj = Pls<PushControllerStarterImpl>(uid);
    return *retObj;
#else
    if (sPushContStarter == 0)
    {
        sPushContStarter = new PushControllerStarterImpl();
    }
    return *sPushContStarter;
#endif
}

/**
 *
 */
PushControllerStarterImpl::PushControllerStarterImpl()
        : mSrvConnPluginMgr(0),mAppStateController(0),mState(INITIAL_STATE),
        mThreadId(0),mMonitor(Monitor::createMonitor()),mConnCompletedMonitor(0),
        mIsUpdate(false),mNumOfPendingUpdates(0)
{
    JELOG2(EJavaPush);
    mSrvConnPluginMgr.reset(new PushServerConnPluginManager(this,this,this));
}

/**
 *
 */
PushControllerStarterImpl::~PushControllerStarterImpl()
{
    JELOG2(EJavaPush);
    try
    {
        CommsClientEndpoint& comms = getComms();
        comms.unregisterListener(PLUGIN_ID_PUSH_CONTROLLER_C,this);
    }
    catch (...) {}
}

/**
 * This operation throws exception if initialization of all connections
 * fails. RuntimeStarter closes application if it gets exception from this
 * operation.
 * MidpStarterPushSupport::closeRuntimeRequestFromPush()
 * operation is called from error() operation if opening all connections fails.
 */
void PushControllerStarterImpl::startListen(const java::util::Uid& aUid,
        MidpStarterPushSupport* aAppStateController)
{
    JELOG2(EJavaPush);

    setState(LOADING_PUSH_CONNS);
    mAppStateController = aAppStateController;

    registerCommsListener();
    mIsUpdate = false;
    mUid = aUid;
    bool retValue = sendGetAllPushRegsMsg(false);
    if (false == retValue)
        setState(INITIAL_STATE);
}

/**
 *
 */
void PushControllerStarterImpl::updatePushRegs(const java::util::Uid& aUid,
        MidpStarterPushSupport* aAppStateController)

{
    JELOG2(EJavaPush);
    updatePushRegs2(aUid,aAppStateController,false);
}

/**
 * This operation can not throw exceptions.
 * This operation is always called from comms thread so locks for mNumOfPendingUpdates
 * datamember is not needed. Value of mNumOfPendingUpdates datamember is increased if
 * previous updating is ongoing when next update request arrives.
 */
void PushControllerStarterImpl::updatePushRegs2(const java::util::Uid& aUid,
        MidpStarterPushSupport* aAppStateController,
        bool aThrowingExceptionAllowed)
{
    JELOG2(EJavaPush);

    try
    {
        if (LOADING_PUSH_CONNS == getState())
        {
            ++mNumOfPendingUpdates;
            return;
        }

        registerCommsListener();
        mUid = aUid;
        mAppStateController = aAppStateController;
        mIsUpdate = true;
        sendGetAllPushRegsMsg(aThrowingExceptionAllowed);
        ++mNumOfPendingUpdates;
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! updatePushRegs2(): Unexpected exception was caught");
        if (true == aThrowingExceptionAllowed)
            throw;
    }
}

/**
 *
 */
void PushControllerStarterImpl::close()
{
    JELOG2(EJavaPush);
    try
    {
        //Destructor of PushServerConnPluginManager class calls
        //ServerConnectionFactory::closeAllConnections() operation for each plugin.
        delete mSrvConnPluginMgr.release();
        setState(INITIAL_STATE);
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! PushControllerStarterImpl::close(): Unexpected exception was caught");
    }
}

/**
 *
 */
void PushControllerStarterImpl::msgArrived(const std::wstring& aInfo)
{
    JELOG2(EJavaPush);

    if (0 != mAppStateController)
    {
        mAppStateController->startMidletRequestFromPush(aInfo);
    }
}

/**
 *
 */
void PushControllerStarterImpl::error(const std::wstring& aUri,int aErrCode,
                                      const std::string& aErrText)
{
    JELOG2(EJavaPush);

    ELOG2(EJavaPush, "ERROR!!! err code: %d, err txt: %s",aErrCode,aErrText.c_str());
    int numOfSuccessfulConns = getPushServerConnPluginManager().setConnFailed(aUri);
    if (0 == numOfSuccessfulConns)
    {
        sendAllConnsFailedMsg();
        closeRuntime();
    }
}

/**
 *
 */
void PushControllerStarterImpl::processMessage(CommsMessage& aMessage)
{
    JELOG2(EJavaPush);

    switch (aMessage.getMessageId())
    {
    case IPC_PR_LIST_OF_ALL_PUSH_REGS:
    {
        handleListOfAllPushRegistrationsMsg(aMessage);
        break;
    }
    case IPC_PR_CONN_MANAGED_BY_OTHER_MIDLET:
    {
        mConnCompletedMonitor->notify();
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
bool PushControllerStarterImpl::sendGetAllPushRegsMsg(bool aThrowingExceptionAllowed)
{
    JELOG2(EJavaPush);

    CommsMessage getAllPushRegsMsg;
    getAllPushRegsMsg.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
    getAllPushRegsMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    getAllPushRegsMsg.setModuleId(PLUGIN_ID_PUSH_REGISTRY_C);
    getAllPushRegsMsg.setMessageId(IPC_PR_GET_ALL_PUSH_REGS);

    getAllPushRegsMsg << mUid;
    CommsClientEndpoint& comms = getComms();
    EState stateOfObj = getState();
    setState(LOADING_PUSH_CONNS);
    int retValue = comms.send(getAllPushRegsMsg);
    if (0 != retValue)
    {
        setState(stateOfObj);
        std::string errTxt("Sending a comms request failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
        ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        if (false == aThrowingExceptionAllowed)
        {
            sendAllConnsFailedMsg();
            closeRuntime();
            return false;
        }
        throw PushException(GENERAL_COMMS_ERROR,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }
    //setState(LOADING_PUSH_CONNS);
    return true;
}

/**
 *
 */
void PushControllerStarterImpl::handleListOfAllPushRegistrationsMsg(CommsMessage& aMessage)
{
    JELOG2(EJavaPush);

    try
    {
        //std::string tmpErrTxt("Retrieving push registrations failed");
        validateCommonParamsOfReplyCommsMsg(0,IPC_PR_LIST_OF_ALL_PUSH_REGS,
                                            aMessage,mUid);
        std::wstring uriStr;
        std::wstring filterStr;
        PushServerConnPluginManager::UriList_t uriList;
        while ((aMessage >> uriStr))
        {
            if (!(aMessage >> filterStr))
            {
                throw PushException(INCORRECT_COMMS_MSG,"Could not resolve 'filter' parameter",
                                    __FILE__,__FUNCTION__,__LINE__);
            }
            PushRegData tmpObj(uriStr,filterStr);
            uriList.push_back(tmpObj);
        }

        if (false == mIsUpdate)
            getPushServerConnPluginManager().startListen(uriList,false,true);
        else
        {
            getPushServerConnPluginManager().updatePushRegs(uriList,*this);
            //In this case updatePushRegs() has called closeRuntime() operation
            //so no sense to retrieve new updates.
            if (INITIAL_STATE == getState())
                return;

            if (1 < mNumOfPendingUpdates)
            {
                --mNumOfPendingUpdates;
                sendGetAllPushRegsMsg(false);
                return;
            }
            else if (1 == mNumOfPendingUpdates)
                --mNumOfPendingUpdates;
        }//end else

        setState(PUSH_CONNS_LOADED);
    }
    catch (PushException& ex)
    {
        setState(INITIAL_STATE);
        ELOG1(EJavaPush, "ERROR!!! PushControllerStarterImpl::getAndInitAllPushRegs(): %s",
              ex.toString().c_str());
        //"All connection has failed" message is sent to pushregistryplugin if any error occurs
        //when push connections are retrieved. This prevents infinite loop if pushregistryplugin
        //has problems to provide push connections.
        sendAllConnsFailedMsg();
        closeRuntime();

    }
    catch (...)
    {
        setState(INITIAL_STATE);
        ELOG(EJavaPush,"ERROR!!! getAndInitAllPushRegs(): Unexpected exception was caught");
        sendAllConnsFailedMsg();
        closeRuntime();
    }
    mIsUpdate = false;
    mMonitor->notify();
}

/**
 *
 */
void PushControllerStarterImpl::registerDynamicPushConn(const std::wstring& aUri,
        const std::wstring& aMidlet,
        const std::wstring& aFilter,
        const Uid& aUid,
        bool aIsUidOfThisMidlet)
{
    JELOG2(EJavaPush);

    bool pushSrvConnObjectCreated = false;
    try
    {
        int isUidOfThisMidlet = 0;
        if (true == aIsUidOfThisMidlet)
        {
            loadPushConns(aUid);
            isUidOfThisMidlet = 1;
            PushRegData tmpRegData(aUri,aFilter);
            PushServerConnPluginManager::UriList_t pushList;
            pushList.push_back(tmpRegData);
            getPushServerConnPluginManager().startListen(pushList,true,false);
            pushSrvConnObjectCreated = true;
        }

        if (false == aIsUidOfThisMidlet)
        {
            registerCommsListener();
            //We have to open a connection in this situation to be sure that connection
            //is not already reserved. Connection is closed immediately.
            if (false == getPushServerConnPluginManager().isValidPushUri(aUri))
            {
                throw PushException(PUSH_CONNECTION_ALREADY_EXISTS,
                                    "Connection is reserved by another application",
                                    __FILE__,__FUNCTION__,__LINE__);
            }
        }

        sendRegDynPushConnMsg(aUri,aMidlet,aFilter,aUid,isUidOfThisMidlet);

        if (false == aIsUidOfThisMidlet)
        {
            startToWaitCompletionOfOper();
        }
    }
    catch (PushException& ex)
    {
        ELOG1(EJavaPush, "ERROR!!! PushControllerStarterImpl::registerDynamicPushConn(): %s",
              ex.toString().c_str());
        if (true == pushSrvConnObjectCreated)
        {
            //ScopedLock is not needed in this situation.
            getPushServerConnPluginManager().deletePushServerConn(aUri);
        }
        throw;
    }
    catch (java::util::LibraryLoaderException& ex)
    {
        ELOG1(EJavaPush, "ERROR!!! PushControllerStarterImpl::registerDynamicPushConn(): %s",
              ex.toString().c_str());
        throw;
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Unexpected exception was caught");
        if (true == pushSrvConnObjectCreated)
            getPushServerConnPluginManager().deletePushServerConn(aUri);
        throw java::util::ExceptionBase("Unexpected error occurred",__FILE__,__FUNCTION__,__LINE__);
    }
}

/**
 *
 */
void PushControllerStarterImpl::listConnections(bool aAvailable,const java::util::Uid& aUid,
        std::list<std::wstring>& aUriList)
{
    JELOG2(EJavaPush);

    //loadPushConns(aUid);
    //mSrvConnPluginMgr->listConnections(aAvailable,aUriList);

    int available = 0;
    if (true == aAvailable)
        available = 1;

    CommsMessage getPushConns;
    getPushConns.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
    getPushConns.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    getPushConns.setModuleId(PLUGIN_ID_PUSH_REGISTRY_C);
    getPushConns.setMessageId(IPC_PR_LIST_CONNECTIONS);
    getPushConns << aUid;
    getPushConns << available;
    CommsClientEndpoint& comms = getComms();
    CommsMessage replyMsgToListConns;
    int retValue = comms.sendReceive(getPushConns,replyMsgToListConns,COMMS_TIMEOUT);
    //std::string tmpErrTxt("Retrieving push URIs failed");
    validateCommonParamsOfReplyCommsMsg(retValue,IPC_PR_LIST_CONNECTIONS_REPLY,
                                        replyMsgToListConns,aUid);

    std::wstring uriStr;
    while ((replyMsgToListConns >> uriStr))
    {
        aUriList.push_back(uriStr);
    }
}

/**
 *
 */
std::wstring PushControllerStarterImpl::getFilter(const std::wstring& aUri,
        const java::util::Uid& aSuiteUid)
{
    JELOG2(EJavaPush);

    //loadPushConns(aUid);
    //ServerConnection& conn = mSrvConnPluginMgr->getPushConnection(aUri);
    //return conn.getFilter();

    return sendPushDataQuery(aUri,aSuiteUid,FILTER_OF_PUSH_CONN);
}

/**
 *
 */
std::wstring PushControllerStarterImpl::getClassNameOfMidlet(const std::wstring& aUri,
        const java::util::Uid& aSuiteUid)
{
    JELOG2(EJavaPush);

    return sendPushDataQuery(aUri,aSuiteUid,CLASSNAME_OF_PUSH_CONN);
}

/**
 *
 */
bool PushControllerStarterImpl::isPushConnection(const std::wstring& aUri,
        const java::util::Uid& aUid)
{
    JELOG2(EJavaPush);

    try
    {
        loadPushConns(aUid);
        getPushServerConnPluginManager().getPushConnection(aUri);
        return true;
    }
    catch (...)
    {
        ELOG(EJavaPush,"ERROR!!! Exception was caught");
        return false;
    }
}

/**
 *
 */
void PushControllerStarterImpl::unregisterDynamicPushConn(const std::wstring& aUri,
        const Uid& aUid,
        bool aIsUidOfThisMidlet)
{
    JELOG2(EJavaPush);

    int isUidOfThisMidlet = 0;
    if (true == aIsUidOfThisMidlet)
    {
        loadPushConns(aUid);
        isUidOfThisMidlet = 1;
    }
    else
    {
        registerCommsListener();
    }

    CommsMessage regDynPushRegMsg;
    regDynPushRegMsg.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
    regDynPushRegMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    regDynPushRegMsg.setModuleId(PLUGIN_ID_PUSH_REGISTRY_C);
    regDynPushRegMsg.setMessageId(IPC_PR_UNREGISTER_DYNAMIC_PUSH_CONN);
    regDynPushRegMsg << aUid;
    regDynPushRegMsg << aUri;
    regDynPushRegMsg << isUidOfThisMidlet;
    CommsClientEndpoint& comms = getComms();
    CommsMessage statusOfUnRegDynConnMsg;
    int retValue = comms.sendReceive(regDynPushRegMsg,statusOfUnRegDynConnMsg,COMMS_TIMEOUT);
    //std::string tmpErrTxt("Unegistering a push connection failed");
    validateCommonParamsOfReplyCommsMsg(retValue,IPC_PR_UNREGISTER_DYNAMIC_PUSH_CONN_REPLY,
                                        statusOfUnRegDynConnMsg,aUid);

    //Unregister dynamic push connection from ServerConnection Plugin if it belongs to 'this'
    //MIDlet.
    if (true == aIsUidOfThisMidlet)
        getPushServerConnPluginManager().unregisterDynamicPushConn(aUri);
    else
    {
        startToWaitCompletionOfOper();
    }
}

/**
 *
 */
long long PushControllerStarterImpl::registerAlarm(const long long& aAlarmTimeInMilliSecs,
        const Uid& aSuiteUid,
        const std::wstring& aClassNameOfMidlet,
        bool aIsUidOfThisMidlet)
{
    JELOG2(EJavaPush);

    LOG1WSTR(EJavaPush,EInfo, "registerAlarm() aClassNameOfMidlet: %s",aClassNameOfMidlet);

    int isUidOfThisMidlet = 0;
    if (true == aIsUidOfThisMidlet)
        isUidOfThisMidlet = 1;

    CommsMessage regAlarmMsg;
    regAlarmMsg.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
    regAlarmMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    regAlarmMsg.setModuleId(PLUGIN_ID_PUSH_REGISTRY_C);
    regAlarmMsg.setMessageId(IPC_PR_REGISTER_ALARM);
    regAlarmMsg << aSuiteUid;
    regAlarmMsg << aAlarmTimeInMilliSecs;
    regAlarmMsg << aClassNameOfMidlet;
    regAlarmMsg << isUidOfThisMidlet;

    CommsClientEndpoint& comms = getComms();
    CommsMessage statusOfRegAlarmMsg;
    int retValue = comms.sendReceive(regAlarmMsg,statusOfRegAlarmMsg,COMMS_TIMEOUT);
    //std::string tmpErrTxt("Setting an alarm failed");
    validateCommonParamsOfReplyCommsMsg(retValue,IPC_PR_REGISTER_ALARM_REPLY,statusOfRegAlarmMsg,
                                        aSuiteUid);

    long long retTime = 0LL;
    if (!(statusOfRegAlarmMsg >> retTime))
    {
        std::string errTxt("Cannot resolve 'time' from the IPC_PR_REGISTER_ALARM_REPLY msg");
        throw PushException(INCORRECT_COMMS_MSG,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }

    return retTime;
}

/**
 * Note: This operation ignores all errors.
 */
void PushControllerStarterImpl::sendAllConnsFailedMsg()
{
    JELOG2(EJavaPush);

    try
    {
        CommsMessage allConnsFailedMsg;
        allConnsFailedMsg.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
        allConnsFailedMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
        allConnsFailedMsg.setModuleId(PLUGIN_ID_PUSH_REGISTRY_C);
        allConnsFailedMsg.setMessageId(IPC_PR_ALL_CONNS_FAILED);
        allConnsFailedMsg << mUid;
        CommsClientEndpoint& comms = getComms();
        int retValue = comms.send(allConnsFailedMsg);
        if (0 != retValue)
        {
            std::string errTxt("sendAllConnsFailedMsg() Sending a comms request failed: ");
            errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
            ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        }
    }
    catch (...)
    {
        ELOG(EJavaPush, "ERROR!!! Unexpected exception caught in the PushControllerStarterImpl::sendAllConnsFailedMsg()");
    }
}

/**
 *
 */
void PushControllerStarterImpl::closeRuntime()
{
    JELOG2(EJavaPush);

    if (0 != mAppStateController)
    {
        mAppStateController->closeRuntimeRequestFromPush();
        setState(INITIAL_STATE);
    }
}

/**
 *
 */
void PushControllerStarterImpl::setPendingStatus(const std::wstring& aUri,
        bool aMsgWaitsHandling)
{
    JELOG2(EJavaPush);

    try
    {
        int msgWaitsHandling = 0;
        if (true == aMsgWaitsHandling)
            msgWaitsHandling = 1;

        CommsMessage pendingStatusMsg;
        pendingStatusMsg.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
        pendingStatusMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
        pendingStatusMsg.setModuleId(PLUGIN_ID_PUSH_REGISTRY_C);
        pendingStatusMsg.setMessageId(IPC_PR_PENDING_STATUS);
        pendingStatusMsg << mUid;
        pendingStatusMsg << aUri;
        pendingStatusMsg << msgWaitsHandling;
        CommsClientEndpoint& comms = getComms();
        int retValue = comms.send(pendingStatusMsg);
        if (0 != retValue)
        {
            std::string errTxt("setPendingStatus(): Sending a comms request failed: ");
            errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
            ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        }
    }
    catch (...)
    {
        ELOG(EJavaPush, "ERROR!!! Unexpected exception caught in the PushControllerStarterImpl::setPendingStatus()");
    }
}

/**
 *
 */
void PushControllerStarterImpl::connCompleted(const wstring& aUri)
{
    JELOG2(EJavaPush);

    try
    {
        CommsMessage unRegCompletedMsg;
        unRegCompletedMsg.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
        unRegCompletedMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
        unRegCompletedMsg.setModuleId(PLUGIN_ID_PUSH_REGISTRY_C);
        unRegCompletedMsg.setMessageId(IPC_PR_CONN_COMPLETED);
        unRegCompletedMsg << aUri;
        CommsClientEndpoint& comms = getComms();
        int retValue = comms.send(unRegCompletedMsg);
        if (0 != retValue)
        {
            std::string errTxt("connCompleted(): Sending a comms request failed: ");
            errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
            ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        }
    }
    catch (...)
    {
        ELOG(EJavaPush,
             "ERROR!!! Unexpected exception caught in the PushControllerStarterImpl::unregCompleted()");
    }
}

/**
 *
 */
CommsClientEndpoint& PushControllerStarterImpl::getComms()
{
    JELOG2(EJavaPush);

    CommsClientEndpoint* commsObj = CommsClientEndpoint::find(L"javacaptain");
    if (0 == commsObj)
    {
        throw PushException(GENERAL_COMMS_ERROR,"Retrieving CommsClientEndpoint object failed",
                            __FILE__,__FUNCTION__,__LINE__);
    }
    return *commsObj;
}

/**
 *
 */
void PushControllerStarterImpl::validateUid(const Uid& aUid,CommsMessage& aMessage,
        const std::string& aFile,
        const std::string& aFunction,int aLine)
{
    JELOG2(EJavaPush);
    Uid uid;
    if (!(aMessage >> uid))
    {
        std::string errTxt("Cannot resolve uid from the comms msg");
        throw PushException(INCORRECT_COMMS_MSG,errTxt,aFile.c_str(),aFunction.c_str(),aLine);
    }
    if (uid != aUid)
    {
        std::wstring errTxt(L"Push comms msg with incorrect uid arrived: ");
        errTxt.append(uid.toString());
        ELOG1WSTR(EJavaPush,"ERROR!!!", errTxt);
        throw PushException(INCORRECT_COMMS_MSG,errTxt,aFile,aFunction,aLine);
    }
}

/**
 *
 */
void PushControllerStarterImpl::checkErrorParamsOfCommsMsg(CommsMessage& aMessage)
{
    JELOG2(EJavaPush);

    int status = 0;
    if (!(aMessage >> status))
    {
        std::string errTxt("Cannot resolve status from the comms msg");
        throw PushException(INCORRECT_COMMS_MSG,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }

    if (0 != status)
    {
        std::string errStatusText("");
        //Error check is not needed here.
        aMessage >> errStatusText;

        //Exception text already contains error code.
        throw PushException(0,errStatusText,__FILE__,__FUNCTION__,__LINE__);
    }

    //Error text is skipped in the success case.
    std::string errTxt;
    aMessage >> errTxt;
}

/**
 *
 */
void PushControllerStarterImpl::validateCommonParamsOfReplyCommsMsg
(int aRetValue,int aMsgId,CommsMessage& aMsg,const Uid& aUid)
{
    JELOG2(EJavaPush);

    if (0 != aRetValue)
    {
        std::string errTxt("Sending a comms request failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(aRetValue));
        ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        throw PushException(GENERAL_COMMS_ERROR,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }

    if (aMsgId != aMsg.getMessageId())
    {
        std::string errTxt("Incorrect message arrived: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(aMsg.getMessageId()));
        ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        throw PushException(GENERAL_COMMS_ERROR,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }

    validateUid(aUid,aMsg,__FILE__,__FUNCTION__,__LINE__);
    checkErrorParamsOfCommsMsg(aMsg);
}

/**
 * We have to call this operation in each methods of PushRegistry interface
 * because it might be that push connections are not retrieved before MIDlet
 * calls methods of PushRegistry. Other case is that all existing connections are
 * failed before a new push connection is registered to teh MIDlet. In this case push
 * connections must be retrieved from java captain.
 */
void PushControllerStarterImpl::loadPushConns(const java::util::Uid& aUid)
{
    JELOG2(EJavaPush);

    EState state = getState();
    if (INITIAL_STATE == state)
    {
        updatePushRegs2(aUid,mAppStateController,true);
        mMonitor->wait();
    }
    else if (LOADING_PUSH_CONNS == state)
    {
        mMonitor->wait();
    }
}

/**
 *
 */
std::wstring PushControllerStarterImpl::sendPushDataQuery(const std::wstring& aUri,
        const java::util::Uid& aSuiteUid,
        const EPushInfoQueryParams& aQueryCondition)
{
    JELOG2(EJavaPush);

    CommsMessage pushQuery;
    pushQuery.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
    pushQuery.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    pushQuery.setModuleId(PLUGIN_ID_PUSH_REGISTRY_C);
    pushQuery.setMessageId(IPC_PR_DATA_OF_PUSH_CONN_QUERY);
    pushQuery << aSuiteUid;
    pushQuery << aUri;
    pushQuery << (int) aQueryCondition;
    CommsClientEndpoint& comms = getComms();
    CommsMessage statusOfPushQueryMsg;
    int retValue = comms.sendReceive(pushQuery,statusOfPushQueryMsg,COMMS_TIMEOUT);
    //std::string tmpErrTxt("Retrieving data of the push connection failed");
    validateCommonParamsOfReplyCommsMsg(retValue,IPC_PR_DATA_OF_PUSH_CONN_QUERY_REPLY,
                                        statusOfPushQueryMsg,aSuiteUid);

    std::wstring returnedStr;
    if (!(statusOfPushQueryMsg >> returnedStr))
    {
        std::string errTxt("Cannot resolve 'filter' from the IPC_PR_DATA_OF_PUSH_CONN_QUERY_REPLY msg");
        throw PushException(INCORRECT_COMMS_MSG,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }
    return returnedStr;
}

/**
 *
 */
void PushControllerStarterImpl::sendRegDynPushConnMsg(const std::wstring& aUri,
        const std::wstring& aMidlet,
        const std::wstring& aFilter,
        const java::util::Uid& aUid,
        int aIsUidOfThisMidlet)
{
    JELOG2(EJavaPush);

    CommsMessage regDynPushRegMsg;
    regDynPushRegMsg.setReceiver(IPC_ADDRESS_JAVA_CAPTAIN_C);
    regDynPushRegMsg.setSender(IPC_ADDRESS_JAVA_CAPTAIN_C);
    regDynPushRegMsg.setModuleId(PLUGIN_ID_PUSH_REGISTRY_C);
    regDynPushRegMsg.setMessageId(IPC_PR_REGISTER_DYNAMIC_PUSH_CONN);
    regDynPushRegMsg << aUid;
    regDynPushRegMsg << aUri;
    regDynPushRegMsg << aMidlet;
    regDynPushRegMsg << aFilter;
    regDynPushRegMsg << aIsUidOfThisMidlet;
    CommsClientEndpoint& comms = getComms();
    CommsMessage statusOfDynRegMsg;
    int retValue = comms.sendReceive(regDynPushRegMsg,statusOfDynRegMsg,COMMS_TIMEOUT);
    //std::string tmpErrTxt("Registering a push connection failed");
    validateCommonParamsOfReplyCommsMsg(retValue,IPC_PR_REGISTER_DYNAMIC_PUSH_CONN_REPLY,
                                        statusOfDynRegMsg,aUid);
}

/**
 *
 */
PushServerConnPluginManager& PushControllerStarterImpl::getPushServerConnPluginManager()
{
    if (0 == mSrvConnPluginMgr.get())
        mSrvConnPluginMgr.reset(new PushServerConnPluginManager(this,this,this));
    return *mSrvConnPluginMgr.get();
}

/**
 *
 */
void PushControllerStarterImpl::setState(const EState& aState)
{
    JELOG2(EJavaPush);
    ScopedLock lockObj(mMutex);
    mState = aState;
}

/**
 *
 */
PushControllerStarterImpl::EState PushControllerStarterImpl::getState() const
{
    JELOG2(EJavaPush);
    ScopedLock lockObj(mMutex);
    return mState;
}

/**
 *
 */
void PushControllerStarterImpl::registerCommsListener()
{
    CommsClientEndpoint& comms = getComms();
    int retValue = comms.registerListener(PLUGIN_ID_PUSH_CONTROLLER_C,this);
    if ((0 != retValue) && (retValue != EEXIST))
    {
        std::string errTxt("Registering to comms failed: ");
        errTxt.append(java::util::JavaCommonUtils::intToString(retValue));
        ELOG1(EJavaPush, "ERROR!!! %s",errTxt.c_str());
        throw PushException(GENERAL_COMMS_ERROR,errTxt,__FILE__,__FUNCTION__,__LINE__);
    }
}

/**
 *
 */
void PushControllerStarterImpl::startToWaitCompletionOfOper()
{
    JELOG2(EJavaPush);

    if (0 == mConnCompletedMonitor.get())
        mConnCompletedMonitor.reset(Monitor::createMonitor());
    mConnCompletedMonitor->wait();
}

