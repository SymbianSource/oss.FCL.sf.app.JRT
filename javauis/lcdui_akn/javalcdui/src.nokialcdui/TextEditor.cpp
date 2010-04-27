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
* Description: JNI implementation of TextEditor class
*
*/


// INTERNAL INCLUDES
#include "com_nokia_mid_ui_TextEditor.h"
#include "CMIDCanvas.h" // userinclude in nokialcdui.component

// EXTERNAL INCLUDES
#include <MMIDTextEditor.h>
#include <CMIDToolkit.h>
#include <jutils.h>
#include <jdebug.h>

// UNNAMED LOCAL NAMESPACE
namespace
{
// The number of colors (r,g,b,a)
const TInt KNumColors       = 4;
const TInt KColorIndexRed   = 0;
const TInt KColorIndexGreen = 1;
const TInt KColorIndexBlue  = 2;
const TInt KColorIndexAlpha = 3;
}

/**
 * Local helper method for handling native strings and converting them to
 * Java strings.
 *
 * Error is set also in this method.
 *
 * @param aJniEnv A valid reference JNI environment.
 * @param aText The native descriptor to be converted.
 * @param aError The return value from ExecuteTrap.
 * @param aReturnError On return, contains an error code.
 * @return The converted Java string of NULL if the conversion failed.
 */
LOCAL_C jstring HandleString(
    JNIEnv& aJniEnv,
    const TDesC& aText,
    TInt aError,
    jintArray aReturnError)
{
    DEBUG("TextEditor.cpp - HandleString +");

    jstring javaText = NULL;

    // Create Java string if the operation was successful.
    if (aError == KErrNone)
    {
        javaText = CreateJavaString(aJniEnv, aText);

        // If NULL is returned, it indicates that the creation failed.
        if (!javaText)
        {
            aError = KErrNoMemory;
        }
    }

    jint errorArray[ 1 ] = { aError };
    aJniEnv.SetIntArrayRegion(aReturnError, 0, 1, errorArray);

    DEBUG_INT("TextEditor.cpp - HandleString -, aError=%d", aError);

    return javaText;
}

/**
 * Local helper function for creating the native side peer object for
 * TextEditor.
 *
 * @param aNativePeerHandle On return, contains the native peer's handle
 */
LOCAL_C void CreateNativePeerL(
    CMIDToolkit* aToolkit,
    jobject aPeer,
    TInt* aNativePeerHandle,
    TInt aMaxSize,
    TInt aWidth,
    TInt aHeight,
    TBool aHeightInRows,
    TSize* aSize)
{
    // Get LCDUI component factory.
    MMIDComponentFactory* factory = aToolkit->ComponentFactory();

    // Create new text editor component.
    MMIDTextEditor* textEditor = factory->CreateTextEditorL(
                                     aMaxSize, aWidth, aHeight, aHeightInRows);

    // Put the component to cleanup stack during the register operation.
    CleanupDisposePushL(textEditor);

    // Register component to the LCDUI.
    *aNativePeerHandle = aToolkit->RegisterComponentL(textEditor, aPeer);

    // Component can be popped from the cleanup stack.
    CleanupPopComponent(textEditor);

    // Store the size of the text editor.
    *aSize = textEditor->EditorSize();
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _createNativePeer
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1createNativePeer(
    JNIEnv* aJniEnv,
    jobject aPeer,
    jint aToolkitHandle,
    jint aMaxSize,
    jint aWidth,
    jint aHeight,
    jboolean aHeightInRows,
    jintArray aSize)
{
    DEBUG("TextEditor.cpp - createNativePeer +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    jobject peer = aJniEnv->NewWeakGlobalRef(aPeer);
    TInt handle = -1;
    TSize size;

    TInt error = toolkit->ExecuteTrap(
                     &CreateNativePeerL,
                     toolkit,
                     peer,
                     &handle,
                     aMaxSize,
                     aWidth,
                     aHeight,
                     (TBool)aHeightInRows,
                     &size);

    // Store returned size.
    if (error == KErrNone)
    {
        jint sizeArray[ 2 ] = { size.iWidth, size.iHeight };
        aJniEnv->SetIntArrayRegion(aSize, 0, 2, sizeArray);

        DEBUG_INT2("TextEditor.cpp - createNativePeer -, width=%d height=%d",
                   size.iWidth, size.iHeight);
    }
    else
    {
        // Global reference must be removed at this point if construction
        // failed for some reason.
        aJniEnv->DeleteWeakGlobalRef(static_cast< jweak >(peer));
    }

    DEBUG_INT("TextEditor.cpp - createNativePeer, error = %d", error);

    return (error != KErrNone ? error : handle);
}

/**
 * Local helper function for setting the multiline status of text editor
 *
 * @param aTextEditor The text editor object to be modified.
 * @param aMultiline The multiline status of the text editor to be set.
 */
LOCAL_C void SetMultilineL(
    MMIDTextEditor* aTextEditor,
    TBool aMultiline)
{
    aTextEditor->SetMultilineL(aMultiline);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setMultiline
 * Signature: (IIZ)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setMultiline(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jboolean aMultiline)
{
    DEBUG("TextEditor.cpp - setMultiline +");

    CMIDToolkit* toolkit =
        JavaUnhand<CMIDToolkit>(aToolkitHandle);
    MMIDTextEditor* editor =
        MIDUnhandObject<MMIDTextEditor>(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(&SetMultilineL, editor,
                                      (TBool)aMultiline);

    DEBUG_INT("TextEditor.cpp - setMultiline -, error=%d", error);

    return error;
}

/**
 * Local helper function for getting of multiline state of a text editor
 *
 * @param aTextEditor The text editor object to be modified.
 * @return The multiline state of the text editor.
 */
LOCAL_C TBool IsMultiline(MMIDTextEditor* aTextEditor)
{
    return aTextEditor->IsMultiline();
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _isMultiline
 * Signature: (IIZ)I
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mid_ui_TextEditor__1isMultiline(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("TextEditor.cpp - isMultiline +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TBool multiline = toolkit->Execute(&IsMultiline, editor);

    DEBUG_INT("TextEditor.cpp - isMultiline -, multiline=%d", multiline);

    return multiline;
}

/**
 * Local helper function for setting the size of a text editor
 *
 * @param aTextEditor The text editor object to be modified.
 * @param aWidth The width of the editor.
 * @param aHeight The height of the editor.
 * @param aHeightInRows Indicates if the height is presented in rows.
 */
LOCAL_C void SetSize(
    MMIDTextEditor* aTextEditor,
    TInt aWidth,
    TInt aHeight)
{
    aTextEditor->SetEditorSize(aWidth, aHeight);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setSize
 * Signature: (IIII)I
 */
JNIEXPORT int JNICALL Java_com_nokia_mid_ui_TextEditor__1setSize(
    JNIEnv* /*aJniEnv*/,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aWidth,
    jint aHeight)
{
    DEBUG("TextEditor.cpp - setSize +");

    CMIDToolkit* toolkit =
        JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    toolkit->ExecuteV(
        &SetSize,
        editor,
        aWidth,
        aHeight);

    DEBUG("TextEditor.cpp - setSize -");

    return KErrNone;
}

/**
 * Local helper function for setting the parent of a text editor
 *
 * @param aTextEditor The text editor object to be modified.
 * @param aParent The parent to be set.
 */
LOCAL_C void SetParentL(MMIDTextEditor* aTextEditor, MMIDComponent* aParent)
{
    MMIDCustomComponentContainer* container = NULL;
    MDirectContainer* directContainer = NULL;

    if (aParent)
    {
        MMIDComponent::TType type(aParent->Type());

        __ASSERT_DEBUG(type == MMIDComponent::ECanvas ||
                       type == MMIDComponent::ECustomItem,
                       User::Invariant());

        // Use static cast instead of reinterpret_cast because
        // reinterpret_cast does not preform the conversion correctly.
        // static_cast is OK eventhough CMIDCanvas is non-sharable class.
        // We don't use its methods.
        container = static_cast< CMIDCanvas* >(aParent);
        directContainer = static_cast< CMIDCanvas* >(aParent);
    }

    aTextEditor->SetParentL(container);
    aTextEditor->SetDirectContainerL(directContainer);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setParent
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setParent(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aParentHandle)
{
    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDComponent* parent = NULL;

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    if (aParentHandle)
    {
        parent = MIDUnhandObject< MMIDComponent >(aParentHandle);
    }

    TInt error = toolkit->ExecuteTrap(
                     &SetParentL,
                     editor,
                     parent);

    DEBUG_INT("TextEditor.cpp - setParent, error = %d", error);

    return error;
}

/**
 * Local helper function for setting a text editor visible.
 *
 * @param aEditor The editor to set visible.
 */
LOCAL_C void SetVisibleL(MMIDTextEditor* aEditor, TBool aVisible)
{
    aEditor->SetVisibleL(aVisible);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _show
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setVisible(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jboolean aVisible)
{
    DEBUG_INT("TextEditor.cpp - setVisible +, aVisible=%d", aVisible);

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &SetVisibleL,
                     editor,
                     (TBool)aVisible);

    DEBUG_INT("TextEditor.cpp - setVisible -, error=%d", error);

    return error;
}

/**
 * Local helper function for setting a text editor's position.
 *
 * @param aEditor The editor.
 * @param aX The x coordinate of the anchor point.
 * @param aY The y coordinate of the anchor point.
 */
LOCAL_C void SetPosition(MMIDTextEditor* aEditor, TInt aX, TInt aY)
{
    aEditor->SetPosition(aX, aY);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setPosition
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setPosition(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aX,
    jint aY)
{
    DEBUG("TextEditor.cpp - setPosition +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    toolkit->ExecuteV(&SetPosition, editor, aX, aY);

    DEBUG("TextEditor.cpp - setPosition -");

    return KErrNone;
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
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setTouchEnabled
 * Signature: (IIZ)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setTouchEnabled(
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

/**
 * Local helper function for setting the focus state of a text editor
 *
 * @param aTextEditor The text editor object to be modified.
 * @param aFocused The focus status of the text editor to be set.
 */
LOCAL_C void SetFocusStateL(
    MMIDTextEditor* aTextEditor,
    TBool aFocusState)
{
    aTextEditor->SetFocusStateL(aFocusState);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setFocusState
 * Signature: (IIZ)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setFocusState(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jboolean aFocusState)
{
    DEBUG("TextEditor.cpp - setFocusState +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &SetFocusStateL,
                     editor,
                     (TBool)aFocusState);

    DEBUG_INT("TextEditor.cpp - setFocusState -, error=%d", error);

    return error;
}

/**
 * Local helper function for getting of focus state of a text editor
 *
 * @param aTextEditor The text editor object to be modified.
 * @return The focus state of the text editor.
 */
LOCAL_C TBool GetFocusState(MMIDTextEditor* aTextEditor)
{
    return aTextEditor->GetFocusState();
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setFocusState
 * Signature: (IIZ)I
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mid_ui_TextEditor__1getFocusState(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("TextEditor.cpp - getFocusState +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TBool focusState = toolkit->Execute(&GetFocusState, editor);

    DEBUG_INT("TextEditor.cpp - getFocusState -, focusState=%d", focusState);

    return focusState;
}

/**
 * Local helper function for setting the constraints of a text editor
 *
 * @param aTextEditor The text editor object to be modified.
 * @param aConstraints The new constraints for the editor.
 */
LOCAL_C void SetConstraintsL(MMIDTextEditor* aTextEditor, TInt aConstraints)
{
    aTextEditor->SetConstraintsL(aConstraints);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setConstraints
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setConstraints(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aConstraints)
{
    DEBUG("TextEditor.cpp - setConstraints +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &SetConstraintsL,
                     editor,
                     aConstraints);

    DEBUG_INT("TextEditor.cpp - setConstraints -, error=%d", error);

    return error;
}

/**
 * Local helper function for setting the initial input mode of a text editor
 *
 * @param aTextEditor The text editor object to be modified.
 * @param aCharacterSubset The character subset for the input mode
 */
LOCAL_C void SetInitialInputModeL(
    MMIDTextEditor* aTextEditor,
    const TDesC* aCharacterSubset)
{
    // Subset must be at least an empty string.
    __ASSERT_DEBUG(aCharacterSubset, User::Invariant());

    aTextEditor->SetInitialInputModeL(*aCharacterSubset);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setInitialInputMode
 * Signature: (IILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setInitialInputMode(
    JNIEnv* aJniEnv,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jstring aCharacterSubset)
{
    DEBUG("TextEditor.cpp - setInitialInputMode +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    // Convert the subset to a native descriptor. NULL indicates
    // that an empty string is set as initial input mode.
    const RJString subset(*aJniEnv, aCharacterSubset);
    const TDesC* nativeSubset = (aCharacterSubset ? &subset : &KNullDesC);

    TInt error = toolkit->ExecuteTrap(
                     &SetInitialInputModeL,
                     editor,
                     nativeSubset);

    DEBUG_INT("TextEditor.cpp - setInitialInputMode -, error=%d", error);

    return error;
}

/**
 * Local helper function for setting the content of a text editor
 *
 * @param aTextEditor The text editor object to be modified.
 * @param aConstraints The new content for the editor.
 */
LOCAL_C void SetContentL(MMIDTextEditor* aTextEditor, const TDesC* aContent)
{
    // Content must be at least an empty string.
    __ASSERT_DEBUG(aContent, User::Invariant());

    aTextEditor->SetContentL(*aContent);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setContent
 * Signature: (IILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setContent(
    JNIEnv* aJniEnv,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jstring aContent)
{
    DEBUG("TextEditor.cpp - setContent +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    // Convert the content to a native descriptor. NULL indicates
    // that an empty string is set as content.
    const RJString content(*aJniEnv, aContent);
    const TDesC* nativeContent = (aContent ? &content : &KNullDesC);

    TInt error = toolkit->ExecuteTrap(
                     &SetContentL,
                     editor,
                     nativeContent);

    DEBUG_INT("TextEditor.cpp - setContent -, error=%d", error);

    return error;
}

/**
 * Local helper function for getting the content of a text editor
 *
 * @param aTextEditor The text editor object to be modified.
 * @param aContent On return, contains the content of the editor.
 *        The ownership is transferred to the caller.
 */
LOCAL_C void GetContentL(MMIDTextEditor* aTextEditor, HBufC** aContent)
{
    *aContent = aTextEditor->ContentL();
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _getContent
 * Signature: (II[I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mid_ui_TextEditor__1getContent(
    JNIEnv* aJniEnv,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jintArray aError)
{
    DEBUG("TextEditor.cpp - getContent +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    HBufC* text = NULL;

    TInt error = toolkit->ExecuteTrap(
                     &GetContentL,
                     editor,
                     &text);

    // Handle text conversion an errors.
    jstring content = HandleString(*aJniEnv, *text, error, aError);

    // Text is not needed anymore.
    delete text;

    DEBUG("TextEditor.cpp - getContent -");

    return content;
}

/**
 * Local helper function for deleting a content from a text editor.
 *
 * @param aEditor The editor.
 * @param aOffset The beginning of the range to be deleted.
 * @param aLength The length of the range to be deleted.
 */
LOCAL_C void DeleteContentL(
    MMIDTextEditor* aEditor,
    TInt aOffset,
    TInt aLength)
{
    return aEditor->DeleteContentL(aOffset, aLength);
}


/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _delete
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1delete(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aOffset,
    jint aLength)
{
    DEBUG("TextEditor.cpp - delete +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &DeleteContentL,
                     editor,
                     aOffset,
                     aLength);

    DEBUG_INT("TextEditor.cpp - delete -, error=%d", error);

    return error;
}

/**
 * Local helper function for getting the number of characters in a text
 * editor.
 *
 * @param aEditor The editor.
 * @return The size of the editor's content.
 */
LOCAL_C TInt Size(MMIDTextEditor* aEditor)
{
    return aEditor->ContentLength();
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _size
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1size(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("TextEditor.cpp - size +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt size = toolkit->Execute(&Size, editor);

    DEBUG_INT("TextEditor.cpp - size -, size=%d", size);

    return size;
}

/**
 * Local helper function for getting the line margin height in a text editor.
 *
 * @param aEditor The editor.
 * @return The line margin height of the editor.
 */
LOCAL_C TInt LineMarginHeight(MMIDTextEditor* aEditor)
{
    return aEditor->LineMarginHeight();
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _getLineMarginHeight
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1getLineMarginHeight(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("TextEditor.cpp - getLineMarginHeight +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt lineMarginHeight = toolkit->Execute(&LineMarginHeight, editor);

    DEBUG_INT("TextEditor.cpp - getLineMarginHeight -, lineMarginHeight=%d",
              lineMarginHeight);

    return lineMarginHeight;
}

/**
 * Local helper function for getting the content height in a text editor.
 *
 * @param aEditor The editor.
 * @return The content height of the editor.
 */
LOCAL_C TInt ContentHeight(MMIDTextEditor* aEditor)
{
    return aEditor->ContentHeight();
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _getContentHeight
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1getContentHeight(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("TextEditor.cpp - getContentHeight +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt contentHeight = toolkit->Execute(&ContentHeight, editor);

    DEBUG_INT("TextEditor.cpp - getContentHeight -, contentHeight=%d",
              contentHeight);

    return contentHeight;
}

/**
 * Local helper function for inserting content to a text editor.
 *
 * @param aEditor The editor.
 * @param aContent The content to be inserted.
 * @param aPosition The position of the inserted text.
 */
LOCAL_C void InsertContentL(
    MMIDTextEditor* aEditor,
    const TDesC* aContent,
    TInt aPosition)
{
    // Content must not be NULL.
    __ASSERT_DEBUG(aContent, User::Invariant());
    aEditor->InsertContentL(*aContent, aPosition);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _insert
 * Signature: (IILjava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1insert(
    JNIEnv* aJniEnv,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jstring aContent,
    jint aPosition)
{
    DEBUG("TextEditor.cpp - insert +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    // Convert the content to a native descriptor.
    const RJString content(*aJniEnv, aContent);
    const TDesC* nativeContent = (aContent ? &content : NULL);

    TInt error = toolkit->ExecuteTrap(
                     &InsertContentL,
                     editor,
                     nativeContent,
                     aPosition);

    DEBUG_INT("TextEditor.cpp - insert -, error=%d", error);

    return error;
}

/**
 * Local helper function for setting the maximum size of a text editor.
 *
 * @param aEditor The editor.
 * @param aMaxSize The new maximum size for the editor.
 * @param aChangedMaxSize The actual size which the editor has accepted.
 */
LOCAL_C void SetMaxSizeL(
    MMIDTextEditor* aEditor,
    TInt aMaxSize,
    TInt* aChangedMaxSize)
{
    *aChangedMaxSize = aEditor->SetMaxSizeL(aMaxSize);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setMaxSize
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setMaxSize(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aMaxSize)
{
    DEBUG("TextEditor.cpp - setMaxSize +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt changedMaxSize = 0;

    TInt error = toolkit->ExecuteTrap(
                     &SetMaxSizeL,
                     editor,
                     aMaxSize,
                     &changedMaxSize);

    DEBUG_INT("TextEditor.cpp - setMaxSize -, error=%d", error);

    return (error == KErrNone ? changedMaxSize : error);
}

/**
 * Local helper function for getting the cursor position in a text editor.
 *
 * @param aEditor The editor.
 * @return The current position of the cursor in the text editor.
 */
LOCAL_C TInt CursorPosition(MMIDTextEditor* aEditor)
{
    return aEditor->CursorPosition();
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _getCaretPosition
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1getCaretPosition(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("TextEditor.cpp - getCaretPosition +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt position = toolkit->Execute(&CursorPosition, editor);

    DEBUG_INT("TextEditor.cpp - getCaretPosition -, position=%d", position);

    return position;
}

/**
 * Local helper function for getting the topmost pixel's position
 * in a text editor.
 *
 * @param aEditor The editor.
 * @return The topmost pixel's position of the visible content.
 */
LOCAL_C TInt VisibleContentPosition(MMIDTextEditor* aEditor)
{
    return aEditor->VisibleContentPosition();
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _getVisibleContentPosition
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mid_ui_TextEditor__1getVisibleContentPosition(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("TextEditor.cpp - getVisibleContentPosition +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt position = toolkit->Execute(&VisibleContentPosition, editor);

    DEBUG_INT("TextEditor.cpp - getVisibleContentPosition -, position=%d",
              position);

    return position;
}

/**
 * Local helper function for setting the cursor's position in a text editor.
 *
 * @param aEditor The editor.
 * @param aIndex The new position of the cursor.
 */
LOCAL_C void SetCursorPositionL(
    MMIDTextEditor* aEditor,
    TInt aIndex)
{
    aEditor->SetCursorPositionL(aIndex);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setCaret
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setCaret(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aIndex)
{
    DEBUG("TextEditor.cpp - setCaret +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &SetCursorPositionL,
                     editor,
                     aIndex);

    DEBUG_INT("TextEditor.cpp - setCaret -, error=%d", error);

    return error;
}

/**
 * Local helper function for setting the color in a text editor.
 *
 * @param aEditor The editor.
 * @param aColor The new color for the specified color type.
 * @param aColorType The color type to be modified.
 */
LOCAL_C void SetColorL(
    MMIDTextEditor* aEditor,
    const TRgb* aColor,
    TInt aColorType)
{
    aEditor->SetColorL(
        *aColor,
        static_cast< MMIDTextEditor::TColorType >(aColorType));
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setColor
 * Signature: (II[II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setColor(
    JNIEnv* aJniEnv,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jintArray aColor,
    jint aColorType)
{
    DEBUG("TextEditor.cpp - setColor +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    jint colorArray[ KNumColors ];
    aJniEnv->GetIntArrayRegion(aColor, 0, KNumColors, colorArray);

    const TRgb color(
        colorArray[ KColorIndexRed ],
        colorArray[ KColorIndexGreen ],
        colorArray[ KColorIndexBlue ],
        colorArray[ KColorIndexAlpha ]);

    TInt error = toolkit->ExecuteTrap(
                     &SetColorL,
                     editor,
                     &color,
                     aColorType);

    DEBUG_INT("TextEditor.cpp - setColor -, error=%d", error);

    return error;
}

/**
 * Local helper function for getting the color from a text editor.
 *
 * @param aEditor The editor.
 * @param aColor The new color for the specified color type.
 * @param aColorType The color type to be modified.
 */
LOCAL_C TInt GetColor(MMIDTextEditor* aEditor, TInt aColorType)
{
    return aEditor->GetColor(
               static_cast< MMIDTextEditor::TColorType >(aColorType));
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setColor
 * Signature: (II[II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1getColor(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aColorType)
{
    DEBUG("TextEditor.cpp - getColor +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    jint color = toolkit->Execute(&GetColor, editor, (TInt)aColorType);

    //DEBUG_INT("TextEditor.cpp - getColor -, error=%d", error);

    return color;
}


/**
 * Local helper function for selecting a content from a text editor.
 *
 * @param aEditor The editor.
 * @param aIndex The beginning of the range to be selected.
 * @param aLength The length of the range to be selected.
 */
LOCAL_C void SetSelectionL(
    MMIDTextEditor* aEditor,
    TInt aIndex,
    TInt aLength)
{
    return aEditor->SetSelectionL(aIndex, aLength);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setSelection
 * Signature: (IIII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setSelection(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aIndex,
    jint aLength)
{
    DEBUG("TextEditor.cpp - setSelection +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &SetSelectionL,
                     editor,
                     aIndex,
                     aLength);

    DEBUG_INT("TextEditor.cpp - setSelection -, error=%d", error);

    return error;
}

/**
 * Local helper function for getting the current selection of a text editor
 *
 * @param aTextEditor The text editor object to be modified.
 * @param aContent On return, contains the current selection of the editor.
 *        The ownership is transferred to the caller.
 */
LOCAL_C void GetSelectionL(MMIDTextEditor* aTextEditor, HBufC** aContent)
{
    *aContent = aTextEditor->SelectionL();
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _getSelection
 * Signature: (II[I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mid_ui_TextEditor__1getSelection(
    JNIEnv* aJniEnv,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jintArray aError)
{
    DEBUG("TextEditor.cpp - getSelection +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    HBufC* text = NULL;

    TInt error = toolkit->ExecuteTrap(
                     &GetSelectionL,
                     editor,
                     &text);

    // Handle text conversion an errors.
    jstring selection = HandleString(*aJniEnv, *text, error, aError);

    // Text is not needed anymore.
    delete text;

    DEBUG("TextEditor.cpp - getSelection -");

    return selection;
}

/**
 * Local helper function for setting the font to text editor.
 *
 * @param aEditor The editor.
 * @param aFont The font to be set.
 * @param aSize On return, contains the new size of the editor.
 */
LOCAL_C void SetFontL(
    MMIDTextEditor* aEditor,
    MMIDFont* aFont,
    TSize* aSize)
{
    aEditor->SetFontL(aFont);

    // Store the size of the text editor.
    *aSize = aEditor->EditorSize();
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setFont
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setFont(
    JNIEnv* aJniEnv,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aFontHandle,
    jintArray aNewSize)
{
    DEBUG("TextEditor.cpp - setFont +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    MMIDFont* font = MIDUnhandObject< MMIDFont >(aFontHandle);
    TSize size;

    TInt error = toolkit->ExecuteTrap(
                     &SetFontL,
                     editor,
                     font,
                     &size);

    if (error == KErrNone)
    {
        jint sizeArray[ 2 ] = { size.iWidth, size.iHeight };
        aJniEnv->SetIntArrayRegion(aNewSize, 0, 2, sizeArray);

        DEBUG_INT2("TextEditor.cpp - setFont -, width=%d height=%d",
                   size.iWidth, size.iHeight);
    }

    DEBUG_INT("TextEditor.cpp - setFont -, error=%d", error);

    return error;
}

/**
 * Local helper function for setting an observer for a text editor.
 *
 * @param aTextEditor The text editor object to be modified.
 * @param aObserver The text editor observer or NULL.
 */
LOCAL_C void SetObserver(
    MMIDTextEditor* aTextEditor,
    MMIDTextEditorObserver* aObserver)
{
    aTextEditor->SetObserver(aObserver);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setListener
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setListener(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aListenerHandle)
{
    DEBUG("TextEditor.cpp - setListener +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    MMIDTextEditorObserver* observer = NULL;

    if (aListenerHandle > 0)
    {
        observer =
            MIDUnhandObject< MMIDTextEditorObserver >(aListenerHandle);
    }

    toolkit->ExecuteV(
        &SetObserver,
        editor,
        observer);

    DEBUG("TextEditor.cpp - setListener -");

    return KErrNone;
}

/**
 * Local helper function for setting the elevation of a text editor.
 *
 * @param aTextEditor The text editor object to be modified.
 * @param aZ The z-position defining the elevation.
 */
LOCAL_C void SetElevationL(MMIDTextEditor* aTextEditor, TInt aZ)
{
    aTextEditor->SetElevationL(aZ);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _setZPosition
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1setZPosition(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aZ)
{
    DEBUG("TextEditor.cpp - setZPosition +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &SetElevationL,
                     editor,
                     aZ);

    DEBUG_INT("TextEditor.cpp - setZPosition -, error=%d", error);

    return error;
}

/**
 * Local helper function for getting the elevation of a text editor.
 *
 * @param aEditor The editor.
 * @return The size of the editor's content.
 */
LOCAL_C TInt Elevation(MMIDTextEditor* aEditor)
{
    return aEditor->Elevation();
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _getZPosition
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_TextEditor__1getZPosition(
    JNIEnv* /* aJniEnv */,
    jobject /*aPeer*/,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("TextEditor.cpp - getZPosition +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    TInt position = toolkit->Execute(&Elevation, editor);

    DEBUG_INT("TextEditor.cpp - getZPosition -, position=%d", position);

    return position;
}


/**
 * Local helper function for dispoisng text editor native side component.
 *
 * @param aEditor The editor to be destroyed.
 */
LOCAL_C void Dispose(CMIDToolkit* aToolkit, MMIDTextEditor* aEditor)
{
    aToolkit->DisposeObject(aEditor);
}

/*
 * Class:     com_nokia_mid_ui_TextEditor
 * Method:    _dispose
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mid_ui_TextEditor__1dispose(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("TextEditor.cpp - dispose +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDTextEditor* editor =
        MIDUnhandObject< MMIDTextEditor >(aNativePeerHandle);

    toolkit->ExecuteV(&Dispose, toolkit, editor);

    DEBUG("TextEditor.cpp - dispose -");
}

// End of file
