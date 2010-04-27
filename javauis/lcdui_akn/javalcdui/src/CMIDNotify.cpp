/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CMIDNotify.h"
#include "CMIDToolkit.h"

CMIDNotifyEvent::CMIDNotifyEvent(TJavaPeer aPeer)
        : CJavaEvent<CMIDToolkit>(EReusableEvent)
        , iPeer(aPeer)
{
}

void CMIDNotifyEvent::SetResult(TInt aResult)
{
    iResult=aResult;
}

void CMIDNotifyEvent::Dispose()
{
    delete this;
}

void CMIDNotifyEvent::Dispatch(JNIEnv& aJni)
{
#ifdef _TRACE_ASYNC_
    RDebug::Print(_L("CMIDNotifyEvent::Dispatch: result=%d"), iResult);
#endif

    CMIDToolkit& toolkit = Object();
    jobject sourceObject = aJni.NewLocalRef((jobject)iPeer);
    if (sourceObject)
    {
        jobject peerObject = aJni.NewLocalRef(toolkit.Peer());
        if (peerObject)
        {
#ifdef _TRACE_ASYNC_
            RDebug::Print(_L("CMIDNotifyEvent::Dispatch: calling Toolkit.handleNotifyCallback(%x, %d)"), source, iResult);
#endif
            aJni.CallVoidMethod(
                peerObject,
                toolkit.iHandleNotifyMethod,
                sourceObject,
                iResult
            );
            aJni.DeleteLocalRef(peerObject);
        }
        aJni.DeleteLocalRef(sourceObject);
    }

#if defined(_DEBUG) && defined(_TRACE_ASYNC_)
    if (aJni.IsSameObject(source, NULL))
    {
        RDebug::Print(_L("CMIDNotifyEvent: source has been gc'd!"));
    }
    if (aJni.IsSameObject(toolkit.Peer(), NULL))
    {
        RDebug::Print(_L("CMIDNotifyEvent: Toolkit has been gc'd!!!!"));
    }
#endif
}

void CMIDNotifyEvent::Close(JNIEnv& aJni)
{
    //
    // ASSERT( !IsQueued() );
    //
    if (NULL != iPeer)
    {
        aJni.DeleteWeakGlobalRef((jweak)iPeer);
        iPeer = NULL;
    }
}
