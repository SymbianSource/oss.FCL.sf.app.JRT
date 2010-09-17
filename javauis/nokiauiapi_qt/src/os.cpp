/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#include <QWidget>
#include <coecntrl.h>
#include <touchfeedback.h>
#include <com_nokia_mid_ui_internal_OS.h>
#include "autorelease.h"
#include "csoftnotification.h"

static CCoeControl* convertToSymbian(QWidget* window)
    {
    CCoeControl* control = 0;
    if (window && window->winId())
        {
        control = reinterpret_cast<CCoeControl*> (window->winId());
        }
    return control;
    }

JNIEXPORT void JNICALL Java_com_nokia_mid_ui_internal_OS_MTouchFeedback_1InstantFeedback(
        JNIEnv *, jclass, jint aStyle)
    {
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
        {
        feedback->InstantFeedback(static_cast<TTouchLogicalFeedback> (aStyle));
        }
    }

JNIEXPORT jboolean JNICALL Java_com_nokia_mid_ui_internal_OS_MTouchFeedback_1TouchFeedbackSupported(
        JNIEnv *, jclass)
    {
    jboolean enabled = JNI_FALSE;
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
        {
        enabled = feedback->TouchFeedbackSupported() ? JNI_TRUE : JNI_FALSE;
        }
    return enabled;
    }

JNIEXPORT void JNICALL Java_com_nokia_mid_ui_internal_OS_MTouchFeedback_1SetFeedbackArea(
        JNIEnv *, jclass, jint aControl, jint aAreaIndex, jint aX, jint aY,
        jint aWidth, jint aHeight, jint aStyle)
    {
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
        {
        CCoeControl* control = convertToSymbian(
                reinterpret_cast<QWidget*> (aControl));
        TRect feedbackRect(aX, aY, aX + aWidth, aY + aHeight);
        feedback->SetFeedbackArea(control, static_cast<TUint32> (aAreaIndex),
                feedbackRect, static_cast<TTouchLogicalFeedback> (aStyle),
                ETouchEventStylusDown);
        }
    }

JNIEXPORT void JNICALL Java_com_nokia_mid_ui_internal_OS_MTouchFeedback_1RemoveFeedbackArea(
        JNIEnv *, jclass, jint aControl, jint aAreaIndex)
    {
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
        {
        CCoeControl* control = convertToSymbian(
                reinterpret_cast<QWidget*> (aControl));
        feedback->RemoveFeedbackArea(control, aAreaIndex);
        }
    }

JNIEXPORT void JNICALL Java_com_nokia_mid_ui_internal_OS_MTouchFeedback_1RemoveFeedbackForControl(
        JNIEnv *, jclass, jint aControl)
    {
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
        {
        CCoeControl* control = convertToSymbian(
                reinterpret_cast<QWidget*> (aControl));
        feedback->RemoveFeedbackForControl(control);
        }
    }

JNIEXPORT void JNICALL Java_com_nokia_mid_ui_internal_OS_MTouchFeedback_1MoveFeedbackAreaToFirstPriority(
        JNIEnv *, jclass, jint aControl, jint aAreaIndex)
    {
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if (feedback)
        {
        CCoeControl* control = convertToSymbian(
                reinterpret_cast<QWidget*> (aControl));
        feedback->MoveFeedbackAreaToFirstPriority(control,
                static_cast<TUint32> (aAreaIndex));
        }
    }

JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_internal_OS_createNativePeer(
        JNIEnv* aJniEnv, jclass /*aClazz*/, jint aMidletId,
        jint aNotificationId, jobject aPeer)
    {
    jobject peer = aJniEnv->NewWeakGlobalRef(aPeer);
    if (!peer)
        return KErrNoMemory;

    // creates soft notification instance
    CSoftNotification* softNote = NULL;
    TRAPD(err, softNote = CSoftNotification::NewL(aMidletId,
            aNotificationId));
    if(err!=KErrNone)
        {
        aJniEnv->DeleteWeakGlobalRef((jweak)peer);
        return err;
        }
    
    // callback object class
    jclass clazz = aJniEnv->GetObjectClass(peer);
    jmethodID methodId = aJniEnv->GetMethodID(clazz, "notificationCallback",
               "(I)V");
   
    softNote->SetPeerObserver(peer, methodId, aJniEnv);
    aJniEnv->DeleteLocalRef(clazz);
    return reinterpret_cast<jint> (softNote);
    }

JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_internal_OS_getId(JNIEnv* /*aJniEnv*/,
        jclass /*aPeer*/, jint aSoftNotificationHandle)
    {
    CSoftNotification* softNotification =
            reinterpret_cast<CSoftNotification*> (aSoftNotificationHandle);
    return softNotification->Id();
    }

HBufC* JavaStringToSymbianString(JNIEnv* aJniEnv, jstring aText)
    {
    HBufC* bufferText = NULL;

    jboolean isCopy;
    const jchar* javaChars = aJniEnv->GetStringChars(aText, &isCopy);
    if (javaChars)
        {
        AutoReleaseStringChars cleaner(aJniEnv, aText, javaChars);
        jsize length = aJniEnv->GetStringLength(aText);
        TRAPD(err, bufferText = HBufC::NewL(length));
        if (err == KErrNone)
            {
            TText* ptr = const_cast<TText*> (bufferText->Des().Ptr());
            memcpy(ptr, javaChars, length * sizeof(jchar));
            bufferText->Des().SetLength(length);
            }
        else
            {
            return bufferText;
            }
        }
    else
        {
        return bufferText;
        }
        
    return bufferText;
    }

JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_internal_OS_setText(JNIEnv* aJniEnv, jclass /*aPeer*/,
        jint aSoftNotificationHandle, jstring aPrimaryText, jstring aSecondaryText)
    {
    CSoftNotification* softNotification =
            reinterpret_cast<CSoftNotification*> (aSoftNotificationHandle);
    // Convert from java string to Symbian descriptor
    HBufC* bufferPrimaryText = NULL;
    if (aPrimaryText != NULL)
        {
        bufferPrimaryText = JavaStringToSymbianString(aJniEnv, aPrimaryText);
        if (bufferPrimaryText == NULL)
            {
            return KErrNoMemory;
            }
        }

    HBufC* bufferSecondaryText = NULL;
    if (aSecondaryText != NULL)
        {
        bufferSecondaryText = JavaStringToSymbianString(aJniEnv,
                aSecondaryText);
        if (bufferSecondaryText == NULL)
            {
            return KErrNoMemory;
            }
        }
    TRAPD(err,softNotification->SetTextL(*bufferPrimaryText, *bufferSecondaryText));
    return err;
    }

JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_internal_OS_removeSoftNotification(JNIEnv* /*aJniEnv*/,
        jclass /*aPeer*/, jint aSoftNotificationHandle)
    {
    CSoftNotification* softNotification =
            reinterpret_cast<CSoftNotification*> (aSoftNotificationHandle);
    TRAPD(err,softNotification->RemoveSoftNotificationL());
    delete softNotification;
    return err;
    }

JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_internal_OS_showSoftNotification(JNIEnv* /*aJniEnv*/,
        jclass /*aPeer*/, jint aSoftNotificationHandle)
    {
    CSoftNotification* softNotification =
            reinterpret_cast<CSoftNotification*> (aSoftNotificationHandle);
    jboolean status(false);
    TRAPD(err,status = softNotification->ShowSoftNotificationL());
    if(status)
        {
        return err;
        }
    else
        {
        return KErrNone;
        }
    }

JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_internal_OS_setImagePath(JNIEnv* aJniEnv,
        jclass /*aPeer*/, jint aSoftNotificationHandle, jstring aImagePath )
    {
    CSoftNotification* softNotification =
    reinterpret_cast<CSoftNotification*> (aSoftNotificationHandle);
    // Convert from java string to Symbian descriptor
    HBufC* bufferImagePath = NULL;
    if (aImagePath != NULL)
        {
        bufferImagePath = JavaStringToSymbianString(aJniEnv, aImagePath);
        if (bufferImagePath == NULL)
            {
            return KErrNoMemory;
            }
        }
    TRAPD(err,softNotification->SetImagePathL(*bufferImagePath));
    return err;
    }

JNIEXPORT void JNICALL
Java_com_nokia_mid_ui_internal_OS_destroy(JNIEnv* /*aJniEnv*/,
        jclass /*clazz*/, jint aSoftNotificationHandle)
    {
    if (aSoftNotificationHandle > KErrNone)
        {
        CSoftNotification
                * softNotification =
                        reinterpret_cast<CSoftNotification*> (aSoftNotificationHandle);
        delete softNotification;
        }
    }
