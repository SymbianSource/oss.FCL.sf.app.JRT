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
* Description:  This class is used to send direct content fullscreen event.
*
*/


#ifndef CMMADCFULLSCREENEVENT_H
#define CMMADCFULLSCREENEVENT_H

#include <mevents.h>
#include "cmmaevent.h"


//  CLASS DECLARATION
/**
* This class is used to send direct content fullscreen event.
*
*
*/
NONSHARABLE_CLASS(CMMADCFullScreenEvent): public CMMAEvent
{
public:
    /**
     * Default constructor.
     * @param aGUIObject Java object to set fullscreen status
     * @param aFullScreen Indicates whether or not to render in full
     * screen mode
     */
    CMMADCFullScreenEvent(jobject aGUIObject, TBool aFullScreen);

private: // from CJavaEvent
    /**
     * @param aJni JNI environment
     */
    void Dispatch(JNIEnv& aJni);
};

#endif // CMMADCFULLSCREENEVENT_H
