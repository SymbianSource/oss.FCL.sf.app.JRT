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
* Description:  This main entry point of Java processes.
*
*/



#include<QWidget>
#include <coecntrl.h>
#include <touchfeedback.h>
#include <com_nokia_mid_ui_internal_OS.h>

static CCoeControl* convertToSymbian(QWidget* window)
{
    CCoeControl* control = 0;
    if (window && window->winId())
    {
        control = reinterpret_cast<CCoeControl*>(window->winId());
    }
    return control;
}

JNIEXPORT void JNICALL Java_com_nokia_mid_ui_internal_OS_MTouchFeedback_1InstantFeedback
(JNIEnv *, jclass, jint aStyle)
{
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
    {
        feedback->InstantFeedback(static_cast<TTouchLogicalFeedback>(aStyle));
    }
}

JNIEXPORT jboolean JNICALL Java_com_nokia_mid_ui_internal_OS_MTouchFeedback_1TouchFeedbackSupported
(JNIEnv *, jclass)
{
    jboolean enabled = JNI_FALSE;
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
    {
        enabled = feedback->TouchFeedbackSupported() ? JNI_TRUE : JNI_FALSE;
    }
    return enabled;
}

JNIEXPORT void JNICALL Java_com_nokia_mid_ui_internal_OS_MTouchFeedback_1SetFeedbackArea
(JNIEnv *, jclass, jint aControl, jint aAreaIndex, jint aX, jint aY, jint aWidth, jint aHeight, jint aStyle)
{
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
    {
        CCoeControl* control = convertToSymbian(reinterpret_cast<QWidget*>(aControl));
        TRect feedbackRect(aX, aY, aX + aWidth, aY + aHeight);
        feedback->SetFeedbackArea(control,
                                  static_cast<TUint32>(aAreaIndex),
                                  feedbackRect,
                                  static_cast<TTouchLogicalFeedback>(aStyle),
                                  ETouchEventStylusDown);
    }
}

JNIEXPORT void JNICALL Java_com_nokia_mid_ui_internal_OS_MTouchFeedback_1RemoveFeedbackArea
(JNIEnv *, jclass, jint aControl, jint aAreaIndex)
{
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
    {
        CCoeControl* control = convertToSymbian(reinterpret_cast<QWidget*>(aControl));
        feedback->RemoveFeedbackArea(control, aAreaIndex);
    }
}

JNIEXPORT void JNICALL Java_com_nokia_mid_ui_internal_OS_MTouchFeedback_1RemoveFeedbackForControl
(JNIEnv *, jclass, jint aControl)
{
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
    {
        CCoeControl* control = convertToSymbian(reinterpret_cast<QWidget*>(aControl));
        feedback->RemoveFeedbackForControl(control);
    }
}

JNIEXPORT void JNICALL Java_com_nokia_mid_ui_internal_OS_MTouchFeedback_1MoveFeedbackAreaToFirstPriority
(JNIEnv *, jclass, jint aControl, jint aAreaIndex)
{
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
    {
        CCoeControl* control = convertToSymbian(reinterpret_cast<QWidget*>(aControl));
        feedback->MoveFeedbackAreaToFirstPriority(control, static_cast<TUint32>(aAreaIndex));
    }
}



