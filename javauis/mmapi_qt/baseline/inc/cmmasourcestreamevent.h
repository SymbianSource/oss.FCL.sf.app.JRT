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

#ifndef CMMASOURCESTREAMEVENT_H
#define CMMASOURCESTREAMEVENT_H

//  INCLUDES
#include "cmmaevent.h"

//  CLASS DECLARATION
/**
*   This class is used to post events to the java.
*
*
*/

NONSHARABLE_CLASS(CMMASourceStreamEvent): public CMMAEvent
{
public:
    CMMASourceStreamEvent(jmethodID aHandleEventMethod,
    jobject aNotifyObject);

    // length of the requested source stream data
    void SetLength(TInt aLength);

private: // from CJavaEvent
    void Dispatch(JNIEnv& aJni);

private:
    jmethodID iHandleEventMethod;
    jobject iListenerObject;
    TInt iLength;
};

#endif // CMMASOURCESTREAMEVENT_H
