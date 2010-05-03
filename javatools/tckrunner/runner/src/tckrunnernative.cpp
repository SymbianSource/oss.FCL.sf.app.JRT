/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Launches JavaInstaller with given arguments
*
*/

#ifdef __SYMBIAN32__
#include <spawn.h>
#else
#include <stdio.h>
#endif //__SYMBIAN32__
#include <errno.h>
#include <string>
#include <string.h>

#include "logger.h"
#include "javajniutils.h"
#include "javacommonutils.h"
#include "javaprocessconstants.h"
#include "com_nokia_mj_impl_tckrunner_Installer.h"

using std::wstring;


const int MAX_PARAMS = 16;

void jobjectArrayToCharArray(JNIEnv* aEnv, jobjectArray aArgs, char** aArr)
{
    int len = aEnv->GetArrayLength(aArgs);

    for (int i = 0; i < len; i++)
    {
        jstring jstr = (jstring)aEnv->GetObjectArrayElement(aArgs, i);
        wstring s = java::util::JniUtils::jstringToWstring(aEnv, jstr);
        aEnv->DeleteLocalRef(jstr);

        aArr[i] = java::util::JavaCommonUtils::wstringToUtf8(s);
        LOG1WSTR(ETckRunner, EInfo, "Added arg=%s", s);
    }
}


JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_tckrunner_Installer__1launchJavaInstaller(
    JNIEnv* aEnv,
    jobject,
    jobjectArray aArgs)
{
    int rc = 0;

    const char* av[MAX_PARAMS + 5];
    int index = 0;
    av[index++] = java::runtime::JAVA_PROCESS;
    av[index++] = java::runtime::JAVA_INSTALLER_STARTER_DLL;
    av[index++] = "poll";
    av[index++] = "-address=tck";

    int args = aEnv->GetArrayLength(aArgs);
    char** installerArgs = new char*[args];
    jobjectArrayToCharArray(aEnv, aArgs, installerArgs);

    for (int i=0; i<args && i < MAX_PARAMS; i++)
    {
        av[index++] = installerArgs[i];
    }
    av[index] = NULL;

    int pid = 0;
#ifdef __SYMBIAN32__
    rc = posix_spawn(&pid, av[0], NULL, NULL, (char*const*)av, NULL);
#else
    if (!(pid = fork()))
    {
        rc = execvp(av[0], (char*const*)av);
        if (rc == -1)
        {
            rc = errno;
        }
    }
#endif // __SYMBIAN32__

    for (int i=0; i<args; i++)
    {
        delete[] installerArgs[i];
    }
    delete[] installerArgs;

    if (rc)
    {
        ELOG3(ETckRunner, "%s failed, %s - errno=%d", __PRETTY_FUNCTION__, strerror(rc), rc);
    }

    return rc;
}
