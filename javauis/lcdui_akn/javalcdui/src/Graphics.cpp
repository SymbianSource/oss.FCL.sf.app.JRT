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

#include <e32def.h> // MAKE_TINT64
#include "javax_microedition_lcdui_Graphics.h"
#include "CMIDToolkit.h"
#include "CMIDBuffer.h"
#include "MIDUtils.h"
#include <stdio.h>


LOCAL_C void CreateL(CMIDToolkit* aToolkit,TInt* aHandle, MMIDComponent* aComponent)
{
    MMIDGraphicsFactory& factory = aToolkit->GraphicsFactory();

    MMIDGraphics* graphics = NULL;
    switch (aComponent->Type())
    {
    case MMIDComponent::EImage:
    {
        graphics = factory.NewGraphicsL((MMIDImage*)aComponent);
    }
    break;

    case MMIDComponent::ECustomItem:
    {
        graphics = factory.NewGraphicsL((MMIDCustomItem*)aComponent);
    }
    break;

    case MMIDComponent::ECanvas:
    {
        graphics = factory.NewGraphicsL((MMIDCanvas*)aComponent);
    }
    break;

    case MMIDComponent::ECanvasGraphicsItemPainter:
    {
        graphics = factory.NewGraphicsL((MMIDCanvasGraphicsItemPainter*)aComponent);
    }
    break;

    default:
        break;
    }
    ASSERT(graphics);

    CleanupDisposePushL(graphics);
    *aHandle = aToolkit->RegisterComponentL(graphics, NULL);
    CleanupPopComponent(graphics);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Graphics__1create
(JNIEnv*, jobject, jint aToolkit, jint aDrawable)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    TInt handle=0;
    MMIDComponent* drawable = MIDUnhandObject<MMIDComponent>(aDrawable);
    TInt err = toolkit->ExecuteTrap(&CreateL, toolkit, &handle, drawable);
    return err == KErrNone ? handle : err;
}


LOCAL_C TInt InvokeDisplayColor(MMIDGraphics* aGraphics, TUint32 aColor)
{
    TUint32 color = aGraphics->DisplayColor(aColor);
    return TInt(color);
}
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Graphics__1getDisplayColor
(
    JNIEnv* /* aEnv */,
    jclass  /* aCls */,
    jint    aToolkit,
    jint    aGraphics,
    jint    aColor
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDGraphics* graphics = MIDUnhand<MMIDGraphics>(aGraphics);
#ifdef _TRACE
    TInt red   = (aColor >> 16) & 0xff;
    TInt green = (aColor >> 8) & 0xff;
    TInt blue  = (aColor) & 0xff;
    RDebug::Print(_L("getDisplayColor(%d,%d,%d)"), red, green, blue);
#endif
    return toolkit->Execute(InvokeDisplayColor, graphics, (TUint32)aColor);
}

struct TDrawPixels
{
    TInt    iType;
    TUint8* iAddress;       // pixel array
    TInt    iLength;        // in bytes
    TInt    iScanLength;    // in bytes
    TBool   iAlpha;
    TRect   iRect;
    TInt    iTransform;
};

LOCAL_C TInt InvokeDrawPixels(MMIDGraphics* aGraphics, const TDrawPixels* aDrawPixels)
{
    return aGraphics->DrawPixels(
               aDrawPixels->iType,
               aDrawPixels->iAddress,
               aDrawPixels->iLength,
               aDrawPixels->iScanLength,
               aDrawPixels->iAlpha,
               aDrawPixels->iRect,
               aDrawPixels->iTransform
           );
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Graphics__1drawPixels
(
    JNIEnv*   aEnv,
    jclass    /* aClass */,
    jint      aBuffer,
    jint      aHandle,
    jint      aType,
    jintArray aRgbData,
    jint      aOffset,
    jint      aLength,
    jint      aScanLength,
    jboolean  aProcessAlpha,
    jint      aX,
    jint      aY,
    jint      aWidth,
    jint      aHeight,
    jint      aTransform
)
{
    CMIDBuffer*   buffer   = JavaUnhand<CMIDBuffer>(aBuffer);
    MMIDGraphics* graphics = MIDUnhand<MMIDGraphics>(aHandle);

    TInt  err = KErrNoMemory;
    TInt  length = aEnv->GetArrayLength(aRgbData);
    jint* array  = aEnv->GetIntArrayElements(aRgbData, NULL);
    if (array)
    {
        TDrawPixels drawPixels;

        drawPixels.iType       = aType;
        drawPixels.iAddress    = (TUint8*)(array + aOffset);
        drawPixels.iLength     = aLength*sizeof(jint);
        drawPixels.iScanLength = aScanLength*sizeof(jint);
        drawPixels.iAlpha      = aProcessAlpha;
        drawPixels.iRect.iTl.iX=aX;
        drawPixels.iRect.iTl.iY=aY;
        drawPixels.iRect.iBr.iX=aX+aWidth;
        drawPixels.iRect.iBr.iY=aY+aHeight;
        drawPixels.iTransform  = aTransform;

        err = buffer->Execute(&InvokeDrawPixels, graphics, &const_cast<const TDrawPixels&>(drawPixels));

        aEnv->ReleaseIntArrayElements(aRgbData, array, JNI_ABORT);  // don't copy back
    }

    return err;
}
