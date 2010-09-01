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
* Description: JNI implementation of Clipboard class
*
*/


// INTERNAL INCLUDES
#include "com_nokia_mid_ui_Clipboard.h"

// EXTERNAL INCLUDES
#include <CMIDToolkit.h>
#include <jutils.h>
#include <jdebug.h>
#include <txtetext.h>
#include <baclipb.h>
#include <s32mem.h>
#include <s32ucmp.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <txtclipboard.h>
#endif

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
 * @return The converted Java string or NULL if the conversion failed.
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
 * Local helper function for inserting the text to clipboard.
 *
 * @param aText The text to be copied to the clipboard.
 */
LOCAL_C void CopyToClipboardL(const TDesC* aText)
{
    if (aText == &KNullDesC)
    {
        CClipboard::Clear(CCoeEnv::Static()->FsSession());
    }
    else
    {
        CClipboard* clipboard = CClipboard::NewForWritingLC(
                                    CCoeEnv::Static()->FsSession());

        CPlainText* text = CPlainText::NewL();
        CleanupStack::PushL(text);
        text->InsertL(0, *aText);
        text->CopyToStoreL(clipboard->Store(), clipboard->StreamDictionary(), 0,
                           text->DocumentLength());
        CleanupStack::PopAndDestroy(); // text
        clipboard->CommitL();
        CleanupStack::PopAndDestroy();   // clipboard;
    }
}

/*
 * Class:     com_nokia_mid_ui_Clipboard
 * Method:    _copyToClipboard
 * Signature: (IIZ)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mid_ui_Clipboard__1copyToClipboard(
    JNIEnv* aJniEnv,
    jclass /* aPeer */,
    jint aToolkitHandle,
    jstring aText)
{
    DEBUG("Clipboard.cpp - copyToClipboard +");
    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    // Convert the content to a native descriptor. NULL indicates
    // that an empty string is set as content.
    const RJString content(*aJniEnv, aText);

    const TDesC* nativeContent = (aText ? &content : &KNullDesC);

    jint error = toolkit->ExecuteTrap(&CopyToClipboardL,
                                      nativeContent);
    DEBUG("Clipboard.cpp - copyToClipboard -");
    return error;
}

/**
 * Local helper function for retrieving the text from clipboard.
 *
 * @return The text copied from the clipboard.
 */
LOCAL_C void CopyFromClipboardL(HBufC** aText)
{
    CClipboard* cb = NULL;
    TRAPD(error, (cb = CClipboard::NewForReadingL(
                           CCoeEnv::Static()->FsSession()))); // codescanner::eikonenvstatic
    // Nothing in the clipboard, not an error
    if ((error != KErrPathNotFound) && (error != KErrNotFound))
    {
        TStreamId id(cb->StreamDictionary().At(KClipboardUidTypePlainText));
        if (id != KNullStreamId)
        {
            CleanupStack::PushL(cb);

            RStoreReadStream stream;
            stream.OpenLC(cb->Store(), id);
            TInt length = stream.ReadInt32L();
            CBufFlat* buffer = CBufFlat::NewL(length);
            CleanupStack::PushL(buffer);

            RBufWriteStream bufStream(*buffer);
            CleanupClosePushL(bufStream);
            TMemoryStreamUnicodeSink sink(bufStream);
            TUnicodeExpander e;
            e.ExpandL(sink, stream, length);
            bufStream.CommitL();

            CleanupStack::PopAndDestroy();      // bufStream
            CleanupStack::Pop(buffer);
            CleanupStack::PopAndDestroy(2, cb);   // stream, cb

            CleanupStack::PushL(buffer);

            TPtrC8 ptr8(buffer->Ptr(0));

            TPtr ptr = TPtr16(reinterpret_cast<TText16*>(const_cast<TText8*>(ptr8.Ptr())),
                              length,
                              length);

            if (ptr.Length() != 0)
            {
                *aText = ptr.AllocL();
                CleanupStack::PopAndDestroy(); // ptr;
            }
        }
    }
}

/*
 * Class:     com_nokia_mid_ui_Clipboard
 * Method:    _copyFromClipboard
 * Signature: (II)Z
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mid_ui_Clipboard__1copyFromClipboard(
    JNIEnv* aJniEnv,
    jclass /* aPeer */,
    jint aToolkitHandle,
    jintArray aError)
{
    DEBUG("Clipboard.cpp - copyFromClipboard +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    HBufC* text = NULL;

    jstring content = NULL;

    TInt error = toolkit->ExecuteTrap(
                     &CopyFromClipboardL,
                     &text);
    if (text != NULL)
    {
        // Handle text conversion an errors.
        content = HandleString(*aJniEnv, *text, error, aError);

        // Text is not needed anymore.
        delete text;
    }

    DEBUG("Clipboard.cpp - copyFromClipboard -");
    return content;
}

// End of file
