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
* Description: JNI implementation of TextEditorImpl class
*
*/


// INTERNAL INCLUDES
#include "com_nokia_mid_ui_TextEditorImpl.h"

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
 * Class:     com_nokia_mid_ui_TextEditorImpl
 * Method:    _setIndicatorVisibility
 * Signature: (IIZ)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_TextEditorImpl__1setIndicatorVisibility(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jboolean aVisible)
{
    DEBUG_INT("TextEditorImpl.cpp - setIndicatorVisibility +, aVisible=%d",
              aVisible);

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &SetIndicatorVisibilityL,
                     editor,
                     (TBool)aVisible);

    DEBUG_INT("TextEditorImpl.cpp - setIndicatorVisibility -, error=%d",
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
 * Class:     com_nokia_mid_ui_TextEditorImpl
 * Method:    _setIndicatorLocation
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_TextEditorImpl__1setIndicatorLocation(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aX,
    jint aY)
{
    DEBUG("TextEditorImpl.cpp - setIndicatorLocation +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    toolkit->ExecuteV(&SetIndicatorLocation, editor, aX, aY);

    DEBUG("TextEditorImpl.cpp - setIndicatorLocation -");

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
 * Class:     com_nokia_mid_ui_TextEditorImpl
 * Method:    _getIndicatorSize
 * Signature: (II[I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditorImpl__1getIndicatorSize(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jintArray aSize)
{
    DEBUG("TextEditorImpl.cpp - getIndicatorSize +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TSize size;

    toolkit->ExecuteV(&IndicatorSize, editor, &size);

    // Store returned size.
    jint sizeArray[ 2 ] = { size.iWidth, size.iHeight };
    aJniEnv->SetIntArrayRegion(aSize, 0, 2, sizeArray);

    DEBUG_INT2("TextEditorImpl.cpp - getIndicatorSize -, width=%d height=%d",
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
 * Class:     com_nokia_mid_ui_TextEditorImpl
 * Method:    _setDisabledTouchInputModes
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_TextEditorImpl__1setDisabledTouchInputModes(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aTouchInputModes)
{
    DEBUG("TextEditorImpl.cpp - setDisabledTouchInputModes +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    toolkit->ExecuteV(
        &SetDisabledTouchInputModes, editor, aTouchInputModes);

    DEBUG("TextEditorImpl.cpp - setDisabledTouchInputModes -");

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
 * Class:     com_nokia_mid_ui_TextEditorImpl
 * Method:    _getDisabledTouchInputModes
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_TextEditorImpl__1getDisabledTouchInputModes(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("TextEditorImpl.cpp - getDisabledTouchInputModes +");

    CMIDToolkit* toolkit =
        JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt modes = toolkit->Execute(&DisabledTouchInputModes, editor);

    DEBUG("TextEditorImpl.cpp - getDisabledTouchInputModes -");

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
 * Class:     com_nokia_mid_ui_TextEditorImpl
 * Method:    _setPreferredTouchMode
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_TextEditorImpl__1setPreferredTouchMode(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aTouchInputMode)
{
    DEBUG("TextEditorImpl.cpp - setPreferredTouchMode +");

    CMIDToolkit* toolkit =
        JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    toolkit->ExecuteV(&SetPreferredTouchInputMode, editor, aTouchInputMode);

    DEBUG("TextEditorImpl.cpp - setPreferredTouchMode -");

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
 * Class:     com_nokia_mid_ui_TextEditorImpl
 * Method:    _getPreferredTouchMode
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_TextEditorImpl__1getPreferredTouchMode(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("TextEditorImpl.cpp - getPreferredTouchMode +");

    CMIDToolkit* toolkit =
        JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt mode = toolkit->Execute(&PreferredTouchInputMode, editor);

    DEBUG("TextEditorImpl.cpp - getPreferredTouchMode -");

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
 * Class:     com_nokia_mid_ui_TextEditorImpl
 * Method:    _setDefaultIndicators
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_TextEditorImpl__1setDefaultIndicators(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jboolean /*aIsFullScreen*/)
{
    DEBUG("TextEditorImpl.cpp - setDefaultIndicators +");

    CMIDToolkit* toolkit =
        JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(&ResetIndicatorStateL, editor);

    DEBUG("TextEditorImpl.cpp - setDefaultIndicators -");

    return error;
}

/**
 * Local helper function for setting position of a caret.
 *
 * @param aEditor The editor.
 * @param aX The x coordinate of the wanted caret position.
 * @param aY The y coordinate of the wanted caret position.
 */
LOCAL_C void SetCaretXYL(MMIDTextEditor* aEditor, TInt aX, TInt aY)
{
    aEditor->SetCaretXYL(aX, aY);
}

/*
 * Class:     com_nokia_mid_ui_TextEditorImpl
 * Method:    _setCaretXY
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_TextEditorImpl__1setCaretXY(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aX,
    jint aY)
{
    DEBUG("TextEditorImpl.cpp - setCaretXY +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(&SetCaretXYL, editor, aX, aY);

    DEBUG("TextEditorImpl.cpp - setCaretXY -");

    return error;
}

/**
 * Local helper function for setting the touch enabled state of a text editor
 *
 * @param aTextEditor The text editor object to be modified.
 * @param aEnabled The touch enabled status of the text editor to be set.
 */
LOCAL_C void SetTouchEnabled(
    MMIDTextEditor* aTextEditor,
    TBool aEnabled)
{
    aTextEditor->SetTouchEnabled(aEnabled);
}

/*
 * Class:     com_nokia_mid_ui_TextEditorImpl
 * Method:    _setTouchEnabled
 * Signature: (IIZ)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditorImpl__1setTouchEnabled(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jboolean aEnabled)
{
    DEBUG("TextEditor.cpp - setTouchEnabled +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    toolkit->ExecuteV(&SetTouchEnabled, editor, (TBool)aEnabled);

    DEBUG("TextEditor.cpp - setTouchEnabled -");

    return KErrNone;
}

// End of file
