/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef JNIENVWRAPPER_H
#define JNIENVWRAPPER_H

#include <jni.h>
#define JNIENV_TO_VM(aJni) JavaVM* vm;\
                           aJni->GetJavaVM(&vm); /* Get pointer to VM*/\
                           JNIEnv* castedJNIEnv = reinterpret_cast< JNIEnv* >(vm); /* Get pointer to VM*/
#define JNI_PARAM castedJNIEnv
#define JNIENV_ATTACH(aJni) (reinterpret_cast< JavaVM* >(aJni))->AttachCurrentThreadAsDaemon((void**)&aJni, 0)

class JniEnvWrapper
{
public:
    static void     InitJavaVmRef(JNIEnv* aJni);
    IMPORT_C static JNIEnv*  GetValidJniRef();
};
#endif // JNIENVWRAPPER_H
