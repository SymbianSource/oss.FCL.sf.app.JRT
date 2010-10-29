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


#ifndef PUSHREGISTRYHANDLER_H
#define PUSHREGISTRYHANDLER_H

#include <memory>

#include "javaosheaders.h"

#include "comms.h"
#include "extensionplugininterface.h"
#include "applicationruntimeeventsinterface.h"
#include "applicationmanagementeventsinterface.h"
#include "eventconsumerinterface.h"
#include "pushregistrymessages.h"
#include "commslistener.h"
#include "exceptionbase.h"
#include "scopedlocks.h"
#include "pushalarmutilsinterface.h"
#include "driveutilities.h"
#include "pushconncompletedtimercontainer.h"

namespace java
{

namespace captain
{
class CoreInterface;
}
namespace comms
{
class CommsMessage;
}
namespace util
{
class Uid;
}

namespace push
{

class PushDataContainer;
class PushAlarmHandler;
class PushDBHandler;

using java::captain::uids_t;

/**
 * This class implements ExtensionPlugin interface of Java Manager.
 * I.e. this class is a starting point for all push related tasks in the
 * Java Captain process.
 */
OS_NONSHARABLE_CLASS(PushRegistryHandler) : public java::captain::ExtensionPluginInterface,
        public java::comms::CommsListener,
        public java::captain::ApplicationRuntimeEventsInterface,
        public java::captain::ApplicationManagementEventsInterface,
        public java::captain::EventConsumerInterface,
        public PushAlarmUtilsInterface,
        public PushConnCompletedTimerListener
{
public:

    OS_IMPORT PushRegistryHandler();

    OS_IMPORT virtual ~PushRegistryHandler();

    // PluginInterface
    virtual void startPlugin(java::captain::CoreInterface* aCore);
    virtual void stopPlugin();

    // ExtensionPluginInterface.
    virtual ApplicationRuntimeEventsInterface* getApplicationRuntimeListener();
    virtual ApplicationManagementEventsInterface* getApplicationManagementListener();
    virtual EventConsumerInterface* getEventConsumer();


    //Method of CommsListener.
    virtual void processMessage(java::comms::CommsMessage& aMessage);

    // ApplicationRuntimeEventsInterface
    virtual void arLaunched(const Uid& aUID, const int& aRuntimeCommsAddress);
    virtual void arTerminated(const Uid& aUID, const int& aExitCode);

    // ApplicationManagementEventsInterface
    virtual void amAdded(const uids_t& uids);
    virtual void amUpdated(const uids_t& uids);
    virtual void amDeleted(const uids_t& uids);

    // EventConsumerInterface
    virtual void event(const std::string& eventProvider,
                       java::comms::CommsMessage& aMsg);

    // PushConnCompletedTimerListener
    virtual void connCompletedTimerExpired(const java::comms::CommsMessage& aMsg);

private:

    //Methods of PushAlarmUtilsInterface.
    virtual bool launchMidlet(const java::util::Uid& aMidletUid,
                              const int& aLaunchType);
    virtual int getMediaIdByMidletUid(const java::util::Uid& aMidletUid);

    //Datamembers.
    //This class does not take ownership of this datamember.
    java::captain::CoreInterface* mCore;
    std::auto_ptr<PushDBHandler> mPushDbHandler;
    std::auto_ptr<PushDataContainer> mPushDataContainer;
    std::auto_ptr<PushAlarmHandler> mPushAlarmHandler;
    //java::util::ScopedMutex mMutex;
    java::fileutils::driveInfos mDriveInfo;
    std::auto_ptr<PushConnCompletedTimerContainer> mConnCompletedTimerContainer;

    //Internal operations.
    void handleGetAllPushRegsMsg(java::comms::CommsMessage& aMsg);
    void getUris(std::wstring& aNumOfUriBuf,std::wstring& aUriBuf);
    void handleRegDynamicPushConnMsg(java::comms::CommsMessage& aMsg);
    void handleUnregDynamicPushConnMsg(java::comms::CommsMessage& aMsg);
    void handleRegisterAlarmMsg(java::comms::CommsMessage& aMsg);
    void handleAllConnsFailedMsg(java::comms::CommsMessage& aMsg);
    void handlePendingStatusMsg(java::comms::CommsMessage& aMsg);
    void handleListConnectionsMsg(java::comms::CommsMessage& aMsg);
    void handleDataOfPushConnQueryMsg(java::comms::CommsMessage& aMsg);
    void handlePushUriStartsWithQueryMsg(java::comms::CommsMessage& aMsg);
    void handleConnCompletedMsg(java::comms::CommsMessage& aMsg);
    void sendCommsResponseInSuccessCase(int aMsgId,const java::util::Uid& aUid,
                                        int aSenderOfOrigMsg,int aMsgRef);
    void sendRegisterAlarmReply(int aSenderOfOrigMsg,const java::util::Uid& aUid,
                                const long long& aTimeInMilliSecs,int aMsgRef);
    void sendRegisterAlarmReplyInErrorCase(java::util::ExceptionBase& aEx,int aSenderOfOrigMsg,
                                           const java::util::Uid& aUid,int aMsgRef);
    void sendCommsResponseInErrorCase(java::util::ExceptionBase& aEx,int aMsgId,
                                      const java::util::Uid& aUid,
                                      int aSenderOfOrigMsg,int aMsgRef);
    void sendPushUriStartsWithQueryReplyInErrorCase(java::comms::CommsMessage& aMsg);
    void startPushMidlets();
    void deleteAppData(const java::util::Uid& aUid,const bool& aDeleteAlarm);
    void readAppData(const java::util::Uid& aUid,const bool& aUpdateAlarm);
    void manageDriveInsertion(const java::fileutils::driveInfo& aDriveInfo);
    void manageDriveRemoval(const java::fileutils::driveInfo& aDriveInfo);
    void startOrKillMidlet(const unsigned int aMediaId,bool aTerminateMidlet);
    void removeDriveFromContainer(const java::fileutils::driveInfo& aDriveInfo);
    bool isDriveAvailable(int aMediaId);
    void readUidArg(java::comms::CommsMessage& aMsg,const std::string& aMsgName,java::util::Uid& aUid);
    void readWStrArg(java::comms::CommsMessage& aMsg,const std::string& aMsgName,std::wstring& aUri);
    int readIntAsBooleanArg(java::comms::CommsMessage& aMsg,const std::string& aMsgName);
    int readIntArg(java::comms::CommsMessage& aMsg,const std::string& aMsgName);
    long long readLongLongArg(CommsMessage& aMsg,const std::string& aMsgName);

    //Internal utility classes.
    
    /**
     * This "function object" class is used as an "comparison object"
     * when all driveInfo objects, which has equal root path, is removed from
     * mDriveInfo vector container. This wrapper class is needed because 
     * we did not wanted to add operator() method to fileutils::driveInfo struct.
     */
    class DriveInfoComparisonUtil
    {
        public:
        DriveInfoComparisonUtil(const java::fileutils::driveInfo& aInfo)
        : mObj(aInfo) {}
        
        ~DriveInfoComparisonUtil(){}
        
        DriveInfoComparisonUtil(const DriveInfoComparisonUtil& x)
        :mObj(x.mObj){}
        
        bool operator()(const java::fileutils::driveInfo& x)
        {
            if (mObj.iRootPath == x.iRootPath)
                return true;
            return false;
        }
        
        private:
        const java::fileutils::driveInfo& mObj;
        
        //Not implemented.
        DriveInfoComparisonUtil &operator=(const DriveInfoComparisonUtil& x);
    };

    //Not implemented.
    PushRegistryHandler(const PushRegistryHandler &x);
    PushRegistryHandler &operator=(const PushRegistryHandler &x);
};

}//end namespace push
}//end namespace java

#endif // PUSHREGISTRYHANDLER_H
