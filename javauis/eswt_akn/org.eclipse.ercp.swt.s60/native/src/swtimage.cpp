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


#include "swtimage.h"
#include "swtgrimagedata.h"
#include "swtgrpalettedata.h"
#include "swtcontrolhelper.h"
#include "swtdisplay.h" // needed for macro definitions


// Look-up table for the bit-reversed value of a byte.
static const TUint8 KBitReverseTable[256] =
{
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtImage::NewL
// ---------------------------------------------------------------------------
//
CSwtImage* CSwtImage::NewL(MSwtDevice& aDevice, const TSize& aSize,
                           MSwtDisplay& aDisplay, TDisplayMode aMode /*=ENone*/)
{
    CSwtImage* self = new(ELeave) CSwtImage(&aDevice, &aDisplay);
    CleanupStack::PushL(self);
    self->ConstructL(aSize, aMode);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtImage::NewL
// ---------------------------------------------------------------------------
//
CSwtImage* CSwtImage::NewL(MSwtDevice* aDevice, const MSwtImageData& aData,
                           MSwtDisplay* aDisplay)
{
    CSwtImage* self = new(ELeave) CSwtImage(aDevice, aDisplay);
    CleanupStack::PushL(self);
    self->ConstructL(aData);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtImage::NewL
// ---------------------------------------------------------------------------
//
CSwtImage* CSwtImage::NewL(CFbsBitmap& aBitmap, CFbsBitmap* aMask,
                           const TPoint& aTopLeft, TInt aDelayTime, TSwtGifDisposal aDisposalMethod)
{
    CSwtImage* self = new(ELeave) CSwtImage(NULL, NULL);
    CleanupStack::PushL(self);
    self->ConstructL(aBitmap, aMask, aTopLeft, aDelayTime, aDisposalMethod, ETrue);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtImage::NewL
// ---------------------------------------------------------------------------
//
CSwtImage* CSwtImage::NewL(CFbsBitmap& aBitmap, CFbsBitmap* aMask)
{
    CSwtImage* self = new(ELeave) CSwtImage(NULL, NULL);
    CleanupStack::PushL(self);
    self->ConstructL(aBitmap, aMask, TPoint(0, 0), 0, KSwtDisposalUnspecified, EFalse);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtImage::CSwtImage
// ---------------------------------------------------------------------------
//
inline CSwtImage::CSwtImage(MSwtDevice* aDevice, MSwtDisplay* aDisplay)
        : iDevice(aDevice)
        , iMaskType(ENoMask)
        , iDisplay(aDisplay)
{
}

// ---------------------------------------------------------------------------
// CSwtImage::~CSwtImage
// ---------------------------------------------------------------------------
//
CSwtImage::~CSwtImage()
{
    iScaledMasks.ResetAndDestroy();
    iScaledMasksInverted.ResetAndDestroy();
    iScaledBitmaps.ResetAndDestroy();
    iScaledBitmapRefs.Close();
    delete iColorKey;
    if (!iOwnExternally)
    {
        delete iMask;
        delete iBitmap;
    }
    delete iMaskInverted;
    delete iBitmapDevice;
    if (iDisplay)
    {
        iDisplay->RemoveResourceChangeObserver(this);
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtImage::ConstructL(const TSize& aSize, TDisplayMode aMode)
{
    ASSERT(iDevice);
    ASSERT(iDisplay);

    // Create the bitmap
    iBitmap = new(ELeave) CFbsBitmap;

    // By default set the image color depth to that of the device.
    TDisplayMode mode = (aMode != ENone) ? aMode :
                        iDisplay->CoeEnv()->ScreenDevice()->DisplayMode();

    // 12-bit images aren't handled by SWT, neither will we
    if (mode == EColor4K)
    {
        mode = iDisplay->CoeEnv()->ScreenDevice()->DisplayMode();
    }

    User::LeaveIfError(iBitmap->Create(aSize, mode));

    // We have to set twips size of bitmap because this is used in order to get
    // best sized fonts if a GC is created on this bitmap ( if we don't then
    // HAL will be used and maybe this could return wrong values if device
    // resolution has changed )
    iBitmap->SetSizeInTwips(&(iDevice->GraphicsDevice()));
    iDisplay->AddResourceChangeObserverL(this);

    // Create the bitmap device
    iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);

    // Initialise the image information
    iInfo.iSize = aSize;
    iInfo.iDepth = BitDepth(mode);
    iInfo.iScanlinePad = ScanlinePadding(iInfo.iDepth);
    iInfo.iBytesPerLine = BytesPerLine(iInfo.iSize.iWidth, iInfo.iDepth,
                                       iInfo.iScanlinePad);
    iInfo.iTransparentPixel = -1;
    iInfo.iMaskPad =  0;
    iInfo.iAlpha = -1;
    iInfo.iType = ESwtImagePng;
    iInfo.iTopLeft = TPoint(0, 0);
    iInfo.iDisposalMethod = KSwtDisposalUnspecified;
    iInfo.iDelayTime = 0;
}

// ---------------------------------------------------------------------------
// CSwtImage::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtImage::ConstructL(const MSwtImageData& aData)
{
    TDisplayMode mode(DisplayMode(aData));
    if (mode == ENone)
    {
        User::Leave(ESwtErrorUnsupportedDepth);
    }

    iInfo = aData.Info();

    // Create the bitmap and its device
    iBitmap = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iBitmap->Create(iInfo.iSize, mode));
    iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);

    // Fill the bitmap
    if (aData.PixelBuffer().Length() != 0)
    {
        CopyData(*iBitmap, aData.PixelBuffer(), iInfo.iSize.iHeight,
                 iInfo.iDepth, iInfo.iBytesPerLine, &(aData.Palette()));
    }
    else
    {
        FillBitmapL(*iBitmapDevice, KRgbBlack, TRect(iInfo.iSize));
    }

    if (iInfo.iAlpha != -1)
    {
        // Global alpha value, causes alphaData to be ignored.
        iMaskType = EAlphaMask;
        iMask = new(ELeave) CFbsBitmap;
        User::LeaveIfError(iMask->Create(iInfo.iSize, EGray256));

        iMask->LockHeap();
        Mem::Fill(iMask->DataAddress(),
                  iInfo.iSize.iWidth * iInfo.iSize.iHeight, iInfo.iAlpha);
        iMask->UnlockHeap();
    }
    else
    {
        // Binary mask
        if (aData.MaskBuffer())
        {
            iMaskType = EBinaryMask;
            iMask = new(ELeave) CFbsBitmap;
            User::LeaveIfError(iMask->Create(iInfo.iSize, EGray2));
            TInt bpl = BytesPerLine(iInfo.iSize.iWidth, 1, iInfo.iMaskPad);
            CopyData(*iMask, *aData.MaskBuffer(), iInfo.iSize.iHeight, 1, bpl, NULL);
        }
        // Colour key mask, binary mask
        else if (iInfo.iTransparentPixel != -1)
        {
            iMaskType = EDirtyColorKey;
            iMask = new(ELeave) CFbsBitmap;
            User::LeaveIfError(iMask->Create(iInfo.iSize, EGray2));
            SetColorKeyL(aData);
            ComputeColorKeyMask();
        }
        // Alpha mask
        else if (aData.AlphaBuffer())
        {
            iMaskType = EAlphaMask;
            iMask = new(ELeave) CFbsBitmap;
            User::LeaveIfError(iMask->Create(iInfo.iSize, EGray256));
            CopyData(*iMask, *aData.AlphaBuffer(), iInfo.iSize.iHeight, 8,
                     iInfo.iSize.iWidth, NULL);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtImage::ConstructL(CFbsBitmap& aBitmap, CFbsBitmap* aMask,
                           const TPoint& aTopLeft, TInt aDelayTime,
                           TSwtGifDisposal aDisposalMethod, TBool aBmpOwnExternally)
{
    // Caution: if true, the object depends on the external bitmap's lifetime
    iOwnExternally = aBmpOwnExternally;

    iBitmap = &aBitmap;
    iMask = aMask;

    TDisplayMode mode = iBitmap->DisplayMode();
    if (mode == ENone || mode == EColor4K)
    {
        // 12-bit images aren't handled by SWT, neither will we
        User::Leave(ESwtErrorUnsupportedDepth);
    }

    iInfo.iSize = iBitmap->SizeInPixels();
    iInfo.iDepth = BitDepth(mode);
    iInfo.iScanlinePad = ScanlinePadding(iInfo.iDepth);
    iInfo.iBytesPerLine = BytesPerLine(iInfo.iSize.iWidth, iInfo.iDepth, iInfo.iScanlinePad);
    iInfo.iTransparentPixel = -1;
    iInfo.iMaskPad = 0;
    iInfo.iAlpha = -1;
    iInfo.iType = ESwtImagePng;
    iInfo.iTopLeft = aTopLeft;
    iInfo.iDisposalMethod = aDisposalMethod;
    iInfo.iDelayTime = aDelayTime;

    if (iMask)
    {
        if (iMask->DisplayMode() == EGray2)
        {
            iMaskType = EBinaryMask;
        }
        else
        {
            iMaskType = EAlphaMask;
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::SetColorKeyL
// ---------------------------------------------------------------------------
//
void CSwtImage::SetColorKeyL(const MSwtImageData& aData)
{
    ASSERT(!iColorKey);

    iColorKey = new(ELeave) TRgb;

    const MSwtPaletteData& palette = aData.Palette();
    const MSwtImageData::TInfo& info = aData.Info();

    if (palette.IsDirect())
    {
        switch (info.iDepth)
        {
            // Monochrome, the transparent pixel is either black or white
        case 1:
            *iColorKey = (info.iTransparentPixel == 0) ?
                         TRgb(0, 0, 0) : TRgb(255u, 255u, 255u);
            break;
            // Direct colour, extract components
        case 16:
        case 24:
        case 32:
            *iColorKey = GetRgb(info.iTransparentPixel,
                                palette.DirectData());
            break;
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    }
    else
    {
        switch (aData.Info().iDepth)
        {
            // Look up the transparent pixel in the palette
        case 1:
        case 2:
        case 4:
        case 8:
            *iColorKey = palette.IndirectData()->GetEntry(
                             info.iTransparentPixel);
            break;
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    }
}


// ---------------------------------------------------------------------------
// CSwtImage::EnsureMaskIsUpToDate
// ---------------------------------------------------------------------------
//
void CSwtImage::EnsureMaskIsUpToDate() const
{
    if (iMaskType == EDirtyColorKey)
    {
        const_cast<CSwtImage*>(this)->ComputeColorKeyMask();
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::ComputeColorKeyMask
// ---------------------------------------------------------------------------
//
void CSwtImage::ComputeColorKeyMask()
{
    ASSERT(iBitmap);
    ASSERT(iMask && iMask->DisplayMode() == EGray2);
    ASSERT(iColorKey);
    ASSERT(iMaskType == EDirtyColorKey);

    iMask->LockHeap(EFalse);
    TUint32* maskPtr = iMask->DataAddress();

    const TSize size(iBitmap->SizeInPixels());
    const TInt wordsPerMaskLine = CFbsBitmap::ScanLineLength(size.iWidth, EGray2) / 4;

    TRgb pixel;
    for (TInt y = 0; y < size.iHeight; ++y)
    {
        TUint32 word32 = 0;
        TInt bitsInWord = 0;
        TInt wordsWritten = 0;

        for (TInt x = 0; x < size.iWidth; ++x)
        {
            iBitmap->GetPixel(pixel, TPoint(x, y));
            word32 >>= 1;
            word32 |= (pixel == *iColorKey) ? 0 : 0x80000000;
            if (++bitsInWord == 32)
            {
                maskPtr[wordsWritten++] = word32;
                bitsInWord = 0;
            }
        }

        if (bitsInWord != 0)
        {
            maskPtr[wordsWritten] = word32 >> (32 - bitsInWord);
        }

        maskPtr += wordsPerMaskLine;
    }

    iMask->UnlockHeap();
    iMaskType = EColorKey;
}

// ---------------------------------------------------------------------------
// CSwtImage::IsGrayscale
// ---------------------------------------------------------------------------
//
TBool CSwtImage::IsGrayscale() const
{
    switch (iBitmap->DisplayMode())
    {
    case EGray2:
    case EGray4:
    case EGray16:
    case EGray256:
        return ETrue;
    default:
        return EFalse;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::GetPaletteDataL
// ---------------------------------------------------------------------------
//
CSwtGrPaletteData* CSwtImage::GetPaletteDataL() const
{
    TDisplayMode mode(iBitmap->DisplayMode());

    // Decide whether the palette is direct or indirect
    TBool isDirect;
    switch (mode)
    {
    case EGray2:
    case EGray4:
    case EGray16:
    case EGray256:
        isDirect = EFalse;
        break;
    case EColor64K:
    case EColor16M:
    case EColor16MU:
    case EColor16MA:
    case EColor16MAP:
        isDirect = ETrue;
        break;
    default:
        ASSERT(EFalse);
        return NULL;
    }

    // Create and fill the palette data
    CSwtGrPaletteData* data = CSwtGrPaletteData::NewL(isDirect);
    if (isDirect)
    {
        CSwtGrPaletteData::TDirectData directData;
        if (mode == EColor64K)
        {
            // 16-bit Symbian bitmaps are stored as [rrrrrggg gggbbbbb] ( binary )
            directData.iRedMask = 0xF800;   // [11111000 00000000] binary
            directData.iGreenMask = 0x07E0; // [00000111 11100000] binary
            directData.iBlueMask = 0x001F;  // [00000000 00011111] binary
            directData.iRedShift = -8;
            directData.iGreenShift = -3;
            directData.iBlueShift =  3;
        }
        else if (mode == EColor16M)
        {
            // 24-bit Symbian bitmaps are stored in BGR order; 24-bit SWT images
            // are stored in MSB first order, this yields a pixel format of 0xbbggrr
            directData.iRedMask = 0x0000FF;
            directData.iGreenMask = 0x00FF00;
            directData.iBlueMask = 0xFF0000;
            directData.iRedShift = 0;
            directData.iGreenShift = -8;
            directData.iBlueShift = -16;
        }
        else
        {
            // EColor16MU, EColor16MA or EColor16MAP
            // 32-bit Symbian bitmaps are stored in BGR order; 32-bit SWT images
            // are stored in MSB first order, this yields a pixel format of 0xbbggrraa
            directData.iRedMask = 0x0000FF00;
            directData.iGreenMask = 0x00FF0000;
            directData.iBlueMask = 0xFF000000;
            directData.iRedShift = -8;
            directData.iGreenShift = -16;
            directData.iBlueShift = -24;
        }
        data->SetDirectData(directData);
    }
    else
    {
        data->SetIndirectData(NewGrayPaletteL(mode));
    }

    return data;
}

// ---------------------------------------------------------------------------
// CSwtImage::PaletteGrayMode
// ---------------------------------------------------------------------------
//
TDisplayMode CSwtImage::PaletteGrayMode(TInt aDepth, const CPalette& aPalette)
{
    TDisplayMode mode;

    // Find the mode by looking at the bit depth
    switch (aDepth)
    {
    case 8:
        mode = EGray256;
        break;
    case 4:
        mode = EGray16;
        break;
    case 2:
        mode = EGray4;
        break;
    case 1:
        mode = EGray2;
        break;
    default:
        mode = ENone;
        break;
    }

    // Check the colours
    if (mode != ENone)
    {
        // Check the number of colours is 2^depth
        const TInt count = aPalette.Entries();
        if (count != (1 << aDepth))
        {
            return ENone;
        }

        // Check the colours are indeed gray and evenly scattered
        for (TInt i = 0; i<count; ++i)
        {
            TInt l = (i*255) / (count - 1);
            if (aPalette.GetEntry(i) != TRgb(l, l, l))
            {
                return ENone;
            }
        }
    }

    return mode;
}

// ---------------------------------------------------------------------------
// CSwtImage::DisplayMode
// ---------------------------------------------------------------------------
//
TDisplayMode CSwtImage::DisplayMode(const MSwtImageData& aData)
{
    TDisplayMode mode(ENone);

    const MSwtPaletteData& palette = aData.Palette();
    const MSwtImageData::TInfo& info = aData.Info();

    // Non paletted modes
    if (palette.IsDirect())
    {
        switch (info.iDepth)
        {
        case 1:
            mode = EGray2;
            break;
        case 16:
            mode = EColor64K;
            break;
        case 8:
        case 24:
            mode = EColor16M;
            break;
        case 32:
            mode = EColor16MU;
            break;
        default:
            break;
        }
    }
    // Paletted modes
    else
    {
        // For some reason, CFbsBitmap::SetPalette() is implemented as empty.
        // The result is that paletted modes are not handled correctly. The
        // workaround is to convert all colour paletted modes to 24 bits, we
        // waste a lot of memory but at least it works.
        TDisplayMode grayMode(PaletteGrayMode(info.iDepth,
                                              *(palette.IndirectData())));
        mode = (grayMode != ENone) ? grayMode : EColor16MU;
    }

    return mode;
}

// ---------------------------------------------------------------------------
// CSwtImage::BitDepth
// ---------------------------------------------------------------------------
//
TInt CSwtImage::BitDepth(TDisplayMode aMode)
{
    return TDisplayModeUtils::NumDisplayModeBitsPerPixel(aMode);
}

// ---------------------------------------------------------------------------
// CSwtImage::ImageDepth
// ---------------------------------------------------------------------------
//
TInt CSwtImage::ImageDepth() const
{
    switch (iBitmap->DisplayMode())
    {
    case EGray2:
        return 1;
    case EGray4:
        return 2;
    case EGray16:
        return 4;
    case EGray256:
        return 8;
    case EColor16:
        return 4;
    case EColor256:
        return 8;
    case EColor64K:
        return 16;
    case EColor16M:
        return 24;
    case EColor16MU:
        return 32;
    case EColor16MA:
        return 32;
    case EColor16MAP:
        return 32;
    case ERgb:
        return 24;
    case EColor4K:
        return 12;

    default:
    case ENone:
    case EColorLast:
        ASSERT(EFalse);
        return 0;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::ScanlinePadding
// ---------------------------------------------------------------------------
//
TInt CSwtImage::ScanlinePadding(TInt aBitDepth)
{
    switch (aBitDepth)
    {
    case 1:
    case 2:
    case 4:
    case 16:
    case 24:
    case 32:
        return 4;
    case 8:
        // This is because of alpha data which is not supposed to have padding
        return 1;
    default:
        ASSERT(EFalse);
        return 0;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::BytesPerLine
// ---------------------------------------------------------------------------
//
TInt CSwtImage::BytesPerLine(TInt aPixelsPerLine, TInt aBitsPerPixel,
                             TInt aPadding)
{
    TInt byteCount = (aPixelsPerLine * aBitsPerPixel + 7) / 8;
    TInt remainder = byteCount % aPadding;
    TInt extra = (remainder != 0) ? aPadding-remainder : 0;
    return byteCount + extra;
}

// ---------------------------------------------------------------------------
// CSwtImage::FillBitmapL
// ---------------------------------------------------------------------------
//
void CSwtImage::FillBitmapL(CFbsBitmapDevice& aDevice, const TRgb& aColor,
                            const TRect& aRect)
{
    CFbsBitGc* gc = CFbsBitGc::NewL();
    gc->Activate(&aDevice);

    gc->SetPenStyle(CGraphicsContext::ENullPen);
    gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc->SetBrushColor(aColor);
    gc->DrawRect(aRect);

    delete gc;
}

// ---------------------------------------------------------------------------
// CSwtImage::BitBltBitmapL
// ---------------------------------------------------------------------------
//
void CSwtImage::BitBltBitmapL(CFbsBitmapDevice& aDevice, const CFbsBitmap& aBitmap)
{
    CFbsBitGc* gc = CFbsBitGc::NewL();
    gc->Activate(&aDevice);
    gc->BitBlt(TPoint(0,0), &aBitmap);
    delete gc;
}


// ---------------------------------------------------------------------------
// CSwtImage::FillBitmapL
// ---------------------------------------------------------------------------
//
void CSwtImage::FillBitmapL(CFbsBitmap& aBitmap, const TRgb& aColor,
                            const TRect& aRect)
{
    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(&aBitmap);
    CleanupStack::PushL(device);
    FillBitmapL(*device, aColor, aRect);
    CleanupStack::PopAndDestroy(device);
}

// ---------------------------------------------------------------------------
// CSwtImage::NewGrayPaletteL
// ---------------------------------------------------------------------------
//
CPalette* CSwtImage::NewGrayPaletteL(TDisplayMode aMode)
{
    TInt count;

    switch (aMode)
    {
    case EGray2:
        count = 2;
        break;
    case EGray4:
        count = 4;
        break;
    case EGray16:
        count = 16;
        break;
    case EGray256:
        count = 256;
        break;
    default:
        ASSERT(EFalse);
        return NULL;
    }

    CPalette* palette = CPalette::NewL(count);
    for (TInt i = 0; i < count; ++i)
    {
        TInt shade = (i*255) / (count-1);
        palette->SetEntry(i, TRgb(shade, shade, shade));
    }

    return palette;
}

// ---------------------------------------------------------------------------
// CSwtImage::BitShift
// ---------------------------------------------------------------------------
//
inline TUint CSwtImage::BitShift(TUint aValue, TInt aShift)
{
    return (aShift >= 0) ? (aValue << aShift) : (aValue >> -aShift);
}

// ---------------------------------------------------------------------------
// CSwtImage::GetRed
// ---------------------------------------------------------------------------
//
inline TUint CSwtImage::GetRed(TUint aPixel,
                               const MSwtPaletteData::TDirectData& aPalette)
{
    return BitShift(aPixel & aPalette.iRedMask, aPalette.iRedShift);
}

// ---------------------------------------------------------------------------
// CSwtImage::GetGreen
// ---------------------------------------------------------------------------
//
inline TUint CSwtImage::GetGreen(TUint aPixel,
                                 const MSwtPaletteData::TDirectData& aPalette)
{
    return BitShift(aPixel & aPalette.iGreenMask, aPalette.iGreenShift);
}

// ---------------------------------------------------------------------------
// CSwtImage::GetBlue
// ---------------------------------------------------------------------------
//
inline TUint CSwtImage::GetBlue(TUint aPixel,
                                const MSwtPaletteData::TDirectData& aPalette)
{
    return BitShift(aPixel & aPalette.iBlueMask, aPalette.iBlueShift);
}

// ---------------------------------------------------------------------------
// CSwtImage::GetBlue
// ---------------------------------------------------------------------------
//
inline TRgb CSwtImage::GetRgb(TUint aPixel,
                              const MSwtPaletteData::TDirectData& aPalette)
{
    return TRgb(GetRed(aPixel, aPalette), GetGreen(aPixel, aPalette), GetBlue(aPixel, aPalette));
}

// ---------------------------------------------------------------------------
// CSwtImage::WritePixel16
// ---------------------------------------------------------------------------
//
inline void CSwtImage::WritePixel16(TUint8*& aPtr, TUint aPixel,
                                    const MSwtPaletteData::TDirectData& aPalette)
{
    TUint pixel16;
    pixel16  = (GetRed(aPixel, aPalette) & 0xF8) << 8;
    pixel16 |= (GetGreen(aPixel, aPalette) & 0xFC) << 3;
    pixel16 |= (GetBlue(aPixel, aPalette)) >> 3;

    *reinterpret_cast<TUint16*&>(aPtr)++ = static_cast<TUint16>(pixel16);
}

// ---------------------------------------------------------------------------
// CSwtImage::WritePixelTrueColor
// ---------------------------------------------------------------------------
//
inline void CSwtImage::WritePixelTrueColor(TUint8*& aPtr, const TRgb& aColor, TBool a24Bit)
{
    *aPtr++ = static_cast<TUint8>(aColor.Blue());
    *aPtr++ = static_cast<TUint8>(aColor.Green());
    *aPtr++ = static_cast<TUint8>(aColor.Red());

    if (!a24Bit)
    {
        // Since we don't get any alpha channel data for the 32-bit pixel,
        // set it opaque.
        *aPtr++ = 0xFF;
    }
}


// ---------------------------------------------------------------------------
// CSwtImage::WritePixelTrueColor
// ---------------------------------------------------------------------------
//
inline void CSwtImage::WritePixelTrueColor(TUint8*& aPtr, TUint aPixel,
        const MSwtPaletteData::TDirectData& aPalette, TBool a24Bit)
{
    if (a24Bit)
    {
        *aPtr++ = static_cast<TUint8>(GetBlue(aPixel, aPalette));
        *aPtr++ = static_cast<TUint8>(GetGreen(aPixel, aPalette));
        *aPtr++ = static_cast<TUint8>(GetRed(aPixel, aPalette));
    }
    else
    {
        *aPtr++ = static_cast<TUint8>(GetBlue(aPixel, aPalette));
        *aPtr++ = static_cast<TUint8>(GetGreen(aPixel, aPalette));
        *aPtr++ = static_cast<TUint8>(GetRed(aPixel, aPalette));
        // Alpha value is the last byte of the pixel.
        *aPtr++ = static_cast<TUint8>(aPixel & 0xFF);
    }
}


// ---------------------------------------------------------------------------
// CSwtImage::CopyData
// ---------------------------------------------------------------------------
//
void CSwtImage::CopyData(const CFbsBitmap& aBitmap, const TDesC8& aData,
                         TInt aLineCount, TInt aDepth, TInt aBytesPerLine,
                         const MSwtPaletteData* aPalette)
{
    aBitmap.LockHeap(EFalse);

    const TUint8* srce = aData.Ptr();
    TUint8* dest = reinterpret_cast<TUint8*>(aBitmap.DataAddress());

    TDisplayMode mode(aBitmap.DisplayMode());
    TInt destBytesPerLine = CFbsBitmap::ScanLineLength(
                                aBitmap.SizeInPixels().iWidth, mode);

    switch (mode)
    {
    case EGray2:
        Copy1(dest, destBytesPerLine, srce, aBytesPerLine, aLineCount);
        break;
    case EGray4:
        Copy2(dest, destBytesPerLine, srce, aBytesPerLine, aLineCount);
        break;
    case EGray16:
        Copy4(dest, destBytesPerLine, srce, aBytesPerLine, aLineCount);
        break;
    case EGray256:
        Copy8(dest, destBytesPerLine, srce, aBytesPerLine, aLineCount);
        break;
    case EColor64K:
        ASSERT(aPalette);
        Copy16(dest, destBytesPerLine, srce, aBytesPerLine, aLineCount,
               aPalette->DirectData());
        break;
    case EColor16M:
    case EColor16MU:
    case EColor16MA:
    case EColor16MAP:
        ASSERT(aPalette);
        switch (aDepth)
        {
        case 1:
            Copy1ToTrueColor(dest, destBytesPerLine, srce, aBytesPerLine,
                             aLineCount, *aPalette->IndirectData(), mode == EColor16M);
            break;
        case 2:
            Copy2ToTrueColor(dest, destBytesPerLine, srce, aBytesPerLine,
                             aLineCount, *aPalette->IndirectData(), mode == EColor16M);
            break;
        case 4:
            Copy4ToTrueColor(dest, destBytesPerLine, srce, aBytesPerLine,
                             aLineCount, *aPalette->IndirectData(), mode == EColor16M);
            break;
        case 8:
            if (aPalette->IsDirect())
            {
                Copy8ToTrueColor(dest, destBytesPerLine, srce, aBytesPerLine,
                                 aLineCount, aPalette->DirectData(), mode == EColor16M);
            }
            else
            {
                Copy8ToTrueColor(dest, destBytesPerLine, srce, aBytesPerLine,
                                 aLineCount, *aPalette->IndirectData(), mode == EColor16M);
            }
            break;
        case 24:
            Copy24(dest, destBytesPerLine, srce, aBytesPerLine, aLineCount,
                   aPalette->DirectData());
            break;
        case 32:
            Copy32(dest, destBytesPerLine, srce, aBytesPerLine, aLineCount,
                   aPalette->DirectData());
            break;
        default:
            ASSERT(EFalse);
            break;
        }
        break;
    default:
        ASSERT(EFalse);
        break;
    }

    aBitmap.UnlockHeap();
}


// ---------------------------------------------------------------------------
// CSwtImage::CopyDataWithAlpha
// ---------------------------------------------------------------------------
//
void CSwtImage::AddMaskToAlphaChannel(const CFbsBitmap& aBitmap, const CFbsBitmap& aMask)
{

    ASSERT(aBitmap.DisplayMode() == EColor16MA
           || aBitmap.DisplayMode() == EColor16MAP);
    ASSERT(aMask.DisplayMode() == EGray256);

    aBitmap.LockHeap(EFalse);
    TUint8* dest = reinterpret_cast<TUint8*>(aBitmap.DataAddress());

    TUint8* srce = reinterpret_cast<TUint8*>(aMask.DataAddress());



    for (TInt i = 0; i < aMask.SizeInPixels().iHeight * aMask.SizeInPixels().iWidth; ++i)
    {
        // 1 byte = B; 2 byte = G; 3 byte = R; 4 byte = alpha
        // skip rgb byte
        dest++; // skip b
        dest++; // skip g
        dest++; // skip r

        // copy alpha vaule
        *dest = *srce;

        // go next byte
        srce++;
        dest++;
    }

    aBitmap.UnlockHeap();
}

// ---------------------------------------------------------------------------
// CSwtImage::Copy1
// ---------------------------------------------------------------------------
//
void CSwtImage::Copy1(TUint8* aDest, TInt aBytesPerDestLine, const TUint8* aSrce,
                      TInt aBytesPerSrceLine, TInt aLineCount)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    const TInt bytesToCopyPerLine = Min(aBytesPerDestLine, aBytesPerSrceLine);

    for (TInt y = 0; y < aLineCount; ++y)
    {
        for (TInt x = 0; x < bytesToCopyPerLine; ++x)
        {
            aDest[x] = KBitReverseTable[aSrce[x]];
        }
        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::Copy2
// ---------------------------------------------------------------------------
//
void CSwtImage::Copy2(TUint8* aDest, TInt aBytesPerDestLine, const TUint8* aSrce,
                      TInt aBytesPerSrceLine, TInt aLineCount)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    const TInt bytesToCopyPerLine = Min(aBytesPerDestLine, aBytesPerSrceLine);

    for (TInt y = 0; y < aLineCount; ++y)
    {
        for (TInt x = 0; x < bytesToCopyPerLine; ++x)
        {
            TUint byte = aSrce[x];
            aDest[x] = static_cast<TUint8>
                       ((byte >> 6) |
                        ((byte >> 2) & 0x0C) |
                        ((byte << 2) & 0x30) |
                        (byte << 6));
        }
        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::Copy4
// ---------------------------------------------------------------------------
//
void CSwtImage::Copy4(TUint8* aDest, TInt aBytesPerDestLine, const TUint8* aSrce,
                      TInt aBytesPerSrceLine, TInt aLineCount)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    const TInt bytesToCopyPerLine = Min(aBytesPerDestLine, aBytesPerSrceLine);

    for (TInt y = 0; y < aLineCount; ++y)
    {
        for (TInt x = 0; x < bytesToCopyPerLine; ++x)
        {
            TUint byte = aSrce[x];
            aDest[x] = static_cast<TUint8>(byte >> 4 | byte << 4);
        }
        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::Copy8
// ---------------------------------------------------------------------------
//
void CSwtImage::Copy8(TUint8* aDest, TInt aBytesPerDestLine, const TUint8* aSrce,
                      TInt aBytesPerSrceLine, TInt aLineCount)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    const TInt bytesToCopyPerLine = Min(aBytesPerDestLine, aBytesPerSrceLine);

    for (TInt y = 0; y < aLineCount; ++y)
    {
        Mem::Copy(aDest, aSrce, bytesToCopyPerLine);
        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::Copy16
// ---------------------------------------------------------------------------
//
void CSwtImage::Copy16(TUint8* aDest, TInt aBytesPerDestLine, const TUint8* aSrce,
                       TInt aBytesPerSrceLine, TInt aLineCount, const MSwtPaletteData::TDirectData& aPalette)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    const TInt bytesToCopyPerLine = Min(aBytesPerDestLine, aBytesPerSrceLine) & ~0x1;

    for (TInt y = 0; y < aLineCount; ++y)
    {
        TUint8* ptr = aDest;

        for (TInt x = 0; x < bytesToCopyPerLine; x+=2)
        {
            // Data in source buffer in stored LSB first
            TUint pixel;
            pixel  = static_cast<TUint>(aSrce[x]);
            pixel |= static_cast<TUint>(aSrce[x+1]) << 8;

            WritePixel16(ptr, pixel, aPalette);
        }

        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::Copy1To24
// ---------------------------------------------------------------------------
//
void CSwtImage::Copy1ToTrueColor(TUint8* aDest, TInt aBytesPerDestLine, const TUint8* aSrce,
                                 TInt aBytesPerSrceLine, TInt aLineCount, const CPalette& aPalette, TBool a24Bit)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    const TInt bytesPerPixel = a24Bit ? 3 : 4;
    const TInt pixelsToCopyPerLine = Min(aBytesPerDestLine / bytesPerPixel,
                                         aBytesPerSrceLine * 8);
    const TInt bytesToCopyPerLine  = (pixelsToCopyPerLine + 7) / 8;

    for (TInt y = 0; y < aLineCount; ++y)
    {
        TUint8* ptr = aDest;

        TInt bitsLeftInLine = pixelsToCopyPerLine;
        for (TInt x = 0; x < bytesToCopyPerLine; ++x)
        {
            TUint8 byte = aSrce[x];
            TInt bitsToCopy = Min(bitsLeftInLine, 8);
            for (TInt i = 0; i < bitsToCopy; ++i)
            {
                WritePixelTrueColor(ptr, aPalette.GetEntry(byte >> 7), a24Bit);
                byte <<= 1;
            }
            bitsLeftInLine -= 8;
        }

        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::Copy2To24
// ---------------------------------------------------------------------------
//
void CSwtImage::Copy2ToTrueColor(TUint8* aDest, TInt aBytesPerDestLine, const TUint8* aSrce,
                                 TInt aBytesPerSrceLine, TInt aLineCount, const CPalette& aPalette, TBool a24Bit)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    const TInt bytesPerPixel = a24Bit ? 3 : 4;
    const TInt pixelsToCopyPerLine = Min(aBytesPerDestLine / bytesPerPixel,
                                         aBytesPerSrceLine*4);
    const TInt bytesToCopyPerLine  = (pixelsToCopyPerLine + bytesPerPixel) / 4;

    for (TInt y = 0; y < aLineCount; ++y)
    {
        TUint8* ptr = aDest;

        TInt bitsLeftInLine = pixelsToCopyPerLine * 2;
        for (TInt x = 0; x < bytesToCopyPerLine; ++x)
        {
            TUint8 byte = aSrce[x];
            TInt bitsToCopy = Min(bitsLeftInLine, 8);
            for (TInt i = 0; i < bitsToCopy; i += 2)
            {
                WritePixelTrueColor(ptr, aPalette.GetEntry(byte >> 6), a24Bit);
                byte <<= 2;
            }
            bitsLeftInLine -= 8;
        }

        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::Copy4To24
// ---------------------------------------------------------------------------
//
void CSwtImage::Copy4ToTrueColor(TUint8* aDest, TInt aBytesPerDestLine, const TUint8* aSrce,
                                 TInt aBytesPerSrceLine, TInt aLineCount, const CPalette& aPalette, TBool a24Bit)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    const TInt bytesPerPixel = a24Bit ? 3 : 4;
    const TInt pixelsToCopyPerLine = Min(aBytesPerDestLine / bytesPerPixel,
                                         aBytesPerSrceLine * 2);
    const TInt bytesToCopyPerLine = pixelsToCopyPerLine / 2;
    const TInt remainderPerLine = pixelsToCopyPerLine % 2;

    for (TInt y = 0; y < aLineCount; ++y)
    {
        TUint8* ptr = aDest;
        TInt x;
        for (x = 0; x < bytesToCopyPerLine; ++x)
        {
            TUint8 byte = aSrce[x];
            WritePixelTrueColor(ptr, aPalette.GetEntry(byte >> 4), a24Bit);
            WritePixelTrueColor(ptr, aPalette.GetEntry(byte & 0x0F), a24Bit);
        }

        if (remainderPerLine != 0)
        {
            WritePixelTrueColor(ptr, aPalette.GetEntry(aSrce[x] >> 4), a24Bit);
        }

        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::Copy8To24
// ---------------------------------------------------------------------------
//
void CSwtImage::Copy8ToTrueColor(TUint8* aDest, TInt aBytesPerDestLine, const TUint8* aSrce,
                                 TInt aBytesPerSrceLine, TInt aLineCount, const CPalette& aPalette, TBool a24Bit)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    const TInt bytesPerPixel = a24Bit ? 3 : 4;
    const TInt bytesToCopyPerLine = Min(aBytesPerDestLine / bytesPerPixel,
                                        aBytesPerSrceLine);

    for (TInt y = 0; y < aLineCount; ++y)
    {
        TUint8* ptr = aDest;

        for (TInt x = 0; x < bytesToCopyPerLine; ++x)
        {
            WritePixelTrueColor(ptr, aPalette.GetEntry(aSrce[x]), a24Bit);
        }

        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::Copy8To24
// ---------------------------------------------------------------------------
//
void CSwtImage::Copy8ToTrueColor(TUint8* aDest, TInt aBytesPerDestLine, const TUint8* aSrce,
                                 TInt aBytesPerSrceLine, TInt aLineCount, const MSwtPaletteData::TDirectData& aPalette,
                                 TBool a24Bit)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    const TInt bytesPerPixel = a24Bit ? 3 : 4;
    const TInt bytesToCopyPerLine = Min(aBytesPerDestLine / bytesPerPixel,
                                        aBytesPerSrceLine);

    const TUint redMax = BitShift(aPalette.iRedMask,  aPalette.iRedShift);
    const TUint greenMax = BitShift(aPalette.iGreenMask, aPalette.iGreenShift);
    const TUint blueMax = BitShift(aPalette.iBlueMask, aPalette.iBlueShift);

    for (TInt y = 0; y < aLineCount; ++y)
    {
        TUint8* ptr = aDest;

        for (TInt x = 0; x < bytesToCopyPerLine; ++x)
        {
            TUint red = (GetRed(aSrce[x], aPalette) * 255u) / redMax;
            TUint green = (GetGreen(aSrce[x], aPalette) * 255u) / greenMax;
            TUint blue = (GetBlue(aSrce[x], aPalette) * 255u) / blueMax;

            TRgb color(red, green, blue);
            WritePixelTrueColor(ptr, color, a24Bit);
        }

        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::Copy24
// ---------------------------------------------------------------------------
//
void CSwtImage::Copy24(TUint8* aDest, TInt aBytesPerDestLine, const TUint8* aSrce,
                       TInt aBytesPerSrceLine, TInt aLineCount, const MSwtPaletteData::TDirectData& aPalette)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    TInt bytesToCopyPerLine = Min(aBytesPerDestLine, aBytesPerSrceLine);
    bytesToCopyPerLine -= bytesToCopyPerLine % 3; // In order to have an integer number of pixels

    for (TInt y = 0; y < aLineCount; ++y)
    {
        TUint8* ptrDest = aDest;

        for (TInt x = 0; x < bytesToCopyPerLine; x += 3)
        {
            // Data in source buffer in stored MSB first
            TUint pixel;
            pixel = static_cast<TUint>(aSrce[x]) << 16;
            pixel |= static_cast<TUint>(aSrce[x+1]) << 8;
            pixel |= static_cast<TUint>(aSrce[x+2]);

            WritePixelTrueColor(ptrDest, pixel, aPalette, ETrue);
        }

        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::Copy32
// ---------------------------------------------------------------------------
//
void CSwtImage::Copy32(TUint8* aDest, TInt aBytesPerDestLine, const TUint8* aSrce,
                       TInt aBytesPerSrceLine, TInt aLineCount, const MSwtPaletteData::TDirectData& aPalette)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    TInt bytesToCopyPerLine = Min(aBytesPerDestLine, aBytesPerSrceLine);
    bytesToCopyPerLine -= bytesToCopyPerLine % 4; // In order to have an integer number of pixels

    for (TInt y = 0; y < aLineCount; ++y)
    {
        TUint8* ptrDest = aDest;

        for (TInt x = 0; x < bytesToCopyPerLine; x += 4)
        {
            TUint pixel = 0;

            // Data in source buffer in stored MSB first
            pixel = static_cast<TUint>(aSrce[x]) << 24;
            pixel |= static_cast<TUint>(aSrce[x+1]) << 16;
            pixel |= static_cast<TUint>(aSrce[x+2]) << 8;
            pixel |= static_cast<TUint>(aSrce[x+3]);

            WritePixelTrueColor(ptrDest, pixel, aPalette, EFalse);
        }

        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}


// ---------------------------------------------------------------------------
// CSwtImage::GetPixelDataLC
// ---------------------------------------------------------------------------
//
HBufC8* CSwtImage::GetPixelDataLC(const CFbsBitmap& aBitmap)
{
    TSize size(aBitmap.SizeInPixels());
    TDisplayMode mode(aBitmap.DisplayMode());
    TInt depth = BitDepth(mode);
    TInt bplSrce = CFbsBitmap::ScanLineLength(size.iWidth, mode);
    TInt bplDest = BytesPerLine(size.iWidth, depth, ScanlinePadding(depth));
    TInt bufLength = bplDest * size.iHeight;

    HBufC8* buffer = HBufC8::NewLC(bufLength);
    TPtr8 des(buffer->Des());
    des.SetLength(bufLength);
    TUint8* dest = const_cast<TUint8*>(des.Ptr());
    Mem::FillZ(dest, bufLength);

    aBitmap.LockHeapLC(ETrue);
    const TUint8* srce = reinterpret_cast<TUint8*>(aBitmap.DataAddress());

    switch (mode)
    {
    case EGray2:
        Copy1(dest, bplDest, srce, bplSrce, size.iHeight);
        break;
    case EGray4:
        Copy2(dest, bplDest, srce, bplSrce, size.iHeight);
        break;
    case EGray16:
        Copy4(dest, bplDest, srce, bplSrce, size.iHeight);
        break;
    case EGray256:
        /* This is workaround for case that the image width is not mutliple of four.
           The symbian function CFbsBitmap::ScanLineLength() returns muplitple of
           four and not the correct size. On the other hand BytesPerLine() returns
           the correct size. When using the incorrect size the image is skewed.
               Problem occured only on alpha bitmap with PNG images. This is the
           reason, why this fix is present only in this switch. Also it does not
           occur in 3.2 */
#if defined( RD_SCALABLE_UI_V2 )
        bplSrce = bplDest;
#endif // RD_SCALABLE_UI_V2
        Copy8(dest, bplDest, srce, bplSrce, size.iHeight);
        break;
    case EColor64K:
        GetPixels16(dest, bplDest, srce, bplSrce, size);
        break;
    case EColor16M:
        Copy8(dest, bplDest, srce, bplSrce, size.iHeight);
        break;
    case EColor16MU:
        Copy8(dest, bplDest, srce, bplSrce, size.iHeight);
        break;
    case EColor16MA:
        Copy8(dest, bplDest, srce, bplSrce, size.iHeight);
        break;
    case EColor16MAP:
        Copy8(dest, bplDest, srce, bplSrce, size.iHeight);
        break;
    default:
        __DEBUGGER();
        User::Leave(KErrNotSupported);
        break;
    };

    CleanupStack::PopAndDestroy(); // LockHeap

    return buffer;
}

// ---------------------------------------------------------------------------
// CSwtImage::GetPixels16
// ---------------------------------------------------------------------------
//
void CSwtImage::GetPixels16(TUint8* aDest, TInt aBytesPerDestLine,
                            const TUint8* aSrce, TInt aBytesPerSrceLine, const TSize& aSize)
{
    ASSERT(aDest);
    ASSERT(aSrce);

    for (TInt y = 0; y<aSize.iHeight; ++y)
    {
        const TUint16* ptrSrce = reinterpret_cast<const TUint16*>(aSrce);
        TUint8* ptrDest = aDest;

        TInt x;
        for (x = 0; x<aSize.iWidth; ++x)
        {
            TUint16 pixel = *ptrSrce++;

            // Data in dest buffer in stored LSB first
            *ptrDest++ = static_cast<TUint8>(pixel & 0x00FF);
            *ptrDest++ = static_cast<TUint8>(pixel >> 8);
        }

        aDest += aBytesPerDestLine;
        aSrce += aBytesPerSrceLine;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::NewGcL
// From MSwtBitmapDrawable
// ---------------------------------------------------------------------------
//
MSwtGc* CSwtImage::NewGcL()
{
    ASSERT(iDevice);
    ASSERT(iDisplay);

    CFbsBitGc* nativeGc = CFbsBitGc::NewL();
    nativeGc->Activate(iBitmapDevice);

    return iDisplay->Factory().NewBitmapGcL(*this, nativeGc,
                                            DestroyNativeGc, KRgbBlack, KRgbWhite, *iDevice->GetSystemFont());
}

// ---------------------------------------------------------------------------
// CSwtImage::GraphicsDevice
// From MSwtBitmapDrawable
// ---------------------------------------------------------------------------
//
CBitmapDevice& CSwtImage::GraphicsDevice()
{
    return *iBitmapDevice;
}

// ---------------------------------------------------------------------------
// CSwtImage::GraphicsDevice
// From MSwtBitmapDrawable
// ---------------------------------------------------------------------------
//
void CSwtImage::HandleUpdate()
{
    if (iMaskType == EColorKey)
    {
        iMaskType = EDirtyColorKey;
    }
}

// ---------------------------------------------------------------------------
// CSwtImage::DestroyNativeGc
// From MSwtBitmapDrawable
// ---------------------------------------------------------------------------
//
void CSwtImage::DestroyNativeGc(CBitmapContext* aGc)
{
    delete aGc;
}

// ---------------------------------------------------------------------------
// CSwtImage::GetBitmap
// From MSwtImage
// ---------------------------------------------------------------------------
//
CFbsBitmap& CSwtImage::GetBitmap() const
{
    return *iBitmap;
}

// ---------------------------------------------------------------------------
// CSwtImage::GetSubBitmap
// From MSwtImage
// ---------------------------------------------------------------------------
//
CFbsBitmap& CSwtImage::GetSubBitmap(const TSize& aSize) const
{
    ASSERT(iBitmap);

    // For the original size return the main bitmap
    if (iBitmap->SizeInPixels() == aSize)
    {
        return GetBitmap();
    }

    // Search scaled bitmap copy
    CFbsBitmap* bmp = NULL;
    TInt count = iScaledBitmaps.Count();
    for (TInt i = 0; i < count; i++)
    {
        bmp = iScaledBitmaps[i];
        if (bmp->SizeInPixels() == aSize)
        {
            return *bmp;
        }
    }

    // Scaled bitmap copy not found, create one for aSize
    TRAP_IGNORE(bmp = SwtControlHelper::GetCopyOfBitmapL(iBitmap, aSize));
    if (bmp)
    {
        // Create also scaled mask copy for aSize
        CFbsBitmap* mask = NULL;
        CFbsBitmap* maski = NULL;
        if (iMask)
        {
            TRAP_IGNORE(mask = SwtControlHelper::GetCopyOfBitmapL(iMask, aSize));
            if (iMask->IsMonochrome())
            {
                TRAP_IGNORE(maski = SwtControlHelper::GetInvertedCopyOfMonoBitmapL(iMask, aSize));
            }
        }

        // Update arrays

        // NOTE! iScaledBitmapRefs, iScaledBitmaps, iScaledMasks and
        // iScaledMasksInverted must always have the same number of entries.
        // NOTE! iScaledMasksInverted entries are valid only if
        // the main mask is monochrome. Otherwise they are NULL
        // and iScaledMasks entries should be used instead.
        TInt err = KErrNone;
        TInt appendCount = 0;
        err = iScaledBitmapRefs.Append(0);
        if (err == KErrNone)
        {
            ++appendCount;
            err = iScaledBitmaps.Append(bmp);
        }
        if (err == KErrNone)
        {
            ++appendCount;
            err = iScaledMasks.Append(mask);
        }
        if (err == KErrNone)
        {
            ++appendCount;
            err = iScaledMasksInverted.Append(maski);
        }

        if (err)
        {
            if (appendCount > 0)
            {
                iScaledBitmapRefs.Remove(iScaledBitmapRefs.Count() - 1);
            }
            if (appendCount > 1)
            {
                iScaledBitmaps.Remove(iScaledBitmaps.Count() - 1);
            }
            if (appendCount > 2)
            {
                iScaledMasks.Remove(iScaledMasks.Count() - 1);
            }

            delete bmp;
            bmp = NULL;
            delete mask;
            mask = NULL;
            delete maski;
            maski = NULL;
        }
    }

    if (!bmp)
    {
        // In the case of no memory or whatever
        ASSERT(EFalse);
        bmp = &GetBitmap();
    }

    return *bmp;
}

// ---------------------------------------------------------------------------
// CSwtImage::Dispose
// From MSwtImage
// ---------------------------------------------------------------------------
//
void CSwtImage::Dispose()
{
    // Likely reasons for this assertion to fail:
    //  - The SWT program disposing of its images before the objects that
    //    reference them;
    //  - Memory leak in one of the objects referencing the image ( AddRef() was
    //    called but not RemoveRef() ).
#ifdef ESWT_EARLY_DISPOSAL_CHECKING_ENABLED
    ASSERT(RefCount() == 1);
#endif // ESWT_EARLY_DISPOSAL_CHECKING_ENABLED
    RemoveRef();
}

// ---------------------------------------------------------------------------
// CSwtImage::MaskBitmap
// From MSwtImage
// ---------------------------------------------------------------------------
//
const CFbsBitmap* CSwtImage::MaskBitmap(TBool aInvertedIfMonochrome /*= EFalse*/) const
{
    EnsureMaskIsUpToDate();
    CFbsBitmap* mask = iMask;
    if (mask && aInvertedIfMonochrome && mask->IsMonochrome())
    {
        if (!iMaskInverted)
        {
            TRAP_IGNORE(iMaskInverted = SwtControlHelper::GetInvertedCopyOfMonoBitmapL(mask));
        }
        mask = iMaskInverted;
    }
    return mask;
}

// ---------------------------------------------------------------------------
// CSwtImage::SubMaskBitmap
// From MSwtImage
// ---------------------------------------------------------------------------
//
const CFbsBitmap* CSwtImage::SubMaskBitmap(const TSize& aSize,
        TBool aInvertedIfMonochrome /*= EFalse*/) const
{
    ASSERT(iBitmap);

    // For the original size return the main mask
    if (iBitmap->SizeInPixels() == aSize)
    {
        return MaskBitmap(aInvertedIfMonochrome);
    }

    // Find scaled copy of the bitmap and return its scaled mask.
    CFbsBitmap* bmp = NULL;
    CFbsBitmap* mask = NULL;
    TInt count = iScaledBitmaps.Count();
    for (TInt i = 0; i < count; i++)
    {
        bmp = iScaledBitmaps[i];
        if (bmp && bmp->SizeInPixels() == aSize)
        {
            mask = iScaledMasks[i];
            if (mask && aInvertedIfMonochrome && mask->IsMonochrome())
            {
                // NOTE! iScaledMasksInverted entries are valid only if
                // the main mask is monochrome. Otherwise they are NULL
                // and iScaledMasks entries should be used instead.
                mask = iScaledMasksInverted[i];
            }
            return mask;
        }
    }

    return mask;
}

// ---------------------------------------------------------------------------
// CSwtImage::GetBounds
// From MSwtImage
// ---------------------------------------------------------------------------
//
TRect CSwtImage::GetBounds() const
{
    return TRect(iBitmap->SizeInPixels());
}

// ---------------------------------------------------------------------------
// CSwtImage::GetImageDataL
// From MSwtImage
// ---------------------------------------------------------------------------
//
MSwtImageData* CSwtImage::GetImageDataL() const
{
    // Get the palette
    CSwtGrPaletteData* paletteData = GetPaletteDataL();
    CleanupStack::PushL(paletteData);

    // Get the buffers
    HBufC8* pixelData = GetPixelDataLC(*iBitmap);
    HBufC8* maskData = NULL;
    if (iMaskType == EBinaryMask)
    {
        ASSERT(iMask);
        maskData = GetPixelDataLC(*iMask);
    }

    HBufC8* alphaData = NULL;
    if (iMaskType == EAlphaMask)
    {
        ASSERT(iMask);
        alphaData = GetPixelDataLC(*iMask);
    }

    // Prepare the information
    MSwtImageData::TInfo info;
    info.iSize = iInfo.iSize;
    info.iDepth = BitDepth(iBitmap->DisplayMode());
    info.iScanlinePad = ScanlinePadding(info.iDepth);
    info.iBytesPerLine = BytesPerLine(info.iSize.iWidth, info.iDepth,
                                      info.iScanlinePad);
    info.iMaskPad = (maskData) ? ScanlinePadding(1) : 0;
    info.iAlpha = iInfo.iAlpha;
    info.iType = iInfo.iType;
    info.iTopLeft = iInfo.iTopLeft;
    info.iDisposalMethod = iInfo.iDisposalMethod;
    info.iDelayTime = iInfo.iDelayTime;

    if (!iColorKey)
    {
        info.iTransparentPixel = -1;
    }
    else
    {
        if (paletteData->IsDirect())
        {
            const MSwtPaletteData::TDirectData& directData =
                paletteData->DirectData();
            TUint r = BitShift(iColorKey->Red(),
                               - directData.iRedShift) & directData.iRedMask;
            TUint g = BitShift(iColorKey->Green(),
                               - directData.iGreenShift) & directData.iGreenMask;
            TUint b = BitShift(iColorKey->Blue(),
                               - directData.iBlueShift) & directData.iBlueMask;
            info.iTransparentPixel = r | g | b;
        }
        else
        {
            info.iTransparentPixel = paletteData->IndirectData()->
                                     NearestIndex(*iColorKey);
        }
    }

    if (alphaData)
    {
        CleanupStack::Pop(alphaData);
    }
    if (maskData)
    {
        CleanupStack::Pop(maskData);
    }
    CleanupStack::Pop(pixelData);
    CleanupStack::Pop(paletteData);

    return CSwtGrImageData::NewL(info, paletteData, pixelData, maskData, alphaData);
}

// ---------------------------------------------------------------------------
// CSwtImage::AddSubRef
// From MSwtImage
// ---------------------------------------------------------------------------
//
TInt CSwtImage::AddSubRef(const TSize& aSize) const
{
    ASSERT(iBitmap);
    if (iBitmap->SizeInPixels() == aSize)
    {
        AddRef();
        return KErrNone;
    }

    CFbsBitmap* bmp = NULL;
    TInt count = iScaledBitmaps.Count();
    for (TInt i = 0; i < count; i++)
    {
        bmp = iScaledBitmaps[i];
        if (bmp && bmp->SizeInPixels() == aSize)
        {
            iScaledBitmapRefs[i]++; // mutable
            return KErrNone;
        }
    }
    return KErrNotFound;
}

// ---------------------------------------------------------------------------
// CSwtImage::RemoveSubRef
// From MSwtImage
// ---------------------------------------------------------------------------
//
TInt CSwtImage::RemoveSubRef(const TSize& aSize) const
{
    ASSERT(iBitmap);
    if (iBitmap->SizeInPixels() == aSize)
    {
        RemoveRef();
        return KErrNone;
    }

    CFbsBitmap* bmp = NULL;
    CFbsBitmap* mask = NULL;
    CFbsBitmap* maski = NULL;
    TInt count = iScaledBitmaps.Count();
    for (TInt i = 0; i < count; i++)
    {
        // NOTE! iScaledBitmapRefs, iScaledBitmaps, iScaledMasks and
        // iScaledMasksInverted must always have the same number of entries.
        // NOTE! iScaledMasksInverted entries are valid only if
        // the main mask is monochrome. Otherwise they are NULL
        // and iScaledMasks entries should be used instead.
        bmp = iScaledBitmaps[i];
        mask = iScaledMasks[i];
        maski = iScaledMasksInverted[i];
        if (bmp && bmp->SizeInPixels() == aSize)
        {
            iScaledBitmapRefs[i]--; // mutable
            if (iScaledBitmapRefs[i] == 0)
            {
                delete bmp;
                delete mask;
                delete maski;
                iScaledMasks.Remove(i);
                iScaledMasksInverted.Remove(i);
                iScaledBitmaps.Remove(i);
                iScaledBitmapRefs.Remove(i);
            }
            return KErrNone;
        }
    }
    return KErrNotFound;
}

// ---------------------------------------------------------------------------
// CSwtImage::SubRefCount
// From MSwtImage
// ---------------------------------------------------------------------------
//
TInt CSwtImage::SubRefCount(const TSize& aSize) const
{
    ASSERT(iBitmap);
    if (iBitmap->SizeInPixels() == aSize)
    {
        return RefCount();
    }

    CFbsBitmap* bmp = NULL;
    TInt count = iScaledBitmaps.Count();
    for (TInt i = 0; i < count; i++)
    {
        bmp = iScaledBitmaps[i];
        if (bmp && bmp->SizeInPixels() == aSize)
        {
            return iScaledBitmapRefs[i];
        }
    }
    return 0;
}


// ---------------------------------------------------------------------------
// CSwtImage::BitmapWithAlphaLC
// From MSwtImage
// ---------------------------------------------------------------------------
//
CFbsBitmap* CSwtImage::BitmapWithAlphaLC()
{

    ASSERT(iMask);
    ASSERT(iBitmap);


    // Create the bitmap
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
#ifdef RD_JAVA_S60_RELEASE_9_2
    User::LeaveIfError(bitmap->Create(iBitmap->SizeInPixels(), EColor16MAP));
#else
    User::LeaveIfError(bitmap->Create(iBitmap->SizeInPixels(), EColor16MA));
#endif //RD_JAVA_S60_RELEASE_9_2


    // Create bitmap mask
    CFbsBitmap* mask = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(mask);
    User::LeaveIfError(mask->Create(iMask->SizeInPixels(), EGray256));

    // copy bitmap
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
    CleanupStack::PushL(bitmapDevice);
    BitBltBitmapL(*bitmapDevice, *iBitmap);
    CleanupStack::PopAndDestroy(bitmapDevice);

    // copy bitmap mask
    CFbsBitmapDevice* maskDevice = CFbsBitmapDevice::NewL(mask);
    CleanupStack::PushL(maskDevice);
    BitBltBitmapL(*maskDevice, *iMask);
    CleanupStack::PopAndDestroy(maskDevice);

    // add mask to alpha bitmap's alpha channel
    AddMaskToAlphaChannel(*bitmap, *mask);
    CleanupStack::PopAndDestroy(mask);
    return bitmap;
}


// ---------------------------------------------------------------------------
// CSwtImage::OfferResourceChangeL
// From MSwtResourceChangeObserver
// ---------------------------------------------------------------------------
//
void CSwtImage::OfferResourceChangeL(TInt /*aType*/)
{
    ASSERT(iDevice);

    // We set new twips size of bitmap in case device resolution has changed
    iBitmap->SetSizeInTwips(&(iDevice->GraphicsDevice()));
}
