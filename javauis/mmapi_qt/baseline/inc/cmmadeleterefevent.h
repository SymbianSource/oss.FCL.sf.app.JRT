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


#ifndef CMMADELETEREFEVENT_H
#define CMMADELETEREFEVENT_H

//#include <mevents.h>
#include "cmmaevent.h"

class MMAFunctionServer;

//  CLASS DECLARATION
/**
*   This class is used to delete references created with JNIEnv.
*   Reference deletion needs JNI environvent, which can't be stored
*   as an member variable. JNI environment is delivered to Dispatch
*   method that deletes the reference.
*
*
*
*/

NONSHARABLE_CLASS(CMMADeleteRefEvent): public CMMAEvent
{
public:
    /**
     * Default constructor. This event is always disposable.
     * @param aDeleteRefObject Object which reference will be deleted.
     */
    CMMADeleteRefEvent(jobject aDeleteRefObject);

private: // from CJavaEvent
    /**
     * This method deletes iDeleteRefObject reference with JNIEnv::DeleteGlobalRef method.
     * @param aJni JNI environment which is used to delete reference.
     */
    void Dispatch(JNIEnv& aJni);

protected:
    // Object reference which will be deleted.
    jobject iDeleteRefObject;
};

#endif // CMMADELETEREFEVENT_H
