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

#include <logger.h>

#include "cmmasourcestreamevent.h"


CMMASourceStreamEvent::CMMASourceStreamEvent(
    jmethodID aHandleEventMethod,
    jobject aNotifyObject):
        CMMAEvent(EReusableEvent),
        iHandleEventMethod(aHandleEventMethod),
        iListenerObject(aNotifyObject)
{
}


void CMMASourceStreamEvent::SetLength(TInt aLength)
{
    iLength = aLength;
}



void CMMASourceStreamEvent::Dispatch(JNIEnv& aJni)
{
    LOG1(EJavaMMAPI, EInfo, "CMMASourceStreamEvent:Dispatch len=%d", iLength);
    aJni.CallVoidMethod(iListenerObject,
                        iHandleEventMethod,
                        iLength);
}

//  END OF FILE
