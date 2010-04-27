/*
* Copyright (c) 1999-2002 Nokia Corporation and/or its subsidiary(-ies).
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
#include "javax_microedition_lcdui_Image.h"
#include "CMIDToolkit.h"
#include "CMIDImageLoader.h"
#include "MIDUtils.h"
#include "lcdgr.h"

inline MMIDImage* Image(jint aHandle)
{
    return MIDUnhand<MMIDImage>(aHandle);
}

struct TImageAttributes
{
    jint    iHandle;
    jint    iWidth;
    jint    iHeight;
    jint    iTransparency;
    jint    iSizeBytes;
};

TInt WordAlignedSize(const TSize& aSize, TInt aBpp)
{
    TInt scan = (((aSize.iWidth * aBpp) + 31) & ~31) >> 3;
    return (scan * aSize.iHeight);
}

LOCAL_C void GetAttribs(MMIDGraphicsFactory& /*aFactory*/, MMIDImage* aImage, TImageAttributes* aAttribs)
{
    TSize size = aImage->Size();
    TInt  type = aImage->TransparencyType();

    aAttribs->iWidth  = size.iWidth;
    aAttribs->iHeight = size.iHeight;
    aAttribs->iTransparency = type;


    TInt colorBpp = 32;
    TInt alphaBpp = 0;
    if (type == MMIDImage::EMask)
    {
        // assume 1bpp mask bitmap
        alphaBpp = 1;
    }
    else if (type == MMIDImage::EAlpha)
    {
        // assume 8bpp alpha bitmap
        alphaBpp = 8;
    }

    aAttribs->iSizeBytes = WordAlignedSize(size, colorBpp) + WordAlignedSize(size, alphaBpp);
}

LOCAL_C void SetAttribArray(JNIEnv* aJni, jintArray aArray, const TImageAttributes& aAttribs)
{
    ASSERT(aJni->GetArrayLength(aArray) == javax_microedition_lcdui_Image_ATTRIB_COUNT);
    TInt array[javax_microedition_lcdui_Image_ATTRIB_COUNT];
    array[javax_microedition_lcdui_Image_ATTRIB_WIDTH] = aAttribs.iWidth;
    array[javax_microedition_lcdui_Image_ATTRIB_HEIGHT]= aAttribs.iHeight;
    array[javax_microedition_lcdui_Image_ATTRIB_TYPE]  = aAttribs.iTransparency;
    array[javax_microedition_lcdui_Image_ATTRIB_SIZE]  = aAttribs.iSizeBytes;
    aJni->SetIntArrayRegion(aArray, 0, javax_microedition_lcdui_Image_ATTRIB_COUNT, &(array[0]));
}

LOCAL_C void InvokeCreateMutableL(CMIDToolkit* aToolkit, TImageAttributes* aAttribs, jint aWidth, jint aHeight, jint aColor)
{
    MMIDGraphicsFactory& factory = aToolkit->GraphicsFactory();

    if (aColor != 0xffffffff)
    {
        User::Leave(KErrNotSupported);
    }

    TSize size(aWidth,aHeight);

    MMIDImage* image = factory.NewMutableImageL(size);
    CleanupDisposePushL(image);
    aAttribs->iHandle = aToolkit->RegisterComponentL(image, NULL);
    CleanupPopComponent(image);

    GetAttribs(factory, image, aAttribs);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Image__1createMutable
(
    JNIEnv* aJni,
    jobject,
    jint        aToolkit,
    jint        aWidth,
    jint        aHeight,
    jint        aColor,
    jintArray   aAttribs
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    TImageAttributes attribs;

    jint err = toolkit->ExecuteTrap(&InvokeCreateMutableL, toolkit, &attribs, aWidth, aHeight, aColor);
    if (0 == err)
    {
        SetAttribArray(aJni, aAttribs, attribs);
        return attribs.iHandle;
    }

    return err;
}

LOCAL_C void InvokeCreateFromLoaderL(CMIDToolkit* aToolkit, TImageAttributes* aAttribs, jint aLoader)
{
    MMIDGraphicsFactory& factory = aToolkit->GraphicsFactory();

    CMIDImageLoader* loader = JavaUnhand<CMIDImageLoader>(aLoader);
    MMIDImage* image = factory.NewImageL(loader->Decoder());
    CleanupDisposePushL(image);
    aAttribs->iHandle = aToolkit->RegisterComponentL(image, NULL);
    CleanupPopComponent(image);

    GetAttribs(factory, image, aAttribs);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Image__1createImmutable
(
    JNIEnv*   aJni,
    jobject,
    jint      aToolkit,
    jint      aLoader,
    jintArray aAttributes
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    TImageAttributes attribs;

    jint error = toolkit->ExecuteTrap(InvokeCreateFromLoaderL, toolkit, &attribs, aLoader);
    if (KErrNone == error)
    {
        SetAttribArray(aJni, aAttributes, attribs);
        return attribs.iHandle;
    }

    return error;
}

LOCAL_C void InvokeCreateFromCanvasL(CMIDToolkit* aToolkit, TImageAttributes* aAttribs, jint aCanvas)
{
    MMIDGraphicsFactory& factory = aToolkit->GraphicsFactory();
    MMIDImage* image = factory.NewImageL(MIDUnhand<MMIDCanvas>(aCanvas));
    CleanupDisposePushL(image);
    aAttribs->iHandle = aToolkit->RegisterComponentL(image, NULL);
    CleanupPopComponent(image);
    GetAttribs(factory, image, aAttribs);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Image__1createFromCanvas
(
    JNIEnv*   aJni,
    jobject,
    jint      aToolkit,
    jint      aCanvas,
    jintArray aAttributes
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    TImageAttributes attribs;

    jint error = toolkit->ExecuteTrap(InvokeCreateFromCanvasL, toolkit, &attribs, aCanvas);
    if (KErrNone == error)
    {
        SetAttribArray(aJni, aAttributes, attribs);
        return attribs.iHandle;
    }

    return error;
}


LOCAL_C void InvokeCreateRegionL(CMIDToolkit* aToolkit, TImageAttributes* aAttribs, jint aSource, const TRect* aRect, jint aTransform)
{
    MMIDGraphicsFactory& factory = aToolkit->GraphicsFactory();

    MMIDImage* source = Image(aSource);
    TSize size = aRect->Size();
    if (aTransform & 4)
    {
        TInt tmp = size.iWidth;
        size.iWidth  = size.iHeight;
        size.iHeight = tmp;
    }
    MMIDImage* image = factory.NewImageL(size, source->TransparencyType());
    CleanupDisposePushL(image);
    User::LeaveIfError(image->CopyRegion(source, *aRect, aTransform));
    aAttribs->iHandle = aToolkit->RegisterComponentL(image, NULL);
    CleanupPopComponent(image);
    GetAttribs(factory, image, aAttribs);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Image__1createRegion
(
    JNIEnv*   aJni,
    jobject,
    jint      aToolkit,
    jint      aSource,
    jint      aX,
    jint      aY,
    jint      aWidth,
    jint      aHeight,
    jint      aTransform,
    jintArray aAttributes
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    TImageAttributes attribs;
    const TRect rect(TPoint(aX,aY),TSize(aWidth,aHeight));

    jint error = toolkit->ExecuteTrap(InvokeCreateRegionL, toolkit, &attribs, aSource, &rect, aTransform);
    if (KErrNone == error)
    {
        SetAttribArray(aJni, aAttributes, attribs);
        return attribs.iHandle;
    }

    return error;
}

TInt GetTransparencyType(const jint* /*aPixels*/, jint /*aLength*/, jboolean aAlpha)
{
    return aAlpha ? MMIDImage::EAlpha : MMIDImage::ENone;
}

LOCAL_C void InvokeCreateRGBL(CMIDToolkit* aToolkit, TImageAttributes* aAttribs, jint* aArray, jint aWidth, jint aHeight, jboolean aAlpha)
{
    MMIDGraphicsFactory& factory = aToolkit->GraphicsFactory();

    TSize size(aWidth,aHeight);
    TRect rect(size);
    TInt  type = GetTransparencyType(aArray, aWidth*aHeight, aAlpha);
    MMIDImage* image = factory.NewImageL(size, type);

    CleanupDisposePushL(image);
    User::LeaveIfError(image->SetPixels((const TUint32*)aArray, aWidth*aHeight, 0, aWidth, rect, aAlpha));
    aAttribs->iHandle = aToolkit->RegisterComponentL(image, NULL);
    CleanupPopComponent(image);

    GetAttribs(factory, image, aAttribs);
}


JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Image__1createRGB
(
    JNIEnv*   aJni,
    jobject,
    jint      aToolkit,
    jintArray aPixels,
    jint      aWidth,
    jint      aHeight,
    jboolean  aAlpha,
    jintArray aAttributes
)
{
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    jint error = KErrNoMemory;

    jint  length = aJni->GetArrayLength(aPixels);
    ASSERT(length >= aWidth*aHeight);

    jint* pixels = (jint*)aJni->GetIntArrayElements(aPixels, NULL);
    if (pixels)
    {
        TImageAttributes attribs;
        error = toolkit->ExecuteTrap(InvokeCreateRGBL, toolkit, &attribs, pixels, aWidth, aHeight, aAlpha);
        aJni->ReleaseIntArrayElements(aPixels, pixels, JNI_ABORT);
        if (KErrNone == error)
        {
            SetAttribArray(aJni, aAttributes, attribs);
            return attribs.iHandle;
        }
    }

    return error;
}

LOCAL_C jint InvokeCopyRegion(jint aTarget, jint aSource, const TRect* aRect, jint aTransform)
{
    return Image(aTarget)->CopyRegion(Image(aSource), *aRect, aTransform);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Image__1copyRegion
(
    JNIEnv* /*aEnv*/,
    jobject,
    jint aToolkit,
    jint aTarget,
    jint aSource,
    jint aX,
    jint aY,
    jint aWidth,
    jint aHeight,
    jint aTransform
)
{
    const TRect srcRect(TPoint(aX,aY), TSize(aWidth, aHeight));
    return JavaUnhand<CMIDToolkit>(aToolkit)->Execute(&InvokeCopyRegion, aTarget, aSource, &srcRect, aTransform);
}

LOCAL_C jint InvokeSetPixels
(
    jint  aHandle,
    jint* aArray,
    jint  aLength,
    jint  aOffset,
    jint  aScanLength,
    const TRect* aRect,
    jboolean aAlpha
)
{
    return Image(aHandle)->SetPixels((const TUint32*)aArray, aLength, aOffset, aScanLength, *aRect, aAlpha);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Image__1setPixels
(
    JNIEnv* aEnv,
    jobject,
    jint aToolkit,
    jint aHandle,
    jintArray aArray,
    jint aOffset,
    jint aScanLength,
    jint aX,
    jint aY,
    jint aWidth,
    jint aHeight,
    jboolean aAlpha
)
{
    const TRect rect(TPoint(aX,aY),TSize(aWidth,aHeight));

    jint err = KErrNoMemory;
    jint length = aEnv->GetArrayLength(aArray);
    jint* array = (jint*)aEnv->GetIntArrayElements(aArray, NULL);
    if (array)
    {
        err = JavaUnhand<CMIDToolkit>(aToolkit)->Execute(&InvokeSetPixels, aHandle, array, length, aOffset, aScanLength, &rect, aAlpha);
        aEnv->ReleaseIntArrayElements(aArray, array, JNI_ABORT);
    }

    return err;
}

LOCAL_C jint InvokeGetPixels
(
    jint  aHandle,
    jint* aArray,
    jint  aLength,
    jint  aOffset,
    jint  aScanLength,
    const TRect* aRect
)
{
    return Image(aHandle)->GetPixels((TUint32*)aArray, aLength, aOffset, aScanLength, *aRect);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Image__1getPixels
(
    JNIEnv* aEnv,
    jobject,
    jint aToolkit,
    jint aHandle,
    jintArray aArray,
    jint aOffset,
    jint aScanLength,
    jint aX,
    jint aY,
    jint aWidth,
    jint aHeight
)
{
    const TRect rect(TPoint(aX,aY),TSize(aWidth,aHeight));
    jint err = KErrNoMemory;
    jint length = aEnv->GetArrayLength(aArray);
    jint* array = (jint*)aEnv->GetIntArrayElements(aArray, NULL);
    if (array)
    {
        err = JavaUnhand<CMIDToolkit>(aToolkit)->Execute(&InvokeGetPixels, aHandle, array, length, aOffset, aScanLength, &rect);
        aEnv->ReleaseIntArrayElements(aArray, array, 0);
    }

    return err;
}


LOCAL_C TInt InvokeDetectCollision(TInt aImageHandle1, const TRect* aRect1, TInt aTransform1,const TPoint* aPoint1,
                                   TInt aImageHandle2, const TRect* aRect2, TInt aTransform2,const TPoint* aPoint2)
{
    MMIDImage*   image1   = MIDUnhand<MMIDImage>(aImageHandle1);
    MMIDImage*   image2   = MIDUnhand<MMIDImage>(aImageHandle2);
    return image1->DetectCollision(*aRect1,aTransform1,*aPoint1,image2,*aRect2,aTransform2,*aPoint2);
}

JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_Image__1collision
(
    JNIEnv*,
    jclass,
    jint aToolkit,
    jint aImage1,
    jint aS1X1,
    jint aS1Y1,
    jint aS1X2,
    jint aS1Y2,
    jint aTransform1,
    jint aD1X1,
    jint aD1Y1,
    jint aImage2,
    jint aS2X1,
    jint aS2Y1,
    jint aS2X2,
    jint aS2Y2,
    jint aTransform2,
    jint aD2X1,
    jint aD2Y1)
{
    const TRect rect1(TPoint(aS1X1,aS1Y1),TPoint(aS1X2,aS1Y2));
    const TRect rect2(TPoint(aS2X1,aS2Y1),TPoint(aS2X2,aS2Y2));
    const TPoint point1(TPoint(aD1X1,aD1Y1));
    const TPoint point2(TPoint(aD2X1,aD2Y1));
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    return toolkit->Execute(InvokeDetectCollision, (TInt)aImage1, &rect1, (TInt)aTransform1, &point1,(TInt)aImage2, &rect2, (TInt)aTransform2, &point2);
}

jint InvokeSetTransparencyType(jint aImage, MMIDImage::TTransparencyType aType)
{
    return MIDUnhand<MMIDImage>(aImage)->SetTransparencyType(aType);
}


JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_Image__1setTransparencyType
(
    JNIEnv*,
    jobject /*aImageRef*/,
    jint    aToolkitHandle,
    jint    aImageHandle,
    jint    aTransparencyType
)
{
    if ((aTransparencyType < 0) || (aTransparencyType > MMIDImage::EAlpha))
    {
        return KErrArgument;
    }
    MMIDImage::TTransparencyType type = (MMIDImage::TTransparencyType)aTransparencyType;
    return JavaUnhand<CMIDToolkit>(aToolkitHandle)->Execute(InvokeSetTransparencyType, aImageHandle, type);
}
