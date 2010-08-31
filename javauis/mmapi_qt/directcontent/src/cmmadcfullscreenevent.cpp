/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used to send direct content fullscreen event.
*
*/


//  INCLUDE FILES
#include <logger.h>

#include "cmmadcfullscreenevent.h"

CMMADCFullScreenEvent::CMMADCFullScreenEvent(jobject aGUIObject, TBool aFullScreen):
        CMMAEvent(EDisposableEvent)
{
    iListenerObject = aGUIObject;
    iEventData = aFullScreen;
}

// from CJavaEvent
void CMMADCFullScreenEvent::Dispatch(JNIEnv& aJni)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMADCFullScreenEvent::Dispatch");
    iHandleEventMethod = aJni.GetMethodID(aJni.GetObjectClass(iListenerObject),
                                          "setFullScreen",
                                          "(Z)V");
    if (iHandleEventMethod &&
            (aJni.IsSameObject(iListenerObject, 0) == JNI_FALSE))
    {
        aJni.CallVoidMethod(iListenerObject,
                            iHandleEventMethod,
                            iEventData);
    }
}

//  END OF FILE
