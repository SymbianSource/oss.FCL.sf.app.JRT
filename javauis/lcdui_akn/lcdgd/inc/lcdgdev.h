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


#ifndef LCDGDEV_H
#define LCDGDEV_H

#include <lcdgdrv.h>
#include "lcdgdrvif.h"

class TCompactImageType
{
public:
    inline TCompactImageType()
    {
    }

    inline TCompactImageType(const TImageType& aType)
    {
        iValue = aType.iColorMode | (aType.iAlphaMode << 8) | (aType.iTransparency << 16);
    }

    inline TCompactImageType(const TCompactImageType& aType)
    {
        iValue=aType.iValue;
    }

    inline TImageType Type() const
    {
        TImageType type;
        type.iColorMode = (TDisplayMode)(iValue & 0xff);
        type.iAlphaMode = (TDisplayMode)((iValue>>8) & 0xff);
        type.iTransparency = (TTransparency)((iValue>>16)&0xff);
        return type;
    }

    inline TUint32 Value() const
    {
        return iValue;
    }

    inline TBool operator == (const TCompactImageType& aType) const
    {
        return iValue == aType.iValue;
    }

private:
    TUint32 iValue;
};

inline TUint32 Hash(const TCompactImageType& aType)
{
    ASSERT(EColorLast <= 0x10);

    //
    // remove sparsity - convert to 12 bits packed, in practise we could be
    // much more aggressive since many of the displaymodes will never be
    // used.
    //
    TUint32 value = aType.Value();

    return (value & 0xf) | ((value >> 4)&0xf0) | ((value >> 8) & 0xf00);
}

class TRenderKey
{
public:
    TCompactImageType iTargetType;
    TCompactImageType iSourceType;
    TUint32 iTransform;
    TInt    iComposite;

    inline TBool operator == (const TRenderKey& aKey) const
    {
        return (iComposite   == aKey.iComposite) &&
               (iTransform  == aKey.iTransform) &&
               (iSourceType == aKey.iSourceType) &&
               (iTargetType == aKey.iTargetType);
    }

    inline TBool Match(const TRenderKey& aKey) const
    {
        return (iComposite   == aKey.iComposite) &&
               (iTransform  &  aKey.iTransform) &&
               (iSourceType == aKey.iSourceType) &&
               (iTargetType == aKey.iTargetType);
    }
};

//
//
//
class TRenderEntry
{
public:
    TImageRenderer iRenderer;
    TRenderKey iKey;

    inline TRenderKey Key() const
    {
        return iKey;
    }
};

class TRenderHash
{
public:
    inline TUint32 Hash(const TRenderKey& aKey)
    {
        return (::Hash(aKey.iTargetType) << 3)
               ^  ::Hash(aKey.iSourceType)
               ^  aKey.iComposite << 1
               ^  aKey.iTransform;
    }
};

//
// Device render functions supporting the target displaymode only.
//
class CRenderFunctions : public CBase
{
public:
    ~CRenderFunctions();
    const TImageRenderer* Get(const TRenderKey& aKey);
    void AppendL(const TImageRenderer& aRenderer);

private:
    RArray<TRenderEntry> iEntries;
    const TRenderEntry*  iLast;
};

class CLcdGraphicsDeviceImpl : public CLcdGraphicsDevice
{
public:
    CLcdGraphicsDeviceImpl
    (
        CLcdGraphicsDriver& aDriver,
        const TImageType& aTargetType,
        CRenderFunctions* aBitBltFunctions,
        const TColorMap& aColorMap,
        const TDrawFunctions& aDrawFunctions
    );
    ~CLcdGraphicsDeviceImpl();

    virtual TUint32 DrawingCaps() const;

    /**
     *@return closest RGB color on 888 lattice to aRGB that can
     * be mapped without loss of information between 888 RGB color
     * space and the RGB color space of the target drawing surface.
     */
    virtual TUint32 Quantize(TUint32 aRGB) const;

    /**
     * Transforming biblt from aColorBitmap/aAlphaBitmap to target surface
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
    );

    /**
     * Transforming biblt from <CODE>aSrcColorBitmap,aSrcAlphaBitmap<CODE>
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
    );

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
    );

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
    );

    /**
     * Fill interior of <CODE>aRect</CODE> with color <CODE>aRGB</CODE>
     */
    virtual TInt FillRect
    (
        const TAcceleratedBitmapInfo* aDstBitmap,
        const TRect& aRect,
        TUint32      aRGB,
        const TRect& aClipRect
    );

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
    );

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
    );

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
    );

    virtual TInt DrawText
    (
        const TAcceleratedBitmapInfo* aDstBitmap,
        const TDesC& aText,
        const TPoint& aPoint,
        const CFont* aFont,
        TUint32 aRGB,
        const TRect& aClipRect
    );

    /**
     * This function is used, when image is drawn and rendering
     * target is framebuffer of CanavsGraphicsItem.
     */
    virtual TInt DrawRegionForCanvasGraphicsItem
    (
        const TAcceleratedBitmapInfo*               aDstBitmap,
        const TRect&                                aDstRect,
        const TAcceleratedBitmapInfo*               aSrcColorBitmap,
        const TAcceleratedBitmapInfo*               aSrcAlphaBitmap,
        TTransparency                               aSrcTransparency,
        const TRect&                                aSrcRect,
        TTransformType                              aSrcTransform,
        const TRect&                                aClipRect,
        const TCanvasGraphicsItemOperationsType&    aOperation
    );

private:

    // support for rendering image on CanavsGraphicsItem frame buffer
    TInt PixelPitch(const TAcceleratedBitmapInfo* aBitmap);

    // support for rendering image on CanavsGraphicsItem frame buffer
    void CLcdGraphicsDeviceImpl::DoBlit
    (
        const TAcceleratedBitmapInfo*               aDstColorBitmap,
        const TRect&                                aDstRect,       // must be clipped to destination
        const TAcceleratedBitmapInfo*               aSrcColorBitmap,
        const TLcdTransform&                        aTransform,     // includes anchor
        const TCanvasGraphicsItemOperationsType&    aOperation
    );

    // support for rendering image on CanavsGraphicsItem frame buffer
    void CLcdGraphicsDeviceImpl::DoBlitLineForImage
    (
        TUint8* aDstAddress,
        TInt    aWidth,
        TUint8* aColorAddress,
        TInt    aColorPixelPitch
    );

    // support for rendering image on CanavsGraphicsItem frame buffer
    void CLcdGraphicsDeviceImpl::DoBlitLineForRgb
    (
        TUint8* aDstAddress,
        TInt    aWidth,
        TUint8* aColorAddress,
        TInt    aColorPixelPitch
    );

protected:
    CLcdGraphicsDriver& iDriver;
    TRenderKey          iRenderKey;
    CRenderFunctions*   iRenderers;
    const TColorMap&    iColorMap;
    const TDrawFunctions& iDrawFunctions;
};


#endif // LCDGDEV_H
