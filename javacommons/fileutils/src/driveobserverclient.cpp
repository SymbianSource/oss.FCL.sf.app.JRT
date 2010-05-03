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
* Description:  DriveObserverImpl - platform S60 specific
*
*/


#include "logger.h"
#include "comms.h"
#include "commsmessage.h"
#include "commsclientendpoint.h"

#include "driveobserverclient.h"
#include "driveobservermessages.h"

using java::comms::CommsClientEndpoint;

namespace java
{
namespace fileutils
{

DriveObserverClient::DriveObserverClient()
        :mEventsSubscribed(false), mComms(0), mCommsOwner(false)
{
    JELOG2(EJavaFile);
}

DriveObserverClient::~DriveObserverClient()
{
    JELOG2(EJavaFile);

    // Clear listeners collection
    mListeners.clear();
    disableEvents();
}

int DriveObserverClient::registerListener(DriveListenerInterface* aListener,
        const int& aServerCommsAddress)
{
    JELOG2(EJavaFile);
    std::set<DriveListenerInterface*>::iterator iter = mListeners.find(aListener);
    if (iter == mListeners.end())
    {
        mListeners.insert(aListener);
    }

    enableEvents(aServerCommsAddress);

    return mListeners.size();
}

int DriveObserverClient::unregisterListener(DriveListenerInterface* aListener,
        const int& /*aServerCommsAddress*/)
{
    JELOG2(EJavaFile);
    std::set<DriveListenerInterface*>::iterator iter = mListeners.find(aListener);
    if (iter != mListeners.end())
    {
        mListeners.erase(iter);
    }
    return mListeners.size();
}

void DriveObserverClient::enableEvents(const int& aServerCommsAddress)
{
    if (!mEventsSubscribed)
    {
        int connecStatus = 0;

        // Create CommsEndpoint either use find or create a new one
        if (aServerCommsAddress == java::comms::IPC_ADDRESS_JAVA_CAPTAIN_C)
        {
            mComms = CommsClientEndpoint::find(L"javacaptain");

            if (mComms == 0)
            {
                mComms = new CommsClientEndpoint;
                mCommsOwner = true;

                connecStatus = mComms->connect(java::comms::IPC_ADDRESS_JAVA_CAPTAIN_C);
            }
        }
        else
        {
            mComms = new CommsClientEndpoint;
            mCommsOwner = true;
            connecStatus = mComms->connect(aServerCommsAddress);
        }

        if (0 == connecStatus)
        {
            // Register moduleId
            mComms->registerListener(java::comms::PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C, this);

            // Subscribe to events
            CommsMessage msg;
            setSubscribeParams(msg, java::comms::PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C);
            if (0 == mComms->send(msg))
            {
                mEventsSubscribed = true;
            }
        }
    }
}

void DriveObserverClient::disableEvents()
{
    if (mEventsSubscribed)
    {
        // Unsubscribe to events
        CommsMessage msg;
        setUnsubscribeMessage(msg, java::comms::PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C);
        mComms->send(msg);

        // Unregister moduleId
        mComms->unregisterListener(java::comms::PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C, this);

        // Close and delete CommsEndpoint if our creation
        if (mCommsOwner)
        {
            mComms->disconnect();
            delete mComms;
            mComms = 0;
            mCommsOwner = false;
        }
        mEventsSubscribed = false;
    }
}

void DriveObserverClient::processMessage(java::comms::CommsMessage& aMessage)
{
    JELOG2(EJavaFile);
    switch (aMessage.getMessageId())
    {
    case DRIVEOBSERVER_MSG_DRIVE_CHANGED_EVENT:
    {
        int operation;
        driveInfo di;
        getDriveChangedParams(aMessage, operation, di);
        std::set<DriveListenerInterface*>::iterator iter;
        for (iter = mListeners.begin(); iter != mListeners.end(); ++iter)
        {
            (*iter)->driveChanged(operation, di);
        }
    }
    break;

    default:
        ELOG1(EJavaFile, "Unknown message received %d", aMessage.getMessageId());
    }
}

} // end of namespace fileutils
} // end of namespace java
