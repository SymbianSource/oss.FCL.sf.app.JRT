/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: RuntimeUI JNI interface.
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
(JNIEnv * aEnv, jobject, jstring aAppName, jobject aConfirmData, jboolean aIdentified)
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
    jclass confirmDataClass = aEnv->GetObjectClass(aConfirmData);
    jmethodID getQuestionMethod
    = aEnv->GetMethodID(confirmDataClass,"getQuestion", "()Ljava/lang/String;");
    jstring jQuestion = (jstring) aEnv->CallObjectMethod(
                            aConfirmData, getQuestionMethod);
    JStringUtils question(*aEnv, jQuestion);

    std::vector<HBufC*> answerOptions;
    jmethodID getAnswerOptionsMethod = aEnv->GetMethodID(
                                           confirmDataClass,"getAnswerOptions", "()[Ljava/lang/String;");
    jobjectArray jAnswerOptions = (jobjectArray)aEnv->CallObjectMethod(
                                      aConfirmData, getAnswerOptionsMethod);
    if (jAnswerOptions != NULL)
    {
        int len = aEnv->GetArrayLength(jAnswerOptions);
        answerOptions.reserve(len);

        for (int i=0; i<len; i++)
        {
            jstring jAnswerOption = (jstring)aEnv->GetObjectArrayElement(
                                        jAnswerOptions, i);
            JStringUtils answerOption(*aEnv, jAnswerOption);
            answerOptions.push_back(answerOption.Alloc()); // If alloc fails NULL is added.
        }
    }

    // Answer suggestion is not supported because of touch UIs.
    ConfirmData confirmData(question, answerOptions, -1);

    int answer = ConfirmData::NO_ANSWER;
    bool result = false;

    TRAPD(err, answer = RuntimeUiQt::confirmL(appName, question, confirmData, aIdentified));

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

    for (int i=0; i<answerOptions.size(); i++)
    {
        delete answerOptions.at(i);
    }

    // cleanup the vector
    answerOptions.clear();
    delete newScheduler;

    return result;
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_rt_ui_qt_RuntimeUiQt__1error
(JNIEnv * aEnv, jobject, jstring aAppName, jstring aShortMsg, jstring aDetailedMsg, jstring aDetailsButton, jstring aOkButton)
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
    JStringUtils detailsButton(*aEnv, aDetailsButton);
    JStringUtils okButton(*aEnv, aOkButton);

    // delegate the UI implementation to handle the error operation
    TRAPD(err, RuntimeUiQt::errorL(appName, shortMsg, detailedMsg, detailsButton, okButton));

    if (KErrNone != err)
    {
        ELOG1(EJavaRuntime, "Error while showing error dialog: %d", err);
    }
    delete newScheduler;
}
