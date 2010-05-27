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
* Description:  This class is used to post events to the java.
*
*/


//  INCLUDE FILES
#include <logger.h>

#include "cmmaplayerevent.h"
#include "mmapiutils.h"
#include "s60commonutils.h"
using namespace java::util;


CMMAPlayerEvent::CMMAPlayerEvent(jobject aNotifyObject,
                                 jmethodID aHandleEventMethod,
                                 TDisposability aDisposable):
        CMMAEvent(aNotifyObject, aHandleEventMethod, aDisposable)
{
}

CMMAPlayerEvent::~CMMAPlayerEvent()
{
    //delete iStringEventType;
    delete iStringEventData;
}

void CMMAPlayerEvent::SetObjectEventL(TEventType aEventType,
                                      const jobject aEventData)
{
    iEventType = aEventType;
    iObjectEventData = aEventData;
    iEventParams = EObject;
}

void CMMAPlayerEvent::SetLongEvent(TEventType aEventType,
                                   const TInt64& aEventData)
{
    iEventType = aEventType;
    iLongEventData = aEventData;
    iEventParams = ELong;
}

void CMMAPlayerEvent::SetStringEventL(TEventType aEventType,
                                      const TDesC& aEventData)
{
    //  __ASSERT_DEBUG(iStringEventData == NULL, User::Invariant());
    iEventType = aEventType;
    iStringEventData = aEventData.AllocL();
    iEventParams = EString;
}

void CMMAPlayerEvent::Dispatch(JNIEnv& aJni)
{
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAPlayerEvent::Dispatch type=%d", iEventType);
    if (aJni.IsSameObject(iListenerObject, 0))
    {
        LOG1(EJavaMMAPI, EInfo, "MMA::CMMAPlayerEvent::Dispatch EVENT_NOT_VALID type=%d", iEventType);
        return;
    }
    switch (iEventParams)
    {
    case ENormal:
    {
        jstring javaString = S60CommonUtils::NativeToJavaString(aJni, *iStringEventType);
        aJni.CallVoidMethod(iListenerObject,
                            iHandleEventMethod,
                            javaString,
                            iObjectEventData);
        break;
    }
    case ELong:
    {
        aJni.CallVoidMethod(iListenerObject,
                            iHandleEventMethod,
                            iEventType,
                            iLongEventData);
        break;
    }
    case EString:
    {
        jstring javaString = S60CommonUtils::NativeToJavaString(aJni, *iStringEventData);
        aJni.CallVoidMethod(iListenerObject,
                            iHandleEventMethod,
                            iEventType,
                            javaString);
        break;
    }
    case EObject:
    {
        aJni.CallVoidMethod(iListenerObject,
                            iHandleEventMethod,
                            iEventType,
                            iObjectEventData);
        break;
    }
    default:
    {
        // cannot occur
        __ASSERT_DEBUG(EFalse, User::Invariant());
    }
    }

}

//  END OF FILE
