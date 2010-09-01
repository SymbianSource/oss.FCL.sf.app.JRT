/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LCDGDRV_H
#define LCDGDRV_H

#include <e32std.h>
#include <gdi.h>
#include <graphicsaccelerator.h>    // for TAcceleratedBitmapInfo. TO BE REVIEWED

//
// NEED NAMESPACE!
//

/**
 * Supported reflections.
 */
enum TReflection
{
    /**
     * Identity transformation.
     */
    EReflectNone=0,
    /**
     * Vertical reflection (in horizontal line through region center).
     */
    EReflectVert=1,
    /**
     * Horizontal reflection (in vertical line through region center).
     */
    EReflectHorz=2,
    /**
     * Reflection in diagonal line from top left corner to bottom right corner
     */
    EReflectDiag=4
};

/**
 * Supported transformation are all combinations of the supported reflections.
 * Note that two reflections make a rotation, so that this list includes pure
 * rotations by multiples of 90 degrees.
 */
enum TTransformType
{
    ETransNone          =EReflectNone,
    ETransMirrorRot180  =EReflectVert,
    ETransMirror        =EReflectHorz,
    ETransRot180        =EReflectVert|EReflectHorz,
    ETransMirrorRot270  =EReflectDiag,
    ETransRot90         =EReflectVert|EReflectDiag,
    ETransRot270        =EReflectHorz|EReflectDiag,
    ETransMirrorRot90   =EReflectVert|EReflectHorz|EReflectDiag
};

/**
 * lcdui anchor description.
 */
enum TAnchor
{
    EAnchorHCenter  = 1,
    EAnchorVCenter  = 2,
    EAnchorLeft     = 4,
    EAnchorRight    = 8,
    EAnchorTop      = 16,
    EAnchorBottom   = 32,
    EAnchorBaseline = 64
};

/**
 * Transparency description.
 *
 * Note drivers need not support all these formats, but must support
 * at least one format that includes transparency information.
 */
enum TTransparency
{
    /**
     * No transparency, alpha bitmap may not be present or is to be ignored.
     */
    ETransparencyNone,

    /**
     * Image has a mask bitmap. Supported mask bitmap formats may include 1bpp
     * and a format that matches the color bitmap.
     */
    ETransparencyMaskBitmap,

    /**
     * Image has an alpha bitmap containing alpha values.
     * Typically 8bpp 256 level alpha bitmaps will be supported.
     */
    ETransparencyAlphaBitmap,

    /**
     * Image has alpha contained as channel in the color bitmap.
     *
     *@see EColor16MA display mode.
     */
    ETransparencyAlphaChannel,

    /**
     * Image has mask contained as a channel in the color bitmap.
     * Equivalent to ETransparencyAlphaChannel with only two alpha
     * levels.
     */
    ETransparencyMaskChannel,

#ifdef RD_JAVA_NGA_ENABLED
    /**
     * Target surface has alpha contained as channel in the color bitmap
     * Color components are premultiplied.
     * @see ETransparencyAlphaChannel.
     * @since S60 9.2
     */
    ETransparencyAlphaChannelPre,

    /**
     * Target surface has alpha contained as channel in the color bitmap
     * Color components are premultiplied.
     * @see ETransparencyMaskChannel.
     * @since S60 9.2
     */
    ETransparencyMaskChannelPre,
#endif // RD_JAVA_NGA_ENABLED


    /**
     * Image color bitmap may be able to hold transparency channel
     * but this should be ignored and the pixels treated as though
     * they were fully opaque.
     */
    ETransparencyIgnoreChannel,

    /**
     * Count of enumerated transparency types - must always be the
     * last entry in this enumeration: add new values above this one.
     */
    ETransparencyCount
};

/**
 * Image type descriptor.
 */
struct TImageType
{
    TDisplayMode iColorMode;            // color bitmap displaymode
    TDisplayMode iAlphaMode;            // alpha bitmap (if any) displaymode.
    TTransparency iTransparency;        // transparency type
};

enum TStrokeStyle
{
    EStrokeSolid,
    EStrokeDotted
};

/**
 * For CanavsGraphicsItem support
 */
enum TCanvasGraphicsItemOperations
{
    ECanvasGraphicsItemImageRendering,
    ECanvasGraphicsItemRGBRendering
};
typedef TCanvasGraphicsItemOperations TCanvasGraphicsItemOperationsType;


/**
 * Private implementation class.
 */
class CLcdGraphicsDevice;


/**
 * Interface to graphics driver dll's.
 *
 * Encapsulates support for bitblts and renderers for various image formats.
 *
 */
class CLcdGraphicsDriver : public CBase
{
public:
    /**
     * Transparency capabilities.
     */
    enum TTranspanrencyCaps
    {
        ECapTransparencyMask1Bpp=0x1,
        ECapTransparencyMaskMatched=0x2,
        ECapTransparencyAlpha8Bpp=0x4,
        ECapTransparencyAlphaChannel8=0x8,
        ECapTransparencyMaskChannel8=0x10
    };

    /**
     * Driver info.
     */
    struct TDriverInfo
    {
        TVersion        iVersion;           //
        TBuf<64>        iProvider;          // driver provider
        TInt            iTransparencyCaps;  //
        TDisplayMode    iDisplayMode;       // primary display mode
        TDisplayMode    iARGB8888Mode;      // how driver recognises ARGB8888 pixels
    };

public:
    /**
     * Factory method creates a new CLcdGraphicsDriver instance for
     * images of type <CODE>aPrimaryMode</CODE>).
     */
    IMPORT_C static CLcdGraphicsDriver* NewL(TDisplayMode aPrimaryMode);

    /**
     *
     */
    virtual void GetDriverInfo(TDriverInfo& aInfo) const = 0;

    /**
     * Transforming bitblt from aColorBitmap/aAlphaBitmap to aDestBitmap
     * Composites source image over destination image (either alpha blending
     * or masking as appropriate to <CODE>aSrcTransparency</CODE>).
     * Destination bitmap is treated as being opaque.
     *
     * Note: This method does not support target bitmaps with alpha channel.
     */
    virtual TInt DrawRegion
    (
        const TAcceleratedBitmapInfo*   aDstColorBitmap,
        const TRect&                    aDstRect,
        const TAcceleratedBitmapInfo*   aSrcColorBitmap,
        const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
        TTransparency                   aSrcTransparency,
        const TRect&                    aSrcRect,
        TTransformType                  aSrcTransform,
        const TRect&                    aClipRect
    ) = 0;

    /**
     * Transforming bitblt from <CODE>aSrcColorBitmap,aSrcAlphaBitmap<CODE>
     * to <CODE>aDstColorBitmap,aDstAlphaBitmap</CODE>.
     * Copies source image pixels to destination image converting color
     * and transparency pixels to the destination format. Supports translation
     * and symmetry transformation of source image region, specified by
     * <CODE>aSrcTransform</CODE>.
     */
    virtual TInt CopyRegion
    (
        const TAcceleratedBitmapInfo*   aDstColorBitmap,
        const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
        TTransparency                   aDstTransparency,
        const TRect&                    aDstRect,
        const TAcceleratedBitmapInfo*   aSrcColorBitmap,
        const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
        TTransparency                   aSrcTransparency,
        const TRect&                    aSrcRect,
        TTransformType                  aSrcTransform,
        const TRect&                    aClipRect
    ) = 0;

    /**
     *
     */
    virtual TBool DetectCollision
    (
        const TAcceleratedBitmapInfo* aMask1,
        TTransparency       aTransparency1,
        const TSize&        aSize1,
        const TRect&        aRect1,
        TInt                aTransform1,
        const TPoint&       aPoint1,

        const TAcceleratedBitmapInfo* aMask2,
        TTransparency       aTransparency2,
        const TSize&        aSize2,
        const TRect&        aRect2,
        TInt                aTransform2,
        const TPoint&       aPoint2
    ) = 0;

    /**
     *
     */
    virtual CLcdGraphicsDevice* CreateDeviceL(const TImageType& aImageType) = 0;
};

class CLcdGraphicsDevice : public CBase
{
public:
    /**
     * Drawing capabilities
     */
    enum TDrawingCaps
    {
        ECapDrawLine    =0x1,
        ECapDrawRect    =0x2,
        ECapFillRect    =0x4,
        ECapDrawArc     =0x8,
        ECapFillArc     =0x10,
        ECapDrawRegion  =0x20,
        ECapCopyRegion  =0x40,
        ECapFillTriangle=0x80,
        ECapDrawText    =0x100,
        ECapStrokeDotted=0x200,
    };

    virtual TUint32 DrawingCaps() const = 0;

    /**
     *@return closest RGB color on 888 lattice to aRGB that can
     * be mapped without loss of information between 888 RGB color
     * space and the RGB color space of the target drawing surface.
     */
    virtual TUint32 Quantize(TUint32 aRGB) const = 0;

    /**
     * Transforming bitblt from aColorBitmap/aAlphaBitmap to target surface
     * Composites source image over destination image (either alpha blending
     * or masking as appropriate to <CODE>aSrcTransparency</CODE>).
     */
    virtual TInt DrawRegion
    (
        const TAcceleratedBitmapInfo*   aDstBitmap,
        const TRect&                    aDstRect,
        const TAcceleratedBitmapInfo*   aSrcColorBitmap,
        const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
        TTransparency                   aSrcTransparency,
        const TRect&                    aSrcRect,
        TTransformType                  aSrcTransform,
        const TRect&                    aClipRect
    ) = 0;

    /**
     * Transforming bitblt from <CODE>aSrcColorBitmap,aSrcAlphaBitmap<CODE>
     * to <CODE>aDstColorBitmap,aDstAlphaBitmap</CODE>.
     * Copies source image pixels to destination image converting color
     * and transparency pixels to the destination format. Supports translation
     * and symmetry transformation of source image region, specified by
     * <CODE>aSrcTransform</CODE>.
     */
    virtual TInt CopyRegion
    (
        const TAcceleratedBitmapInfo*   aDstBitmap,
        const TRect&                    aDstRect,
        const TAcceleratedBitmapInfo*   aSrcColorBitmap,
        const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
        TTransparency                   aSrcTransparency,
        const TRect&                    aSrcRect,
        TTransformType                  aSrcTransform,
        const TRect&                    aClipRect
    ) = 0;

    /*
     * Draw line from aStart to aEnd including both end points and
     * using line style TStrokeStyle.
     */
    virtual TInt DrawLine
    (
        const TAcceleratedBitmapInfo* aDstBitmap,
        const TPoint& aStart,
        const TPoint& aEnd,
        TUint32 aRGB,
        TStrokeStyle aStyle,
        const TRect& aClipRect
    ) = 0;

    /**
     * Draw outline of <CODE>aRect</CODE>
     */
    virtual TInt DrawRect
    (
        const TAcceleratedBitmapInfo* aDstBitmap,
        const TRect& aRect,
        TUint32      aRGB,
        TStrokeStyle aStyle,
        const TRect& aClipRect
    ) = 0;

    /**
     * Fill interior of <CODE>aRect</CODE> with color <CODE>aRGB</CODE>
     */
    virtual TInt FillRect
    (
        const TAcceleratedBitmapInfo* aDstBitmap,
        const TRect& aRect,
        TUint32      aRGB,
        const TRect& aClipRect
    ) = 0;

    /**
     * Draw the arc of an ellipse bounded by aBoundingRect in device coordinates,
     * starting the arc at aStartAngle from the ellipse horizontal axis and
     * extending for aArcAngle degrees anticlockwise. Draw with color aRGB and
     * clip to aClipRect in device coords.
     */
    virtual TInt DrawArc
    (
        const TAcceleratedBitmapInfo* aDstBitmap,
        const TRect& aBoundingRect,
        const TInt aStartAngle,
        const TInt aArcAngle,
        TUint32 aRGB,
        TStrokeStyle aStyle,
        const TRect& aClipRect
    ) = 0;

    /**
     * Fill the region bounded by an arc and the radii of its end points of an ellipse bounded
     * by aBoundingRect in device coordinates. The first radius lies at aStartAngle from the
     * ellipse horizontal axis and the second radies lies aArcAngle degrees anticlockwise
     * from the first. Fill with color aRGB and clip to aClipRect in device coords.
     */
    virtual TInt FillArc
    (
        const TAcceleratedBitmapInfo* aDstBitmap,
        const TRect& aBoundingRect,
        const TInt aStartAngle,
        const TInt aArcAngle,
        TUint32 aRGB,
        const TRect& aClipRect
    ) = 0;

    /**
     * Fill a triangle in device coordinates with color aRGB,
     * clipping to aClipRect in device coordinates.
     */
    virtual TInt FillTriangle
    (
        const TAcceleratedBitmapInfo* aDstBitmap,
        const TPoint aPoints[3],
        TUint32 aRGB,
        const TRect& aClipRect
    ) = 0;

    /**
     *
     */
    virtual TInt DrawText
    (
        const TAcceleratedBitmapInfo* aDstBitmap,
        const TDesC&  aText,
        const TPoint& aPoint,
        const CFont*  aFont,
        TUint32 aRGB,
        const TRect&  aClipRect
    ) = 0;

    /**
     * This function is used, when image is drawn and rendering
     * target is framebuffer of CanavsGraphicsItem.
     */
    virtual TInt DrawRegionForCanvasGraphicsItem
    (
        const TAcceleratedBitmapInfo*               /*aDstBitmap*/,
        const TRect&                                /*aDstRect*/,
        const TAcceleratedBitmapInfo*               /*aSrcColorBitmap*/,
        const TAcceleratedBitmapInfo*               /*aSrcAlphaBitmap*/,
        TTransparency                               /*aSrcTransparency*/,
        const TRect&                                /*aSrcRect*/,
        TTransformType                              /*aSrcTransform*/,
        const TRect&                                /*aClipRect*/,
        const TCanvasGraphicsItemOperationsType&    /*aOperation*/
    )
    {
        return KErrNone;
    }

};

#endif // LCDGDRV_H
