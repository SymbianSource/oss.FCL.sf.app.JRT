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
* Description: JNI implementation of CanvasGraphicsItemPainter class
*
*/


// INTERNAL INCLUDES
#include "javax_microedition_lcdui_CanvasGraphicsItemPainter.h"
#include "CMIDCanvas.h" // userinclude in nokialcdui.component

// EXTERNAL INCLUDES
#include <MMIDCanvasGraphicsItemPainter.h>
#include <CMIDToolkit.h>
#include <jutils.h>
#include <jdebug.h>

/**
 * Local helper function for disposing graphics item painter
 * native side component.
 *
 * @param aItem The graphics item painter to be destroyed.
 */
LOCAL_C void Dispose(CMIDToolkit* aToolkit, MMIDCanvasGraphicsItemPainter* aItem)
{
    aToolkit->DisposeObject(aItem);
}

/*
 * Class:     com_nokia_mid_ui_CanvasGraphicsItemPainter
 * Method:    _dispose
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_javax_microedition_lcdui_CanvasGraphicsItemPainter__1dispose(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle)
{
    DEBUG("CanvasGraphicsItemPainter.cpp - dispose +");

    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);

    MMIDCanvasGraphicsItemPainter* item =
        MIDUnhandObject<MMIDCanvasGraphicsItemPainter>(aNativePeerHandle);

    toolkit->ExecuteV(&Dispose, toolkit, item);

    DEBUG("CanvasGraphicsItemPainter.cpp - dispose -");
}

LOCAL_C void CreateNativePeerL
(
    CMIDToolkit* aToolkit,
    jobject aPeer,
    TInt* aNativePeerHandle,
    int aWidth,
    int aHeight
)
{
    // Get LCDUI component factory.
    MMIDComponentFactory* factory = aToolkit->ComponentFactory();

    // Create new text canvas graphics item painter component.
    MMIDCanvasGraphicsItemPainter* itemPainter =
        factory->CreateCanvasGraphicsItemPainterL(aWidth, aHeight);

    // Put the component to cleanup stack during the register operation.
    CleanupDisposePushL(itemPainter);

    // Register component to the LCDUI.
    *aNativePeerHandle = aToolkit->RegisterComponentL(itemPainter, aPeer);

    // Component can be popped from the cleanup stack.
    CleanupPopComponent(itemPainter);
}

JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_CanvasGraphicsItemPainter__1createNativePeer
(
    JNIEnv* aJniEnv,
    jobject aPeer,
    jint aToolkitHandle,
    jint aWidth,
    jint aHeight
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkitHandle);
    jobject peer = aJniEnv->NewWeakGlobalRef(aPeer);
    TInt handle = -1;

    TInt err = toolkit->ExecuteTrap(
                   &CreateNativePeerL,
                   toolkit,
                   peer,
                   &handle,
                   aWidth,
                   aHeight);

    if (err != KErrNone)
    {
        // Global reference must be removed at this point if construction
        // failed for some reason.
        aJniEnv->DeleteWeakGlobalRef(static_cast<jweak>(peer));

        return err;
    }

    return handle;
}

LOCAL_C void ResetFrameBuffer
(
    MMIDCanvasGraphicsItemPainter* aPainter,
    int aPositionX,
    int aPositionY,
    int aWidth,
    int aHeight
)
{
    aPainter->ResetFrameBuffer(
        TPoint(aPositionX, aPositionY), TSize(aWidth, aHeight));
}

JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_CanvasGraphicsItemPainter__1resetFrameBuffer
(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */,
    jint aToolkitHandle,
    jint aNativePeerHandle,
    jint aPositionX,
    jint aPositionY,
    jint aWidth,
    jint aHeight
)
{
    CMIDToolkit* toolkit = JavaUnhand< CMIDToolkit >(aToolkitHandle);
    MMIDCanvasGraphicsItemPainter* painter =
        MIDUnhandObject< MMIDCanvasGraphicsItemPainter >(aNativePeerHandle);

    TInt error = toolkit->ExecuteTrap(
                     &ResetFrameBuffer,
                     painter,
                     (TInt)aPositionX, (TInt)aPositionY,
                     (TInt)aWidth, (TInt)aHeight);

    return error;
}
