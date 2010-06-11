/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial API and implementation
 *******************************************************************************/
#include <QFont>
#include <QFontMetrics>
#include <QSharedDataPointer>

#include <org_eclipse_swt_internal_qt_graphics_OS.h>
#include "graphics.h"
#include "gfxlog.h"
#include "jniutils.h"
#include "swtapplication.h"
#include "autorelease.h"
#include "gfxutils.h"

using namespace Java::GFX;

#define POINTER_TO_HANDLE(pointer) reinterpret_cast<jint>( pointer )
#define HANDLE_TO_POINTER(type, variable, handle) type variable = reinterpret_cast<type>( handle )
#define ARRAY_CAST(type, variable) reinterpret_cast<type>( variable )

#define GFX_TRY try
#define GFX_CATCH \
catch(std::bad_alloc const&) \
    { \
        GfxException e(EGfxErrorNoMemory, "Bad alloc"); \
        swtApp->jniUtils().Throw(aJniEnv, e); \
    } \
catch(GfxException e) \
    { \
        swtApp->jniUtils().Throw(aJniEnv, e); \
    }


jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1get_1windowsurface
  (JNIEnv* aJniEnv, jclass, jint aHandle) 
{   
    WindowSurface* ws = NULL;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        ws = gc->getWindowSurface();
    }
    GFX_CATCH
    
    return POINTER_TO_HANDLE(ws);
}

// Creates a new instance of native graphics context (gc)
jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1init
  (JNIEnv* aJniEnv , jclass)
{
    GraphicsContext* gc = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        gc = GraphicsFactory::createGraphicsContext();
    }
    GFX_CATCH

    return POINTER_TO_HANDLE(gc);
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1dispose
  (JNIEnv* aJniEnv , jclass, jint aHandle) 
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);  
        gc->dispose();
        gc = NULL;
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1bindTarget
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aTarget, jint aType, jint aBufferFlushTargetHandle)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->bindTarget(aTarget, (TTargetType)aType, static_cast<int>(aBufferFlushTargetHandle));
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1render
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aBufferHandle)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        HANDLE_TO_POINTER(Buffer*, buffer, aBufferHandle);
        gc->render(buffer);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1releaseTarget
  (JNIEnv* aJniEnv , jclass, jint aHandle) 
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->releaseTarget();
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1copyArea__IIII
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aTargetHandle, jint aX, jint aY)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        HANDLE_TO_POINTER(Image*, image, aTargetHandle);
    
        gc->copyArea(image, aX, aY);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1copyArea__IIIIIIIZ
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aSrcX, jint aSrcY, jint aWidth, jint aHeight, jint aDestX, jint aDestY, jboolean aPaint)
{   
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->copyArea(aSrcX, aSrcY, aWidth, aHeight, aDestX, aDestY, aPaint == JNI_TRUE ? true : false);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawArc
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight, jint aStartAngle, jint aArcAngle)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->drawArc(aX, aY, aWidth, aHeight, aStartAngle, aArcAngle);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawEllipse
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->drawEllipse(aX, aY, aWidth, aHeight);
    }
    GFX_CATCH
}

JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawFocus
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->drawFocus(aX, aY, aWidth, aHeight);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawImage__IIII
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aImageHandle, jint aX, jint aY)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        HANDLE_TO_POINTER(Image*, image, aImageHandle);
        gc->drawImage(image, aX, aY);
    }
    GFX_CATCH
}
                                                                                      
void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawImage__IIIIIIIIIII
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aImageHandle, jint aTx, jint aTy, jint aTw, jint aTh, 
  jint aSx, jint aSy, jint aSw, jint aSh, jint aManipulation)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        HANDLE_TO_POINTER(Image*, image, aImageHandle);
        gc->drawImage(image, aManipulation, aTx, aTy, aTw, aTh, aSx, aSy, aSw, aSh);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawLine
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX1, jint aY1, jint aX2, jint aY2)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->drawLine(aX1, aY1, aX2, aY2);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawPoint
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->drawPoint(aX, aY);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawPolygon
  (JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aPointArray)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        int length = aJniEnv->GetArrayLength(aPointArray);
        int* buffer = new int[length]; // might throw bad_alloc
        AutoRelease<int> release(buffer, true);
        swtApp->jniUtils().GetJavaIntArrayRegionToIntArray(aJniEnv, aPointArray, 0, length, buffer); // might throw bad_alloc
        gc->drawPolygon(buffer, length);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawPolyline
  (JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aPointArray)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        int length = aJniEnv->GetArrayLength(aPointArray);
        int* buffer = new int[length]; // might throw bad_alloc
        AutoRelease<int> release(buffer, true);
        swtApp->jniUtils().GetJavaIntArrayRegionToIntArray(aJniEnv, aPointArray, 0, length, buffer); // might throw bad_alloc
        gc->drawPolyline(buffer, length);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawRect
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->drawRect(aX, aY, aWidth, aHeight);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawRGB__I_3IIIIIIIZI
  (JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aRgbData, jint aOffset, jint aScanlength, 
    jint aX, jint aY, jint aWidth, jint aHeight, jboolean aProcessAlpha, jint aManipulation)
{
    GFX_TRY 
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        int length = aJniEnv->GetArrayLength(aRgbData);

        int* buffer = new int[length]; // might throw bad_alloc
        AutoRelease<int> release(buffer, true);
        swtApp->jniUtils().GetJavaIntArrayRegionToIntArray(aJniEnv, aRgbData, 0, length, buffer); // might throw bad_alloc

        gc->drawRGB(buffer, length, aOffset, aScanlength, aX, aY, aWidth, aHeight, aProcessAlpha, aManipulation);
    } 
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawRGB__I_3B_3BIIIIIIII
  (JNIEnv *aJniEnv, jclass, jint aHandle, jbyteArray aRgbData, jbyteArray aTransparencyMask, 
  jint aOffset, jint aScanLength, jint aX, jint aY, jint aWidth, jint aHeight, jint aTransform, jint aFormat)
  {
    GFX_TRY 
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        
        int dataLength = aJniEnv->GetArrayLength(aRgbData);
        int maskLength = aJniEnv->GetArrayLength(aTransparencyMask);
        
        char* dataBuffer = new char[dataLength]; // might throw bad_alloc
        AutoRelease<char> releaseData(dataBuffer, true);
        swtApp->jniUtils().GetJavaByteArrayRegionToCharArray(aJniEnv, aRgbData, 0, dataLength, dataBuffer); // might throw bad_alloc
        
        char* maskBuffer = NULL;
        AutoRelease<char> releaseMask(NULL, true); // Will delete mask or NULL if there's no mask
        
        if(aTransparencyMask)
        {
            maskBuffer = new char[maskLength]; // might throw bad_alloc
            releaseMask.reset(maskBuffer);
            swtApp->jniUtils().GetJavaByteArrayRegionToCharArray(aJniEnv, aTransparencyMask, 0, maskLength, maskBuffer); // might throw bad_alloc        
        }
        
        gc->drawRGB(dataBuffer, maskBuffer, dataLength, aOffset, aScanLength, aX, aY, aWidth, aHeight, aTransform, aFormat);
    }
    GFX_CATCH
  }

 void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawRGB__I_3SIIIIIIZII
  (JNIEnv * aJniEnv, jclass, jint aImageHandle, jshortArray aRgbData, jint aOffset, jint aScanlength,
   jint aX, jint aY, jint aWidth, jint aHeight, jboolean aProcessAlpha, jint aManipulation, jint aFormat)
 {
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aImageHandle);
        
        int length = aJniEnv->GetArrayLength(aRgbData);
        
        short* buffer = new short[length];
        AutoRelease<short> release(buffer, true);
        ::memset(buffer, 0, sizeof(short)*length);
        
        swtApp->jniUtils().GetJavaShortArrayRegionToShortArray(aJniEnv, aRgbData, 0, length, buffer); // might throw bad_alloc
        
        gc->drawRGB(buffer, length, aOffset, aScanlength, aX, aY, aWidth, aHeight, aProcessAlpha, aManipulation, aFormat);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawRoundRect
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight, jint aArcWidth, jint aArcHeight)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->drawRoundRect(aX, aY, aWidth, aHeight, aArcWidth, aArcHeight);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawString
  (JNIEnv* aJniEnv, jclass, jint aHandle, jstring aText, jint aX, jint aY, jint aWidth, jint aHeight,
  jint aAlignments, jint aFlags, jboolean aIsTransparent) 
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        const jchar* chars; // unsigned short (2 bytes) 
        int length;
    
        length = aJniEnv->GetStringLength(aText);
        chars = aJniEnv->GetStringChars(aText, NULL);
        AutoReleaseStringChars release(aJniEnv, aText, chars);
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->drawString(
            (const unsigned short*)chars, 
            aX, 
            aY,
            aWidth,
            aHeight,
            length, 
            aAlignments,
            aFlags,
            aIsTransparent);
    }
    GFX_CATCH
}

JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1drawWindowSurface
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aSurfaceHandle, jint aX, jint aY, jint aWidth, jint aHeight) 
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        HANDLE_TO_POINTER(WindowSurface*, surface, aSurfaceHandle);
        gc->drawWindowSurface(surface, aX, aY, aWidth, aHeight);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1fillArc
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight, jint aStartAngle, jint aArcAngle)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->fillArc(aX, aY, aWidth, aHeight, aStartAngle, aArcAngle);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1fillEllipse
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight) 
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->fillEllipse(aX, aY, aWidth, aHeight);   
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1fillGradientRect
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight, jboolean aVertical, jboolean aSwapColors)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->fillGradientRect(aX, aY, aWidth, aHeight, aVertical, aSwapColors);  
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1fillPolygon
  (JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aPointArray)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        int length = aJniEnv->GetArrayLength(aPointArray);
        
        int* buffer = new int[length]; // might throw bad_alloc
        AutoRelease<int> release(buffer, true);
        swtApp->jniUtils().GetJavaIntArrayRegionToIntArray(aJniEnv, aPointArray, 0, length, buffer); // might throw bad_alloc
        
        gc->fillPolygon(buffer, length);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1fillRect
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->fillRect(aX, aY, aWidth, aHeight);  
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1fillRoundRect
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight, jint aArcWidth, jint aArcHeight)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->fillRoundRect(aX, aY, aWidth, aHeight, aArcWidth, aArcHeight);
    }
    GFX_CATCH
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getBackgroundAlpha
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    jint bgAlpha = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        bgAlpha = static_cast<jint>( gc->getBackgroundAlpha() );
    }
    GFX_CATCH
    return bgAlpha;
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getBackgroundColor
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    jint bgColor = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        bgColor = static_cast<jint>( gc->getBackgroundColor() );
    }
    GFX_CATCH
    return bgColor;
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getCharacterWidth
  (JNIEnv* aJniEnv , jclass, jint aHandle, jchar aCh, jboolean aIsAdvanced)
{
    jint charWidth = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        charWidth = static_cast<jint>( gc->getCharacterWidth(aCh, aIsAdvanced) );
    }
    GFX_CATCH
    return charWidth;
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getFontMetricsData
  (JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aDataArray, jint aFontHandle)
{
    GFX_TRY 
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);

        int length = aJniEnv->GetArrayLength(aDataArray);

        int* buffer = new int[length]; // might throw bad_alloc
        AutoRelease<int> release(buffer, true);
        ::memset(buffer, 0, sizeof(int)*length);

        if (aFontHandle)
        {
            HANDLE_TO_POINTER(QFont*, font, aFontHandle);
            gc->getFontMetricsData(buffer, *font);
        }
        else
        {
            QFont font;
            gc->getFontMetricsData(buffer, font);
        }
        // Copy back changes to the Java array
        swtApp->jniUtils().SetJavaIntArrayRegionFromIntArray(aJniEnv, aDataArray, 0, length, buffer);
    }
    GFX_CATCH
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getBlendingMode
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    jint blendingMode = 0;
    GFX_TRY 
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        blendingMode = static_cast<jint>( gc->getBlendingMode() );
    }
    GFX_CATCH
    return blendingMode;
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getClip
  (JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aClipArray)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);        
        int length = aJniEnv->GetArrayLength(aClipArray);

        int* buffer = new int[length]; // might throw bad_alloc
        AutoRelease<int> release(buffer, true);
        ::memset(buffer, 0, sizeof(int)*length);
        
        gc->getClip(buffer);
        
        swtApp->jniUtils().SetJavaIntArrayRegionFromIntArray(aJniEnv, aClipArray, 0, length, buffer);
    }
    GFX_CATCH
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getForegroundAlpha
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    jint fgAlpha = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        fgAlpha = static_cast<jint>( gc->getForegroundAlpha() );
    }
    GFX_CATCH
    return fgAlpha;
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getForegroundColor
  (JNIEnv* aJniEnv , jclass, jint aHandle) 
{
    jint fgColor = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        fgColor = static_cast<jint>( gc->getForegroundColor() );
    }
    GFX_CATCH
    return fgColor;
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getTextBoundingBox
  (JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aBoundingBox, jstring aText, 
   jint aAlignments, jint aFlags, jint aRectX, jint aRectY, jint aRectWidth, jint aRectHeight)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        
        int length = aJniEnv->GetArrayLength(aBoundingBox);

        int* buffer = new int[length]; // might throw bad_alloc
        AutoRelease<int> releaseBuffer(buffer, true);
        ::memset(buffer, 0, sizeof(int)*length);
        
        const jchar* textPtr = aJniEnv->GetStringChars(aText, NULL);
        AutoReleaseStringChars releaseStringChars(aJniEnv, aText, textPtr);

        jint textLength = aJniEnv->GetStringLength(aText);
        
        gc->getTextBoundingBox(
            buffer,
            static_cast<const unsigned short*>(textPtr), 
            textLength,
            aAlignments,
            aFlags,
            aRectX, aRectY, aRectWidth, aRectHeight);

        // Copy changes to boundingBox
        swtApp->jniUtils().SetJavaIntArrayRegionFromIntArray(aJniEnv, aBoundingBox, 0, length, buffer);
    }
    GFX_CATCH
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getStrokeWidth
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    jint strokeWidth = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        strokeWidth = static_cast<jint>( gc->getStrokeWidth() );
    }
    GFX_CATCH
    return strokeWidth;
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getStrokeStyle
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    jint strokeStyle = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        strokeStyle = static_cast<jint>( gc->getStrokeStyle() );
    }
    GFX_CATCH
    return strokeStyle;
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getTranslateX
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    jint translateX = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        translateX = static_cast<jint>( gc->getTranslateX() );
    }
    GFX_CATCH
    return translateX;
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1getTranslateY
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    jint translateY = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        translateY = static_cast<jint>( gc->getTranslateY() );
    }
    GFX_CATCH
    return translateY;
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1setBackgroundAlpha
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aAlpha)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->setBackgroundAlpha(aAlpha);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1setBackgroundColor
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aArgb, jboolean aUpdateAlpha)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->setBackgroundColor(aArgb, aUpdateAlpha);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1setBlendingMode
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aMode) 
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->setBlendingMode((TBlendingMode)aMode);
    }
    GFX_CATCH
}

jboolean JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1hasClipping
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    jboolean hasClipping = JNI_FALSE;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        hasClipping = static_cast<jboolean>( gc->hasClipping() );
    }
    GFX_CATCH
    return hasClipping;
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1cancelClipping
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->cancelClipping();
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1setClip
 (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight, jboolean aIntersects)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->setClip(aX, aY, aWidth, aHeight, aIntersects);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1setFont
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aFontHandle)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->setFont(aFontHandle);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1setForegroundAlpha
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aAlpha)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->setForegroundAlpha(aAlpha);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1setForegroundColor
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aArgb, jboolean aUpdateAlpha)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->setForegroundColor(aArgb, aUpdateAlpha);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1setStrokeWidth
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidth)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->setStrokeWidth(aWidth);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1setStrokeStyle
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aStyle) 
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->setStrokeStyle((TStrokeStyle)aStyle);
    }
    GFX_CATCH
}

JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1translate
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aX, jint aY)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->translate(aX,aY);
    }
    GFX_CATCH
}

JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1scale
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aX, jint aY)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->scale(aX,aY);
    }
    GFX_CATCH
}


JNIEXPORT void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1resetTransform
  (JNIEnv* aJniEnv, jclass, jint aHandle)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->resetTransform();
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1saveSettings
  (JNIEnv* aJniEnv, jclass, jint aHandle)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->saveSettings();
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_graphicsContext_1restoreSettings
  (JNIEnv* aJniEnv, jclass, jint aHandle)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(GraphicsContext*, gc, aHandle);
        gc->restoreSettings();
    }
    GFX_CATCH
}

//
// Image class JNI calls
//

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1create__III
  (JNIEnv* aJniEnv , jclass, jint aWidth , jint aHeight, jint aFillColor)
{
    Image* img = NULL;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        img = GraphicsFactory::createImage(aWidth, aHeight, aFillColor);
    } 
    GFX_CATCH
    return POINTER_TO_HANDLE(img);
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1create__IIIII
  (JNIEnv* aJniEnv , jclass, jint aImageHandle, jint aX, jint aY, jint aWidth, jint aHeight)
{
    Image* newImage = NULL;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(Image*, image, aImageHandle);
        newImage = GraphicsFactory::createImage(image, aX, aY, aWidth, aHeight);
    }
    GFX_CATCH
    return POINTER_TO_HANDLE(newImage);
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1create___3IIIZ
  (JNIEnv* aJniEnv, jclass, jintArray aRgbData, jint aWidth, jint aHeight, jboolean aHasAlpha)
{
    Image* img = NULL;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        
        int length = aJniEnv->GetArrayLength(aRgbData);

        int* buffer = new int[length]; // might throw bad_alloc
        AutoRelease<int> release(buffer, true);
        swtApp->jniUtils().GetJavaIntArrayRegionToIntArray(aJniEnv, aRgbData, 0, length, buffer); // might throw bad_alloc

        img = GraphicsFactory::createImage(buffer, aWidth, aHeight, aHasAlpha);
    } 
    GFX_CATCH
    return POINTER_TO_HANDLE(img);
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1create__Lorg_eclipse_swt_graphics_ImageData_2
  (JNIEnv* aJniEnv , jclass, jobject aImageData)
{
    Image* img = NULL;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();     
        img = swtApp->jniUtils().CreateImage(aJniEnv, aImageData);
    }
    GFX_CATCH
    return POINTER_TO_HANDLE(img);
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1getFormat
  (JNIEnv* aJniEnv, jclass, jint aImageHandle)
{
    jint format = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(Image*, image, aImageHandle);
        format = static_cast<jint>( image->getFormat() );
    }
    GFX_CATCH
    return format;
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1getHeight
  (JNIEnv* aJniEnv, jclass, jint aImageHandle)
{
    jint height = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(Image*, image, aImageHandle);
        height = static_cast<jint>( image->getHeight() );
    }
    GFX_CATCH
    return height;
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1getWidth
  (JNIEnv* aJniEnv , jclass, jint aImageHandle) 
{
    jint width = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(Image*, image, aImageHandle);
        width = static_cast<jint>( image->getWidth() );
    }
    GFX_CATCH
    return width;
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1getRGB__I_3IIIIIII
  (JNIEnv* aJniEnv, jclass, jint aImageHandle, jintArray aRgbData, jint aOffset, jint aScanlength, jint aX, jint aY, jint aWidth, jint aHeight)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(Image*, image, aImageHandle);   
        
        int length = aJniEnv->GetArrayLength(aRgbData);

        int* buffer = new int[length]; // might throw bad_alloc
        AutoRelease<int> release(buffer, true);
        ::memset(buffer, 0, sizeof(int)*length);

        // get the data (populated to data array)
        image->getRgb(buffer, aOffset, aScanlength, aX, aY, aWidth, aHeight);
        
        // Copy data back to java
        swtApp->jniUtils().SetJavaIntArrayRegionFromIntArray(aJniEnv, aRgbData, 0, length, buffer);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1getRGB__I_3B_3BIIIIIII
  (JNIEnv * aJniEnv, jclass, jint aImageHandle, jbyteArray aRgbData, jbyteArray aTransparencyMask, jint aOffset, jint aScanlength, jint aX, jint aY, jint aWidth, jint aHeight, jint aFormat)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(Image*, image, aImageHandle);
        
        int dataLength = aJniEnv->GetArrayLength(aRgbData);
        int maskLength = aJniEnv->GetArrayLength(aTransparencyMask);
        
        char* dataBuffer = new char[dataLength]; // might throw bad_alloc
        AutoRelease<char> releaseData(dataBuffer, true);
        char* maskBuffer = new char[maskLength]; // might throw bad_alloc
        AutoRelease<char> releaseMask(maskBuffer, true);
        ::memset(dataBuffer, 0, sizeof(char)*dataLength);
        ::memset(maskBuffer, 0, sizeof(char)*maskLength);
        
        // get the data (populated to data array)
        image->getRgb(dataBuffer, maskBuffer, aOffset, aScanlength, aX, aY, aWidth, aHeight, aFormat);
        
        // Copy data back to Java
        swtApp->jniUtils().SetJavaByteArrayRegionFromCharArray(aJniEnv, aRgbData, 0, dataLength, dataBuffer);
        swtApp->jniUtils().SetJavaByteArrayRegionFromCharArray(aJniEnv, aTransparencyMask, 0, maskLength, maskBuffer);
     }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1getRGB__I_3SIIIIIII
  (JNIEnv * aJniEnv, jclass, jint aImageHandle, jshortArray aRgbData, jint aOffset, jint aScanlength, jint aX, jint aY, jint aWidth, jint aHeight, jint aFormat) {
    GFX_TRY
{
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(Image*, image, aImageHandle);
        
        int length = aJniEnv->GetArrayLength(aRgbData);
        short* buffer = new short[length];
        AutoRelease<short> release(buffer, true);
        ::memset(buffer, 0, sizeof(short)*length);
        
        // get the data (populated to data array)
        image->getRgb(buffer, aOffset, aScanlength, aX, aY, aWidth, aHeight, aFormat);
        
        // Copy data back to java
        swtApp->jniUtils().SetJavaShortArrayRegionFromShortArray(aJniEnv, aRgbData, 0, length, buffer);
    }
    GFX_CATCH
}

jobject JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1getImageData
  (JNIEnv* aJniEnv , jclass, jint aImageHandle)
{
    GFX_LOG_JNI_CALL();
    jobject imageData = 0;
    GFX_TRY
    {
        ImageDataWrapper* data = NULL;
        HANDLE_TO_POINTER(Image*, image, aImageHandle);   
        data = GraphicsFactory::createImageData(image);
        AutoRelease<ImageDataWrapper> release(data, false);    
        imageData = swtApp->jniUtils().CreateJavaImageData(aJniEnv, *data);
    }
    GFX_CATCH
    return imageData;
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1transform
  (JNIEnv* aJniEnv , jclass, jint aImageHandle, jint aTransform)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(Image*, image, aImageHandle);
        image->transform((TTransform)aTransform);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1dispose
  (JNIEnv* aJniEnv , jclass, jint aImageHandle) 
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(Image*, image, aImageHandle);
        image->dispose();
        image = NULL;
    }
    GFX_CATCH
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1getPixmapHandle
  (JNIEnv* aJniEnv , jclass, jint aImageHandle)
{
    jint pixmapHandle = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(Image*, image, aImageHandle);
        pixmapHandle = POINTER_TO_HANDLE(image->getPixmap());
    }
    GFX_CATCH
    return pixmapHandle;
}

jboolean JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_image_1detectCollision
  (JNIEnv* aJniEnv, jclass, jint aImage1PixmapHandle, jint aTransform1, jint aP1x, jint aP1y, jint aR1x1, jint aR1y1, jint aR1x2, jint aR1y2,
                            jint aImage2PixmapHandle, jint aTransform2, jint aP2x, jint aP2y, jint aR2x1, jint aR2y1, jint aR2x2, jint aR2y2)
{
  	jboolean collides = JNI_FALSE;
  	GFX_TRY
    {
    	  GFX_LOG_JNI_CALL();
    	  collides = gfxUtils::detectCollision(aImage1PixmapHandle, aTransform1, aP1x, aP1y, aR1x1, aR1y1, aR1x2, aR1y2,
    	                                       aImage2PixmapHandle, aTransform2, aP2x, aP2y, aR2x1, aR2y1, aR2x2, aR2y2);
    }
    GFX_CATCH
    return collides;
}

//
// ImageLoader JNI calls
//

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_imageLoader_1append
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aLenght, jint aOffset, jbyteArray aImageData)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        jbyte* data = NULL; 
        data = aJniEnv->GetByteArrayElements(aImageData, NULL);
        HANDLE_TO_POINTER(ImageLoader*, loader, aHandle);
        loader->append(reinterpret_cast<const char*>(data), aLenght, aOffset);
        // release arrays, don't copy back
        aJniEnv->ReleaseByteArrayElements(aImageData, data, JNI_ABORT);
    } 
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_imageLoader_1beginStream
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aBufferSize)
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(ImageLoader*, loader, aHandle);
        loader->beginStream(aBufferSize);
    } 
    GFX_CATCH
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_imageLoader_1endStream
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    Image* image = NULL;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(ImageLoader*, loader, aHandle);
        image = loader->endStream();
    }
    GFX_CATCH
    return POINTER_TO_HANDLE(image);
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_imageLoader_1init
  (JNIEnv* aJniEnv , jclass) 
{
    ImageLoader* loader = NULL;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();        
        loader = GraphicsFactory::createImageLoader();
    }
    GFX_CATCH
    return POINTER_TO_HANDLE(loader);
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_imageLoader_1dispose
  (JNIEnv* aJniEnv , jclass, jint aHandle) 
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(ImageLoader*, loader, aHandle);
        loader->dispose();
    }
    GFX_CATCH
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_imageLoader_1load
  (JNIEnv* aJniEnv , jclass, jint aHandle, jstring aFileName) 
{
    Image* image = NULL;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(ImageLoader*, loader, aHandle);
        image = loader->load(swtApp->jniUtils().JavaStringToQString(aJniEnv, aFileName));
    }
    GFX_CATCH
    return POINTER_TO_HANDLE(image);
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_imageLoader_1setLoadSize
  (JNIEnv* aJniEnv , jclass, jint aHandle, jint aWidth, jint aHeight) 
{
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(ImageLoader*, loader, aHandle);
        loader->setLoadSize(aWidth, aHeight);
    }
    GFX_CATCH
}

//
// FontUtils
//

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_fontUtils_1getAscent
  (JNIEnv* aJniEnv , jclass, jint aFontHandle)
{
    jint ascent = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(QFont*, font, aFontHandle);
        QFontMetrics fm(*font);
        ascent = static_cast<jint>( fm.ascent() );
    }
    GFX_CATCH
    return ascent;
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_fontUtils_1getBoundingRect__I_3ILjava_lang_String_2
  (JNIEnv* aJniEnv, jclass, jint aFontHandle, jintArray aRectArray, jstring aStr)
{
    GFX_TRY 
    {
        GFX_LOG_JNI_CALL();
        QString string = swtApp->jniUtils().JavaStringToQString(aJniEnv, aStr);
        
        int length = aJniEnv->GetArrayLength(aRectArray);

        int* buffer = new int[length]; // might throw bad_alloc
        AutoRelease<int> release(buffer, true);
        ::memset(buffer, 0, sizeof(int)*length);
        
        HANDLE_TO_POINTER(QFont*, font, aFontHandle); 
        QFontMetrics fm(*font);
        
        buffer[0] = 0;
        buffer[1] = 0;
        buffer[2] = fm.width(string);
        buffer[3] = fm.height();

        // Copy changes to rectArray
        swtApp->jniUtils().SetJavaIntArrayRegionFromIntArray(aJniEnv, aRectArray, 0, length, buffer);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_fontUtils_1getBoundingRect__I_3ILjava_lang_String_2IIIIII
  (JNIEnv* aJniEnv, jclass, jint aFontHandle, jintArray aRectArray, jstring aStr, jint /*aRectX*/, jint /*aRectY*/, jint /*aRectW*/, jint /*aRectH*/, jint /*aAlignments*/, jint /*aFlags*/)
{
    GFX_TRY 
    {
        GFX_LOG_JNI_CALL();
        QString string = swtApp->jniUtils().JavaStringToQString(aJniEnv, aStr);
        int length = aJniEnv->GetArrayLength(aRectArray);

        int* buffer = new int[length]; // might throw bad_alloc
        AutoRelease<int> release(buffer, true);
        ::memset(buffer, 0, sizeof(int)*length);
 
        HANDLE_TO_POINTER(QFont*, font, aFontHandle); 
        QFontMetrics fm(*font);
        
        buffer[0] = 0;
        buffer[1] = 0;
        buffer[2] = fm.width(string);
        buffer[3] = fm.height();
        
        // Copy changes to rectArray
        swtApp->jniUtils().SetJavaIntArrayRegionFromIntArray(aJniEnv, aRectArray, 0, length, buffer);
    }
    GFX_CATCH
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_fontUtils_1getDescent
  (JNIEnv* aJniEnv , jclass, jint aFontHandle)
{
    jint descent = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(QFont*, font, aFontHandle); 
        QFontMetrics fm(*font);
        descent = static_cast<jint>( fm.descent() );
    }
    GFX_CATCH
    return descent;
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_fontUtils_1getStringWidth
  (JNIEnv* aJniEnv , jclass, jint aFontHandle, jstring aStr)
{
    int strWidth = 0;
    GFX_TRY 
    {
        GFX_LOG_JNI_CALL();
        HANDLE_TO_POINTER(QFont*, font, aFontHandle); 
        QString string = swtApp->jniUtils().JavaStringToQString(aJniEnv,  aStr);
        QFontMetrics fm(*font);
        strWidth = fm.width(string);
    }
    GFX_CATCH
    return static_cast<jint>(strWidth);
}

//
// Buffer JNI calls
//

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_buffer_1init
  (JNIEnv* aJniEnv , jclass)
{
    Buffer* buffer = 0;
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        buffer = GraphicsFactory::createBuffer();
    }
    GFX_CATCH
    return POINTER_TO_HANDLE(buffer);
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_buffer_1dispose
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    GFX_TRY
    {
        HANDLE_TO_POINTER(Buffer*, buffer, aHandle);
        buffer->dispose();
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_buffer_1getInvalidRect
  (JNIEnv* aJniEnv, jclass, jint aHandle, jintArray aRect) 
{
    GFX_TRY
    {
        HANDLE_TO_POINTER(Buffer*, buffer, aHandle);
        
        int length = aJniEnv->GetArrayLength(aRect);

        int* intBuffer = new int[length]; // might throw bad_alloc
        AutoRelease<int> release(intBuffer, true);
        ::memset(intBuffer, 0, sizeof(int)*length);
        
        buffer->getInvalidRect(intBuffer);

        // Copy data back to java
        swtApp->jniUtils().SetJavaIntArrayRegionFromIntArray(aJniEnv, aRect, 0, length, intBuffer);
    }
    GFX_CATCH
}

//
// Windowsurface
//
jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_windowsurface_1create
  (JNIEnv* aJniEnv, jclass, jint aHandle, jboolean aAutoRefresh)
{
    WindowSurface* surface = 0;
    HANDLE_TO_POINTER(QWidget*, widget, aHandle);
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        surface = GraphicsFactory::createWindowSurface(widget, aAutoRefresh);
    }
    GFX_CATCH

    return POINTER_TO_HANDLE(surface);
}
void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_windowsurface_1beginPaint
  (JNIEnv* aJniEnv, jclass, jint aHandle, jint aX, jint aY, jint aWidth, jint aHeight)
{
    HANDLE_TO_POINTER(WindowSurface*, wsurf, aHandle);
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        wsurf->beginPaint(aX, aY, aWidth, aHeight);
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_windowsurface_1endPaint
  (JNIEnv* aJniEnv, jclass, jint aHandle)
{
    HANDLE_TO_POINTER(WindowSurface*, wsurf, aHandle);
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        wsurf->endPaint();
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_windowsurface_1flush
  (JNIEnv* aJniEnv, jclass, jint aHandle)
{
    HANDLE_TO_POINTER(WindowSurface*, wsurf, aHandle);
    GFX_TRY
    {
        GFX_LOG_JNI_CALL();
        wsurf->flush();
    }
    GFX_CATCH
}

jint JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_windowsurface_1getType
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    jint type = 0;
    GFX_TRY
    {
        HANDLE_TO_POINTER(WindowSurface*, wsurf, aHandle);
        type = static_cast<jint>( wsurf->getType() );
    }
    GFX_CATCH
    return type;
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_windowsurface_1dispose
  (JNIEnv* aJniEnv , jclass, jint aHandle)
{
    GFX_TRY
    {
        HANDLE_TO_POINTER(WindowSurface*, wsurf, aHandle);
        wsurf->dispose();       
    }
    GFX_CATCH
}

void JNICALL Java_org_eclipse_swt_internal_qt_graphics_OS_windowsurface_1refresh
  (JNIEnv* aJniEnv, jclass, jint aHandle)
{
    GFX_TRY
    {
        HANDLE_TO_POINTER(WindowSurface*, wsurf, aHandle);
        wsurf->refresh();       
    }
    GFX_CATCH 
}




