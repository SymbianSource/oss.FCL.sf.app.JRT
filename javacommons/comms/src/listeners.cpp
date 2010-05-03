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
* Description:  Class for hiding differences between native and java listeners
*
*/

#include "logger.h"

#include "commsmessage.h"
#include "listeners.h"

namespace java
{
namespace comms
{

ListenerWrapper::ListenerWrapper(CommsListener* aListener) : mListener(aListener), mObject(0), mMethodID(0)
{
}

ListenerWrapper::ListenerWrapper(jobject aListener, JNIEnv* aEnv) : mListener(0), mObject(0), mMethodID(0)
{
    mObject = aEnv->NewGlobalRef(aListener);
    jclass cls = aEnv->GetObjectClass(aListener);
    if (cls != NULL)
    {
        mMethodID = aEnv->GetMethodID(cls, "processMessage", "(Lcom/nokia/mj/impl/comms/CommsMessage;)V");
    }

    if (mObject == NULL || mMethodID == NULL)
    {
        ELOG3(EJavaComms, "%s failed, mObject=%d - cls=%d", __PRETTY_FUNCTION__, mObject, mMethodID);
    }
}

ListenerWrapper::~ListenerWrapper()
{
}

void ListenerWrapper::processMessage(CommsMessage& aMessage, JNIEnv* aEnv)
{
    JELOG2(EJavaComms);
    if (mListener)
    {
        mListener->processMessage(aMessage);
        return;
    }

    char* buf = aMessage.toByteArray();
    int len = (reinterpret_cast<ipcMessage_t*>(buf))->ipcHeader.length;

    // create bytearray
    jbyteArray bytes = aEnv->NewByteArray(len);
    if (bytes!=NULL)
    {
        aEnv->SetByteArrayRegion(bytes, 0, len, (jbyte*)buf);
        // find CommsMessage java class
        jclass clazz = aEnv->FindClass("com/nokia/mj/impl/comms/CommsMessage");
        if (clazz!=NULL)
        {
            // find CommsMessage java class ctor
            jmethodID mid = aEnv->GetMethodID(clazz, "<init>", "([B)V");
            if (mid!=NULL)
            {
                // create CommsMessage java object
                jobject messageObj = aEnv->NewObject(clazz, mid, bytes);
                if (messageObj!=NULL)
                {
                    // make callback
                    callVoidJavaMethod(aEnv, mObject, mMethodID,
                                       "(Lcom/nokia/mj/impl/comms/CommsMessage;)V", messageObj);
                    // free local references manually as we run in dispatch loop that does not return to VM
                    aEnv->DeleteLocalRef(messageObj);
                }
                else
                {
                    ELOG(EJavaComms, "NewObject() failed");
                }
            }
            else
            {
                ELOG(EJavaComms, "GetMethodID() failed");
            }
            aEnv->DeleteLocalRef(clazz);
        }
        else
        {
            ELOG(EJavaComms, "FindClass() failed");
        }
        aEnv->DeleteLocalRef(bytes);
    }
    else
    {
        ELOG(EJavaComms, "NewByteArray() failed");
    }
}

void ListenerWrapper::callVoidJavaMethod(JNIEnv* aEnv, jobject aObject, jmethodID aMethodID, const char* aMethodSignature, ...)
{
    // make the callback
    va_list args;
    va_start(args, aMethodSignature);
    aEnv->CallVoidMethodV(aObject, aMethodID, args);
    va_end(args);

    // check for pending exception
    jboolean hasException = aEnv->ExceptionCheck();
    if (hasException == JNI_TRUE)
    {
        // Report any exceptions that were generated and
        // then clear them from the JNI environment
        aEnv->ExceptionDescribe();
        aEnv->ExceptionClear();
        WLOG(EJavaComms, "processMessage() threw exception (in CallVoidMethodV)");
    }
}

CommsListener* ListenerWrapper::getListener()
{
    return mListener;
}

void ListenerWrapper::release(JNIEnv* aEnv)
{
    // aEnv can be NULL if message loop thread has not attached to VM
    // (aka no messages have been received)
    if (aEnv)
    {
        aEnv->DeleteGlobalRef(mObject);
    }
}




} // namespace comms
} // namespace java
