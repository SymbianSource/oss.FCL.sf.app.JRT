/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JNI class for DirectGraphicsImpl.java.
*
*/


#include "CMIDGraphics.h"
#include "CMIDImage.h"

#include "javax_microedition_lcdui_DirectGraphicsImpl.h"
#include "CMIDToolkit.h"
#include "CMIDBufferProcessor.h"
#include "CMIDDirectGraphics.h"
#include "TMIDBitmapParameters.h"
#include "CMIDConvertFactory.h"

// Cleanup method for ensuring that bitmaputil is ended
LOCAL_C void EndBitmapUtil(TAny* aBitmapUtil)
{
    ((TBitmapUtil*)aBitmapUtil)->End();
}

// wrapper for calling draw or get bitmap methods
void DirectGraphicsCallL(CMIDDirectGraphics* aDirectGraphics,
                         void(CMIDDirectGraphics::*aFunc)(TMIDBitmapParameters* aParams),
                         TMIDBitmapParameters* aParams,
                         CMIDBufferProcessor* aBufferProcessor)
{
    // locking bitmap, just to be sure that data address does not change
    TBitmapUtil bitmapUtil(aBufferProcessor->iGraphics->Bitmap());
    CleanupStack::PushL(TCleanupItem(&EndBitmapUtil,
                                     &bitmapUtil));
    bitmapUtil.Begin(TPoint(0, 0));

    (aDirectGraphics->*aFunc)(aParams);

    CleanupStack::Pop();  // CSI: 12 TCleanupItem parameter cannot be used with Pop #
    bitmapUtil.End();
}
/*
 * Class:     javax_microedition_lcdui_DirectGraphicsImpl
 * Method:    _createBufferProcessor
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_DirectGraphicsImpl__1createBufferProcessor(
    JNIEnv*, jobject, jint aToolkit, jint aGraphics, jint aImage)
{
    CMIDGraphics* graphics = MIDUnhand<CMIDGraphics>(aGraphics);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    MMIDImage* image = MIDUnhandObject< MMIDImage >(aImage);
    TInt handle = 0;
    TInt err = toolkit->ExecuteTrap(&CMIDBufferProcessor::NewL, toolkit,
                                    &handle, graphics, image);
    return err == KErrNone ? handle : err;
}

/*
   * Class:     javax_microedition_lcdui_DirectGraphicsImpl
   * Method:    _drawBitmapInt
   * Signature: ([IZIIIIIIII)I
   */
JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_DirectGraphicsImpl__1drawBitmapInt(
    JNIEnv* aJni, jobject, jintArray aPixels, jboolean aTransparency,
    jint aOffset, jint aScanlength,
    jint aX, jint aY, jint aWidth, jint aHeight, jint aManipulation,
    jint aFormat, jint aBufferProcessorHandle, jint aToolkit)
{
    jint* pixels = aJni->GetIntArrayElements(aPixels, 0);
    jint size = aJni->GetArrayLength(aPixels);
    size *= sizeof(jint);   // jint is 32bit and contains 4 bytes


    TMIDBitmapParameters params;
    params.iPixels = (TUint32*)pixels;
    params.iPixelsSize = size;
    params.iFormat = aFormat;
    params.iManipulation = aManipulation;
    params.iX = aX;
    params.iY = aY;
    params.iWidth = aWidth;
    params.iHeight = aHeight;
    params.iScanLength = aScanlength;
    params.iOffset = aOffset;
    params.iTransparency = aTransparency & (aFormat == KMIDTypeInt8888ARGB);

    CMIDBufferProcessor* bufferProcessor =
        MIDUnhand<CMIDBufferProcessor>(aBufferProcessorHandle);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    TInt retval = toolkit->ExecuteTrap(&DirectGraphicsCallL,
                                       bufferProcessor->iDirectGraphics,
                                       &CMIDDirectGraphics::DrawBitmapL,
                                       &params,
                                       bufferProcessor);

    aJni->ReleaseIntArrayElements(aPixels, pixels, JNI_ABORT);
    return retval;
}

/*
 * Class:     javax_microedition_lcdui_DirectGraphicsImpl
 * Method:    _drawBitmapByte
 * Signature: ([B[BIIIIIIII)I
 */
JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_DirectGraphicsImpl__1drawBitmapByte
(JNIEnv* aJni, jobject, jbyteArray aPixels, jbyteArray aTransparencyMask,
 jint aOffset, jint aScanlength,
 jint aX, jint aY, jint aWidth, jint aHeight, jint aManipulation,
 jint aFormat, jint aBufferProcessorHandle, jint aToolkit)
{
    jbyte* pixels = aJni->GetByteArrayElements(aPixels, 0);

    jbyte* transparency = NULL;
    jint sizeTrans = 0;
    if (aTransparencyMask)
    {
        transparency = aJni->GetByteArrayElements(aTransparencyMask, 0);
        sizeTrans = aJni->GetArrayLength(aTransparencyMask);
    }
    jint size = aJni->GetArrayLength(aPixels);

    TMIDBitmapParameters params;
    params.iPixels = (TUint32*)pixels;
    params.iPixelsSize = size;
    params.iAlpha = (TUint32*)transparency;
    params.iAlphaSize = sizeTrans;
    params.iFormat = aFormat;
    params.iManipulation = aManipulation;
    params.iX = aX;
    params.iY = aY;
    params.iWidth = aWidth;
    params.iHeight = aHeight;
    params.iScanLength = aScanlength;
    params.iOffset = aOffset;
    params.iTransparency = params.iAlpha ? ETrue : EFalse;

    CMIDBufferProcessor* bufferProcessor =
        MIDUnhand<CMIDBufferProcessor>(aBufferProcessorHandle);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);


    TInt retval = toolkit->ExecuteTrap(&DirectGraphicsCallL,
                                       bufferProcessor->iDirectGraphics,
                                       &CMIDDirectGraphics::DrawBitmapL,
                                       &params,
                                       bufferProcessor);

    aJni->ReleaseByteArrayElements(aPixels, pixels, JNI_ABORT);
    if (aTransparencyMask)
    {
        aJni->ReleaseByteArrayElements(aTransparencyMask, transparency,
                                       JNI_ABORT);
    }
    return retval;
}

/*
 * Class:     javax_microedition_lcdui_DirectGraphicsImpl
 * Method:    _drawBitmapShort
 * Signature: ([SZIIIIIIII)I
 */
JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_DirectGraphicsImpl__1drawBitmapShort
(JNIEnv* aJni, jobject, jshortArray aPixels, jboolean aTransparency,
 jint aOffset, jint aScanlength,
 jint aX, jint aY, jint aWidth, jint aHeight, jint aManipulation,
 jint aFormat, jint aBufferProcessorHandle, jint aToolkit)
{
    jshort* pixels = aJni->GetShortArrayElements(aPixels, 0);
    jint size = aJni->GetArrayLength(aPixels);
    size *= sizeof(jshort);   // short is 16 bit and contains two bytes

    TMIDBitmapParameters params;
    params.iPixels = (TUint32*)pixels;
    params.iPixelsSize = size;
    params.iFormat = aFormat;
    params.iManipulation = aManipulation;
    params.iX = aX;
    params.iY = aY;
    params.iWidth = aWidth;
    params.iHeight = aHeight;
    params.iScanLength = aScanlength;
    params.iOffset = aOffset;
    params.iTransparency = aTransparency
                           & (aFormat == KMIDTypeUshort4444ARGB);

    CMIDBufferProcessor* bufferProcessor =
        MIDUnhand<CMIDBufferProcessor>(aBufferProcessorHandle);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    TInt retval = toolkit->ExecuteTrap(&DirectGraphicsCallL,
                                       bufferProcessor->iDirectGraphics,
                                       &CMIDDirectGraphics::DrawBitmapL,
                                       &params,
                                       bufferProcessor);

    aJni->ReleaseShortArrayElements(aPixels, pixels, JNI_ABORT);
    return retval;
}

// For getting native pixel format
TInt DirectGraphicsGetNativePixelFormat(CMIDDirectGraphics* aDirectGraphics,
                                        TInt(CMIDDirectGraphics::*aFunc)())
{
    return (aDirectGraphics->*aFunc)();
}

/*
 * Class:     javax_microedition_lcdui_DirectGraphicsImpl
 * Method:    _getPixelsInt
 * Signature: ([IIIIIIII)I
 */
JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_DirectGraphicsImpl__1getPixelsInt
(JNIEnv* aJni, jobject, jintArray aPixels, jint aOffset, jint aScanlength,
 jint aX, jint aY,
 jint aWidth, jint aHeight, jint aFormat, jint aBufferProcessorHandle,
 jint aToolkit)
{
    jint* pixels = aJni->GetIntArrayElements(aPixels, 0);
    jint size = aJni->GetArrayLength(aPixels);
    size *= sizeof(jint);   // jint is 32bit and contains 4 bytes

    TMIDBitmapParameters params;
    params.iPixels = (TUint32*)pixels;
    params.iPixelsSize = size;
    params.iFormat = aFormat;
    params.iX = aX;
    params.iY = aY;
    params.iWidth = aWidth;
    params.iHeight = aHeight;
    params.iScanLength = aScanlength;
    params.iOffset = aOffset;

    CMIDBufferProcessor* bufferProcessor =
        MIDUnhand<CMIDBufferProcessor>(aBufferProcessorHandle);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    TInt retval = toolkit->ExecuteTrap(&DirectGraphicsCallL,
                                       bufferProcessor->iDirectGraphics,
                                       &CMIDDirectGraphics::GetBitmapL,
                                       &params,
                                       bufferProcessor);

    aJni->ReleaseIntArrayElements(aPixels, pixels, NULL);
    return retval;
}

/*
 * Class:     javax_microedition_lcdui_DirectGraphicsImpl
 * Method:    _getPixelsByte
 * Signature: ([BIIIIIII)I
 */
JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_DirectGraphicsImpl__1getPixelsByte
(JNIEnv* aJni, jobject, jbyteArray aPixels, jbyteArray aTransparencyMask,
 jint aOffset, jint aScanlength, jint aX, jint aY,
 jint aWidth, jint aHeight, jint aFormat, jint aBufferProcessorHandle,
 jint aToolkit)
{

    jbyte* pixels = aJni->GetByteArrayElements(aPixels, NULL);
    jint size = aJni->GetArrayLength(aPixels);   // byte is 8bit

    jbyte* transparency = NULL;
    jint sizeTrans = 0;

    if (aTransparencyMask)
    {
        transparency = aJni->GetByteArrayElements(aTransparencyMask, NULL);
        sizeTrans = aJni->GetArrayLength(aTransparencyMask);
    }

    TMIDBitmapParameters params;
    params.iPixels = (TUint32*)pixels;
    params.iPixelsSize = size;
    params.iAlpha = (TUint32*)transparency;
    params.iAlphaSize = sizeTrans;
    params.iFormat = aFormat;
    params.iX = aX;
    params.iY = aY;
    params.iWidth = aWidth;
    params.iHeight = aHeight;
    params.iScanLength = aScanlength;
    params.iOffset = aOffset;

    CMIDBufferProcessor* bufferProcessor =
        MIDUnhand<CMIDBufferProcessor>(aBufferProcessorHandle);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    TInt retval = toolkit->ExecuteTrap(&DirectGraphicsCallL,
                                       bufferProcessor->iDirectGraphics,
                                       &CMIDDirectGraphics::GetBitmapL,
                                       &params,
                                       bufferProcessor);

    aJni->ReleaseByteArrayElements(aPixels, pixels, NULL);

    if (aTransparencyMask)
    {
        aJni->ReleaseByteArrayElements(aTransparencyMask, transparency,
                                       NULL);
    }

    return retval;
}

/*
 * Class:     javax_microedition_lcdui_DirectGraphicsImpl
 * Method:    _getPixelsShort
 * Signature: ([SIIIIIII)I
 */
JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_DirectGraphicsImpl__1getPixelsShort
(JNIEnv* aJni, jobject, jshortArray aPixels, jint aOffset, jint aScanlength,
 jint aX, jint aY,
 jint aWidth, jint aHeight, jint aFormat, jint aBufferProcessorHandle,
 jint aToolkit)
{
    jshort* pixels = aJni->GetShortArrayElements(aPixels, 0);
    jint size = aJni->GetArrayLength(aPixels);
    size *= sizeof(jshort);   // size is number of 8bit elements in array

    TMIDBitmapParameters params;
    params.iPixels = (TUint32*)pixels;
    params.iPixelsSize = size;
    params.iFormat = aFormat;
    params.iX = aX;
    params.iY = aY;
    params.iWidth = aWidth;
    params.iHeight = aHeight;
    params.iScanLength = aScanlength;
    params.iOffset = aOffset;

    CMIDBufferProcessor* bufferProcessor =
        MIDUnhand<CMIDBufferProcessor>(aBufferProcessorHandle);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    TInt retval = toolkit->ExecuteTrap(&DirectGraphicsCallL,
                                       bufferProcessor->iDirectGraphics,
                                       &CMIDDirectGraphics::GetBitmapL,
                                       &params,
                                       bufferProcessor);

    aJni->ReleaseShortArrayElements(aPixels, pixels, NULL);
    return retval;
}


// For color setting
void DirectGraphicsSetColorL(CMIDDirectGraphics* aDirectGraphics,
                             void(CMIDDirectGraphics::*aFunc)(TUint32 aColor, TInt aImageHandle),
                             TUint32 aColor, TInt aImageHandle)
{
    (aDirectGraphics->*aFunc)(aColor, aImageHandle);
}
/*
 * Class:     javax_microedition_lcdui_DirectGraphicsImpl
 * Method:    _setColor
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_javax_microedition_lcdui_DirectGraphicsImpl__1setColor
(JNIEnv *, jobject, jint aColor, jint aBufferProcessor, jint aToolkit,
 jint aImageHandle)
{
    CMIDBufferProcessor* bufferProcessor =
        MIDUnhand<CMIDBufferProcessor>(aBufferProcessor);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    TInt retval = toolkit->ExecuteTrap(&DirectGraphicsSetColorL,
                                       bufferProcessor->iDirectGraphics,
                                       &CMIDDirectGraphics::SetColorL,
                                       (TUint32)aColor,
                                       (TInt) aImageHandle);

    return retval;
}

/*
 * Class:     javax_microedition_lcdui_DirectGraphicsImpl
 * Method:    _getNativePixelFormat
 * Signature: (I)I
 */
JNIEXPORT
jint JNICALL Java_javax_microedition_lcdui_DirectGraphicsImpl__1getNativePixelFormat
(JNIEnv*, jobject, jint aBufferProcessorHandle, jint aToolkit)
{
    CMIDBufferProcessor* bufferProcessor =
        MIDUnhand<CMIDBufferProcessor>(aBufferProcessorHandle);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);
    TInt retval = toolkit->Execute(&DirectGraphicsGetNativePixelFormat,
                                   bufferProcessor->iDirectGraphics,
                                   &CMIDDirectGraphics::GetNativePixelFormat);
    return retval;
}


// For getting native pixel format
void DirectGraphicsDrawPolygonL(CMIDDirectGraphics* aDirectGraphics,
                                void(CMIDDirectGraphics::*aFunc)(CMIDDirectGraphics::TPolygonParameters* aParams),
                                CMIDDirectGraphics::TPolygonParameters* aParams ,
                                CMIDBufferProcessor* aBufferProcessor)
{
    // locking bitmap, just to be sure that data address does not change
    TBitmapUtil bitmapUtil(aBufferProcessor->iGraphics->Bitmap());
    CleanupStack::PushL(TCleanupItem(&EndBitmapUtil,
                                     &bitmapUtil));
    bitmapUtil.Begin(TPoint(0, 0));

    (aDirectGraphics->*aFunc)(aParams);

    CleanupStack::Pop();  // CSI: 12 TCleanupItem parameter cannot be used with Pop #
    bitmapUtil.End();
}

/*
 * Class:       javax_microedition_lcdui_DirectGraphicsImpl
 * Method:      _drawPolygon
 * Signature:   ([II[IIIIIZ)I
 */
JNIEXPORT jint JNICALL
Java_javax_microedition_lcdui_DirectGraphicsImpl__1drawPolygon(
    JNIEnv* aJni, jobject, jintArray aXPoints, jint aXOffset, jint aTransX,
    jintArray aYPoints, jint aYOffset, jint aTransY,
    jint aNPoints, jint aARGBColor, jint aBufferProcessorHandle,
    jboolean aFill, jint aToolkit)
{
    jint* xPoints = aJni->GetIntArrayElements(aXPoints, 0);
    jint xSize = aJni->GetArrayLength(aXPoints);

    jint* yPoints = aJni->GetIntArrayElements(aYPoints, 0);
    jint ySize = aJni->GetArrayLength(aYPoints);

    CMIDBufferProcessor* bufferProcessor =
        MIDUnhand<CMIDBufferProcessor>(aBufferProcessorHandle);
    CMIDToolkit* toolkit = JavaUnhand<CMIDToolkit>(aToolkit);

    CMIDDirectGraphics::TPolygonParameters polygonParameters =
    {
        xPoints, xSize, aXOffset, aTransX, yPoints, ySize, aYOffset, aTransY,
        aNPoints, aARGBColor, aFill
    };

    TInt retval = toolkit->ExecuteTrap(&DirectGraphicsDrawPolygonL,
                                       bufferProcessor->iDirectGraphics,
                                       &CMIDDirectGraphics::DrawPolygonL,
                                       &polygonParameters,
                                       bufferProcessor);
    aJni->ReleaseIntArrayElements(aXPoints, xPoints, JNI_ABORT);
    aJni->ReleaseIntArrayElements(aYPoints, yPoints, JNI_ABORT);
    return retval;
}
