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
#include <logger.h>

#include <mmafunctionserver.h>

// Generated JNI header.
#include "com_nokia_amms_control_VolumeControl.h"

#include "cammsvolumecontrolgroup.h"

/**
 * Calls CAMMSVolumeControlGroup::SetLevelL method.
 * @param aVolumeControl CAMMSVolumeControlGroup instance.
 * @param aLevel Level to set.
 */
LOCAL_C void SetVolumeL(CAMMSVolumeControlGroup* aVolumeControl, TInt aLevel)
{
    aVolumeControl->SetVolumeL(aLevel);
}

/**
 * JNI function.
 */
JNIEXPORT jint JNICALL Java_com_nokia_amms_control_VolumeControl__1setLevel
(JNIEnv* /*aJniEnv*/,
 jclass,
 jint aEventSourceHandle,
 jint aControlHandle,
 jint aVolume)
{
    LOG1( EJavaAMMS, EInfo, "AMMS::VolumeControl.cpp::setLevel level = %d", aVolume);
    MMAFunctionServer* eventSource =
       reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    CAMMSVolumeControlGroup* control =
        reinterpret_cast< CAMMSVolumeControlGroup* >(aControlHandle);

    // call CAMMSVolumeControlGroup::SetVolumeL through local SetVolumeL function.
    return eventSource->ExecuteTrap(&SetVolumeL,
                                    control,
                                    aVolume);
}

//  End of File



