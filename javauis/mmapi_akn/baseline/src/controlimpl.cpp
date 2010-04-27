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
* Description:  This class has JNI wrappers for CMMAControl
*
*/


#include <jutils.h>

#include "com_nokia_microedition_media_control_ControlImpl.h"
#include "cmmaeventsource.h"
#include "cmmacontrol.h"

/*
 * Class:     com_nokia_microedition_media_control_ControlImpl
 * Method:    _setHandle
 */

JNIEXPORT void JNICALL Java_com_nokia_microedition_media_control_ControlImpl__1setHandle
(JNIEnv* aJni, jclass , jint aEventSource, jint aControlHandle, jobject aControlObject)
{
    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSource);

    CMMAControl* control =
        JavaUnhand< CMMAControl >(aControlHandle);

    jobject controlObject = aJni->NewWeakGlobalRef(aControlObject);

    eventSource->ExecuteV(&CMMAControl::StaticSetHandle,
                          control, controlObject);
}

//  END OF FILE
