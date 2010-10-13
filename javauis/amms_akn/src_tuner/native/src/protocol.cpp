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
* Description:  Protocol JNI wrapper.
 *
*/


// EXTERNAL INCLUDES
#include <jutils.h>
#include <jdebug.h>

// INTERNAL INCLUDES
#include "com_nokia_microedition_media_protocol_capture_radio_Protocol.h"
#include <CMMAEventSource.h>
#include "CAMMSTunerControl.h"
#include "CAMMSTunerPlayer.h"
#include "AMMSTunerFactory.h"

// Creates native tuner player
JNIEXPORT jint JNICALL
Java_com_nokia_microedition_media_protocol_capture_radio_Protocol__1createNativeTunerPlayer(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jstring aLocatorParameters)
{
    DEBUG("AMMS Java_com_nokia_microedition_media_protocol_capture_radio_Protocol__1createNativeTunerPlayer +");
    CMMAEventSource* eventSource = JavaUnhand< CMMAEventSource >(
                                       aEventSourceHandle);

    RJString locatorParams(*aJniEnv, aLocatorParameters);

    CAMMSTunerPlayer* tunerPlayer = NULL;

    TInt error = eventSource->ExecuteTrap(
                     &AMMSTunerFactory::CreatePlayerL,
                     &tunerPlayer,
                     (const TDesC*)&locatorParams);

    DEBUG_INT("AMMS __1createNativeTunerPlayer, error = %d", error);

    if (error != KErrNone)
    {
        return error;
    }

    return JavaMakeHandle(tunerPlayer);
}

