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
* Description:  This header is event poster
*
*/


#ifndef MMMAEVENTPOSTER_H
#define MMMAEVENTPOSTER_H

//  INCLUDES
#include "cmmaevent.h"
#include <jni.h>

NONSHARABLE_CLASS(MMMAEventPoster)
{
public:
    /**
     * Posts event to the Java side.
     *
     * @param aEvent Event to be sent.
     * @param aPriority Priority used to send the event.
     */
    virtual void PostEvent(CMMAEvent* aEvent,
                           TInt aPriority = CMMAEvent::EEventPriority) = 0;
		virtual JNIEnv* getValidJniEnv() = 0;                           	
};


#endif // MMMAEVENTPOSTER_H
