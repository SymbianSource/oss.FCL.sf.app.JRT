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
* Description:  Amc
*
*/

#ifndef AMC_H
#define AMC_H

#include <list>
#include <map>

#include "logger.h"
#include "scopedlocks.h"
#include "javauid.h"

#include "commslistener.h"
#include "commsmessage.h"


#include "applicationruntimeeventsinterface.h"
#include "coreinterface.h"
#include "rtcinterface.h"
#include "amcinterface.h"
#include "amcmessages.h"

using namespace java::comms;
using java::util::Uid;

namespace java
{

namespace captain
{

class ApplicationManagementEventsInterface;

using java::util::ScopedLock;

class Amc : public AmcInterface,
        public ApplicationRuntimeEventsInterface,
        public CommsListener
{
public:
    Amc();
    virtual ~Amc();

    bool start(CoreInterface* aCore,
               ApplicationManagementEventsInterface* aAmEventsDispatcher);
    bool stop();

    // ApplicationRuntimeEventsInterface
    virtual void arLaunched(const Uid& aUID, const int& aRuntimeCommsAddress);
    virtual void arTerminated(const Uid& aUID, const int& aExitCode);

    // CommsListener methods
    virtual void processMessage(CommsMessage& aMessage);

private:

    // Helper functions for request handling
    void handleStart(CommsMessage&);
    void handleStop(CommsMessage&);
    void handleUpdate(CommsMessage&);

    class StopRequest
    {
    public:
        StopRequest(CoreInterface* aCore, const int& aAddress, const int& aModuleId, const int& aMsgRef,
                    const int& aNumOfUIDS, Uid* aUIDS)
                :mCore(aCore), mAddress(aAddress), mModuleId(aModuleId), mMsgRef(aMsgRef)
        {
            JELOG2(EJavaCaptain);
            java::util::ScopedLock lock(mUidsCollectionsMutex);
            for (int i = 0 ; i<aNumOfUIDS ; i++)
            {
                mUidsToBeStopped.push_back(aUIDS[i]);
            }
        }

        bool stopUids()
        {
            JELOG2(EJavaCaptain);
            ScopedLock lock(mUidsCollectionsMutex);

            uids_t::iterator iter = mUidsToBeStopped.begin();
            while (iter != mUidsToBeStopped.end())
            {
                mCore->getRtc()->disable(*iter);
                if (mCore->getRtc()->terminate(*iter))
                {
                    // Was not running in the first place can be directly put to stopped list
                    mUidStatuses.insert(std::make_pair(*iter, 0));
                    iter = mUidsToBeStopped.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }

            // Was the request fulfilled.
            if (!mUidsToBeStopped.size())
            {
                notifyRequester();
                return true;
            }

            return false; // Nope some requests are still pending.
        }

        bool updateStatus(const Uid& aUID, const int& status)
        {
            JELOG2(EJavaCaptain);
            ScopedLock lock(mUidsCollectionsMutex);

            for (uids_t::iterator iter = mUidsToBeStopped.begin() ;
                    iter != mUidsToBeStopped.end() ; ++iter)
            {
                if (*iter == aUID)
                {
                    mUidStatuses.insert(std::make_pair(aUID, status));
                    mUidsToBeStopped.erase(iter);
                    break;
                }
            }

            // Was this the last one ie. the request has been fulfilled.
            if (!mUidsToBeStopped.size())
            {
                notifyRequester();
                return true;
            }

            return false; // Nope some requests are still pending.
        }

        void notifyRequester()
        {
            JELOG2(EJavaCaptain);
            CommsMessage message;
            message.setReceiver(mAddress);
            message.setModuleId(mModuleId);
            message.setMessageRef(mMsgRef);
            setAmcResponseParamaters(message, mUidStatuses.size());

            for (uidStatuses_t::iterator iter = mUidStatuses.begin() ;
                    iter != mUidStatuses.end() ; ++iter)
            {
                message << iter->first << iter->second;
            }

            mCore->getComms()->send(message);
        }

        void sendStopNotAllowed(const int& aNumOfUIDS, Uid* aUIDS)
        {
            JELOG2(EJavaCaptain);
            for (int i = 0 ; i<aNumOfUIDS ; i++)
            {
                updateStatus(aUIDS[i], -1);
            }
        }


    private:
        typedef std::list<Uid>  uids_t;
        uids_t                  mUidsToBeStopped;

        typedef std::map<Uid, int> uidStatuses_t;
        uidStatuses_t           mUidStatuses;
        java::util::ScopedMutex mUidsCollectionsMutex;

        CoreInterface*          mCore;

        const int               mAddress;
        const int               mModuleId;
        const int               mMsgRef;
    };

    CoreInterface*                          mCore;
    ApplicationManagementEventsInterface*   mAmEventsDispatcher;
    StopRequest*                            mStopRequest; // only one at a time
};

} // namespace captain
} // namespace java

#endif // AMC_H

