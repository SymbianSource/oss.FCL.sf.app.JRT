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


/**
@file
@internalTechnology
@released
*/
#ifndef LCDGR_H
#define LCDGR_H

#include "lcdui.h"
#include <gdi.h>
#include <fbs.h>

class CFbsBitmap;

//
// LCDGR V2 interfaces
//

class MMIDBitmapImage;

/**
 * Public Image interface. Used by toolkit and plugin library.
 *
 * Images may be shared between native components and a reference count
 * may be maintained. Clients should call AddRef() on taking ownership of
 * an Image and RemoveRef() on releasing ownership.
 *
 * The refcount of a newly instantiated Image is 1. If a newly instantiated
 * image is assigned to a smart pointer that increments the count, then
 * RemoveRef() should be called after the assignment.
 */
class MMIDImage : public MMIDComponent
{
public:
    enum TTransparencyType
    {
        ENone  = 0, /**< Image has no transparency bitmap           */
        EMask  = 1, /**< Image has binary mask transparency bitmap  */
        EAlpha = 2, /**< Image has 256 level alpha channel bitmap   */
    };

    /**
     * Implementation of MMIDComponent interface method.
     * Clients should use RemoveRef() in preference.
     *@see MMIDComponent::Dispose()
     */
    virtual void Dispose() = 0;

    /**
     * Implementation of MMIDComponent interface method.
     *@return Always returns MMIDComponent::EImage.
     *
     * BC break: inline implementation removed
     */
    virtual TType Type() const = 0;

    /**
     *@return A TTransparencyType enumeration indicating the type of
     * transparency information the image has.
     *@see TTransparencyType
     */
    virtual TTransparencyType   TransparencyType() const = 0;

    /**
     * Set the image transparency type.
     *
     *@return KErrNone if succsessful, otherwise one of the system wide
     * error codes.
     */
    virtual TInt SetTransparencyType(TTransparencyType aTransparencyType) = 0;

    /**
     *@return Size in pixels of color (and optional transparency) channel
     * bitmap(s).
     *
     * BC Break: was inline.
     */
    virtual TSize Size() const = 0;

    /**
     * Sets pixels in image from ARGB8888 pixel values in aARGBArray.
     */
    virtual TInt SetPixels(const TUint32* aARGBArray, TInt aLength, TInt aOffset, TInt aScanLength, const TRect& aRect, TBool aAlpha) = 0;

    /**
     * Gets pixels from image as ARGB8888 pixel values in aARGBArray.
     */
    virtual TInt GetPixels(TUint32* aARGBArray, TInt aLength, TInt aOffset, TInt aScanLength, const TRect& aRect) = 0;

    /**
     * Copy a rectangular region of aSource, transformed by aTransform, to the a rectangle
     * position at origin of this image
     */
    virtual TInt CopyRegion(MMIDImage* aSource, const TRect& aRect, TInt aTransform) = 0;

    /**
    Detect pixel level collision between two Images.
    @param aRect1       image1 collison rect in image1 co-ordinates
    @param aTransform1  image1 transform
    @param aPoint1      position of image1 collison rect in painter's co-ordinates.

    @param aImage2      image2
    @param aRect2       image2 collison rect in image2 co-ordinates
    @param aTransform2  image2 transform
    @param aPoint2      position of image2 collison rect in painter's co-ordinates.
    */
    virtual TBool DetectCollision(const TRect& aRect1, TInt aTransform1, const TPoint& aPoint1,
                                  MMIDImage* aImage2,
                                  const TRect& aRect2, TInt aTransform2, const TPoint& aPoint2) = 0;

    /**
     * Obtain the primary bitmap representation of this Image. This will be in the mode used by the
     * graphic subsystem to render this image via Graphics.drawImage and Graphics.drawRegion methods.
     *
     * The reference count of the returned MMIDBitmapImage will have been incremented so the caller does not
     * need to increase the count. The caller is responsible for decreasing the reference count on the
     * returned instance when a reference is no longer required.
     *
     * May return NULL if the primary representation of the Image is not an MMIDBitmapImage.
     */
    virtual MMIDBitmapImage* BitmapImage() = 0;


    /**
     * Obtain a bitmap representation of this Image.
     *
     *@param aColorMode the displaymode to use for the color bitmap, or EDefaultDisplayMode for the default mode.
     *@param aAlphaMode the displaymode to use for the alpha bitmap, EDefaultDisplayMode for the default mode or
     * ENone if no alpha bitmap required.
     *@param aInvert if true return an alpha bitmap with inverted pixels - suitable for an inverted mask bitblt.
     *
     * The reference count of the returned MMIDBitmapImage will have been incremented so the caller does not
     * need to increase the count.
     *
     *@return a bitmap representation of this image if one exists in the desired format, NULL otherwise.
     */
    virtual MMIDBitmapImage*    BitmapImage(TInt aColorMode, TInt aAlphaMode, TBool aInvert) = 0;

    /**
     * As BitmapImage(TInt,TInt,TBool) but attempts to create a bitmap representation of the image if none
     * is found in the desired format.
     *
     * Leaves with KErrNoMemory or other systemwide error code if a bitmap representation cannot be found
     * or created.
     *
     *@see BitmapImage(TInt, TInt, TBool)
     */
    virtual MMIDBitmapImage*    BitmapImageL(TInt aColorMode, TInt aAlphaMode, TBool aInvert) = 0;
};

/**
 * Bitmap representation of an image.
 */
class MMIDBitmapImage
{
public:
    enum { EDefaultDisplayMode = -1 };

    /**
     * Increment ref count
     */
    virtual void AddRef() = 0;

    /**
     * Decrement ref count, destroying resources when ref hits zero.
     */
    virtual void RemoveRef() = 0;

    /**
     *@return bitmap containing color information for this image.
     */
    virtual CFbsBitmap* ColorBitmap() const = 0;

    /**
     *@return bitmap containing transparency information for this image. The
     * returned image may contain binary or multi-level alpha.
     */
    virtual CFbsBitmap* AlphaBitmap() const = 0;

    /**
     * Create a bitmap in displaymode aMode and copy the colour information from
     * this image into that bitmap.
     */
    virtual CFbsBitmap* CreateColorBitmapL(TDisplayMode aMode) const = 0;

    /**
     * Create a bitmap in displaymode aMode and copy the alpha information from
     * this image into that bitmap. If aInvert is true and the image contains
     * binary alpha, the alpha levels will be inverted (NOT) to produce an inverted mask.
     */
    virtual CFbsBitmap* CreateAlphaBitmapL(TDisplayMode aMode, TBool aInvert) const = 0;

    /**
     * Duplicate the handle to the color bitmap. Cheaper than CreateColorBitmapL
     * but returns an object refering to the same pixel data so callers must not
     * mutate the returned bitmaps.
     */
    virtual CFbsBitmap* DuplicateColorBitmapL() const = 0;

    /**
     * Duplicate the handle to the alpha bitmap. Cheaper than CreateAlphaBitmapL
     * but returns an object refering to the same pixel data so callers must not
     * mutate the returned bitmaps.
     */
    virtual CFbsBitmap* DuplicateAlphaBitmapL() const = 0;

    /**
     * Draws whole image to aContext at aPoint, subject to current
     * origin and clipping of aContext.
     */
    virtual void Draw(CBitmapContext& aContext, const TPoint& aPoint) const = 0;

    /**
     * Draws a subimage bounded by aRect to aContext at aPoint, subject
     * to current origin and clipping of aContext.
     *
     *@see Draw(CBitmapContext& aContext, const TPoint& aPoint);
     */
    virtual void Draw(CBitmapContext& aContext, const TPoint& aPoint, const TRect& aRect) const = 0;
};

/**
 *@return default bitmap representation of aImage, or NULL if no bitmap representation exists.
 */
inline MMIDBitmapImage* GetBitmapImage(MMIDImage* aImage)
{
    if (aImage)
    {
        return aImage->BitmapImage(MMIDBitmapImage::EDefaultDisplayMode, MMIDBitmapImage::EDefaultDisplayMode, EFalse);
    }
    return NULL;
}

/**
 *@return default bitmap representation of aImage or NULL if aImage is NULL.
 * leaves if no bitmap representation can be constructed.
 */
inline MMIDBitmapImage* GetBitmapImageL(MMIDImage* aImage)
{
    if (aImage)
    {
        return aImage->BitmapImageL(MMIDBitmapImage::EDefaultDisplayMode, MMIDBitmapImage::EDefaultDisplayMode, EFalse);
    }
    return NULL;
}


/**
 * Native peer for javax.microedition.lcdui.Graphics class
 */
#ifdef RD_JAVA_NGA_ENABLED
class MMIDGraphics : public MMIDComponentNgaExtension
#else // !RD_JAVA_NGA_ENABLED
class MMIDGraphics : public MMIDComponent
#endif // RD_JAVA_NGA_ENABLED
{
public:
    enum TGraphicsOp
    {
        EGrOpSetColor,          // 0
        EGrOpSetStroke,
        EGrOpSetFont,           // 2
        EGrOpSetClipRect,
        EGrOpTranslate,         // 4
        EGrOpReset,
        EGrOpDrawLine,          // 6
        EGrOpDrawRect,
        EGrOpFillRect,          // 8
        EGrOpDrawRoundRect,
        EGrOpFillRoundRect,     // 10
        EGrOpDrawArc,
        EGrOpFillArc,           // 12
        EGrOpDrawText,
        EGrOpDrawImage,         // 14
        EGrOpDrawRegion,
        EGrOpCopyArea,          // 16
        EGrOpFillTriangle,
        EGrOpDrawPixels,        // 18
        EGrOpFlush,             // 19
        EGrOpDrawBackground,    // 20
        EGrOpCopyGraphics,      // 21
        KGrOpCount              // 22

    };

    enum TAnchor
    {
        EAnchorHCenter = 1,
        EAnchorVCenter = 2,
        EAnchorLeft    = 4,
        EAnchorRight   = 8,
        EAnchorTop     = 16,
        EAnchorBottom  = 32,
        EAnchorBaseline = 64
    };

    enum TPixelType
    {
        EPixelTypeARGB8888  = 0,        // Java Pixel Type.
        EPixelTypeARGB4444  = 1,        // not supported
        EPixelTypeEColor4K  = 2,        // not supported
        EPixelTypeEColor64K = 3         // not supported
    };

public:
    /**
     *@return aRGB quantized to display pallete
     */
    virtual TUint32 DisplayColor(TUint32 aRGB) = 0;

    /**
     * Render aARGB pixels onto target with optional alpha blending.
     *
     * aScanLength may be negative indicating that the source image should
     * be reflected about a horizontal line through its center.
     *
     *@return KErrNone if aType is a supported pixel type.
     */
    virtual TInt DrawPixels
    (
        TInt    aType,          // pixel data type
        TUint8* aAddress,       // pixel data array
        TInt    aLength,        // pixel array length
        TInt    aScanLength,    // always positive - vertical reflection handled by point/size
        TBool   aAlpha,         // true if pixel data contains alpha
        const TRect& aRect,     // destination rect
        TInt    aTransform      // transform to apply
    ) = 0;

    /**
     *
     *@return bitmap handle if graphics renders to an image or canvas frame buffer
     * or NULL if graphics renders to a window.
     */
    virtual CFbsBitmap* Bitmap() const = 0;

    /**
     *@return window handle if graphics renders to a canvas window, NULL otherwise.
     */
    virtual RWindow* Window() const = 0;

    /**
     *@return drawing surface viewport rectangle.
     */
    virtual TRect ViewPort() const = 0;
    /**
     *@return ETrue for mutable off-screen images (not for canvas/GameCanvas framebuffer)
     * or EFalse for canvas/GameCanvas framebuffer
     */
    virtual TBool IsImageTarget() const = 0;

#ifdef RD_JAVA_NGA_ENABLED
    /**
     * Get size of target Canvas. Returns TSize(0,0), if target is not Canvas
     * @return
     *      - Size of target Canvas
     * @since S60 9.2
     */
    virtual TSize CanvasTargetSize() const = 0;

    /**
     * Returns pointer to the target Canvas of the Graphics object or NULL in case
     * target is not Canvas.
     * @return
     *      Pointer to MMIDCanvas or NULL
     * @since S60 9.2
     */
    virtual MMIDCanvas* GetTargetCanvas() const = 0;
#endif // RD_JAVA_NGA_ENABLED
};

struct TLcdGrOp : public TMIDBufferOp
{
};

struct TLcdGrOpSetColor : TLcdGrOp
{
    TUint32 iColor;
};

struct TLcdGrOpSetStrokeStyle : TLcdGrOp
{
    TUint32 iStyle;
};

struct TLcdGrOpSetFont : TLcdGrOp
{
    TUint32 iFont;
};

struct TLcdGrOpSetClipRect : TLcdGrOp
{
    TPoint  iPosition;
    TSize   iSize;
};

struct TLcdGrOpTranslate : TLcdGrOp
{
    TPoint  iVector;
};

struct TLcdGrOpReset : TLcdGrOp
{
    TSize   iSize;
};

struct TLcdGrOpDrawLine : TLcdGrOp
{
    TPoint  iPoints[2];
};

struct TLcdGrOpDrawRect : TLcdGrOp
{
    TPoint  iPosition;
    TSize   iSize;
};

struct TLcdGrOpFillRect : TLcdGrOp
{
    TPoint  iPosition;
    TSize   iSize;
};

struct TLcdGrOpDrawRoundRect : TLcdGrOp
{
    TPoint  iPosition;
    TSize   iSize;
    TSize   iCornerSize;
};

struct TLcdGrOpFillRoundRect : TLcdGrOp
{
    TPoint  iPosition;
    TSize   iSize;
    TSize   iCornerSize;
};

struct TLcdGrOpDrawArc : TLcdGrOp
{
    TPoint  iPosition;
    TSize   iSize;
    TInt    iStart;
    TInt    iAngle;
};

struct TLcdGrOpFillArc : TLcdGrOp
{
    TPoint  iPosition;
    TSize   iSize;
    TInt    iStart;
    TInt    iAngle;
};

struct TLcdGrOpDrawText : TLcdGrOp
{
    TPoint  iPoint;
    TInt    iAnchor;
    TInt    iLength;
    TText   iText[1];
};

struct TLcdGrOpDrawImage : TLcdGrOp
{
    TInt    iImage;
    TPoint  iPoint;
    TInt    iAnchor;
};

struct TLcdGrOpDrawRegion : TLcdGrOp
{
    TInt    iImage;
    TPoint  iSrcPosition;
    TSize   iSrcSize;
    TInt    iTransform;
    TPoint  iDstPoint;
    TInt    iAnchor;
};

struct TLcdGrOpCopyArea : TLcdGrOp
{
    TPoint  iSrcPosition;
    TSize   iSrcSize;
    TPoint  iPoint;
    TInt    iAnchor;
};

struct TLcdGrOpFillTriangle : TLcdGrOp
{
    TPoint  iPoints[3];
};

struct TLcdGrOpDrawPixels : TLcdGrOp
{
    TInt    iType;          // pixel data type
    TInt    iAddress;       // handle of pixel data array
    TInt    iLength;        // pixel array length
    TInt    iScanLength;    // always positive - vertical reflection handled by point/size
    TBool   iAlpha;         // true if pixel data contains alpha
    TPoint  iPosition;
    TSize   iSize;
    TInt    iTransform;     // transform to apply
};

struct TLcdGrOpFlush : TLcdGrOp
{
    TPoint  iPosition;
    TSize   iSize;
};

struct TLcdGrOpDrawBackground : TLcdGrOp
{
    TInt    iCanvas;
    TPoint  iPosition;
    TSize   iSize;
};

/**
 * NGA specific change for coping Graphics content
 * @since s60 9.2
 */
struct TLcdGrOpCopyGraphics : TLcdGrOp
{
    TInt    iSrcGraphics;
};

/**
 * Image conversion callback interface. Called by image converter
 * when image has conversion has completed or .
 */
class MMIDImageObserver
{
public:
    virtual void DecodeComplete(TInt aError) = 0;
};

/**
 * Image conversion interface.
 */
class MMIDImageDecoder
{
public:
    /**
     * Append encoded image data to the decoder. The decoder
     * will buffer this data.
     */
    virtual void AppendL(const TDesC8& aBuffer) = 0;

    /**
     * Decode an image in the decoder buffer. Image must be in a
     * self identifying encoding such as GIF, PNG. etc.
     *
     * Calls back to MMIDImageDecoderObserver::DecodeComplete when
     * image has been decoded into a bitmap image.
     */
    virtual void DecodeL(MMIDImageObserver* aObserver) = 0;

    /**
     *@return decoded image as a bitmap image.
     */
    virtual MMIDBitmapImage* BitmapImage() const = 0;

    /**
     *
     */
    virtual void Dispose() = 0;
};

/**
 *@deprecated. Interface maintained for transform type only.
 */
class MMIDSprite
{
public:
    enum TTransform
    {
        ETransNone          = 0,
        ETransMirrorRot180  = 1,
        ETransMirror        = 2,
        ETransRot180        = 3,
        ETransMirrorRot270  = 4,
        ETransRot90         = 5,
        ETransRot270        = 6,
        ETransMirrorRot90   = 7
    };
};

/**
 * Factory object.
 */
class MMIDGraphicsFactory
{
public:
    /**
     * destroy the factory and any resources.
     */
    virtual void            Dispose() = 0;

    /**
     *@return the displaymode to use for canvas framebuffers
     */
    virtual TDisplayMode    DisplayMode() const = 0;

    /**
     *@return ETrue if graphics should use double buffering. This
     * value is read from the lcdgr resource file and may be overridden
     * by supplying a native framebuffer in MMIDCanvas.
     */
    virtual TBool DoubleBuffer() const = 0;

    /**
     *@param aSize image size, must be positive area.
     *@return a newly instantiated mutable image.
     */
    virtual MMIDImage*      NewMutableImageL(const TSize& aSize) = 0;

    /**
     *@param aSize image size, must be positive area.
     *@param aTransparency one of MMIDImage::TTransparencyType
     *@return a newly instantiated immutable image.
     */
    virtual MMIDImage*      NewImageL(const TSize& aSize, TInt aTransparency) = 0;

    /**
     *@return a newly instantiated immutable image constructed from image data
     * previously decoded by <CODE>aDecoder</CODE>.
     */
    virtual MMIDImage*      NewImageL(MMIDImageDecoder* aDecoder) = 0;

    /**
     *@return a newly instantiated mutable image representing the canvas
     * frame buffer. TEMPORARY UNTIL MMIDCanvas interface may be updated.
     */
    virtual MMIDImage*      NewImageL(MMIDCanvas* aCanvas) = 0;

    /**
     *@return a newly instantiated graphics peer to render to a Canvas.
     */
    virtual MMIDGraphics*   NewGraphicsL(MMIDCanvas* aCanvas) = 0;

    /**
     *@return a newly instantiated graphics peer to render to a CustomItem
     */
    virtual MMIDGraphics*   NewGraphicsL(MMIDCustomItem* aCustomItem) = 0;

    /**
     *@return a newly instantiated graphics peer to render to a mutable Image.
     */
    virtual MMIDGraphics*   NewGraphicsL(MMIDImage* aMutableImage) = 0;

    /**
     *
     */
    virtual MMIDGraphics*   NewGraphicsL(MMIDCanvasGraphicsItemPainter* aCanvasGraphicsPainter) = 0;

    /**
     *@return a newly instantiated image decoder instance.
     */
    virtual MMIDImageDecoder* NewImageDecoderL() = 0;
};


#endif // LCDGR_H
