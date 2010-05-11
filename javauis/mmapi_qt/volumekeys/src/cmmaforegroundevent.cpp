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
* Description:  This class is used to post events to the java.
*
*/


//  INCLUDE FILES
#include <logger.h>

#include "cmmaforegroundevent.h"

CMMAForegroundEvent::CMMAForegroundEvent(jobject aNotifyObject,
        jmethodID aHandleEventMethod,
        TDisposability aDisposable):
        CMMAEvent(aNotifyObject, aHandleEventMethod, aDisposable)
{

}

void CMMAForegroundEvent::Dispatch(JNIEnv& aJni)
{
    aJni.CallVoidMethod(iListenerObject,
                        iHandleEventMethod);
}

//  END OF FILE
