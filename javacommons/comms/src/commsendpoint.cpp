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
* Description:  This is an abstract base class for Comms endpoints.
*
*/

#include <errno.h>
#include <utility>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#include "logger.h"

#include "commsendpoint.h"
#include "commslistener.h"
#include "commsmessage.h"

#include "listeners.h"

namespace java
{
namespace comms
{
using java::util::ScopedLock;

OS_EXPORT CommsEndpoint::CommsEndpoint() : mDefaultListener(0), mMessageRefCounter(0), mVm(0), mJNIEnv(0)
{
//    JELOG2(EJavaComms);
    pthread_mutex_init(&mSendReceiveMutex, 0);
    pthread_cond_init(&mSendReceiveCondition, 0);
}

OS_EXPORT CommsEndpoint::~CommsEndpoint()
{
//    JELOG2(EJavaComms);

    for (listeners_t::iterator it = mListeners.begin(); it != mListeners.end(); it++)
    {
        delete it->second;
    }
    mListeners.clear();

    delete mDefaultListener;

    pthread_mutex_destroy(&mSendReceiveMutex);
    pthread_cond_destroy(&mSendReceiveCondition);
}

OS_EXPORT void CommsEndpoint::handleIpcMessage(CommsMessage& aMessage)
{
//    JELOG2(EJavaComms);

    /* Message routing is done in following steps:
     *  1. check if received message is reply to sendReceive method call
     *  2. use moduleId to find correct listener
     * */
    if (! isSendReceiveMessage(aMessage))
    {
        handleMessage(aMessage);
    }
}


OS_EXPORT int CommsEndpoint::handleMessage(CommsMessage& aMessage)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mListenersMutex);


    if (mVm && !mJNIEnv)
    {
        // attach to VM
        mVm->AttachCurrentThread((void**)&mJNIEnv, (void *)NULL);
    }


    listeners_t::iterator it = mListeners.find(aMessage.getModuleId());

    if (it != mListeners.end())
    {
        (it->second)->processMessage(aMessage, mJNIEnv);
    }
    else
    {
        if (mDefaultListener)
        {
            mDefaultListener->processMessage(aMessage, mJNIEnv);
        }
        else
        {
            WLOG3(EJavaComms, "CommsEndpoint::handleMessage: No handlers found, msgId=%d, moduleId=%d, messageRef=%d",
                  aMessage.getMessageId(),
                  aMessage.getModuleId(),
                  aMessage.getMessageRef());
        }
    }
    return 0;
}

OS_EXPORT int CommsEndpoint::registerListener(int aModuleId, CommsListener* aListener)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mListenersMutex);
    if (!aListener) return EINVAL;

    int rc = 0;

    if (mListeners.find(aModuleId) != mListeners.end())
    {
        rc = EEXIST; // already exists
        ELOG2(EJavaComms, "%s failed, listener already exists %d", __PRETTY_FUNCTION__, aModuleId);
    }
    else
    {
        mListeners.insert(std::make_pair(aModuleId, new ListenerWrapper(aListener)));
        LOG1(EJavaComms, EInfo, "Registered listener for module id %d", aModuleId);
    }

    return rc;
}

OS_EXPORT int CommsEndpoint::unregisterListener(int aModuleId, CommsListener* /*aListener*/)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mListenersMutex);
    int rc = 0;

    listeners_t::iterator it = mListeners.find(aModuleId);
    if (it != mListeners.end() /*&& it->second == aListener*/)
    {
        delete it->second;
        mListeners.erase(it);
        LOG1(EJavaComms, EInfo, "Unregistered listener for module id %d", aModuleId);
    }
    else
    {
        rc = ENOENT; // not found
        ELOG2(EJavaComms, "%s failed, listener does not exists %d", __PRETTY_FUNCTION__, aModuleId);
    }

    return rc;
}

OS_EXPORT int CommsEndpoint::registerDefaultListener(CommsListener* aListener)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mListenersMutex);
    if (!aListener) return EINVAL;

    int rc = 0;

    if (!mDefaultListener)
    {
        mDefaultListener = new ListenerWrapper(aListener);
        LOG(EJavaComms, EInfo, "Default listener registered");
    }
    else
    {
        rc = EEXIST; // already exists
        ELOG1(EJavaComms, "%s failed, default listener already exists", __PRETTY_FUNCTION__);
    }
    return rc;
}

OS_EXPORT int CommsEndpoint::unregisterDefaultListener(CommsListener* aListener)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mListenersMutex);

    int rc = 0;

    if (mDefaultListener && mDefaultListener->getListener() == aListener)
    {
        delete mDefaultListener;
        mDefaultListener = 0;
        LOG(EJavaComms, EInfo, "Default listener unregistered");
    }
    else
    {
        rc = ENOENT; // not found
        ELOG2(EJavaComms, "%s failed, errno = %d", __PRETTY_FUNCTION__, rc);
    }

    return rc;
}

int CommsEndpoint::isSendReceiveMessage(CommsMessage& aMessage)
{
//    JELOG2(EJavaComms);

    pthread_mutex_lock(&mSendReceiveMutex);

    int handled = 0;
    sendReceiveListeners_t::iterator it = mSendReceiveListeners.find(aMessage.getMessageRef());

    if (it != mSendReceiveListeners.end())
    {
        sendReceiveMessage_t sync = (it->second);

        *sync.p_message = aMessage;
        *sync.p_done = 1;
        *sync.p_result = 0;

        pthread_cond_broadcast(&mSendReceiveCondition); // wake sendReceive threads
        handled = 1;
    }

    pthread_mutex_unlock(&mSendReceiveMutex);

    return handled;
}

OS_EXPORT int CommsEndpoint::sendReceive(CommsMessage& aMessage, CommsMessage& aReceivedMessage, int aTimeoutInSecs)
{
#ifdef __SYMBIAN32__
    // Workaround for OpenC max timeout value limitation.
    // If OpenC limit is exceeded then EINVAL would be returned from pthread_cond_timedwait()
    const int MAX_TIME = 2148;
    if (aTimeoutInSecs > MAX_TIME)
    {
        WLOG3(EJavaComms, "%s timeout value too big, changing %d to %d", __PRETTY_FUNCTION__, aTimeoutInSecs, MAX_TIME);
        aTimeoutInSecs = MAX_TIME;
    }
#endif // __SYMBIAN32__

//    JELOG2(EJavaComms);
    if (aTimeoutInSecs < 1 && aTimeoutInSecs != WAIT_FOR_EVER)
    {
        ELOG2(EJavaComms, "%s failed, invalid timeout value = %d", __PRETTY_FUNCTION__, aTimeoutInSecs);
        return EINVAL;
    }

    pthread_mutex_lock(&mSendReceiveMutex);

    int messageRef = generateMessageReference();
    aMessage.setMessageRef(messageRef);
    int rc = send(aMessage);

    if (rc != 0)
    {
        pthread_mutex_unlock(&mSendReceiveMutex);
        ELOG2(EJavaComms, "%s failed, errno = %d", __PRETTY_FUNCTION__, rc);
        return rc;
    }

    int done = 0;
    int result = 0;
    sendReceiveMessage_t msg;
    msg.p_message   = &aReceivedMessage;
    msg.p_done      = &done;
    msg.p_result    = &result;

    mSendReceiveListeners.insert(std::make_pair(messageRef, msg));

    // specify duration for timeout
    timeval  curtime;
    rc = gettimeofday(&curtime, 0);

    timespec timeout;
    timeout.tv_sec  = curtime.tv_sec;
    timeout.tv_nsec = curtime.tv_usec * 1000;
    timeout.tv_sec += aTimeoutInSecs;

    // loop used, since signal might stop the wait before the timeout
    while (!done)
    {
        if (aTimeoutInSecs == WAIT_FOR_EVER)
        {
            rc = pthread_cond_wait(&mSendReceiveCondition, &mSendReceiveMutex);
        }
        else
        {
            rc = pthread_cond_timedwait(&mSendReceiveCondition, &mSendReceiveMutex, &timeout);
        }
        switch (rc)
        {
        case 0: // cond. variable being signaled
            if (done)
            {
                mSendReceiveListeners.erase(messageRef);
                rc = result;
            }
            break;

        case ETIMEDOUT:
            mSendReceiveListeners.erase(messageRef);
            done = 1;
            break;

        default:
            ELOG2(EJavaComms, "pthread_cond_timedwait failed %d - %s", rc, strerror(rc));
            break;
        }
    }

    if (rc)
    {
        ELOG3(EJavaComms, "%s failed %d - %s", __PRETTY_FUNCTION__ , rc, strerror(rc));
    }

    pthread_mutex_unlock(&mSendReceiveMutex);
    return rc;
}

OS_EXPORT int CommsEndpoint::generateMessageReference()
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mMessageRefMutex);
    if (mMessageRefCounter == 0)
    {
        // valid msgRef can't be zero, CommsMessage used zero as default value
        mMessageRefCounter++;
    }
    int messageRef = mMessageRefCounter++;
    return messageRef;
}

OS_EXPORT void CommsEndpoint::processMessage(const ipcMessage_t* aMsg)
{
    CommsMessage msg(aMsg);
    handleIpcMessage(msg);
}

OS_EXPORT void CommsEndpoint::onStart()
{
}

OS_EXPORT void CommsEndpoint::onExit()
{
    pthread_mutex_lock(&mSendReceiveMutex);
    for (sendReceiveListeners_t::iterator iter = mSendReceiveListeners.begin(); iter != mSendReceiveListeners.end(); iter++)
    {
        sendReceiveMessage_t sync = (iter->second);
        CommsMessage empty;

        *sync.p_message = empty;
        *sync.p_done = 1;
        *sync.p_result = EINTR;
    }

    pthread_cond_broadcast(&mSendReceiveCondition); // wake sendReceive threads
    pthread_mutex_unlock(&mSendReceiveMutex);

    // wait here until all sendReceive threads are done
    while (true)
    {
        pthread_mutex_lock(&mSendReceiveMutex);
        int listenerCount = mSendReceiveListeners.size();
        pthread_mutex_unlock(&mSendReceiveMutex);

        if (listenerCount!=0)
        {
            WLOG1(EJavaComms, "waiting for sendReceive listeners - count=%d", listenerCount);
            usleep(50); // 0.05s
        }
        else
        {
            break;
        }
    }

    if (mVm)
    {
        mVm->DetachCurrentThread();
        mVm=0;
        mJNIEnv=0;
    }
}

OS_EXPORT int CommsEndpoint::registerJavaListener(int aModuleId, jobject aListener, JNIEnv* aEnv)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mListenersMutex);
    int rc = 0;

    if (mListeners.find(aModuleId) != mListeners.end())
    {
        rc = EEXIST; // already exists
        ELOG2(EJavaComms, "%s failed, listener already exists %d", __PRETTY_FUNCTION__, aModuleId);
    }
    else
    {
        ListenerWrapper* listener = new ListenerWrapper(aListener, aEnv);
        mListeners.insert(std::make_pair(aModuleId, listener));
        LOG1(EJavaComms, EInfo, "Registered java listener for module id %d", aModuleId);
    }

    return rc;
}

OS_EXPORT int CommsEndpoint::unregisterJavaListener(int aModuleId, jobject, JNIEnv* aEnv)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mListenersMutex);
    int rc = 0;

    listeners_t::iterator it = mListeners.find(aModuleId);
    if (it != mListeners.end())
    {
        (it->second)->release(aEnv);
        delete it->second;
        mListeners.erase(it);
        LOG1(EJavaComms, EInfo, "Unregistered java listener for module id %d", aModuleId);
    }
    else
    {
        rc = ENOENT; // not found
        ELOG2(EJavaComms, "%s failed, listener does not exists %d", __PRETTY_FUNCTION__, aModuleId);
    }

    return rc;
}

OS_EXPORT int CommsEndpoint::registerDefaultJavaListener(jobject aListener, JNIEnv* aEnv)
{
    ScopedLock lock(mListenersMutex);
    int rc = 0;

    if (!mDefaultListener)
    {
        mDefaultListener = new ListenerWrapper(aListener, aEnv);
        LOG(EJavaComms, EInfo, "Registered default java listener");
    }
    else
    {
        rc = EEXIST; // already exists
        ELOG1(EJavaComms, "%s failed, default listener already exists", __PRETTY_FUNCTION__);
    }
    return rc;
}

OS_EXPORT int CommsEndpoint::unregisterDefaultJavaListener(jobject, JNIEnv*)
{
    ScopedLock lock(mListenersMutex);
    int rc = 0;

    if (mDefaultListener && mDefaultListener->getListener() == 0)
    {
        delete mDefaultListener;
        mDefaultListener = 0;
        LOG(EJavaComms, EInfo, "Unregistered default java listener");
    }
    else
    {
        rc = ENOENT; // not found
        ELOG2(EJavaComms, "%s failed, errno = %d", __PRETTY_FUNCTION__, rc);
    }
    return rc;
}


OS_EXPORT int CommsEndpoint::attachToVm(JNIEnv* aEnv)
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mListenersMutex);

    int rc = aEnv->GetJavaVM(&mVm);
    if (rc != 0)
    {
        ELOG2(EJavaComms, "%s failed, rc = %d", __PRETTY_FUNCTION__, rc);
    }

    return rc;
}

OS_EXPORT int CommsEndpoint::detachFromVm()
{
//    JELOG2(EJavaComms);
    ScopedLock lock(mListenersMutex);

    // remove java listeners
    if (mDefaultListener && mDefaultListener->getListener() == 0)
    {
        delete mDefaultListener;
        mDefaultListener = 0;
        LOG(EJavaComms, EInfo, "Removed default java listener (detach)");
    }

    for (listeners_t::iterator it = mListeners.begin(); it != mListeners.end();)
    {
        if ((it->second)->getListener() == 0)
        {
            LOG1(EJavaComms, EInfo, "Removed java listener for module id %d (detach)", it->first);

            (it->second)->release(mJNIEnv);
            delete it->second;
            mListeners.erase(it);
            it = mListeners.begin();
        }
        else
        {
            it++;
        }
    }
    return 0;
}

} // namespace comms
} // namespace java

