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


//#include <jutils.h>

#include "com_nokia_amms_control_audio3d_CommitControl.h"
#include <mmafunctionserver.h>
#include "cammscommitcontrolgroup.h"

/**
 * wrapper for CAMMSCommitControlGroup::CommitAllControlsL()
 */
static void CommitL(CAMMSCommitControlGroup* aControl)
{
    aControl->CommitAllControlsL();
}

/**
 * wrapper for CAMMSCommitControlGroup::SetDeferredL()
 */
static void SetDeferredL(CAMMSCommitControlGroup* aControl, TBool aDeferred)
{
    aControl->SetDeferredL(aDeferred);
}

/*
 * Class:     com_nokia_amms_control_audio3d_CommitControl
 * Method:    _commit
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audio3d_CommitControl__1commit(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aCommitControl)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSCommitControlGroup* commitControl =
        static_cast<CAMMSCommitControlGroup*>(
        	reinterpret_cast< CAMMSControlGroup* >(aCommitControl));

    TInt error;
    error = eventSource->ExecuteTrap(&CommitL, commitControl);

    return error;
}

/*
 * Class:     com_nokia_amms_control_audio3d_CommitControl
 * Method:    _setDeferred
 * Signature: (IIZ)V
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_audio3d_CommitControl__1setDeferred(
    JNIEnv*,
    jclass,
    jint aEventSource,
    jint aCommitControl,
    jboolean aDeferred)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CAMMSCommitControlGroup* commitControl =
        static_cast<CAMMSCommitControlGroup*>(
        	reinterpret_cast< CAMMSControlGroup* >(aCommitControl));

    TInt error;
    error = eventSource->ExecuteTrap(&SetDeferredL, commitControl,
                                     (TBool) aDeferred);

    return error;
}

//  End of File
