/*
* Copyright (c) 1999-2002 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CJavaEventServer.h"
#include "CMIDEvent.h"
#include <stdio.h>
#include <e32def.h>
#include "lcdui.h"

CMIDEvent::CMIDEvent
(
    jmethodID   aMethod,
    jobject     aSource,
    TEventType  aType,
    TInt        aParam0,
    TInt        aParam1,
    TInt        aParam2
)
        : iMethod(aMethod)
        , iSource(aSource)
        , iType(aType)
        , iParam0(aParam0)
        , iParam1(aParam1)
        , iParam2(aParam2)
{
}

void CMIDEvent::Dispatch(JNIEnv& aJni)
{
    CMIDToolkit& toolkit = Object();
    jobject sourceObject = aJni.NewLocalRef(iSource);
    if (sourceObject)
    {
        jobject peerObject = aJni.NewLocalRef(toolkit.Peer());
        if (peerObject)
        {
            aJni.CallVoidMethod(
                peerObject,
                iMethod,
                sourceObject,
                iType,
                iParam0,
                iParam1,
                iParam2
            );
            aJni.DeleteLocalRef(peerObject);
        }
        aJni.DeleteLocalRef(sourceObject);
    }
}
