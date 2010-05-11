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
* Description:  This class is used to send direct content invalidate event
*
*/


//  INCLUDE FILES
#include <logger.h>

#include "cmmadcinvalidateevent.h"

CMMADCInvalidateEvent::CMMADCInvalidateEvent(jobject aGUIObject,
        TSize aSize):
        CMMAEvent(EDisposableEvent)
{
    iListenerObject = aGUIObject;
    iSize = aSize;
}

// from CJavaEvent
void CMMADCInvalidateEvent::Dispatch(JNIEnv& aJni)
{
    LOG2( EJavaMMAPI, EInfo, "MMA::CMMADCInvalidateEvent::Dispatch w=%d h=%d",
               iSize.iWidth,
               iSize.iHeight);

    iHandleEventMethod = aJni.GetMethodID(aJni.GetObjectClass(iListenerObject),
                                          "invalidate",
                                          "(II)V");

    // IsSameObject method will return true if iListenerObject has disappeared
    if (iHandleEventMethod &&
            aJni.IsSameObject(iListenerObject, 0) == JNI_FALSE)
    {
        aJni.CallVoidMethod(iListenerObject,
                            iHandleEventMethod,
                            iSize.iWidth,
                            iSize.iHeight);
    }
}

//  END OF FILE
