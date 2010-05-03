/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Render context implementation
*
*/

// INCLUDE FILES
#include <eikenv.h> // CCoeEnv

#include "CMIDGraphics.h"
#include "CM2GRenderContext.h"
#include "MM2GSVGProxy.h"

M2G_NS_START

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
/* static */ const TReal32       MM2GRenderContext::KFullOpaque             = 1.0;
/* static */
const TReal32       MM2GRenderContext::KFullTransparency       = 0.0;
/* static */
const TUint8        MM2GRenderContext::KMaxAlphaValue          = 255;
/* static */
const TDisplayMode  MM2GRenderContext::KDefaultDisplayMode     = EColor16MA;
/* static */
const TDisplayMode  MM2GRenderContext::KMaskDisplayMode        = EGray256;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// -----------------------------------------------------------------------------
// CM2GRenderContext::CM2GRenderContext
// -----------------------------------------------------------------------------
CM2GRenderContext::CM2GRenderContext()
        : CBase(),
        iProxy(NULL),
        iEngineHandle(M2G_INVALID_HANDLE),
        iAlpha(MM2GRenderContext::KFullOpaque),
        iScaledAlpha(MM2GRenderContext::KMaxAlphaValue),
        iMaskBmp(NULL),
        iTargetBmp(NULL),
        iImgBmp(NULL)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::CM2GRenderContext");
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::NewL
// -----------------------------------------------------------------------------
CM2GRenderContext* CM2GRenderContext::NewL(MM2GSVGProxy* aProxy)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::NewL()");

    CM2GRenderContext* self = new(ELeave) CM2GRenderContext;
    CleanupStack::PushL(self);

    self->ConstructL(aProxy);

    CleanupStack::Pop();

    return self;
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::~CM2GRenderContext
// -----------------------------------------------------------------------------
CM2GRenderContext::~CM2GRenderContext()
{
    M2G_DEBUG_2("M2G_DEBUG: CM2GRenderContext::~CM2GRenderContext() - proxy=%d, engine=%d", iProxy, iEngineHandle);
    if ((iEngineHandle != M2G_INVALID_HANDLE) && (iProxy != NULL))
    {
        TRAP_IGNORE(iProxy->DeleteSvgEngineL(iEngineHandle));
    }
    delete iImgBmp;
    delete iMaskBmp;
    iFbsSession.Disconnect();
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::BindL
// -----------------------------------------------------------------------------
void CM2GRenderContext::BindL(TInt& aTargetHandle, TBool aUiToolKit)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::BindL()");
    if (!aUiToolKit)
    {
        // Current Ui Toolkit is LCDUI
        // Set target graphics bitmap
        CMIDGraphics* graphics = MIDUnhand<CMIDGraphics>(aTargetHandle);
        iTargetBmp = graphics->Bitmap();
        User::LeaveIfNull(iTargetBmp);
#ifdef RD_JAVA_NGA_ENABLED
        iTargetCanvas = graphics->GetTargetCanvas();        
#endif // RD_JAVA_NGA_ENABLED
    }
    // initialize the svg image bitmap (on which we will render later during RenderL())
    InitImageBitmapL();
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::GetImgHandleL
// -----------------------------------------------------------------------------
TM2GBitmapHandle CM2GRenderContext::GetImgHandleL() const
{
    User::LeaveIfNull(iImgBmp);

    return REINTERPRET_CAST(TM2GBitmapHandle, iImgBmp);
}
// -----------------------------------------------------------------------------
// CM2GRenderContext::InitImageBitmapL
// -----------------------------------------------------------------------------
void CM2GRenderContext::InitImageBitmapL()
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::InitImageBitmapL() - begin");

    // get the screen size
    TSize screenSize = CEikonEnv::Static()->ScreenDevice()->SizeInPixels();

    if (iImgBmp == NULL || (iImgBmp && iImgBmp->SizeInPixels() != screenSize))
    {
        M2G_DEBUG_2("M2G_DEBUG: CM2GRenderContext::InitImageBitmapL() - new bitmap: w=%d, h=%d", screenSize.iWidth, screenSize.iHeight);
        // Create a new svg surface if first time
        // or the size of the screen is different than the actual buffer size

        delete iImgBmp;
        iImgBmp = NULL;

        iImgBmp = new(ELeave) CFbsBitmap();

        User::LeaveIfError(
            iImgBmp->Create(screenSize, KDefaultDisplayMode));
    }

    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::InitImageBitmapL() - end");
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::InitMaskL
// -----------------------------------------------------------------------------
void CM2GRenderContext::InitMaskL(TSize aSize)
{
    M2G_DEBUG_2("M2G_DEBUG: CM2GRenderContext::InitMaskL( w=%d, h=%d ) - begin", aSize.iWidth, aSize.iHeight);

    if (iMaskBmp == NULL || (iMaskBmp && iMaskBmp->SizeInPixels() != aSize))
    {
        // Create a new mask but first delete the previous one if allocated
        delete iMaskBmp;
        iMaskBmp = NULL;

        M2G_DEBUG_0(" M2G_DEBUG: >CM2GRenderContext::InitMaskL() creates new mask ");

        iMaskBmp = new(ELeave) CFbsBitmap();

        User::LeaveIfError(
            iMaskBmp->Create(aSize, KMaskDisplayMode));
    }

    User::LeaveIfNull(iMaskBmp);

    // Mask is filled with the current alpha factor
    FillBitmapL(iMaskBmp, iScaledAlpha);

    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::InitMaskL() - end");
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::ReleaseL
// -----------------------------------------------------------------------------
void CM2GRenderContext::ReleaseL()
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ReleaseL() - begin");
    iTargetBmp = NULL;
#ifdef RD_JAVA_NGA_ENABLED
    iTargetCanvas = NULL;        
#endif // RD_JAVA_NGA_ENABLED
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ReleaseL() - end");
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::RenderL
// -----------------------------------------------------------------------------
void CM2GRenderContext::RenderLCDUIL(
    TM2GSvgDocumentHandle& aSvgDocHandle,
    const TReal32 aCurrentTime,
    TInt aSvgW, TInt aSvgH,
    TM2GRenderRect& aRect
)
{
    // prepare viewbox
    TRect viewbox;
    TPoint anchor;

    PrepareViewbox(aRect, aSvgW, aSvgH, viewbox, anchor);

    RenderLCDUIL(aSvgDocHandle, aCurrentTime, viewbox, anchor);
#ifdef RD_JAVA_NGA_ENABLED
    if (iTargetCanvas && iTargetCanvas->IsEglAvailable())
    {
        iTargetCanvas->UpdateRect(TRect(anchor, viewbox.Size()));
    }
#endif // RD_JAVA_NGA_ENABLED
    
}

// -----------------------------------------------------------------------------
void CM2GRenderContext::RenderESWTL(
    TM2GSvgDocumentHandle& aSvgDocHandle,
    const TReal32 aCurrentTime,
    TInt aSvgW, TInt aSvgH,
    TM2GRenderRect& aRect,
    MSwtClient* aClientHandle,
    TBool aUseNativeClear,
    TInt* aReturnData)
{
    // prepare viewbox
    TRect viewbox;
    TPoint anchor;

    PrepareViewbox(aRect, aSvgW, aSvgH, viewbox, anchor);

    aReturnData[0] = 0;
    aReturnData[1] = 0;
    aReturnData[2] = 0;
    aReturnData[3] = 0;
    aReturnData[4] = anchor.iX;
    aReturnData[5] = anchor.iY;
    aReturnData[6] = viewbox.iTl.iX;
    aReturnData[7] = viewbox.iTl.iY;
    aReturnData[8] = viewbox.Width();
    aReturnData[9] = viewbox.Height();

    RenderESWTL(aSvgDocHandle, aCurrentTime, viewbox, anchor, aClientHandle, aUseNativeClear, aReturnData);

    return;
}
// CM2GRenderContext::SetRenderingQualityL
// -----------------------------------------------------------------------------
void CM2GRenderContext::SetRenderingQualityL(TInt aMode)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::SetRenderingQualityL()");
    User::LeaveIfNull(iProxy);
    iProxy->RenderQualityL(iEngineHandle, aMode);
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::SetTransparencyL
// -----------------------------------------------------------------------------
void CM2GRenderContext::SetTransparency(TReal32 aAlpha)
{
    iAlpha = aAlpha;
    iScaledAlpha = STATIC_CAST(TUint8, (aAlpha * MM2GRenderContext::KMaxAlphaValue));
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::ConstructL
// -----------------------------------------------------------------------------
void CM2GRenderContext::ConstructL(MM2GSVGProxy* aProxy)
{
    // Init member variables
    SetTransparency(MM2GRenderContext::KFullOpaque);

    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ConstructL() - begin");

    if (aProxy)
    {
        iProxy = aProxy;
        iProxy->CreateSvgEngineL(iEngineHandle);
        M2G_DEBUG_2("M2G_DEBUG: CM2GRenderContext::ConstructL() - proxy: %d, new engine: %d", iProxy, iEngineHandle);
    }
    else
    {
        M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ConstructL() - proxy is invalid");
        M2G_THROW(KM2GArgNotOk);
    }

    User::LeaveIfError(iFbsSession.Connect());
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ConstructL() - end");
}
// -----------------------------------------------------------------------------
// CM2GRenderContext::PrepareViewbox
// -----------------------------------------------------------------------------
void CM2GRenderContext::PrepareViewbox(
    TM2GRenderRect& aRr,
    TInt aSvgW, TInt aSvgH,
    TRect& aViewbox, TPoint& aAnchor)
{
    M2G_DEBUG_6("M2G_DEBUG: CM2GRenderContext::PrepareViewbox() cX=%d, cY=%d, cW=%d, cH=%d, anchorX=%d, anchorY=%d - begin", aRr.GetClipX(), aRr.GetClipY(), aRr.GetClipW(), aRr.GetClipH(), aRr.GetAnchorX(), aRr.GetAnchorY());

    // Create an anchor point and an svg render area rect
    aAnchor.SetXY(aRr.GetAnchorX(), aRr.GetAnchorY());
    aViewbox.SetRect(aAnchor, TSize(aSvgW, aSvgH));

    // NOTE: It's already verified in Java side that the SVG render area and
    // the clip area intersects each other
    aViewbox.Intersection(aRr);

    // Check if the clip rect has changes the svg rect
    if (aViewbox.iTl != aAnchor)
    {
        // Update anchor position
        TPoint oldAnchor(aAnchor);
        aAnchor = aViewbox.iTl;

        // Update svg rect
        aViewbox.Move((-oldAnchor.iX), (-oldAnchor.iY));
    }
    else
    {
        // The clip rect has not changed the svg rect. Only the
        // anchor position need to be updated
        aViewbox.Move(-aAnchor.iX, -aAnchor.iY);
    }
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::PrepareViewbox() - end");
}
// -----------------------------------------------------------------------------
// CM2GRenderContext::RenderL
// -----------------------------------------------------------------------------
/**
 * @see CBitmapContext::BitBltMasked()
 * @note CBitmapContext::BitBltMasked() does alpha blending if the provided mask is 256 and not null,
 * and simply blits otherwise
 */
void CM2GRenderContext::RenderLCDUIL(
    TM2GSvgDocumentHandle& aSvgDocHandle,
    TReal32 aCurrentTime,
    const TRect& aViewbox,
    const TPoint& aAnchor
)
{
    M2G_DEBUG_4("M2G_DEBUG: CM2GRenderContext::RenderL() viewbox: x=%d, y=%d, w=%d, h=%d begin", aViewbox.iTl.iX, aViewbox.iTl.iY, aViewbox.Size().iWidth, aViewbox.Size().iHeight);

    // No need to render if content is fully transparency (i.e. alpha=0)
    if (iScaledAlpha == 0)
    {
        return;
    }

    // 0: clear the previous rendered image
    ClearBitmapL(iImgBmp);

    // 1: render the svg document on the iImgBmp
    iProxy->RenderDocumentL(
        iEngineHandle,
        aSvgDocHandle,
        GetImgHandleL(), NULL, aCurrentTime);

    CFbsBitmap* mask = NULL;

    // 2: calculate mask if alpha-blending is used
    if (iScaledAlpha < MM2GRenderContext::KMaxAlphaValue)
    {
        // mask is always the same size as the viewbox
        CreateAlphaBlendMaskL(aViewbox.Size());

        mask = iMaskBmp; // use this mask for alpha blending
    }

    // 3: blit the rendered image on the target bitmap
    // it will do alpha blending when mask is not null
    M2GBitmapUtils::BitBlt(*iTargetBmp,
                           *iImgBmp,
                           aAnchor,
                           &aViewbox,
                           mask);

    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::RenderL() end");
}

// -----------------------------------------------------------------------------
void CM2GRenderContext::RenderESWTL(
    TM2GSvgDocumentHandle& aSvgDocHandle,
    TReal32 aCurrentTime,
    const TRect& aViewbox,
    const TPoint& aAnchor,
    MSwtClient* /*aClientHandle*/,
    TBool /*aUseNativeClear*/,
    TInt* aReturnData)
{
    M2G_DEBUG_4("M2G_DEBUG: CM2GRenderContext::RenderL() viewbox: x=%d, y=%d, w=%d, h=%d begin", aViewbox.iTl.iX, aViewbox.iTl.iY, aViewbox.Size().iWidth, aViewbox.Size().iHeight);

    // No need to render if content is fully transparency (i.e. alpha=0)
    if (iScaledAlpha == 0)
    {
        return;
    }

    // 0: clear the previous rendered image
    ClearBitmapL(iImgBmp);

    // 1: render the svg document on the iImgBmp
    iProxy->RenderDocumentL(
        iEngineHandle,
        aSvgDocHandle,
        GetImgHandleL(), NULL, aCurrentTime);

    CFbsBitmapHack* tempImgBmp = new(ELeave) CFbsBitmapHack();
    tempImgBmp->Create(aViewbox.Size(), iImgBmp->DisplayMode());
    ClearBitmapL(tempImgBmp);

    M2GBitmapUtils::BitBlt(*tempImgBmp,
                           *iImgBmp,
                           aAnchor,
                           &aViewbox,
                           NULL);
    aReturnData[0] = tempImgBmp->Handle();
    aReturnData[1] = tempImgBmp->GetMyHandle();
    tempImgBmp = NULL;

    // 2: calculate mask if alpha-blending is used
    if (iScaledAlpha < MM2GRenderContext::KMaxAlphaValue)
    {
        // mask is always the same size as the viewbox
        CreateAlphaBlendMaskL(aViewbox.Size());

        if (iMaskBmp)
        {
            CFbsBitmapHack* tempMaskBmp = new(ELeave) CFbsBitmapHack();
            tempMaskBmp->Create(aViewbox.Size(), iMaskBmp->DisplayMode());
            ClearBitmapL(tempMaskBmp);

            M2GBitmapUtils::BitBlt(*tempMaskBmp,
                                   *iMaskBmp,
                                   aAnchor,
                                   &aViewbox,
                                   NULL);

            aReturnData[2] = tempMaskBmp->Handle();
            aReturnData[3] = tempMaskBmp->GetMyHandle();
            tempMaskBmp = NULL;
        }
    }

    // Is this needed ??
    //iImgBmp = NULL;
    //iMaskBmp = NULL;

    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::RenderL() end");
}
// CM2GRenderContext::CreateAlphaBlendMask
// -----------------------------------------------------------------------------
/**
 * <li> mask is initialized with iScaledAlpha for all pixels.
 * <li> reset the mask for those pixels that are not rendered (a = 0).
 * <li> due to anti-aliasing (a != 1),
 * recompute the mask by combining the pixel's alpha with the whole image alpha
 *
 * @note The svg backbuffer area is larger than the viewbox
 * @note The size of the mask is the same as for SVG backbuffer
 */
void CM2GRenderContext::CreateAlphaBlendMaskL(const TSize& aViewBox)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::CreateAlphaBlendMaskL() - begin");

    // the mask will have the same size as the svg image bitmap
    // allocate memory for mask according to the size
    // and initializes it with iScaledAlpha for each pixel ( i.e. Mem::Fill() = fast operation )
    InitMaskL(iImgBmp->SizeInPixels());

    // lock svg buffer and mask bitmaps
    TM2GBitmapLock maskLock(iMaskBmp);
    TM2GBitmapLock imgLock(iImgBmp);

    TUint8*            maskDataPtr    = REINTERPRET_CAST(TUint8*, iMaskBmp->DataAddress());
    const TUint32*     imgDataPtr     = iImgBmp->DataAddress();

    // Each row of CFbsBitmap is aligned to a 4 bytes boundary
    // if the mask width in pixels is not a multiple of 4, then we must align the pointer
    // to the correct address corresponding to the first pixel on the line below

    TInt maskMaxWidthBytes  = iMaskBmp->ScanLineLength(
                                  iMaskBmp->SizeInPixels().iWidth, KMaskDisplayMode);

    const TInt maskDeltaIndex = maskMaxWidthBytes - aViewBox.iWidth;

    // the svg backbuffer width could be larger than the viewbox width
    const TInt imgDeltaIndex = iImgBmp->SizeInPixels().iWidth - aViewBox.iWidth;

    // how many pixels from the viewbox will be rechecked for alpha value
    const TUint32 countViewBoxPixels = aViewBox.iHeight * aViewBox.iWidth;

    TUint32 imgAlphaMSB;
    TUint32 imgAlpha32;
    TUint32 index = 0;

    while (index < countViewBoxPixels)
    {
        // get the alpha channel in the most left octet (MSB) within a 4 octet
        imgAlphaMSB = *imgDataPtr & 0xFF000000;

        if (imgAlphaMSB == 0x0)
        {
            // pixel is not rendered so we mask it out
            *maskDataPtr = 0x0;
        }
        else if (imgAlphaMSB != 0xFF000000)
        {
            // combine the pixel's alpha with the whole image's alpha (iScaledAlpha)
            // we are not sure if shifting to right pads it with 0 to the left

            imgAlpha32 = (imgAlphaMSB >> 24) & 0xFF;

            // compute the combined alpha on 32b because of possible overflow,
            // and then downcast to TUint8
            // iScaledAlpha = 8b, KMaxAlphaValue = 8b

            *maskDataPtr = TUint8(
                               (imgAlpha32 * iScaledAlpha) / MM2GRenderContext::KMaxAlphaValue);
        }

        imgDataPtr++;
        maskDataPtr++;
        index++;

        // synchronize the image and mask indexes because the viewbox could be smaller than the image/mask
        if (index % aViewBox.iWidth == 0)
        {
            imgDataPtr += imgDeltaIndex;
            maskDataPtr += maskDeltaIndex;
        }
    }
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::CreateAlphaBlendMaskL() - end");
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::ClearBitmapL
// -----------------------------------------------------------------------------
void CM2GRenderContext::ClearBitmapL(CFbsBitmap* aBmp)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ClearBitmap - begin");

    User::LeaveIfNull(aBmp);

    TM2GBitmapLock lock(aBmp);

    TSize   size            = aBmp->SizeInPixels();
    TInt    scanlineLength  = aBmp->ScanLineLength(size.iWidth, aBmp->DisplayMode());

    TUint32* buf = aBmp->DataAddress();
    char* bufBytes = REINTERPRET_CAST(char*, buf);

    Mem::FillZ(bufBytes,  size.iHeight * scanlineLength);

    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ClearBitmap - end");
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::FillBitmapL
// -----------------------------------------------------------------------------
void CM2GRenderContext::FillBitmapL(CFbsBitmap* aBmp, const TUint8& aChar)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GRenderContext::FillBitmap() filled with=%d - begin", aChar);

    User::LeaveIfNull(aBmp);

    TM2GBitmapLock lock(aBmp);

    TSize   size            = aBmp->SizeInPixels();
    TInt    scanlineLength  = aBmp->ScanLineLength(size.iWidth, aBmp->DisplayMode());

    TUint32* buf = aBmp->DataAddress();
    char* bufBytes = REINTERPRET_CAST(char*, buf);

    Mem::Fill(bufBytes, size.iHeight * scanlineLength, aChar);

    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::FillBitmap - end");
}

M2G_NS_END
