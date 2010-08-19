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

#ifndef CMIDGRAPHICS_H
#define CMIDGRAPHICS_H

#include <lcdgr.h>

class CFbsBitmap;
class CMIDSprite;
class CMIDTiledLayer;
class CMIDProxyMap;
class CLcdGraphics;

/**
 * Proxy to native peer to javax.microedition.lcdui.Graphics.
 *
 * Presents MMIDGraphics interface for compatability.
 */
NONSHARABLE_CLASS(CMIDGraphics) : public CBase
        , public MMIDGraphics
        , public MMIDBufferProcessor
{
public:
#ifdef RD_JAVA_NGA_ENABLED
    /**
     * Constructor
     * @param aProxyMap
     *      - a proxy
     * @param aGraphics
     *      - a graphics implementation
     * @param aIsImageTarget
     *      - true if an image is the target
     */
    CMIDGraphics(
        CMIDProxyMap& aProxyMap, CLcdGraphics* aGraphics,
        TBool aIsImageTarget, MMIDCanvas* aTarget = NULL);

#else RD_JAVA_NGA_ENABLED

    /**
     * Constructor
     * @param aProxyMap a proxy
     * @param aGraphics a graphics implementation
     * @param aIsImageTarget true if an image is the target
     */
    CMIDGraphics(
        CMIDProxyMap& aProxyMap, CLcdGraphics* aGraphics,
        TBool aIsImageTarget);
#endif // RD_JAVA_NGA_ENABLED

    /**
     * Dtor
     */
    ~CMIDGraphics();

    /**
     * From MMIDComponent
     */
    virtual TType Type() const;
    virtual void Dispose();
    virtual MMIDBufferProcessor* Processor();

    /**
     * From MMIDBufferProcessor
     */
#ifdef RD_JAVA_NGA_ENABLED
    virtual TBool ProcessL(
        const TMIDBufferOp*& aRead, const TMIDBufferOp* aEnd,
        TInt& aCycles, java::util::Monitor* aMonitor);
#else
    virtual TBool   ProcessL(
        const TMIDBufferOp*& aRead, const TMIDBufferOp* aEnd,
        TInt& aCycles, TRequestStatus* aStatus = NULL);
#endif
    virtual void    AbortAsync();


    /**
     *@return aColor quantized to device palette.
     */
    virtual TUint32 DisplayColor(TUint32 aColor);

    /**
     * From MMIDGraphics
     */
    virtual TInt DrawPixels(
        TInt    aType,          // pixel data type
        TUint8* aAddress,       // pixel data array
        TInt    aLength,        // pixel array length (bytes)
        TInt    aScanLength,    // (bytes) always positive - vertical reflection handled by point/size
        TBool   aAlpha,         // true if pixel data contains alpha
        const TRect& aRect,     // destination rect
        TInt    aTransform      // transform to apply
    );

    /**
     *@return bitmap handle if rendering to bitmap
     */
    virtual CFbsBitmap* Bitmap() const;

    /**
     *@return alpha bitmap handle or NULL if there is none
     */
    inline CFbsBitmap*  BitmapAlpha() const;

    /**
     *@return window handle if rendering to window.
     */
    virtual RWindow* Window() const;

    /**
     *@return drawing surface viewport
     */
    virtual TRect ViewPort() const;

    /**
     *@return ETrue for mutable off-screen images (not for canvas/GameCanvas framebuffer)
     * or EFalse for canvas/GameCanvas framebuffer
     */
    TBool IsImageTarget() const;

#ifdef RD_JAVA_NGA_ENABLED

public: // from MMIDComponentNgaExtension
    /**
     * @see MMIDComponentNgaExtension#UpdateEglContent()
     * @since S60 9.2
     */
    virtual void UpdateEglContent();

    /**
     * @see MMIDComponentNgaExtension#IsEglAvailable()
     * @since S60 9.2
     */
    virtual TBool IsEglAvailable() const;

    /**
     * @see MMIDComponentNgaExtension#BindEglSurface()
     * @since S60 9.2
     */
    virtual EGLSurface BindEglSurface();

    /**
     * @see MMIDComponentNgaExtension#ReleaseEglSurface()
     * @since S60 9.2
     */
    virtual void ReleaseEglSurface();

    /**
     * @see MMIDComponentNgaExtension#UpdateOffScreenBitmapL()
     * @since S60 9.2
     */
    virtual void UpdateOffScreenBitmapL(TBool aForced);

    /**
     * @see MMIDComponentNgaExtension#FillEglSurface()
     * @since S60 9.2
     */
    virtual TBool FillEglSurface(const TRect& aRect, const TRgb& aColor);

    /**
     * @see MMIDGraphics#TargetSize()
     * @since S60 9.2
     */
    virtual TSize CanvasTargetSize() const;

    /**
     * @see MMIDGraphics#GetTargetCanvas()
     * @since S60 9.2
     */
    virtual MMIDCanvas* GetTargetCanvas() const;

private:
    /**
     * Merges aRect to member variable TRects that are used for
     * tracking the area that is updated with 2D content.
     * @param aRect TRect that contains new area that has been updated
     *              with 2D content.
     */
    void UpdateRect(const TRect& aRect);

    /**
     * Calculate bounding box for text.
     * @param aTextPtr a text to be drawn
     * @param aP a start point
     * @param aAnchor an anchor that effects to text's start point.
     */
    void UpdateTextRect(TPtrC& aTextPtr, const TPoint& aP, TInt aAnchor);

    /**
     * Finds the minimum of three integers
     * @param aX -
     * @param aY -
     * @param aZ -
     * @return the min of the three attributes
     */
    inline TInt Min3(TInt aX, TInt aY, TInt aZ);

    /**
     * Finds the maximum of three integers
     * @param aX -
     * @param aY -
     * @param aZ -
     * @return the max of the three attributes
     */
    inline TInt Max3(TInt aX, TInt aY, TInt aZ);

private:
    /**
     * Target Canvas of this graphics object.
     * NULL, when this is not canvas graphics.
     */
    MMIDCanvas* iCanvasTarget;
#endif // RD_JAVA_NGA_ENABLED

private:
    CMIDProxyMap& iProxyMap;
    CLcdGraphics* iGraphics;
    // this flag is for mutable off-screen images (not for canvas/GameCanvas framebuffer)
    TBool iIsImageTarget;
};

inline CFbsBitmap* CMIDGraphics::BitmapAlpha() const
{
    return NULL;
}

#ifdef RD_JAVA_NGA_ENABLED
// ---------------------------------------------------------------------------
// CMIDGraphics::Min3
// ---------------------------------------------------------------------------
//
inline TInt CMIDGraphics::Min3(TInt aX, TInt aY, TInt aZ)
{
    return Min(Min(aX, aY), aZ);
}

// ---------------------------------------------------------------------------
// CMIDGraphics::Max3
// ---------------------------------------------------------------------------
//
inline TInt CMIDGraphics::Max3(TInt aX, TInt aY, TInt aZ)
{
    return Max(Max(aX, aY), aZ);
}
#endif // RD_JAVA_NGA_ENABLED

#endif // CMIDGRAPHICS_H

