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
// SwtImage.h


#ifndef SWTIMAGE_H
#define SWTIMAGE_H


#include "eswtgraphics.h"


class CSwtGrPaletteData;


/**
 * CSwtImage
 * C++ implementation of org.eclipse.swt.graphics.Image.
 * Due to lack of support for paletted modes in CFbsBitmap ( the API exists
 * but is empty implemented ), colour images of bit depths 1, 2, 4 and 8 are
 * converted to 24 bits per pixel. This, however, does not affect grayscale
 * images.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtImage)
        : public CBase
        , public MSwtImage
        , public MSwtResourceChangeObserver
{
// Data types
private:
    enum TMaskType
    {
        ENoMask = 0,    // No transparency
        EBinaryMask,    // Binary transparency
        EColorKey,      // Colour key: a colour is considered transparent.
        EDirtyColorKey, // Same as EColorKey but the mask needs to be regenerated
        EAlphaMask      // Alpha transparency
    };

// Own methods
public:
    /**
     * Two phase constructor.
     * @param aDevice graphic device
     * @param aSize initial image dimensions
     * @param aDisplay parent display
     * @param aMode display mode
     * @return pointer to blank image
     */
    static CSwtImage* NewL(MSwtDevice& aDevice, const TSize& aSize,
                           MSwtDisplay& aDisplay, TDisplayMode aMode = ENone);

    /**
     * Two phase constructor.
     * @param aDevice graphic device
     * @param aData image data
     * @param aDisplay parent display
     * @return pointer to image constructed from image data
     */
    static CSwtImage* NewL(MSwtDevice* aDevice, const MSwtImageData& aData,
                           MSwtDisplay* aDisplay);

    /**
     * Two phase constructor.
     * @param aBitmap source bitmap - ownership NOT transferred
     * @param aBitmap source mask - ownership NOT transferred
     * @param aTopLeft initial position
     * @param aDelayTime frame delay
     * @param aDisposalMethod disposal method of previous frames
     * @return pointer to image constructed bitmap + mask
     */
    static CSwtImage* NewL(CFbsBitmap& aBitmap, CFbsBitmap* aMask,
                           const TPoint& aTopLeft, TInt aDelayTime, TSwtGifDisposal aDisposalMethod);

    /**
     * Two phase constructor.
     * @param aBitmap source bitmap - ownership transferred
     * @param aBitmap source mask - ownership transferred
     * @return pointer to image constructed bitmap + mask
     */
    static CSwtImage* NewL(CFbsBitmap& aBitmap, CFbsBitmap* aMask);

private:
    /**
     * First phase constructor.
     * @param aDevice graphic device
     * @param aDisplay parent display
     */
    inline CSwtImage(MSwtDevice* aDevice, MSwtDisplay* aDisplay);

    /**
     * Destructor
     */
    ~CSwtImage();

    /**
     * Second phase constructor.
     * @param aSize initial image dimensions
     * @param aMode display mode
     */
    void ConstructL(const TSize& aSize, TDisplayMode aMode);

    /**
     * Second phase constructor.
     * @param aData image data
     */
    void ConstructL(const MSwtImageData& aData);

    /**
     * Second phase constructor.
     * @param aBitmap source bitmap
     * @param aBitmap source mask
     * @param aTopLeft initial position
     * @param aDelayTime frame delay
     * @param aDisposalMethod disposal method of previous frames
     */
    void ConstructL(CFbsBitmap& aBitmap, CFbsBitmap* aMask, const TPoint& aTopLeft,
                    TInt aDelayTime, TSwtGifDisposal aDisposalMethod, TBool aBmpOwnExternally);

    /**
     * Color key setter.
     * @param aData image data
     */
    void SetColorKeyL(const MSwtImageData& aData);

    /**
     * Ensures the mask is up to date. This method, although const, may actually
     * modify the image's mask but this will only occur if the image's content
     * has been modified and the mask wasn't updated at that moment.
     */
    void EnsureMaskIsUpToDate() const;

    /**
     * Computes the binary mask from the colour key.
     */
    void ComputeColorKeyMask();

    /**
     * Tests whether the image is a grayscale one.
     */
    TBool IsGrayscale() const;

    /**
     * Creates a new palette data object for the image.
     */
    CSwtGrPaletteData* GetPaletteDataL() const;

    /**
     * Computes the gray mode for the provided palette.
     * @return The gray mode suitable for the current palette or ENone if none
     *         has been found.
     */
    static TDisplayMode PaletteGrayMode(TInt aDepth, const CPalette& aPalette);

    /**
     * Computes the display mode of the provided data.
     * @return The display mode, or ENone if no suitable mode could be found.
     */
    static TDisplayMode DisplayMode(const MSwtImageData& aData);

    /**
     * Returns the bit depth for the specified display mode.
     */
    static TInt BitDepth(TDisplayMode aMode);

    /**
     * Returns the image depth bpp according to the bitmap display type.
     */
    TInt ImageDepth() const;

    /**
     * Returns the scanline padding for the specified bit depth.
     */
    static TInt ScanlinePadding(TInt aBitDepth);

    /**
     * Computes the number of bytes needed in a line.
     */
    static TInt BytesPerLine(TInt aPixelsPerLine, TInt aBitsPerPixel,
                             TInt aPadding);

    /**
     * Fills the provided bitmap with the provided colour.
     */
    static void FillBitmapL(CFbsBitmap& aBitmap, const TRgb& aColor,
                            const TRect& aRect);

    /**
     * Bitblt the bitmap with another bitmap.
     */
    void BitBltBitmapL(CFbsBitmapDevice& aDevice, const CFbsBitmap& aBitmap);


    /**
     * Fills the provided bitmap with the provided colour.
     */
    static void FillBitmapL(CFbsBitmapDevice& aDevice, const TRgb& aColor,
                            const TRect& aRect);

    /**
     * Creates a new palette object for a given gray mode.
     */
    static CPalette* NewGrayPaletteL(TDisplayMode aMode);

    /**
     * Shifts an integer by a certain number of bits.
     * @param aValue The integer whose bits are to be shifted
     * @param aShift The number of bits to shift by; a positive value shifts
     *               to the left, a negative value shifts to the right.
     */
    static inline TUint BitShift(TUint aValue, TInt aShift);

    /**
     * Extracts the red component of a pixel. The result is a value in [0;255].
     */
    static inline TUint GetRed(TUint aPixel,
                               const MSwtPaletteData::TDirectData& aPalette);

    /**
     * Extracts the green component of a pixel. The result is a value in [0;255].
     */
    static inline TUint GetGreen(TUint aPixel,
                                 const MSwtPaletteData::TDirectData& aPalette);

    /**
     * Extracts the blue component of a pixel. The result is a value in [0;255].
     */
    static inline TUint GetBlue(TUint aPixel,
                                const MSwtPaletteData::TDirectData& aPalette);

    /**
     * Extracts the RGB components of a pixel.
     */
    static inline TRgb GetRgb(TUint aPixel,
                              const MSwtPaletteData::TDirectData& aPalette);

    /**
     * Writes a 16-bit pixel in the Symbian format ( [rrrrrggg gggbbbbb] binary ).
     * @param aPtr     Where to write the pixel, the pointer is modified to point
     *                 after the pixel upon return.
     * @param aPixel   The pixel to write
     * @param aPalette Information about aPixel's format.
     */
    static inline void WritePixel16(TUint8*& aPtr, TUint aPixel,
                                    const MSwtPaletteData::TDirectData& aPalette);

    /**
     * Writes a True Color pixel in the Symbian format ( BGR ).
     * @param aPtr     Where to write the pixel, the pointer is modified to point
     *                 after the pixel upon return.
     * @param aPixel   The pixel to write
     * @param aPalette Information about aPixel's format.
     * @param a24Bit   Do the write in 24- or 32-bit format
     */
    static inline void WritePixelTrueColor(TUint8*& aPtr, TUint aPixel,
                                           const MSwtPaletteData::TDirectData& aPalette, TBool a24Bit);

    /**
     * Writes a True Color pixel in the Symbian format ( BGR ).
     * @param aPtr   Where to write the pixel, the pointer is modified to point
     *               after the pixel upon return.
     * @param aColor The colour to write at the specified location.
     */
    static inline void WritePixelTrueColor(TUint8*& aPtr, const TRgb& aColor,
                                           TBool a24Bit);

    /**
     * Copies data from a buffer of bytes to a bitmap.
     */
    static void CopyData(const CFbsBitmap& aBitmap, const TDesC8& aData,
                         TInt aLineCount, TInt aDepth, TInt aBytesPerLine,
                         const MSwtPaletteData* aPalette);

    /**
     * Add mask bitmap's alpha channel.
     */
    void AddMaskToAlphaChannel(const CFbsBitmap& aBitmap, const CFbsBitmap& aMask);

    /**
     * Copies data between two buffers arranged in units of 1 bit.
     * The bits of each byte are reversed on the fly.
     */
    static void Copy1(TUint8* aDest, TInt aBytesPerDestLine,
                      const TUint8* aSrce, TInt aBytesPerSrceLine, TInt aLineCount);

    /**
     * Copies data between two buffers arranged in units of 4 bits.
     * The four quarters of each byte are reversed on the fly.
     */
    static void Copy2(TUint8* aDest, TInt aBytesPerDestLine,
                      const TUint8* aSrce, TInt aBytesPerSrceLine, TInt aLineCount);

    /**
     * Copies data between two buffers arranged in units of 4 bits.
     * The two halves of each byte are swapped on the fly.
     */
    static void Copy4(TUint8* aDest, TInt aBytesPerDestLine,
                      const TUint8* aSrce, TInt aBytesPerSrceLine, TInt aLineCount);

    /**
     * Copies data between two buffers arranged in units of 8 bits.
     */
    static void Copy8(TUint8* aDest, TInt aBytesPerDestLine,
                      const TUint8* aSrce, TInt aBytesPerSrceLine, TInt aLineCount);

    /**
     * Copies data between two buffers arranged in units of 16 bits.
     * The first and last bytes of each unit are swapped on the fly to account
     * for the fact that Symbian's and SWT's red and blue channels are inverted.
     */
    static void Copy16(TUint8* aDest, TInt aBytesPerDestLine,
                       const TUint8* aSrce, TInt aBytesPerSrceLine, TInt aLineCount,
                       const MSwtPaletteData::TDirectData& aPalette);

    /**
     * Copies data from a buffer of 1-bit paletted pixels to a buffer of
     * 24/32-bit pixels.
     */
    static void Copy1ToTrueColor(TUint8* aDest, TInt aBytesPerDestLine,
                                 const TUint8* aSrce, TInt aBytesPerSrceLine, TInt aLineCount,
                                 const CPalette& aPalette, TBool a24Bit);

    /**
     * Copies data from a buffer of 2-bit paletted pixels to a buffer of
     * 24/32-bit pixels.
     */
    static void Copy2ToTrueColor(TUint8* aDest, TInt aBytesPerDestLine,
                                 const TUint8* aSrce, TInt aBytesPerSrceLine, TInt aLineCount,
                                 const CPalette& aPalette, TBool a24Bit);

    /**
     * Copies data from a buffer of 4-bit paletted pixels to a buffer of
     * 24/32-bit pixels
     */
    static void Copy4ToTrueColor(TUint8* aDest, TInt aBytesPerDestLine,
                                 const TUint8* aSrce, TInt aBytesPerSrceLine, TInt aLineCount,
                                 const CPalette& aPalette, TBool a24Bit);

    /**
     * Copies data from a buffer of 8-bit paletted pixels to a buffer of
     * 24/32-bit pixels
     */
    static void Copy8ToTrueColor(TUint8* aDest, TInt aBytesPerDestLine,
                                 const TUint8* aSrce, TInt aBytesPerSrceLine, TInt aLineCount,
                                 const CPalette& aPalette, TBool a24Bit);

    /**
     * Copies data from a buffer of 8-bit pixels to a buffer of 24/32-bit pixels.
     */
    static void Copy8ToTrueColor(TUint8* aDest, TInt aBytesPerDestLine,
                                 const TUint8* aSrce, TInt aBytesPerSrceLine, TInt aLineCount,
                                 const MSwtPaletteData::TDirectData& aPalette, TBool a24Bit);

    /**
     * Copies data between two buffers arranged in units of 24 bits.
     */
    static void Copy24(TUint8* aDest, TInt aBytesPerDestLine,
                       const TUint8* aSrce, TInt aBytesPerSrceLine, TInt aLineCount,
                       const MSwtPaletteData::TDirectData& aPalette);

    /**
     * Copies data between two buffers arranged in units of 24 bits.
     */
    static void Copy32(TUint8* aDest, TInt aBytesPerDestLine,
                       const TUint8* aSrce, TInt aBytesPerSrceLine, TInt aLineCount,
                       const MSwtPaletteData::TDirectData& aPalette);

    /**
     * Returns a newly allocated buffer to the data of the specified bitmap.
     */
    static HBufC8* GetPixelDataLC(const CFbsBitmap& aBitmap);

    /**
     * ?
     */
    static void GetPixels16(TUint8* aDest, TInt aBytesPerDestLine,
                            const TUint8* aSrce, TInt aBytesPerSrceLine, const TSize& aSize);

// From MSwtBitmapDrawable
public:
    MSwtGc* NewGcL();
    CBitmapDevice& GraphicsDevice();
    void HandleUpdate();
private:
    static void DestroyNativeGc(CBitmapContext* aGc);

// From MSwtImage
protected:
    CFbsBitmap& GetBitmap() const;
    CFbsBitmap& GetSubBitmap(const TSize& aSize) const;

// From MSwtImage
public:
    void Dispose();
    const CFbsBitmap* MaskBitmap(TBool aInvertedIfMonochrome = EFalse) const;
    const CFbsBitmap* SubMaskBitmap(const TSize& aSize, TBool aInvertedIfMonochrome = EFalse) const;
    TRect GetBounds() const;
    MSwtImageData* GetImageDataL() const;
    TInt AddSubRef(const TSize& aSize) const;
    TInt RemoveSubRef(const TSize& aSize) const;
    TInt SubRefCount(const TSize& aSize) const;
    CFbsBitmap* BitmapWithAlphaLC();

// From MSwtResourceChangeObserver
public:
    void OfferResourceChangeL(TInt aType);

// Data
private:
    /**
     * The device on which the image is created, may be NULL.
     * Not own.
     */
    MSwtDevice* iDevice;

    /**
     * Colour data, may be owned, cannot be NULL.
     * May be own or not own.
     */
    CFbsBitmap* iBitmap;

    /**
     * Device for drawing onto the bitmap, cannot be NULL.
     * Own.
     */
    CFbsBitmapDevice* iBitmapDevice;

    /**
     * The type of mask we're dealing with
     */
    TMaskType iMaskType;

    /**
     * Transparency mask, binary ( 1 bit ) or alpha ( 8 bits ), may be NULL.
     * May be own or not own.
     */
    CFbsBitmap* iMask;

    /**
     * Transparency mask inverted, binary ( 1 bit ) or alpha ( 8 bits ), may be NULL.
     * Created only upon request.
     * Own.
     */
    mutable CFbsBitmap* iMaskInverted;

    /**
     * The transparent colour, may be NULL.
     * Own.
     */
    TRgb* iColorKey;

    /**
     * The coordinates the bitmap's origin is at within an animation.
     */
    TPoint iOffset;

    /**
     * The original image information
     */
    MSwtImageData::TInfo iInfo;

    /**
     * The iBitmap and iMask object have been created outside of the object.
     * Do not delete them in the destructor.
     */
    TBool iOwnExternally;

    /**
     * The Display the image belongs to, may be NULL.
     * Not own.
     */
    MSwtDisplay* iDisplay;

    /**
     * Array holding the reference count for scaled copies of the main bitmap.
     */
    mutable RArray<TInt> iScaledBitmapRefs;

    /**
     * Array holding the scaled copies of the main bitmap.
     */
    mutable RPointerArray<CFbsBitmap> iScaledBitmaps;

    /**
     * Array holding the scaled copies of the main mask.
     */
    mutable RPointerArray<CFbsBitmap> iScaledMasks;

    /**
     * Array holding the scaled inverted copies of the main mask.
     * Entries valid only if mask is monochrome, otherwise NULL.
     */
    mutable RPointerArray<CFbsBitmap> iScaledMasksInverted;
};


#endif // SWTIMAGE_H
