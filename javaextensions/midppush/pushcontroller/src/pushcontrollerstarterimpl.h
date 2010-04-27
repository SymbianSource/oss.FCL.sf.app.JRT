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


#ifndef PUSHCONTROLLERSTARTERIMPL_H
#define PUSHCONTROLLERSTARTERIMPL_H

#include <memory>
#include <list>
#include <string>
#include <exception>

#include "comms.h"
#include "commslistener.h"
#include "pushcontrollerstarter.h"
#include "pushconstant.h"
#include "serverconnectionfactory.h"
#include "connectionlistener.h"
#include "pendingconnectionlistener.h"
#include "dynamiclibloader.h"
#include "pushserverconnpluginmanager.h"
#include "pushregistry.h"
#include "javauid.h"
#include "monitor.h"

namespace java
{

namespace runtime
{
class ApplicationBasicInfo;
class RuntimeInfo;
}
namespace comms
{
class CommsMessage;
class CommsClientEndpoint;
}
namespace util
{
class Uid;
}
namespace push
{

class PushServerConnection;

/**
 * This class provides implementation for PushControllerStarter and PushRegistry
 * interfaces.
 */

class PushControllerStarterImpl : public PushControllerStarter, public ConnectionListener,
        public java::comms::CommsListener, public PushRegistry,
        public PushControllerErrorHandlerInterface,
        public PendingConnectionListener,
        public ConnManagedInterface
{
public:

    PushControllerStarterImpl();

    virtual ~PushControllerStarterImpl();

    //Method of PushControllerStarter.
    virtual void startListen(const java::util::Uid& aUid,
                             java::runtime::MidpStarterPushSupport* aAppStateController);
    virtual void updatePushRegs(const java::util::Uid& aUid,
                                java::runtime::MidpStarterPushSupport* aAppStateController);
    virtual void close();

    //Methhod of ConnectionListener.
    virtual void msgArrived(const std::wstring& aInfo);
    virtual void error(const std::wstring& aUri, int aErrCode, const std::string& aErrText);

    //Methods of CommsListener.
    virtual void processMessage(java::comms::CommsMessage& aMessage);

    //Implementation methods of PushRegistry java class.

    /**
     * See comments from PushRegistry::registerDynamicPushConn() operation.
     */
    virtual void registerDynamicPushConn(const std::wstring& aUri,
                                         const std::wstring& aMidlet,
                                         const std::wstring& aFilter,
                                         const java::util::Uid& aUid,
                                         bool aIsUidOfThisMidlet);

    /**
     * See comments from PushRegistry::listConnections() operation.
     */
    virtual void listConnections(bool aAvailable,const java::util::Uid& aUid,
                                 std::list<std::wstring>& aUriList);

    /**
     * See comments from PushRegistry::getFilter() operation.
     */
    virtual std::wstring getFilter(const std::wstring& aUri,
                                   const java::util::Uid& aSuiteUid);

    /**
     * See comments from PushRegistry::getClassNameOfMidlet() operation.
     */
    virtual std::wstring getClassNameOfMidlet(const std::wstring& aUri,
            const java::util::Uid& aSuiteUid);

    /**
     * See comments from PushRegistry::isPushConnection() operation.
     */
    virtual bool isPushConnection(const std::wstring& aUri,const java::util::Uid& aUid);

    /**
     * See comments from PushRegistry::isPushConnection() operation.
     */
    virtual void unregisterDynamicPushConn(const std::wstring& aUri,
                                           const java::util::Uid& aUid,
                                           bool aIsUidOfThisMidlet);

    /**
     * @param aMidlet validation of this argument must be done in the java side.
     */
    virtual long long registerAlarm(const long long& aAlarmTimeInMilliSecs,
                                    const java::util::Uid& aSuiteUid,
                                    const std::wstring& aClassNameOfMidlet,
                                    bool aIsUidOfThisMidlet);

    //Methods of PushControllerErrorHandlerInterface interface.
    virtual void sendAllConnsFailedMsg();
    virtual void closeRuntime();

    //Method of PendingConnectionListener interface.
    virtual void setPendingStatus(const std::wstring& aUri,bool aMsgIsWaitingHandling);

    //Method of ConnManagedInterface.
    virtual void connCompleted(const std::wstring& aUri);

private:

    enum EState
    {
        INITIAL_STATE = 0,
        LOADING_PUSH_CONNS,
        PUSH_CONNS_LOADED
    };

    //Datamembers.
    std::auto_ptr<PushServerConnPluginManager> mSrvConnPluginMgr;
    java::util::Uid mUid;
    java::runtime::MidpStarterPushSupport* mAppStateController;
    EState mState;
    pthread_t mThreadId;
    mutable java::util::ScopedMutex mMutex;
    std::auto_ptr<java::util::Monitor> mMonitor;
    std::auto_ptr<java::util::Monitor> mConnCompletedMonitor;
    bool mIsUpdate;
    int mNumOfPendingUpdates;

    //Internal operations.
    bool sendGetAllPushRegsMsg(bool aThrowingExceptionAllowed);
    void handleListOfAllPushRegistrationsMsg(java::comms::CommsMessage& aMessage);
    java::comms::CommsClientEndpoint& getComms();
    void validateUid(const java::util::Uid& aUid,java::comms::CommsMessage& aMessage,
                     const std::string& aFile,const std::string& aFunction,int aLine);
    void checkErrorParamsOfCommsMsg(java::comms::CommsMessage& aMessage);
    void validateCommonParamsOfReplyCommsMsg(int aRetValue,int aMsgId,
            java::comms::CommsMessage& aMsg,
            const java::util::Uid& aUid);
    void loadPushConns(const java::util::Uid& aUid);
    std::wstring sendPushDataQuery(const std::wstring& aUri,
                                   const java::util::Uid& aSuiteUid,
                                   const EPushInfoQueryParams& aQueryCondition);
    void sendRegDynPushConnMsg(const std::wstring& aUri,
                               const std::wstring& aMidlet,
                               const std::wstring& aFilter,
                               const java::util::Uid& aUid,
                               int aIsUidOfThisMidlet);
    PushServerConnPluginManager& getPushServerConnPluginManager();
    void setState(const EState& aState);
    EState getState() const;
    void registerCommsListener();
    void updatePushRegs2(const java::util::Uid& aUid,
                         java::runtime::MidpStarterPushSupport* aAppStateController,
                         bool aThrowingExceptionAllowed);
    void startToWaitCompletionOfOper();

    //Not implemented.
    PushControllerStarterImpl(const PushControllerStarterImpl &x);
    PushControllerStarterImpl &operator=(const PushControllerStarterImpl &x);

};
}//end namespace push
}//end namespace java

#endif // PUSHCONTROLLERSTARTERIMPL_H
