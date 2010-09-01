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
* Description:  This class is used to send direct content repaint event.
*
*/


//  INCLUDE FILES
#include <jdebug.h>

#include "cmmadcrepaintevent.h"

CMMADCRepaintEvent::CMMADCRepaintEvent(jobject aGUIObject):
        CMMAEvent(EReusableEvent)
{
    iListenerObject = aGUIObject;
    iEventData = KErrNone;
}

TBool CMMADCRepaintEvent::IsActive()
{
    return iEventData != KErrNone;
}

void CMMADCRepaintEvent::SetActive()
{
    iEventData = KErrInUse;
}

// from CJavaEvent
void CMMADCRepaintEvent::Dispatch(JNIEnv& aJni)
{
    // create method id if not allready created
    if (!iHandleEventMethod)
    {
        iHandleEventMethod = aJni.GetMethodID(aJni.GetObjectClass(iListenerObject),
                                              "repaint",
                                              "()V");
    }

    // There is internal error if method cannot be found
    __ASSERT_DEBUG(iHandleEventMethod, User::Invariant());

    // IsSameObject method will return true if iListenerObject has disappeared
    if (iHandleEventMethod &&
            aJni.IsSameObject(iListenerObject, 0) == JNI_FALSE)
    {
        aJni.CallVoidMethod(iListenerObject,
                            iHandleEventMethod);
    }

    // IsActive will return EFalse after this
    iEventData = KErrNone;
}

//  END OF FILE
