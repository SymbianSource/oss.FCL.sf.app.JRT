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
* Description:  JavaCaptain
*
*/


#include "com_nokia_mj_impl_javacontrolpanel_JavaCaptain.h"

#ifdef __SYMBIAN32__
#include <spawn.h>
#else
#include <stdio.h>
#endif

#include <errno.h>
#include "logger.h"

/*
 * Class:     com_nokia_mj_impl_javacontrolpanel_JavaCaptain
 * Method:    _start
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_javacontrolpanel_JavaCaptain__1start
  (JNIEnv *, jobject)
{
    int rc = 0;
    int pid = 0;
    char* av[2];
    int index = 0;
    av[index++] = "javacaptain";
    av[index] = NULL;

#ifdef __SYMBIAN32__
    rc = posix_spawn(&pid, "javacaptain", NULL, NULL, av, NULL);
#else
    if (!(pid = fork()))
    {
        rc = execvp("javacaptain", av);
        if (rc == -1)
        {
            rc = errno;
        }
    }
#endif // __SYMBIAN32__

    if (rc)
    {
        ELOG3(EUtils,"%s failed, %s - errno=%d", __PRETTY_FUNCTION__, strerror(rc), rc);
    }

}

