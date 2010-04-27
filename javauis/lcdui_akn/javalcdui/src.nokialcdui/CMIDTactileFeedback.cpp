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
* Description:  JNI class for TactileFeedback.java.
*
*/

#ifdef RD_TACTILE_FEEDBACK

#include "com_nokia_mid_ui_TactileFeedback.h"
#include "CMIDToolkit.h"
#include <touchfeedback.h>
#include "CTactileFeedbackInterfaceLCDUI.h"
#include "CTactileFeedbackInterfaceESWT.h"

#include <aknlayoutscalable_avkon.cdl.h>

#include <eswt.h>
#include <eswtwidgetscore.h>
#include <mswtclient.h>
#include <swtcliententry.h>
#include <swtfunctor.h>


JNIEXPORT jboolean JNICALL Java_com_nokia_mid_ui_TactileFeedback__1isESWT
(JNIEnv *, jobject)
{
    MSwtClient* client = NULL;
    TRAP_IGNORE(client = SWT::CreateClientL());

    if (!client)
    {
        return false; // LCDUI midlet running
    }
    else
    {
        return true;  // eSWT midlet running
    }
}


JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TactileFeedback__1createMTouchFeedbackLCDUI
(JNIEnv *, jobject, jint aToolkitHandle)
{
    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MTactileFeedbackInterface *tfInterface = new CTactileFeedbackInterfaceLCDUI(toolkit);

    return JavaMakeHandle(tfInterface);
}


JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TactileFeedback__1createMTouchFeedbackESWT
(JNIEnv *, jobject, jint)
{

    MSwtClient* client = NULL;
    TRAP_IGNORE(client = SWT::CreateClientL());

    MTactileFeedbackInterface *tfInterface = new CTactileFeedbackInterfaceESWT(client);

    return JavaMakeHandle(tfInterface);
}


JNIEXPORT jboolean JNICALL Java_com_nokia_mid_ui_TactileFeedback__1isTouchFeedbackSupported
(JNIEnv *, jobject, jint aTFInterface)

{
    MTactileFeedbackInterface* tfInterface = JavaUnhand<MTactileFeedbackInterface>(aTFInterface);
    return tfInterface->IsTouchFeedbackSupported();
}


JNIEXPORT void JNICALL Java_com_nokia_mid_ui_TactileFeedback__1doDirectFeedback
(JNIEnv *, jobject, jint aTFInterface, jint aStyle)
{
    MTactileFeedbackInterface* tfInterface = JavaUnhand<MTactileFeedbackInterface>(aTFInterface);

    tfInterface->DirectFeedback((TTouchLogicalFeedback)aStyle);
}


JNIEXPORT void JNICALL Java_com_nokia_mid_ui_TactileFeedback__1doRegisterFeedback
(JNIEnv *, jobject, jint aTFInterface, jint aControlHandle, jint aControlType, jint aAreaIndex, jint aX, jint aY, jint aWidth, jint aHeight, jint aStyle)
{

    MTactileFeedbackInterface* tfInterface = JavaUnhand<MTactileFeedbackInterface>(aTFInterface);
    tfInterface->RegisterFeedbackArea((void*)aControlHandle, aControlType, aAreaIndex, aX, aY, aWidth, aHeight, (TTouchLogicalFeedback)aStyle);
}


JNIEXPORT void JNICALL Java_com_nokia_mid_ui_TactileFeedback__1doRemoveFeedback
(JNIEnv *, jobject, jint aTFInterface, jint aControlHandle,  jint aControlType, jint aAreaIndex)
{
    MTactileFeedbackInterface* tfInterface = JavaUnhand<MTactileFeedbackInterface>(aTFInterface);
    tfInterface->UnregisterFeedbackArea((void*)aControlHandle, aControlType, aAreaIndex);
}


JNIEXPORT void JNICALL Java_com_nokia_mid_ui_TactileFeedback__1doRemoveFeedbackForControl
(JNIEnv *, jobject, jint aTFInterface, jint aControlHandle, jint aControlType)
{
    MTactileFeedbackInterface* tfInterface = JavaUnhand<MTactileFeedbackInterface>(aTFInterface);
    tfInterface->UnregisterFeedbackForControl((void*)aControlHandle, aControlType);
}


JNIEXPORT void JNICALL Java_com_nokia_mid_ui_TactileFeedback__1doMoveFeedbackAreaToFirstPriority
(JNIEnv *, jobject, jint aTFInterface, jint aControlHandle, jint aControlType, jint aAreaIndex)
{
    MTactileFeedbackInterface* tfInterface = JavaUnhand<MTactileFeedbackInterface>(aTFInterface);
    tfInterface->MoveAreaToFirstPriority((void*)aControlHandle, aControlType, aAreaIndex);
}

#endif
