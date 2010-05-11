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
* Description:  This class is used to send direct content repaint event.
*
*/


#ifndef CMMADCREPAINTEVENT_H
#define CMMADCREPAINTEVENT_H

//#include <mevents.h>
#include "cmmaevent.h"


//  CLASS DECLARATION
/**
* This class is used to send direct content repaint event.
*
*/
NONSHARABLE_CLASS(CMMADCRepaintEvent): public CMMAEvent
{
public:
    /**
     * Default constructor.
     * @param aGUIObject Java object to call.
     */
    CMMADCRepaintEvent(jobject aGUIObject);

    /**
     * @return true if this event is active and may not be sent
     */
    TBool IsActive();

    /**
     * Sets this event active and IsActive will return true.
     * IsActive methos will return false after this event is dispatched.
     */
    void SetActive();

private: // from CJavaEvent
    /**
     * @param aJni JNI environment
     */
    void Dispatch(JNIEnv& aJni);

};

#endif // CMMADCREPAINTEVENT_H
