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
* Description:  This class has JNI wrappers for CMMAStopTimeControl
*
*/

//#include <jutils.h>
//#include "mmapiutils.h"
#include "com_nokia_microedition_media_control_StopTimeControl.h"
#include "mmafunctionserver.h"
#include "cmmastoptimecontrol.h"


JNIEXPORT jlong JNICALL Java_com_nokia_microedition_media_control_StopTimeControl__1getStopTime
(JNIEnv *, jclass , jint aEventSource, jint aControl)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CMMAStopTimeControl* control =
        reinterpret_cast< CMMAStopTimeControl* >(aControl);

    TInt64 time(0);
    eventSource->ExecuteV(CMMAStopTimeControl::StaticGetStopTime,
                          control,
                          &time);

    return *reinterpret_cast< jlong* >(&time);
}


JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_StopTimeControl__1setStopTime
(JNIEnv *, jclass , jint aEventSource, jint aControl, jlong aTime)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CMMAStopTimeControl* control =
        reinterpret_cast< CMMAStopTimeControl* >(aControl);

    TInt64 time;
    time = *((TInt64*)&aTime);     // convert 'long long' to TInt64

    TInt err = eventSource->ExecuteTrap(CMMAStopTimeControl::StaticSetStopTimeL,
                                        control,
                                        &time);

    return err;
}

//  END OF FILE
