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
* Description:  This class is used to send direct content invalidate event
*
*/


#ifndef CMMADCINVALIDATEEVENT_H
#define CMMADCINVALIDATEEVENT_H

//#include <mevents.h>
#include "cmmaevent.h"


//  CLASS DECLARATION
/**
* This class is used to send direct content invalidate event.
*
*/
NONSHARABLE_CLASS(CMMADCInvalidateEvent): public CMMAEvent
{
public:
    /**
     * Constructor.
     * @param aGUIObject Java object to invalidate
     * @param aSize New size for direct content
     */
    CMMADCInvalidateEvent(jobject aGUIObject, TSize aSize);

private: // from CJavaEvent
    /**
     * @param aJni JNI environment
     */
    void Dispatch(JNIEnv& aJni);

private:
    // New size to set
    TSize iSize;
};

#endif // CMMADCINVALIDATEEVENT_H
