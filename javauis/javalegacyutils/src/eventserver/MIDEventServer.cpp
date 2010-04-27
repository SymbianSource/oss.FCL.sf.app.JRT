/*
* Copyright (c) 1999-2001 Nokia Corporation and/or its subsidiary(-ies).
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


#include <mevents.h>
#include "eventserverglobals.h"
#include "com_nokia_mj_impl_rt_legacy_MIDEventServer.h"
#include "JniEnvWrapper.h"

/*
 * Class:     com_symbian_epoc_midp_events_MIDEventServer
 * Method:    _createServer
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rt_legacy_MIDEventServer__1createServer(JNIEnv* aJni, jobject, jstring aName)
{
    RJString name(*aJni,aName);

    JniEnvWrapper::InitJavaVmRef(aJni);

    TRAPD(h,h=TJavaEventServer::NewL(name).Handle());
    return h;
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_rt_legacy_MIDEventServer__1shutdown(JNIEnv*, jobject, jint aHandle)
{
    TJavaEventServer(aHandle).Shutdown();
}

// In polling builds we maintain a global record of all the event
// servers running in the system, held in a static RArray of integer
// handles. Event servers are added to the handle array by their
// Java side thread's run() method, which goes native below.
//
// Note that it is necessary for the RArray to be initialized
// by a function-scoped static in order for its constructor to
// get run reliably.

RArray<TInt>& EventServerHandles()
{
    return getEsStaticData()->mHandles;
}

