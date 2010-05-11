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
#include <logger.h>

#include "cmmadeleterefevent.h"

CMMADeleteRefEvent::CMMADeleteRefEvent(jobject aDeleteRefObject):
        CMMAEvent(EDisposableEvent)
{
    iDeleteRefObject = aDeleteRefObject;
}

// from CJavaEvent
void CMMADeleteRefEvent::Dispatch(JNIEnv& aJni)
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMADeleteRefEvent::Dispatch");
    aJni.DeleteGlobalRef(iDeleteRefObject);
}

//  END OF FILE
