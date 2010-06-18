/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "com_nokia_mj_impl_javacontrolpanel_Redirector.h"
#include "logger.h"
#ifdef __SYMBIAN32__
#include "logredirector.h"
#endif

/*
 * Class:     com_nokia_mj_impl_javacontrolpanel_Redirector
 * Method:    _start
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_javacontrolpanel_Redirector__1start
  (JNIEnv *, jobject)
{
#ifdef __SYMBIAN32__
    LogRedirector* redirector = new LogRedirector();
    redirector->start();
    return (jint)redirector;
#else
    return 0;
#endif
}

/*
 * Class:     com_nokia_mj_impl_javacontrolpanel_Redirector
 * Method:    _stop
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_javacontrolpanel_Redirector__1stop
  (JNIEnv *, jobject, jint aHandle)
{
#ifdef __SYMBIAN32__
    LogRedirector* redirector = reinterpret_cast<LogRedirector*>(aHandle);
    redirector->stop();
    delete redirector;
#endif
}
