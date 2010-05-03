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
#include <jdebug.h>

#include "cmmasnapshotevent.h"

CMMASnapshotEvent::CMMASnapshotEvent(jobject aNotifyObject,
                                     jmethodID aHandleEventMethod,
                                     TInt aError,
                                     HBufC8* aImageBuffer,
                                     TDisposability aDisposable):
        CMMAEvent(aNotifyObject,
                  aHandleEventMethod,
                  aDisposable)
{
    iEventData = aError;
    iImageBuffer = aImageBuffer;
}

CMMASnapshotEvent::~CMMASnapshotEvent()
{
    // image buffer need to be deleted if
    // event server hasn't sent this event before
    // middlet is destroyed
    delete iImageBuffer;
}

void CMMASnapshotEvent::Dispatch(JNIEnv& aJni)
{
    DEBUG("MMA::CMMASnapshotEvent::Dispatch");

    // create java byte array
    jbyteArray byteArray;
    if (iImageBuffer)
    {
        byteArray = aJni.NewByteArray(iImageBuffer->Size());
        if (byteArray)
        {
            ArrayUtils::CopyToJava(aJni,
                                   *iImageBuffer,
                                   byteArray,
                                   0,
                                   iImageBuffer->Size());
        }
    }
    else
    {
        byteArray = aJni.NewByteArray(0);
    }

    if (!byteArray)
    {
        delete iImageBuffer;
        iImageBuffer = NULL; // otherwise double delete in destructor

        DEBUG("MMA::CMMASnapshotEvent::Dispatch - Failed to create ByteArray");
        return;
    }


    aJni.CallVoidMethod(iListenerObject,
                        iHandleEventMethod,
                        iEventData,
                        byteArray);

    delete iImageBuffer;
    iImageBuffer = NULL; // otherwise double delete in destructor
}

//  END OF FILE
