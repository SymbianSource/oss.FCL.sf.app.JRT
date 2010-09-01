/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef ESWTGRAPHICS_H
#define ESWTGRAPHICS_H


#include <e32base.h>
#include "eswt.h"


class MSwtGc;
class TRequestStatus;


/**
 * Utility base class for managing reference-counted objects
 */
class MSwtRefCounted
{
protected:
    inline MSwtRefCounted();
    virtual ~MSwtRefCounted() {};

public:
    inline void AddRef()    const;
    inline void RemoveRef() const;
    inline TInt RefCount()  const;

private:
    mutable TInt iRefCount;
};


/**
 * Initialises the reference count to 1.
 */
inline MSwtRefCounted::MSwtRefCounted()
        : iRefCount(1)
{
}

/**
 * Increases the object's reference count by 1.
 */
inline void MSwtRefCounted::AddRef() const
{
    ++iRefCount;
}

/**
 * Decreases the object's reference count by 1.
 * If the reference count reaches 0, the object is destroyed.
 */
inline void MSwtRefCounted::RemoveRef() const
{
    if (--iRefCount == 0)
        delete const_cast<MSwtRefCounted*>(this);
}

/**
 * Returns the object's current reference count.
 */
inline TInt MSwtRefCounted::RefCount() const
{
    return iRefCount;
}

/**
 * Object containing the data needed for the
 * org.eclipse.swt.graphics.PaletteData class.
 */
class MSwtPaletteData
{
// Data types
public:
    /**
     * Data structure for direct palettes
     *
     * To extract a component from a colour: first apply the mask and then
     * shift the result by the specified value.
     *
     * To combine components in order to build a colour, for each component:
     * first shift by the opposite of the specified value and then apply the
     * mask. Finally, combine all three components with a OR operator.
     *
     * In both cases, a positive value indicates a left shift whereas a
     * negative one indicates a right shift.
     */
    struct TDirectData
    {
        TInt iRedMask;    // The red mask for a direct palette.
        TInt iGreenMask;  // The green mask for a direct palette.
        TInt iBlueMask;   // The blue mask for a direct palette.
        TInt iRedShift;   // The red shift for a direct palette.
        TInt iGreenShift; // The green shift for a direct palette.
        TInt iBlueShift;  // The blue shift for a direct palette.
    };

// Methods
public:
    virtual ~MSwtPaletteData() {}

    /**
     * Test whether a palette is a direct or an indirect one
     */
    virtual TBool IsDirect() const =0;

    /**
     * Returns the data of a direct palette
     */
    virtual const TDirectData& DirectData() const =0;

    /**
     * Returns the data of an indirect palette
     */
    virtual const CPalette* IndirectData() const =0;
};

/**
 * Object containing the data needed for the
 * org.eclipse.swt.graphics.ImageData class.
 */
class MSwtImageData
{
// Data types
public:
    struct TInfo
    {
        TSize            iSize;             // The size of the image, in pixels.
        TInt             iDepth;            // The color depth of the image, in bits per pixel.
        TInt             iScanlinePad;      // The scanline padding.
        TInt             iBytesPerLine;     // The number of bytes per scanline.
        TInt             iTransparentPixel; // The transparent pixel.
        TInt             iMaskPad;          // An icon-specific field containing the scanline pad of the mask.
        TInt             iAlpha;            // The global alpha value to be used for every pixel.
        TSwtImageType    iType;             // The type of file from which the image was read.
        TPoint           iTopLeft;          // The coordinates of the top-left corner of the image within the logical screen.
        TSwtGifDisposal  iDisposalMethod;   // A description of how to dispose of the current image before displaying the next.
        TInt             iDelayTime;        // The time to delay before displaying the next image in an animation
    };

// Methods
public:
    virtual ~MSwtImageData() {}
    virtual const TInfo&           Info()        const =0;
    virtual const MSwtPaletteData& Palette()     const =0;
    virtual const HBufC8&          PixelBuffer() const =0;
    virtual const HBufC8*          MaskBuffer()  const =0;
    virtual const HBufC8*          AlphaBuffer() const =0;
};

/**
 * Interface class for the
 * org.eclipse.swt.graphics.Color class.
 */
class MSwtColor
{
protected:
    virtual ~MSwtColor() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Disposes of (i.e. destroys) the colour
     */
    virtual void Dispose() =0;

    /**
     * Retrieves the RGB value representing the colour such as it would
     * appear on the associated device.
     */
    virtual TRgb RgbValue() const =0;
};

/**
 * Interface class for the org.eclipse.swt.graphics.Drawable interface.
 * Devices and GCs have been restricted to bitmap ones rather than fully
 * generic ones as no printer support is available in eSWT.
 */
class MSwtDrawable
{
protected:
    virtual ~MSwtDrawable() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Creates a new graphics context to draw on.
     */
    virtual MSwtGc* NewGcL() =0;

    /**
     * The native device used by this drawable
     */
    virtual CBitmapDevice& GraphicsDevice() =0;

    /**
     * Called after the Drawable's content has been modified.
     */
    virtual void HandleUpdate() =0;
};

/**
 * Interface class for the org.eclipse.swt.graphics.Drawable
 * interface as applied to the window server's GCs.
 */
class MSwtWindowDrawable : public MSwtDrawable
{
protected:
    virtual ~MSwtWindowDrawable() {} // Made protected to prevent destruction through the interface
};

/**
 * Interface class for the org.eclipse.swt.graphics.Drawable
 * interface as applied to the font & bitmap server's GCs.
 */
class MSwtBitmapDrawable : public MSwtDrawable
{
protected:
    virtual ~MSwtBitmapDrawable() {} // Made protected to prevent destruction through the interface
};

/**
 * Interface class for the org.eclipse.swt.graphics.Font class.
 * Fonts are not intended to be disposed of while they are referenced by some
 * control and/or GC. Nonetheless, a broken application may loosely follow the
 * SWT coding guidelines and fail to do so, thereby causing an access violation.
 * In order to prevent such behaviour, fonts are reference counted, see
 * MSwtRefCounted.
 */
class MSwtFont : public MSwtRefCounted
{
protected:
    /**
     * Destructor
     */
    virtual ~MSwtFont() {};

public:

    /**
     * Disposes of (i.e. destroys) the font.
     * The font is disposed of by calling its RemoveRef() method.
     * It is therefore an error to try and dispose of a font whose reference
     * count is higher than 1.
     */
    virtual void Dispose() =0;

    /**
     * Returns the native font object.
     */
    virtual const CFont& Font() const =0;

    /**
     * Returns the height of the native font, in points.
     * The design height of a font is the "aesthetic" height specified by the designer
     * and not the actual "physical" height. See Symbian's Improved_Font_Metrics_How_To.doc
     */
    virtual TInt DesignHeightInPoints() const =0;

    /**
     * Returns the typeface name of the native font.
     * The new descriptor should then be freed when no more needed.
     * If the font has been substituted, this function will return the original
     * name used to create the font, not the substituted name.
     */
    virtual HBufC* GetNameL() const =0;

    /**
     * Returns the style of the native font.
     */
    virtual TInt Style() const =0;

    /**
     * Returns the width in pixels of the specified descriptor when displayed in this font.
     */
    virtual TInt TextWidthInPixels(const TDesC& aText) const=0;
};

/**
 * Structure containing the data needed for the
 *        org.eclipse.swt.graphics.FontMetrics class.
 */
struct TSwtFontMetrics
{
    TInt iAscent;           // Ascent of the font, measured in pixels.
    TInt iAverageCharWidth; // Average character width, measured in pixels.
    TInt iDescent;          // Descent of the font, measured in pixels.
    TInt iHeight;           // Height of the font, measured in pixels.
    TInt iLeading;          // Leading area of the font, measured in pixels.
};


struct TSwtFontData
{
    TFontSpec iFontSpec;
};


/**
 * Interface class for the org.eclipse.swt.graphics.Device class.
 */
class MSwtDevice : public MSwtBitmapDrawable
{
protected:
    virtual ~MSwtDevice() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Creates a Color object for this device.
     */
    virtual MSwtColor* CreateColorL(const TRgb& aRgbValue) const =0;

    /**
     * Returns a rectangle describing device's size and location.
     */
    virtual TRect Bounds() const =0;

    /**
     * Returns a rectangle which describes the area of the device which
     * is capable of displaying data.
     */
    virtual TRect ClientArea() const =0;

    /**
     * Returns the bit depth of the screen.
     */
    virtual TInt Depth() const =0;

    /**
     * Returns the dots per inch resolution of the device.
     */
    virtual TSize Dpi() const =0;

    /**
     * Returns font data objects which describe the fonts that match
     * the given arguments. For each font, the available heights will be provided.
     * @param aFaceName The name of the font to look for. If empty, all fonts
     *        will be returned.
     * @param aScalable If true only scalable fonts are returned, otherwise
     *        only non-scalable fonts are returned.
     * @return An array containing the TSwtFontData objects
     *         describing each font.
     */
    virtual CArrayFixFlat<TSwtFontData>* GetFontListL(
        const TDesC& aFaceName, TBool aScalable) const=0;

    /**
     * Returns the matching standard color for the given constant.
     * @param aId SWT color constant
     * @return The Symbian OS' matching standard color. Any value other
     *         than one of the SWT color constants which is passed
     *         in will result in the color black.
     */
    virtual TRgb GetSystemColor(TSwtColorId aId) const =0;

    /**
     * Returns a reasonable font for applications to use.
     * @return The system font, cannot be NULL
     */
    virtual const MSwtFont* GetSystemFont() const =0;

    /**
     * Returns the default height of a font.
     * This height is used for the Java FontData constructor with no argument.
     */
    virtual TInt GetDefaultFontHeightL() =0;
};


/**
 * Interface class for the org.eclipse.swt.graphics.Image class.
 *
 * Images are not intended to be disposed of while they are referenced by some
 * control and/or GC. Nonetheless, a broken application may loosely follow the
 * SWT coding guidelines and fail to do so, thereby causing an access violation.
 *
 * In order to prevent such behaviour, images are reference counted, see
 * MSwtRefCounted.
 */
class MSwtImage
        : public MSwtBitmapDrawable
        , public MSwtRefCounted
{
protected:
    virtual ~MSwtImage() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Disposes of (i.e. destroys) the image.
     * The image is disposed of by calling its RemoveRef() method.
     * It is therefore an error to try and dispose of an image whose reference
     * count is higher than 1.
     */
    virtual void Dispose() =0;

    /**
     * Retrieves the underlying native bitmap representing the alpha channel
     * @param aInvertedIfMonochrome if true and the main mask is monochrome
     *        the result bitmap is inverted. if false or if main mask is not
     *        monochrome, the returned bitmap is not inverted.
     * The returned value may be NULL.
     * Call this only after calling SubBitmap!
     */
    virtual const CFbsBitmap* MaskBitmap(TBool aInvertedIfMonochrome = EFalse) const =0;

    /**
     * Retrieves a reference to a scaled copy of the main mask.
     * @param aInvertedIfMonochrome if true and the main mask is monochrome
     *        the result bitmap is inverted. if false or if main mask is not
     *        monochrome, the returned bitmap is not inverted.
     * The returned value may be NULL.
     * Call this only after calling SubBitmap!
     */
    virtual const CFbsBitmap* SubMaskBitmap(const TSize& aSize,
                                            TBool aInvertedIfMonochrome = EFalse) const =0;

    /**
     * Returns the bounds of the image.
     * The iTl member of the return value is always (0,0).
     */
    virtual TRect GetBounds() const =0;

    /**
     * Returns the image's data.
     */
    virtual MSwtImageData* GetImageDataL() const =0;

protected:
    virtual CFbsBitmap& GetBitmap() const =0;
    virtual CFbsBitmap& GetSubBitmap(const TSize& aSize) const =0;

public:
    /**
     * Retrieves a const reference to the underlying native bitmap.
     */
    inline const CFbsBitmap& Bitmap() const
    {
        return GetBitmap();
    }

    /**
     * Retrieves a reference to the underlying native bitmap.
     */
    inline CFbsBitmap& Bitmap()
    {
        return GetBitmap();
    }

    /**
     * Retrieves a const reference to a scaled copy of the native bitmap.
     * Allocates memory in the object even if the object is const (see mutable).
     * The allocated copy is owned by the image object.
     * If memory allocation or bitmap scaling fails, NULL is returned.
     * If aSize equals the size of the main bitmap, the main bitmap is returned.
     * Call this before calling SubMaskBitmap!
     */
    inline const CFbsBitmap& SubBitmap(const TSize& aSize) const
    {
        return GetSubBitmap(aSize);
    }

    /**
     * Retrieves a reference to a scaled copy of the native bitmap.
     * Allocates memory in the object even if the object is const (see mutable).
     * The allocated copy is owned by the image object.
     * If memory allocation or bitmap scaling fails, NULL is returned.
     * If aSize equals the size of the main bitmap, the main bitmap is returned.
     * Call this before calling SubMaskBitmap!
     */
    inline CFbsBitmap& SubBitmap(const TSize& aSize)
    {
        return GetSubBitmap(aSize);
    }

public:
    /**
     * Means for clients to register their use of a scaled bitmap copy.
     * If aSize equals the size of the main bitmap, the main bitmap is referenced.
     * @return KErrNotFound if a copy of that size not found.
     *         KErrNone if reference added ok.
     */
    virtual TInt AddSubRef(const TSize& aSize) const =0;

    /**
     * Means for clients to deregister their use of a scaled bitmap copy.
     * If aSize equals the size of the main bitmap, the main bitmap is dereferenced.
     * If no clients left for that copy, the bitmap copy is destroyed.
     * @return KErrNotFound if a copy of that size not found.
     *         KErrNone if reference removed ok.
     */
    virtual TInt RemoveSubRef(const TSize& aSize) const =0;

    /**
     * Return the number of clients using the a scaled bitmap copy.
     * If aSize equals the size of the main bitmap, the main reference count is returned.
     */
    virtual TInt SubRefCount(const TSize& aSize) const = 0;

    /**
     * Create a bitmap with alpha channel based on underlying native bitmap.
     */
    virtual CFbsBitmap* BitmapWithAlphaLC() = 0;
};


/**
 * Interface class for the org.eclipse.swt.graphics.GC class.
 */
class MSwtGc
{
protected:
    virtual ~MSwtGc() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Signature for a function destroying native GCs.
     * Must not leave.
     */
    typedef void (*TDestructor)(CBitmapContext* aGc);

public:
    /**
     * Disposes of (i.e. destroys) the GC
     */
    virtual void Dispose() =0;

    /**
     * The native bitmap context used by this GC
     */
    virtual CBitmapContext& BitmapContext() =0;
    /**
     * Sets alpha value.
     */
    virtual void SetAlpha(TInt aAlpha) =0;

    /**
     * Sets the background color.
     */
    virtual void SetBackground(const MSwtColor& aColor) =0;

    /**
     * Sets the foreground color.
     */
    virtual void SetForeground(const MSwtColor& aColor) =0;

    /**
     * Returns the current line style.
     */
    virtual TSwtLineStyle LineStyle() const =0;

    /**
     * Sets the current line style.
     */
    virtual void SetLineStyleL(TSwtLineStyle aLineStyle) =0;

    /**
     * Returns the width that will be used when drawing lines.
     */
    virtual TInt LineWidth() const =0;

    /**
     * Sets the width that will be used when drawing lines.
     */
    virtual void SetLineWidth(TInt aLineWidth) =0;

    /**
     * Returns ETrue if this GC is drawing in X-OR mode,
     *        EFalse otherwise.
     */
    virtual TBool XORMode() const =0;

    /**
     * Sets the drawing mode to X-Or or plain.
     */
    virtual void SetXORMode(TBool aXor) =0;

    /**
     * Sets the current font.
     */
    virtual void SetFont(const MSwtFont* aFont) =0;

    /**
     * Copies a rectangular area into the provided Image.
     * @param aImage The image to copy data to
     * @param aPoint The position of the data to be copied
     */
    virtual void CopyAreaL(MSwtImage& aImage, const TPoint& aPoint) const =0;

    /**
     * Copies a rectangular area to a given position.
     */
    virtual void CopyArea(const TRect& aSource, const TPoint& aDestination) =0;

#ifdef RD_JAVA_NGA_ENABLED
    /**
     * Ensure that all drawing commands have been issued and
     * alf side has finished rendering.
     */
    virtual TInt FinishOperationL(void) =0;
#endif // RD_JAVA_NGA_ENABLED

    /**
     * Calls DrawImageFromBitmaps() to draw image that was sent from M2G.
     * @param aBitmapHandles    Contains handles for image bitmap and mask of the
     *                          image.
     * @param aSrcePos          The position of the rectangle in the source image
     * @param aSrceSize         The size of the rectangle in the source image;
     *                          if (-1,-1) the whole image is to be taken.
     * @param aDestPos          The position to draw at
     * @param aDestSize         The size of the drawn area; if (-1,-1) then the same
     *                          size as the source is used.
     * @param aUseNativeClear   Used while drawinq M2G animations to clear the
     *                          background.
     */
    virtual void DrawM2GImageL(const TInt* aBitmapHandles, const TPoint& aSrcePos,
                               const TSize& aSrceSize, const TPoint& aDestPos, const TSize& aDestSize,
                               const TBool aUseNativeClear) =0;

    /**
    * Calls DrawImageFromBitmaps() method to draw a portion of an image.
    * @param aImage    The image to draw.
    * @param aSrcePos  The position of the rectangle in the source image
    * @param aSrceSize The size of the rectangle in the source image;
    *                  if (-1,-1) the whole image is to be taken.
    * @param aDestPos  The position to draw at
    * @param aDestSize The size of the drawn area; if (-1,-1) then the same
    *                  size as the source is used.
    */
    virtual void DrawImage(const MSwtImage& aImage, const TPoint& aSrcePos,
                           const TSize& aSrceSize, const TPoint& aDestPos, const TSize& aDestSize) =0;

    /**
     * Does the actual drawing of portion of a an the image at the specified
     * coordinates. If the source and destination rectangle do not have the same size,
     * the image will be scaled accordingly.
     * @param aBitmap           The bitmap of the image to draw.
     * @param aMask             The mask for the bitmap.
     * @param aSrcePos          The position of the rectangle in the source image
     * @param aSrceSize         The size of the rectangle in the source image;
     *                          if (-1,-1) the whole image is to be taken.
     * @param aDestPos          The position to draw at
     * @param aDestSize         The size of the drawn area; if (-1,-1) then the same
     *                             size as the source is used.
     * @prama aUseNativeClear   Used while drawinq M2G animations to clear the background
     */
    virtual void DrawImageFromBitmaps(const CFbsBitmap& aBitmap, const CFbsBitmap* aMask,
                                      const TPoint& aSrcePos, const TSize& aSrceSize, const TPoint& aDestPos,
                                      const TSize& aDestSize, const TBool aUseNativeClear) =0;

    /**
     * Draws a pixel, using the current foreground colour, at the specified point.
     */
    virtual void DrawPoint(const TPoint& aPoint) =0;

    /**
     * Draws a line, using the current foreground colour, between two points.
     */
    virtual void DrawLine(const TPoint& aPoint1, const TPoint& aPoint2) =0;

    /**
     * Draws the polyline which is defined by the specified array of
     * points, using the current foreground colour.
     */
    virtual void DrawPolyline(const TPoint* aPoints, TInt aCount) =0;

    /**
     * Draws the closed polygon which is defined by the specified array
     * of points, using the current foreground colour.
     */
    virtual void DrawPolygon(const TPoint* aPoints, TInt aCount) =0;

    /**
     * Fills the interior of the closed polygon which is defined by the
     * specified array of points, using the current background color.
     */
    virtual void FillPolygon(const TPoint* aPoints, TInt aCount) =0;

    /**
     * Draws the outline of a rectangle, using the current foreground colour.
     */
    virtual void DrawRectangle(const TRect& aRect) =0;

    /**
     * Fills the interior of a rectangle, using the current background colour.
     */
    virtual void FillRectangle(const TRect& aRect) =0;

    /**
     * Fills the interior of the specified rectangle with a gradient
     * between the current foreground and background colours.
     */
    virtual void FillGradientRectangle(const TRect& aRect, TBool aVertical,
                                       TBool aInvertGradient) =0;

    /**
     * Draws the outline of a round-cornered rectangle, using the current
     * foreground colour.
     */
    virtual void DrawRoundRectangle(const TRect& aRect, const TSize& aArcSize) =0;

    /**
     * Fills the interior of a round-cornered rectangle, using the
     * current background colour.
     */
    virtual void FillRoundRectangle(const TRect& aRect, const TSize& aArcSize) =0;

    /**
     * Draws the outline of an oval, using the current foreground colour,
     * within the specified rectangular area.
     */
    virtual void DrawOval(const TRect& aRect) =0;

    /**
     * Fills the interior of an oval, within the specified rectangular
     * area, with the current background colour.
     */
    virtual void FillOval(const TRect& aRect) =0;

    /**
     * Draws the outline of a circular or elliptical arc within the specified area.
     * The resulting arc begins at aStartAngle and extends for ArcAngle
     * degrees, using the current colour. Angles are interpreted such that 0
     * degrees is at the 3 o'clock position. A positive value indicates a
     * counter-clockwise rotation while a negative value indicates a clockwise
     * rotation.
     * @param aRect       The bounding rectangle of the ellipse the arc is part of.
     * @param aStartAngle The arc's start angle, in degrees
     * @param aArcAngle   The arc's aperture, in degrees
     */
    virtual void DrawArc(const TRect& aRect, TInt aStartAngle, TInt aArcAngle) =0;

    /**
     * Fills the interior of a circular or elliptical arc within the
     * specified rectangular area, with the receiver's background colour.
     * @param aRect       The bounding rectangle of the ellipse the arc is part of.
     * @param aStartAngle The arc's start angle, in degrees
     * @param aArcAngle   The arc's aperture, in degrees
     */
    virtual void FillArc(const TRect& aRect, TInt aStartAngle, TInt aArcAngle) =0;

    /**
     * Returns the width (in pixels) of a character in the current font.
     * @param aChar The character whose width to retrieve
     */
    virtual TInt GetCharWidth(const TChar& aChar) const =0;

    /**
     * Returns the advance width (in pixels) of a character in the current font.
     * @param aChar The character whose advance width to retrieve
     */
    virtual TInt GetAdvanceWidth(const TChar& aChar) const =0;

    /**
     * Returns information about the current font.
     * @param aMetrics The structure to fill with metrics information
     */
    virtual void GetFontMetrics(TSwtFontMetrics& aMetrics) const =0;

    /**
     * Returns the extent of the given string.
     * Tab expansion, line delimiter and mnemonic processing are performed
     * according to the specified flags.
     * @param aText     The text to be measured.
     * @param aFlags    Flags, can be any combination of KSwtDrawTransparent,
     *                  KSwtDrawDelimiter, KSwtDrawTab and
     *                  KSwtDrawMnemonic.
     */
    virtual TSize TextExtentL(TDes& aText, TInt aFlags) const =0;

    /**
     * Draws the given text, using the current font and foreground colour.
     * Tab expansion, line delimiter and transparent processing are performed
     * according to the specified flags. Mnemonics is not supported by Symbian.
     * @param aText     The text to be drawn
     * @param aPosition The position to draw the text at
     * @param aFlags    Flags, can be any combination of KSwtDrawTransparent,
     *                  KSwtDrawDelimiter, KSwtDrawTab and
     *                  KSwtDrawMnemonic.
     */
    virtual void DrawTextL(TDes& aText, const TPoint& aPosition, TInt aFlags) =0;

    /**
     * Returns ETrue if this GC has a clipping region set
     * to it, and EFalse otherwise.
     */
    virtual TBool IsClipped() const =0;

    /**
     * Returns the bounding rectangle of the clipping region.
     * The returned rectangle is expressed in GC coordinates. If no clipping
     * region is set, the returned rectangle is the GC's.
     */
    virtual TRect ClippingRect() const =0;

    /**
     * Sets the clipping region to the specified rectangle.
     * @param aRect The clipping rectangle to set, in GC coordinates.
     */
    virtual void SetClippingRect(const TRect& aRect) =0;

    /**
     * Cancels the clipping region
     */
    virtual void CancelClipping() =0;

    /**
     * Returns the font.
     */
    virtual const MSwtFont* Font() const =0;

    /**
     * Draw a rectangle which has the appearance of the platform's
     * focus rectangle.
     * If there is no such thing as a focus rectangle, this method must draw
     * a rectangle with the GC's foreground colour.
     * @param aRect The rectangle to draw
     */
    virtual void DrawFocusRect(const TRect& aRect, const MSwtUiUtils& aUtils) =0;

    /*
     * Sets the current brush rgb value directly to the given value
     * without any color transformation.
     * @param aRgb the value to set
     */
    virtual void SetBrushRgbValue(const TRgb& aRgb) =0;

    /*
     * Sets the current pen rgb value directly to the given value
     * without any color transformation.
     * @param aRgb the value to set
     */
    virtual void SetPenRgbValue(const TRgb& aRgb) =0;
};


typedef CArrayPtrFlat<MSwtImageData> CSwtImageDataArray;

/**
 * Interface class for the org.eclipse.swt.graphics.ImageDataLoader class.
 */
class MSwtImageDataLoader
{
protected:
    virtual ~MSwtImageDataLoader() {} // Made protected to prevent destruction through the interface

public:

    /**
     * Function in charge of decoding an image from ImageData.
     * If any error happened due to an IO failure the method leaves. If the
     * destination already exists the method leaves.
     * @param aImageData   The image's data.
     * @param aFormat      The destination format of the image (see TSwtImageType).
     *                     Possible types are PNG, JPG and GIF.
     * @param aDestination The pathname and the file's name of the result file.
     */
    virtual void EncodeImageToFileL(MSwtImageData& aImageData, TInt aFormat, const TDesC& aDestination) =0;

    /**
     * Function in charge of decoding an image from an ImageData.
     * If any error happened due to an IO failure the method leaves. If the
     * destination already exists the  method leaves.
     * @return A buffer owned by the object containing the data.
     * @param aImageData The image's data.
     * @param aFormat    The destination format of the image (see TSwtImageType).
     *                   Possible types are PNG, JPG, GIF.
     */
    virtual HBufC8* EncodeImageToStreamL(MSwtImageData& aImageData, TInt aFormat) =0;

    /**
     * Function in charge of decoding an image from a file.
     * @param aFileName is the pathname and the file's name of the image file to decode
     */
    virtual void DecodeImageL(const TDesC& aFileName) =0;

    /**
     * Function in charge of decoding an image from a buffer.
     * @param aBuffer contains the whole data to decode
     */
    virtual void DecodeWholeImageFromBufferL(const TDesC8& aBuffer) =0;

    /**
     * Function in charge of returning the decoded ImageData. Does not transfer ownership
     */
    virtual CSwtImageDataArray* GetImageData() =0;

    /**
     * The class is not a control but the dispose method has the
     * same effect as it has in control classes :
     * it frees the object and its associated resources.
     */
    virtual void Dispose() =0;

    /**
     * Function in charge of decoding an image from a buffer. This method is synchronous and enable
     * to save memory space by filling the image buffer step by step (use AppendDataL(const HBufC8& aBuffer)
     * param aBuffer contains a piece of the whole image data to decode
     */
    virtual void DecodeImageFromBufferL(const TDesC8& aBuffer) =0;

    /**
     * Enables to add data when a first call to DecodeImageFromBufferL has been initialized
     *
     * @param aBuffer contains a piece of the whole image data to decode.
     */
    virtual void AppendDataL(const TDesC8& aBuffer) =0;

    /**
     * Simple getter that return the logical values for height
     */
    virtual TInt GetLogicalScreenHeight() =0;

    /**
     * Simple getter that return the logical values for width
     */
    virtual TInt GetLogicalScreenWidth() =0;
};

#endif // ESWTGRAPHICS_H
