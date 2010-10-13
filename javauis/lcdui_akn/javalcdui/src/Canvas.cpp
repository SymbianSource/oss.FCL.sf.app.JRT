/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CMIDToolkit.h"
#include "MIDUtils.h"
#include "javax_microedition_lcdui_Canvas.h"

#ifdef LCDUI_DEBUG_ON
#include <jdebug.h>
#define LCDUI_DEBUG_INT2(msg, x, y) DEBUG_INT2(msg, x, y)
#else
#define LCDUI_DEBUG_INT2(msg, x, y)
#endif

struct TCanvasCreate
{
    jweak       iRef;
    jint        iDisplayable;
    jboolean    iIsGameCanvas;
};

struct TCanvasAttribs
{
    TInt  iHandle;
    TSize iSize;
    TBool iFrameBuffer;
    TBool iHasBackground;
};

LOCAL_C void CreateCanvasL(CMIDToolkit* aToolkit, TCanvasCreate* aCreate, TCanvasAttribs* aAttribs)
{
    MMIDDisplayable* container = MIDUnhand<MMIDDisplayable>(aCreate->iDisplayable);
    // Create a new MMIDCanvas object
    MMIDCanvas* canvas = aToolkit->ComponentFactory()->CreateCanvasL(
                             *container,
                             (aCreate->iIsGameCanvas ? MMIDComponent::EGameCanvas : MMIDComponent::ECanvas));
    CleanupDisposePushL(canvas);
    aAttribs->iHandle = aToolkit->RegisterComponentL(canvas, aCreate->iRef);
    container->SetComponentL(*canvas);
    CleanupPopComponent(canvas);
    aCreate->iRef = 0;
    aAttribs->iSize = canvas->ContentSize();
    aAttribs->iFrameBuffer = static_cast< TBool >(canvas->FrameBuffer() != NULL);

    // Check from a MMIDEnv object if the canvas object has a background or not
    aAttribs->iHasBackground = aToolkit->Env()->CanvasHasBackground(*canvas);

}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Canvas__1create
(
    JNIEnv*   aJni,
    jobject   aCanvas,
    jint      aToolkit,
    jint      aDisplayable,
    jboolean  aIsGameCanvas,
    jintArray aAttribsReturn
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    TCanvasCreate create;
    create.iDisplayable=aDisplayable;
    create.iRef= aJni->NewWeakGlobalRef(aCanvas);
    create.iIsGameCanvas = aIsGameCanvas;
    TCanvasAttribs attribs;
    attribs.iFrameBuffer=EFalse;

    attribs.iHasBackground = EFalse;
    jint error;
    if (create.iRef)
    {
        error = toolkit->ExecuteTrap(&CreateCanvasL, toolkit, &create, &attribs);
    }
    else
    {
        error = KErrNoMemory;
    }

    if (create.iRef)
    {
        // should be cleared in CreateCanvasL if no error
        aJni->DeleteWeakGlobalRef(create.iRef);
    }

    ASSERT(aJni->GetArrayLength(aAttribsReturn) == 5);
    jint attribsArray[5];
    attribsArray[0] = static_cast< jint >(attribs.iHandle);
    attribsArray[1] = static_cast< jint >(attribs.iSize.iWidth);
    attribsArray[2] = static_cast< jint >(attribs.iSize.iHeight);
    attribsArray[3] = static_cast< jint >(attribs.iFrameBuffer);
    attribsArray[4] = static_cast< jint >(attribs.iHasBackground);
    aJni->SetIntArrayRegion(aAttribsReturn, 0, 5, &attribsArray[0]);

    LCDUI_DEBUG_INT2("Canvas_create(%x == %x)", handle, (TInt)MIDUnhandObject<MMIDCanvas>(handle));
    return error;
}

LOCAL_C TInt GetKeyCode(MMIDUtils* aUtils,TInt aGameAction)
{
    return aUtils->GetKeyCode(aGameAction);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Canvas__1getKeyCode
(JNIEnv *,jobject,jint aToolkit,jint aGameAction)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    jint keyCode = toolkit->Execute(&GetKeyCode, toolkit->Utils(),(TInt)aGameAction);
    return keyCode;
}

LOCAL_C void GetKeyName(MMIDUtils* aUtils,TDes* aText,TInt aKeyCode)
{
    aUtils->GetKeyName(*aText,aKeyCode);
}
JNIEXPORT jstring JNICALL Java_javax_microedition_lcdui_Canvas__1getKeyName
(JNIEnv* aJni,jobject,jint aToolkit,jint aKeyCode)
{
    TBuf<KMaxName> keyName;
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    toolkit->ExecuteV(&GetKeyName,toolkit->Utils(),(TDes*)&keyName,(TInt)aKeyCode);
    jstring str = CreateJavaString(*aJni,keyName);
    return str;
}

LOCAL_C TInt GetGameAction(MMIDUtils* aUtils,TInt aKeyCode)
{
    return aUtils->GetGameAction(aKeyCode);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Canvas__1getGameAction
(JNIEnv*, jclass, jint aToolkit, jint aKeyCode)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    jint gameAction = toolkit->Execute(&GetGameAction, toolkit->Utils(), (TInt)aKeyCode);
    LCDUI_DEBUG_INT2("getGameAction(%d) -> %d", aKeyCode, gameAction);
    return gameAction;
}

LOCAL_C void InvokeGetCanvasSize(MMIDCanvas* aCanvas, TSize* aSize)
{
    *aSize = aCanvas->ContentSize();
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Canvas__1getCanvasSize
(
    JNIEnv*,
    jobject,
    jint aToolkit,
    jint aCanvas
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDCanvas* canvas = MIDUnhand<MMIDCanvas>(aCanvas);
    TSize size;
    toolkit->ExecuteV(&InvokeGetCanvasSize, canvas, &size);
    return (size.iWidth << 16) | (size.iHeight & 0x0000ffff);
}

LOCAL_C void InvokeSuppressKeys(MMIDCanvas* aCanvas)
{
    ASSERT(aCanvas);
    aCanvas->SuppressKeys();
}

JNIEXPORT void JNICALL Java_javax_microedition_lcdui_Canvas__1suppressKeyEvents
(JNIEnv*,jobject,jint aToolkit,jint aCanvas)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDCanvas*  canvas  = MIDUnhand<MMIDCanvas>(aCanvas);
    toolkit->ExecuteV(&InvokeSuppressKeys, canvas);
}

LOCAL_C TInt InvokeGameActions(MMIDCanvas* aCanvas)
{
    ASSERT(aCanvas);
    return TInt(aCanvas->GameActions());
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Canvas__1getKeyStates
(JNIEnv *,jobject,jint aToolkit, jint aCanvas)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDCanvas*  canvas  = MIDUnhand<MMIDCanvas>(aCanvas);

    TInt actions = toolkit->Execute(&InvokeGameActions, canvas);
    return actions;
}

LOCAL_C void InvokeSetFullScreenModeL(jint aDisplayable,jboolean aFullScreen, jint* aSizeArray)
{
    MMIDDisplayable* displayable = MIDUnhandObject<MMIDDisplayable>(aDisplayable);
    MMIDCanvas* canvas = (MMIDCanvas*)displayable->Component();
    displayable->SetFullScreenModeL(aFullScreen);
    TSize size = canvas->ContentSize();
    aSizeArray[0]=size.iWidth;
    aSizeArray[1]=size.iHeight;
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Canvas__1setFullScreenMode
(JNIEnv* aEnv,jobject,jint aToolkit,jint aDisplayable,jboolean aFullScreen, jintArray aSizeReturn)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    jint sizeArray[2];
    jint err = toolkit->ExecuteTrap(&InvokeSetFullScreenModeL, aDisplayable, aFullScreen, sizeArray);
    aEnv->SetIntArrayRegion(aSizeReturn, 0, 2, sizeArray);
    return err;
}

LOCAL_C jboolean InvokeNetworkIndicatorLocation(MMIDCanvas* aCanvas, jint* aPositionArray, jint* aSizeArray)
{
    TSize size;
    TPoint position;
    TBool indicatorExists = aCanvas->NetworkIndicatorLocation(position, size);
    aPositionArray[0] = position.iX;
    aPositionArray[1] = position.iY;
    aSizeArray[0] = size.iWidth;
    aSizeArray[1] = size.iHeight;
    return indicatorExists;
}

JNIEXPORT jboolean JNICALL Java_javax_microedition_lcdui_Canvas__1getNetworkIndicatorLocation
(JNIEnv* aEnv,jobject, jint aToolkit, jint aCanvas, jintArray aPosition, jintArray aSize)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDCanvas*  canvas  = MIDUnhand<MMIDCanvas>(aCanvas);
    jint positionArray[2];
    jint sizeArray[2];
    jboolean indicatorExists = toolkit->Execute(&InvokeNetworkIndicatorLocation, canvas, positionArray, sizeArray);
    aEnv->SetIntArrayRegion(aPosition, 0, 2, positionArray);
    aEnv->SetIntArrayRegion(aSize,     0, 2, sizeArray);
    return indicatorExists;
}


