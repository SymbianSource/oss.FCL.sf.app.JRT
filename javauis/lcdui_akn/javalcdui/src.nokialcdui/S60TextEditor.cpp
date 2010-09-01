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
* Description: JNI implementation of S60TextEditor class
*
*/


// INTERNAL INCLUDES
#include "com_nokia_mid_ui_S60TextEditor.h"

// EXTERNAL INCLUDES
#include <MMIDTextEditor.h>
#include <CMIDToolkit.h>
#include <jutils.h>
#include <jdebug.h>

/**
 * Local helper function for setting a text editor visible.
 *
 * @param aEditor The editor to set visible.
 */
LOCAL_C void SetIndicatorVisibilityL(
    MMIDTextEditor* aEditor,
    TBool aVisible)
{
    aEditor->SetIndicatorVisibilityL(aVisible);
}

/*
 * Class:     com_nokia_mid_ui_S60TextEditor
 * Method:    _setIndicatorVisibility
 * Signature: (IIZ)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_S60TextEditor__1setIndicatorVisibility(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jboolean aVisible)
{
    DEBUG_INT("S60TextEditor.cpp - setIndicatorVisibility +, aVisible=%d",
              aVisible);

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &SetIndicatorVisibilityL,
                     editor,
                     (TBool)aVisible);

    DEBUG_INT("S60TextEditor.cpp - setIndicatorVisibility -, error=%d",
              error);

    return error;
}

/**
 * Local helper function for setting a position of an indicator.
 *
 * @param aEditor The editor.
 * @param aX The x coordinate of the anchor point.
 * @param aY The y coordinate of the anchor point.
 */
LOCAL_C void SetIndicatorLocation(MMIDTextEditor* aEditor, TInt aX, TInt aY)
{
    aEditor->SetIndicatorPosition(aX, aY);
}

/*
 * Class:     com_nokia_mid_ui_S60TextEditor
 * Method:    _setIndicatorLocation
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_S60TextEditor__1setIndicatorLocation(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aX,
    jint aY)
{
    DEBUG("S60TextEditor.cpp - setIndicatorLocation +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    toolkit->ExecuteV(&SetIndicatorLocation, editor, aX, aY);

    DEBUG("S60TextEditor.cpp - setIndicatorLocation -");

    return KErrNone;
}

/**
 * Local helper function for getting the size of an indicator.
 *
 * @param aEditor The editor.
 * @param aSize On return contains the size of the indicator.
 */
LOCAL_C void IndicatorSize(MMIDTextEditor* aEditor, TSize* aSize)
{
    *aSize = aEditor->IndicatorSize();
}

/*
 * Class:     com_nokia_mid_ui_S60TextEditor
 * Method:    _getIndicatorSize
 * Signature: (II[I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_S60TextEditor__1getIndicatorSize(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jintArray aSize)
{
    DEBUG("S60TextEditor.cpp - getIndicatorSize +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TSize size;

    toolkit->ExecuteV(&IndicatorSize, editor, &size);

    // Store returned size.
    jint sizeArray[ 2 ] = { size.iWidth, size.iHeight };
    aJniEnv->SetIntArrayRegion(aSize, 0, 2, sizeArray);

    DEBUG_INT2("S60TextEditor.cpp - getIndicatorSize -, width=%d height=%d",
               size.iWidth, size.iHeight);

    return KErrNone;
}

/**
 * Local helper function for setting the disabled touch UI input mode.
 *
 * @param aEditor The editor.
 * @param aTouchInputMode The disabled touch input mode.
 */
LOCAL_C void SetDisabledTouchInputModes(
    MMIDTextEditor* aEditor,
    TInt aTouchInputModes)
{
    aEditor->SetDisabledTouchInputModes(aTouchInputModes);
}

/*
 * Class:     com_nokia_mid_ui_S60TextEditor
 * Method:    _setDisabledTouchInputModes
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_S60TextEditor__1setDisabledTouchInputModes(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aTouchInputModes)
{
    DEBUG("S60TextEditor.cpp - setDisabledTouchInputModes +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    toolkit->ExecuteV(
        &SetDisabledTouchInputModes, editor, aTouchInputModes);

    DEBUG("S60TextEditor.cpp - setDisabledTouchInputModes -");

    return KErrNone;
}

/**
 * Local helper function for getting the disabled touch UI input mode.
 *
 * @param aEditor The editor.
 * @return The disabled touch input mode.
 */
LOCAL_C TInt DisabledTouchInputModes(MMIDTextEditor* aEditor)
{
    return aEditor->DisabledTouchInputModes();
}

/*
 * Class:     com_nokia_mid_ui_S60TextEditor
 * Method:    _getDisabledTouchInputModes
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_S60TextEditor__1getDisabledTouchInputModes(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("S60TextEditor.cpp - getDisabledTouchInputModes +");

    CMIDToolkit* toolkit =
        JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt modes = toolkit->Execute(&DisabledTouchInputModes, editor);

    DEBUG("S60TextEditor.cpp - getDisabledTouchInputModes -");

    return modes;
}

/**
 * Local helper function for setting the preferred touch UI input mode.
 *
 * @param aEditor The editor.
 * @param aTouchInputMode The preferred touch input mode.
 */
LOCAL_C void SetPreferredTouchInputMode(
    MMIDTextEditor* aEditor,
    TInt aTouchInputMode)
{
    aEditor->SetPreferredTouchInputMode(aTouchInputMode);
}

/*
 * Class:     com_nokia_mid_ui_S60TextEditor
 * Method:    _setPreferredTouchMode
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_S60TextEditor__1setPreferredTouchMode(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aTouchInputMode)
{
    DEBUG("S60TextEditor.cpp - setPreferredTouchMode +");

    CMIDToolkit* toolkit =
        JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    toolkit->ExecuteV(&SetPreferredTouchInputMode, editor, aTouchInputMode);

    DEBUG("S60TextEditor.cpp - setPreferredTouchMode -");

    return KErrNone;
}

/**
 * Local helper function for getting the preferred touch UI input mode.
 *
 * @param aEditor The editor.
 * @return The preferred touch input mode.
 */
LOCAL_C TInt PreferredTouchInputMode(MMIDTextEditor* aEditor)
{
    return aEditor->PreferredTouchInputMode();
}

/*
 * Class:     com_nokia_mid_ui_S60TextEditor
 * Method:    _getPreferredTouchMode
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_S60TextEditor__1getPreferredTouchMode(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("S60TextEditor.cpp - getPreferredTouchMode +");

    CMIDToolkit* toolkit =
        JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt mode = toolkit->Execute(&PreferredTouchInputMode, editor);

    DEBUG("S60TextEditor.cpp - getPreferredTouchMode -");

    return mode;
}


/**
 * Local helper function for reseting indicator state.
 *
 * @param aEditor The editor.
 */
LOCAL_C void ResetIndicatorStateL(MMIDTextEditor* aEditor)
{
    aEditor->SetDefaultIndicatorsL();
}

/*
 * Class:     com_nokia_mid_ui_S60TextEditor
 * Method:    _setDefaultIndicators
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_S60TextEditor__1setDefaultIndicators(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jboolean /*aIsFullScreen*/)
{
    DEBUG("S60TextEditor.cpp - setDefaultIndicators +");

    CMIDToolkit* toolkit =
        JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(&ResetIndicatorStateL, editor);

    DEBUG("S60TextEditor.cpp - setDefaultIndicators -");

    return error;
}

// End of file
