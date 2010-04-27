/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "eventserverglobals.h"
#include "JniEnvWrapper.h"
#include <jni.h>
#include <e32def.h>

EXPORT_C JNIEnv* JniEnvWrapper::GetValidJniRef()
{
    JNIEnv* aJni;
    getEsStaticData()->mVm->AttachCurrentThreadAsDaemon((void**)&aJni, 0);
    return aJni;
}

void JniEnvWrapper::InitJavaVmRef(JNIEnv* aJni)
{
    JavaVM* vm = getEsStaticData()->mVm;
    if (vm == NULL)
    {
        aJni->GetJavaVM(&getEsStaticData()->mVm);
    }
}
