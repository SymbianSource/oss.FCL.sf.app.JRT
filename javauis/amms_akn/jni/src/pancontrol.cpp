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
* Description:
*
*/


#include <jutils.h>

#include "com_nokia_amms_control_PanControl.h"
#include <cmmaeventsource.h>
#include "cammspancontrolgroup.h"

/**
 * wrapper for CAMMSPanControlGroup::Pan()
 */
static TInt GetPan(CAMMSPanControlGroup* aControl)
{
    return aControl->Pan();
}

/**
 * wrapper for CAMMSPanControlGroup::SetPanL()
 */
static void SetPanL(CAMMSPanControlGroup* aControl, TInt aPan, TInt *aReturnedPan)
{
    aControl->SetPanL(aPan, *aReturnedPan);
}

/*
 * Class:     com_nokia_amms_control_PanControl
 * Method:    _getPan
 *
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_PanControl__1getPan(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aControl)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CAMMSPanControlGroup* control = static_cast< CAMMSPanControlGroup* >(
                                        JavaUnhand< CAMMSControlGroup >(aControl));

    return eventSource->Execute(GetPan, control);
}

/*
 * Class:     com_nokia_amms_control_PanControl
 * Method:    _setPan
 *
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_PanControl__1setPan(
    JNIEnv* aJniEnv,
    jclass,
    jint aEventSource,
    jint aControl,
    jint aPan,
    jintArray aError)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CAMMSPanControlGroup* control = static_cast< CAMMSPanControlGroup* >(
                                        JavaUnhand< CAMMSControlGroup >(aControl));

    TInt error;
    TInt aReturnedPan;
    error = eventSource->ExecuteTrap(SetPanL, control, (TInt) aPan, &aReturnedPan);

    // the returned pan value is between -100 and 100 (and the error code as well)
    jint javaError[ 1 ] = { error };
    aJniEnv->SetIntArrayRegion(aError, 0, 1, javaError);
    return aReturnedPan;
}


//  End of File
