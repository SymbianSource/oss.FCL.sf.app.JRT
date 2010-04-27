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

#ifndef LISTENERS_H
#define LISTENERS_H
#include <jni.h>

#include "scopedlocks.h"

#include "commslistener.h"

namespace java
{
namespace comms
{

OS_NONSHARABLE_CLASS(ListenerWrapper)
{
    ListenerWrapper();
public:
    ListenerWrapper(CommsListener* aListener);
    ListenerWrapper(jobject aListener, JNIEnv* aEnv);
    virtual ~ListenerWrapper();

    virtual void processMessage(CommsMessage& aMessage, JNIEnv* aEnv);
    virtual CommsListener* getListener();

    virtual void release(JNIEnv* aEnv);

private:
    void callVoidJavaMethod(JNIEnv* aJniEnv, jobject aObject, jmethodID aMethodID, const char* aMethodSignature, ...);
    void dispatchMessageToJava(CommsMessage& aMessage, JNIEnv* aJniEnv);

    CommsListener* mListener;

    jobject     mObject;
    jmethodID   mMethodID;
};

} // namespace comms
} // namespace java

#endif // LISTENERS_H
