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


#ifndef LCDGDRVIF_H
#define LCDGDRVIF_H

#include <e32std.h>
#include <gdi.h>
#include <lcdgdrv.h>
#include "lcdgd.hrh"

class TAcceleratedBitmapInfo;
class TRect;
class TLcdTransform;

/**
 * Render function composites pixels with optional transformation
 * Target image is always opaque.
 * Source image is may have optional transparency bitmap, or may
 * contain transparency channel in color bitmap.
 */
typedef void (*TImageRenderFunction)
(
    const TAcceleratedBitmapInfo*   aDstColorBitmap,
    const TAcceleratedBitmapInfo*   aDstAlphaBitmap,
    const TRect&                    aDstRect,
    const TAcceleratedBitmapInfo*   aSrcColorBitmap,
    const TAcceleratedBitmapInfo*   aSrcAlphaBitmap,
    const TLcdTransform&            aTransform          // from dst to src coords
);

/**
 * Color map function maps a pixel from one RGB format to another.
 */
typedef TUint32(*TColorMapFunction)(TUint32);

/**
 *
 */
typedef TBool(*TCollisionDetectorFunction)
(
    const TAcceleratedBitmapInfo*   aBitmap1,
    const TAcceleratedBitmapInfo*   aBitmap2,
    const TRect&                    aRect1,
    const TRect&                    aRect2,
    const TLcdTransform&            aTransform1,
    const TLcdTransform&            aTransform2
);

/**
 * Quantize function maps color triplet on lattice of discreet 888-RGB space
 * to closest color in another discreet RGB space that can be mapped without
 * loss of information from a point on the lattice in 888-RGB space.
 */
typedef TUint32(*TQuantizeFunction)(TUint32);


/**
 * Rendering composition rule.
 */
enum TCompositRule
{
    ECompositeSrcCopy,  // copy source to destination
    ECompositeSrcOver,  // alpha blend source over destination, (also used for binary masks)
};

/**
 * Renderer descriptor
 */
struct TImageRenderer
{
    TImageType iTargetType;             // target image type.
    TImageType iSourceType;             // source image type
    TUint32 iTransformMask;             // supported symmetry transforms.
    TCompositRule iCompositeRule;       // composition rule
    TImageRenderFunction iFunction;     // draw source image type onto an iTargetMode bitmap with symmetry transform.
};

struct TColorMap
{
    TDisplayMode iTargetMode;
    TColorMapFunction iForward;     // RGB888 to iTargetMode
    TColorMapFunction iReverse;     // iTargetMode to RGB888
    TQuantizeFunction iQuantize;    // quantizes RGB888 color to closest color in target mode, returns RGB888
};

/**
 * CollisionDetector descriptor
 */
struct TCollisionDetector
{
    TUint iDisplayMode1;            // mask of supported display modes: bitwise OR of (1<<TDisplayMode)
    TUint iTransparency1;           // mask of supported transparencies: bitwise OR of (1<<TTransparency)
    TUint iDisplayMode2;            // mask of supported display modes: bitwise OR of (1<<TDisplayMode)
    TUint iTransparency2;           // mask of supported transparencies: bitwise OR of (1<<TTransparency)
    TCollisionDetectorFunction iFunction;
};

enum TTransformBit
{
    ETransNoneBit = (1<<ETransNone),
    ETransMirrorRot180Bit = (1<<ETransMirrorRot180),
    ETransMirrorBit = (1<<ETransMirror),
    ETransRot180Bit = (1<<ETransRot180),
    ETransMirrorRot270Bit = (1<<ETransMirrorRot270),
    ETransRot90Bit = (1<<ETransRot90),
    ETransRot270Bit = (1<<ETransRot270),
    ETransMirrorRot90Bit = (1<<ETransMirrorRot90)
};

enum TTransformMask
{
    ETransNoneMask = ETransNoneBit,
    ETransVertMask = ETransMirrorRot180Bit, // drawRGB/createRGBImage
    ETransStretch  = 0x100,                 // not supported yet!
    ETransAllMask  = ETransNoneBit|ETransMirrorRot180Bit|ETransMirrorBit|ETransRot180Bit|ETransMirrorRot270Bit|ETransRot90Bit|ETransRot270Bit|ETransMirrorRot90Bit
};

typedef void (*TDrawLineFunction)(const TAcceleratedBitmapInfo* aBitmap, const TPoint& aStart, const TPoint& aEnd, TUint32 aColor, TStrokeStyle aStyle, const TRect& aClipRect);
typedef void (*TDrawRectFunction)(const TAcceleratedBitmapInfo* aDstBitmap,const TRect& aRect,TUint32 aRGB,TStrokeStyle aStyle,const TRect& aClipRect);
typedef void (*TFillRectFunction)(const TAcceleratedBitmapInfo* aDstBitmap,const TRect& aRect,TUint32 aRGB,const TRect& aClipRect);
typedef void (*TDrawArcFunction)(const TAcceleratedBitmapInfo* aDstBitmap,const TRect& aBoundingRect,const TInt aStartAngle,const TInt aArcAngle,TUint32 aRGB,TStrokeStyle aStyle,const TRect& aClipRect);
typedef void (*TFillArcFunction)(const TAcceleratedBitmapInfo* aDstBitmap,const TRect& aBoundingRect,const TInt aStartAngle,const TInt aArcAngle,TUint32 aRGB,const TRect& aClipRect);
typedef void (*TFillTriangleFunction)(const TAcceleratedBitmapInfo* aBitmap, const TPoint aPoints[3], TUint32 aColor, const TRect& aClipRect);
typedef void (*TDrawTextFunction)(const TAcceleratedBitmapInfo* aBitmap, const TDesC& aText, const TPoint& aPoint, TUint32 aColor, const CFont* aFont, const TRect& aClipRect);

struct TDrawFunctions
{
    TDisplayMode iDisplayMode;
    TUint32 iDrawCaps;
    TDrawLineFunction iDrawLine;
    TDrawRectFunction iDrawRect;
    TFillRectFunction iFillRect;
    TDrawArcFunction iDrawArc;
    TFillArcFunction iFillArc;
    TFillTriangleFunction iFillTriangle;
    TDrawTextFunction iDrawText;
};


/**
 * ECOM plugin driver implementation base class.
 *
 * construction/factory methods, members and private/protected methods
 * may be changed providing the CLcdGraphicsDriver interface is maintained.
 */
class CLcdGraphicsDriverImpl : public CLcdGraphicsDriver
{
public:
    /**
     * Selects implementation dll
     */
    IMPORT_C static CLcdGraphicsDriverImpl* NewL(TDisplayMode aDisplayMode);

    IMPORT_C CLcdGraphicsDriverImpl
    (
        const TDriverInfo& aInfo,
        const TImageRenderer* aRendererArray,
        TInt aRendererCount,
        const TColorMap* aColorMapArray,
        TInt aColorMapCount,
        const TCollisionDetector* aCollisionDetectorArray,
        TInt aCollisionDetectorCount,
        const TDrawFunctions* aDrawFunctionsArray,
        TInt aDrawFunctionsCount
    );

    /**
     *
     */
    IMPORT_C ~CLcdGraphicsDriverImpl();

    /**
     *
     */
    IMPORT_C void  GetDriverInfo(CLcdGraphicsDriver::TDriverInfo& aDriverInfo) const;

    IMPORT_C TInt DrawRegion
    (
        const TAcceleratedBitmapInfo* aDstColorBitmap,
        const TRect&                  aDstRect,
        const TAcceleratedBitmapInfo* aSrcColorBitmap,
        const TAcceleratedBitmapInfo* aSrcAlphaBitmap,
        TTransparency                 aSrcTransparency,
        const TRect&                  aSrcRect,
        TTransformType                aSrcTransform,
        const TRect&                  aClipRect
    );

    IMPORT_C TInt CopyRegion
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
    );

    IMPORT_C TBool DetectCollision
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
    );

    IMPORT_C CLcdGraphicsDevice* CreateDeviceL(const TImageType& aImageType);

protected:
    /**
     *
     */
    IMPORT_C const TImageRenderer* GetRenderer
    (
        const TImageType& aDstType,
        const TImageType& aSrcType,
        TTransformType aTransform,
        TCompositRule aCompositRule
    ) const;

    /**
     *
     */
    IMPORT_C const TColorMap* GetColorMap(TDisplayMode aMode);

    /**
     *
     */
    IMPORT_C const TCollisionDetector* GetCollisionDetector
    (
        TDisplayMode    aDisplayMode1,
        TTransparency   aTransparency1,
        TDisplayMode    aDisplayMode2,
        TTransparency   aTransparency2
    ) const;

    IMPORT_C const TDrawFunctions* GetDrawFunctions(TDisplayMode aMode) const;

protected:
    const TDriverInfo       iDriverInfo;

    //
    // simple array representation
    //

    const TImageRenderer*   iRendererArray;
    const TInt              iRendererCount;

    const TColorMap*        iColorMapArray;
    const TInt              iColorMapCount;

    const TCollisionDetector* iCollisionDetectorArray;
    const TInt              iCollisionDetectorCount;

    const TDrawFunctions*   iDrawFunctionsArray;
    const TInt              iDrawFunctionsCount;

private:
    TUid iDtor_ID_Key;
};

const TInt KLcdImgVersionMajor = 0;
const TInt KLcdImgVersionMinor = 0;
const TUid KLcdDriverInterfaceUid = { LCDGD_DRIVER_INTERFACE_UID };

/**
 * Display mode to use for 32bpp ARGB8888 image data
 *
 * This format is not supported on all OS releases so we
 * select an appropriate format here.
 */
//
// EColor16MA is exact match
//
const TDisplayMode EColorARGB8888 = EColor16MA;

#ifdef _DEBUG
TBool DbgCheckBounds(const TSize& aDstSize, const TSize& aSrcSize, const TRect& aDstRect, const TLcdTransform& aTransform);
#endif

/**
 * driver capability macros
 */
#if defined(LCDGD_SUPPORT_1BPP_MASK_BITMAP)
#define CAP_1BPP_MASK_BITMAP CLcdGraphicsDriver::ECapTransparencyMask1Bpp
#else
#define CAP_1BPP_MASK_BITMAP 0
#endif

#if defined(LCDGD_SUPPORT_MATCHED_MASK_BITMAP)
#define CAP_MATCHED_MASK_BITMAP CLcdGraphicsDriver::ECapTransparencyMaskMatched
#else
#define CAP_MATCHED_MASK_BITMAP 0
#endif

#if defined(LCDGD_SUPPORT_ALPHA_BITMAP)
#define CAP_ALPHA_BITMAP CLcdGraphicsDriver::ECapTransparencyAlpha8Bpp
#else
#define CAP_ALPHA_BITMAP 0
#endif

#endif // LCDGDRVIF_H
