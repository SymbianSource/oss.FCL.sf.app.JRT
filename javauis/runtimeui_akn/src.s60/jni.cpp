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


#include "javajniutils.h"
#include "jstringutils.h"
#include "runtimeuiavkon.h"
#include "com_nokia_mj_impl_rt_ui_avkon_RuntimeUiAvkon.h"

using namespace java::runtimeui;

JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_rt_ui_avkon_RuntimeUiAvkon__1confirm
(JNIEnv * aEnv, jobject, jstring aAppName, jobject aConfirmData, jboolean aIdentified)
{
    // unmarshall the JNI parameters
    JStringUtils appName(*aEnv, aAppName);
    jclass confirmDataClass = aEnv->GetObjectClass(aConfirmData);
    jmethodID getQuestionMethod = aEnv->GetMethodID(
                                      confirmDataClass,"getQuestion", "()Ljava/lang/String;");
    jstring jQuestion = (jstring)aEnv->CallObjectMethod(
                            aConfirmData, getQuestionMethod);
    JStringUtils question(*aEnv, jQuestion);
    std::vector<HBufC*> answerOptions;
    jmethodID getAnswerOptionsMethod = aEnv->GetMethodID(
                                           confirmDataClass,"getAnswerOptions", "()[Ljava/lang/String;");
    jobjectArray jAnswerOptions = (jobjectArray)aEnv->CallObjectMethod(
                                      aConfirmData, getAnswerOptionsMethod);
    bool answerAvailable = -1;
    TRAP_IGNORE(
        if (jAnswerOptions != NULL)
{
    int len = aEnv->GetArrayLength(jAnswerOptions);
        answerOptions.reserve(len);
        for (int i=0; i<len; i++)
        {
            jstring jAnswerOption = (jstring)aEnv->GetObjectArrayElement(
                                        jAnswerOptions, i);
            JStringUtils answerOption(*aEnv, jAnswerOption);
            answerOptions.push_back(answerOption.AllocLC());
        }
    }
    jmethodID getAnswerSuggestionMethod = aEnv->GetMethodID(
                                              confirmDataClass,"getAnswerSuggestion", "()I");
    jint jAnswerSuggestion = (jint)aEnv->CallIntMethod(
                                 aConfirmData, getAnswerSuggestionMethod);
    ConfirmData confirmData(question, answerOptions, jAnswerSuggestion);
    // delegate the UI implementation to handle the confirm operation
    answerAvailable = RuntimeUiAvkon::confirm(appName, confirmData, aIdentified);
    // cleanup the vector
    for (int i=0; i<answerOptions.size(); i++)
{
    CleanupStack::PopAndDestroy();
    }
    answerOptions.clear();
    // marshall the answer back to Java
    jmethodID setAnswerMethod = aEnv->GetMethodID(
                                    confirmDataClass,"setAnswer", "(I)V");
    aEnv->CallVoidMethod(aConfirmData, setAnswerMethod, confirmData.iAnswer);
    );
    return answerAvailable;
}

JNIEXPORT void JNICALL Java_com_nokia_mj_impl_rt_ui_avkon_RuntimeUiAvkon__1error
(JNIEnv * aEnv, jobject, jstring aAppName, jstring aShortMsg, jstring aDetailedMsg)
{
    // unmarshall the JNI parameters
    JStringUtils appName(*aEnv, aAppName);
    JStringUtils shortMsg(*aEnv, aShortMsg);
    JStringUtils detailedMsg(*aEnv, aDetailedMsg);
    // delegate the UI implementation to handle the error operation
    RuntimeUiAvkon::error(appName, shortMsg, detailedMsg);
}
