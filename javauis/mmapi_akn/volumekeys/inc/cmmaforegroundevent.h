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


#ifndef CMMAFOREGROUNDEVENT_H
#define CMMAFOREGROUNDEVENT_H

//  INCLUDES
#include "cmmaevent.h"

//  CLASS DECLARATION
/**
*   This class is used to post events to the java.
*
*
*/

NONSHARABLE_CLASS(CMMAForegroundEvent) : public CMMAEvent
{
public: // Construction
    /**
     * Constructor.
     *
     * @param aNotifyObject java side foreground object
     * @param aHandleEventMethod  java side method id
     * @param aDisposable Indicates if this event will be destroyed
     *                    after dispatch.
     */
    CMMAForegroundEvent(jobject aNotifyObject,
    jmethodID aHandleEventMethod,
    TDisposability aDisposable = EDisposableEvent);


private: // from CMMAEvent

    void Dispatch(JNIEnv& aJni);

};

#endif // CMMAFOREGROUNDEVENT_H
