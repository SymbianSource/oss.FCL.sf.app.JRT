/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


//#include <jutils.h>

#include "com_nokia_amms_control_PriorityControl.h"
#include <mmafunctionserver.h>
#include "cammsprioritycontrol.h"

/**
 * wrapper for CAMMSPriorityControl::Priority()
 */
static TInt GetPriority(CAMMSPriorityControl* aControl)
{
    return aControl->Priority();
}

/**
 * wrapper for CAMMSPriorityControl::SetPriorityL()
 */
static void SetPriorityL(CAMMSPriorityControl* aControl, TInt aPriority)
{
    aControl->SetPriorityL(aPriority);
}

/*
 * Class:     com_nokia_amms_control_PriorityControl
 * Method:    _getPriority
 *
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_PriorityControl__1getPriority(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSPriorityControl* control = static_cast< CAMMSPriorityControl* >(
                                        reinterpret_cast< CAMMSControl *>(aControl));

    return eventSource->Execute(GetPriority, control);
}

/*
 * Class:     com_nokia_amms_control_PriorityControl
 * Method:    _setPriority
 *
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_PriorityControl__1setPriority(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl,
    jint aPriority)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSource);

    CAMMSPriorityControl* control = static_cast< CAMMSPriorityControl* >(
                                        reinterpret_cast< CAMMSControl* >(aControl));

    TInt error = eventSource->ExecuteTrap(
                     &SetPriorityL, control, (TInt) aPriority);

    return error;
}

//  End of File
