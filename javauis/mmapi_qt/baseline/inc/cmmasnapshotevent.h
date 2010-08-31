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


#ifndef CMMASNAPSHOTEVENT_H
#define CMMASNAPSHOTEVENT_H

//  INCLUDES
#include "cmmaevent.h"

//  CLASS DECLARATION
/**
*   This class is used to post events to the java.
*
*
*/
NONSHARABLE_CLASS(CMMASnapshotEvent): public CMMAEvent
{
public:
    CMMASnapshotEvent(jobject aNotifyObject,
    jmethodID aHandleEventMethod,
    TInt aError,
    HBufC8* aImage,
    TDisposability aDisposable = EDisposableEvent);

    ~CMMASnapshotEvent();

private: // from CJavaEvent
    void Dispatch(JNIEnv& aJni);

private: // data
    HBufC8* iImageBuffer;
};

#endif // CMMASNAPSHOTEVENT_H
