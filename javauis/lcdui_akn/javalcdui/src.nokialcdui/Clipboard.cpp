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

/**
 * Local helper function for inserting the text to clipboard.
 *
 * @param aText The text to be copied to the clipboard.
 */
LOCAL_C void CopyToClipboardL(const TDesC* aText)
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

// End of file
