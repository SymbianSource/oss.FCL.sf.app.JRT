/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#include <vector>
#include "javajniutils.h"
#include "jstringutils.h"
#include "runtimeuiqt.h"
#include "com_nokia_mj_impl_rt_ui_qt_RuntimeUiQt.h"

#include "logger.h"

using namespace java::runtimeui;

JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_rt_ui_qt_RuntimeUiQt__1confirm
(JNIEnv * aEnv, jobject, jstring aAppName, jobject aConfirmData, jboolean /*aIdentified*/)
{
    // Identified parameter is not used. It was earlier used to add icon to query header
    // and that is currently not supported.

    CActiveScheduler* newScheduler = 0;

    if (CActiveScheduler::Current() == 0)
    {
        // Create AS as not yet exists.
        newScheduler = new CActiveScheduler;
        CActiveScheduler::Install(newScheduler);
    }

    // unmarshall the JNI parameters
    JStringUtils appName(*aEnv, aAppName);
    jclass confirmDataClass = aEnv->GetObjectClass(aConfirmData);
    jmethodID getQuestionMethod
    = aEnv->GetMethodID(confirmDataClass,"getQuestion", "()Ljava/lang/String;");
    jstring jQuestion = (jstring) aEnv->CallObjectMethod(
                            aConfirmData, getQuestionMethod);
    JStringUtils question(*aEnv, jQuestion);

    int answer = -1;
    bool result = false;

    TRAPD(err, answer = RuntimeUiQt::confirmL(appName, question));

    if (KErrNone != err)
    {
        ELOG1(EJavaRuntime, "Error while showing confirmation dialog: %d", err);
    }
    else
    {
        // marshall the answer back to Java
        jmethodID setAnswerMethod = aEnv->GetMethodID(confirmDataClass,"setAnswer", "(I)V");
        aEnv->CallVoidMethod(aConfirmData, setAnswerMethod, answer);
        result = true;
    }

    delete newScheduler;
    return result;
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_rt_ui_qt_RuntimeUiQt__1error
(JNIEnv * aEnv, jobject, jstring aAppName, jstring aShortMsg, jstring aDetailedMsg)
{
    CActiveScheduler* newScheduler = 0;

    if (CActiveScheduler::Current() == 0)
    {
        // Create AS as not yet exists.
        newScheduler = new CActiveScheduler;
        CActiveScheduler::Install(newScheduler);
    }

    // unmarshall the JNI parameters
    JStringUtils appName(*aEnv, aAppName);
    JStringUtils shortMsg(*aEnv, aShortMsg);
    JStringUtils detailedMsg(*aEnv, aDetailedMsg);

    // delegate the UI implementation to handle the error operation
    TRAPD(err, RuntimeUiQt::errorL(appName, shortMsg, detailedMsg));

    if (KErrNone != err)
    {
        ELOG1(EJavaRuntime, "Error while showing error dialog: %d", err);
    }
    delete newScheduler;
}
