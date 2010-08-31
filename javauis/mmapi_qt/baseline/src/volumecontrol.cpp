/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class has JNI wrappers for CMMAVolumeControl
*
*/

//#include <jutils.h>
//#include "mmapiutils.h"

#include "com_nokia_microedition_media_control_VolumeControl.h"
#include "mmafunctionserver.h"
#include "cmmavolumecontrol.h"


/*
 * Class:     com_nokia_microedition_media_control_VolumeControl
 * Method:    _setLevel
 * Signature: (III)I
 */

JNIEXPORT int JNICALL Java_com_nokia_microedition_media_control_VolumeControl__1setLevel
(JNIEnv *, jclass , jint aEventSource, jint aVolumeControl, jint aLevel)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CMMAVolumeControl* volumeControl =
        reinterpret_cast< CMMAVolumeControl* >(aVolumeControl);

    TInt error = eventSource->ExecuteTrap(&CMMAVolumeControl::StaticSetLevelL,
                                          volumeControl,
                                          aLevel);
    return error;
}

/*
 * Class:     com_nokia_microedition_media_control_VolumeControl
 * Method:    _getLevel
 * Signature: (II)I
 */

JNIEXPORT int JNICALL Java_com_nokia_microedition_media_control_VolumeControl__1getLevel
(JNIEnv *, jclass , jint aEventSource, jint aVolumeControl)
{
    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSource);

    CMMAVolumeControl* volumeControl =
        reinterpret_cast< CMMAVolumeControl* >(aVolumeControl);

    TInt level(0);
    TInt error = eventSource->ExecuteTrap(&CMMAVolumeControl::StaticGetLevelL,
                                          volumeControl, &level);
    if (error == KErrNone)
    {
        return level;
    }
    return error;
}

//  END OF FILE
