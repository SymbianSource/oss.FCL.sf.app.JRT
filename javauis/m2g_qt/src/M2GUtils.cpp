/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Util methods
*
*/

// INCLUDE FILES
#include <bitdev.h> // CFbsBitmapDevice
#include <bitstd.h> // CFbsBitGc
#include <e32math.h> // FRand
#include "M2GUtils.h"


M2G_NS_START

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class TSWTBitBlt;
class TM2GBitmapLock;
// -----------------------------------------------------------------------------
// M2GBitmapUtils::BitBlt
// -----------------------------------------------------------------------------
TInt M2GBitmapUtils::BitBlt(CFbsBitmap& aTarget,
                            const CFbsBitmap& aSource,
                            const TPoint& aPoint,
                            const TRect* aRect,
                            const CFbsBitmap* aSourceMask)
{
    M2G_DEBUG_2("M2G_DEBUG: M2GBitmapUtils::BitBlt() - Point(x=%d, y=%d)", aPoint.iX, aPoint.iY);
    CFbsBitGc* context = NULL;
    CFbsBitmapDevice* device = NULL;
    TInt err = KM2GOk;
    TRAP(err, (device = CFbsBitmapDevice::NewL(&aTarget)));
    if ((err == KM2GOk) && (device != NULL))
    {
        err = device->CreateContext(context);
        if ((err == KM2GOk) && (context != NULL))
        {
            M2G_DEBUG_0("M2G_DEBUG: M2GBitmapUtils::BitBlt() - CFbsBitGc::BitBlt()");
            if (aRect)
            {
                // Check if mask
                if (aSourceMask)
                {
                    M2G_DEBUG_4("M2G_DEBUG: M2GBitmapUtils::BitBlt() -  mask rect(x1=%d, y1=%d, x2=%d, y2=%d)", aRect->iTl.iX, aRect->iTl.iY, aRect->iBr.iX, aRect->iBr.iY);
                    // A pixel that is masked by a BLACK is NOT transferred to a destination rectangle.
                    context->BitBltMasked(aPoint, &aSource, *aRect, aSourceMask, ETrue);
                    err = KM2GOk;
                }
                else
                {
                    M2G_DEBUG_4("M2G_DEBUG: M2GBitmapUtils::BitBlt() - rect(x1=%d, y1=%d, x2=%d, y2=%d)", aRect->iTl.iX, aRect->iTl.iY, aRect->iBr.iX, aRect->iBr.iY);
                    context->BitBlt(aPoint, &aSource, *aRect);
                    err = KM2GOk;
                }
            }
            else
            {
                M2G_DEBUG_0("M2G_DEBUG: M2GBitmapUtils::BitBlt() - no rect");
                context->BitBlt(aPoint, &aSource);
                err = KM2GOk;
            }
        }
    }
    delete context;
    delete device;
    context = NULL;
    device = NULL;
    M2G_DEBUG_1("M2G_DEBUG: M2GBitmapUtils::BitBlt() - end: %d", err);
    return err;
}


TInt M2GBitmapUtils::BitQBlt(QImage& aTargetQimage,
                             const QImage& aSourceQimage,
                             const TPoint& aPoint,
                             const TRect* aRect,
                             const CFbsBitmap* aSourceMask)
{
    M2G_DEBUG_2("M2G_DEBUG: M2GBitmapUtils::BitQBlt() - Point(x=%d, y=%d)", aPoint.iX, aPoint.iY);

    QPixmap pixmapTarget = QPixmap::fromImage(aTargetQimage);
    CFbsBitmap* aTarget  = pixmapTarget.toSymbianCFbsBitmap();

    QPixmap pixmapSource = QPixmap::fromImage(aSourceQimage);
    CFbsBitmap* aSource = pixmapSource.toSymbianCFbsBitmap();

    CFbsBitGc* context = NULL;
    CFbsBitmapDevice* device = NULL;
    TInt err = KM2GOk;
    TRAP(err, (device = CFbsBitmapDevice::NewL(aTarget)));
    if ((err == KM2GOk) && (device != NULL))
    {
        err = device->CreateContext(context);
        if ((err == KM2GOk) && (context != NULL))
        {
            M2G_DEBUG_0("M2G_DEBUG: M2GBitmapUtils::BitBlt() - CFbsBitGc::BitBlt()");
            if (aRect)
            {
                // Check if mask
                if (aSourceMask)
                {
                    M2G_DEBUG_4("M2G_DEBUG: M2GBitmapUtils::BitBlt() -  mask rect(x1=%d, y1=%d, x2=%d, y2=%d)", aRect->iTl.iX, aRect->iTl.iY, aRect->iBr.iX, aRect->iBr.iY);
                    // A pixel that is masked by a BLACK is NOT transferred to a destination rectangle.
                    context->BitBltMasked(aPoint, aSource, *aRect, aSourceMask, ETrue);
                    err = KM2GOk;
                }
                else
                {
                    M2G_DEBUG_4("M2G_DEBUG: M2GBitmapUtils::BitBlt() - rect(x1=%d, y1=%d, x2=%d, y2=%d)", aRect->iTl.iX, aRect->iTl.iY, aRect->iBr.iX, aRect->iBr.iY);
                    context->BitBlt(aPoint, aSource, *aRect);
                    err = KM2GOk;
                }
            }
            else
            {
                M2G_DEBUG_0("M2G_DEBUG: M2GBitmapUtils::BitBlt() - no rect");
                context->BitBlt(aPoint, aSource);
                err = KM2GOk;
            }
        }
    }
    delete context;
    delete device;
    context = NULL;
    device = NULL;
    M2G_DEBUG_1("M2G_DEBUG: M2GBitmapUtils::BitBlt() - end: %d", err);
    return err;
}



TInt M2GBitmapUtils::BitBlt(CBitmapContext& aTargetContext,
                            const CFbsBitmap& aSource,
                            const TPoint& aPoint,
                            const TRect* aRect,
                            const CFbsBitmap* aSourceMask,
                            /*MSwtClient* aClientHandle,*/
                            TBool aUseNativeClear /*= EFalse*/)
{
    M2G_DEBUG_2("M2G_DEBUG: M2GBitmapUtils::BitBlt() - Point(x=%d, y=%d)", aPoint.iX, aPoint.iY);
    TInt err = KM2GOk;
    TSWTBitBlt bitBlitter(aTargetContext, aPoint,
                          &aSource, aRect, aSourceMask, aUseNativeClear);
    bitBlitter();
    M2G_DEBUG_1("M2G_DEBUG: M2GBitmapUtils::BitBlt() - end: %d", err);
    return err;
}

// -----------------------------------------------------------------------------
// TM2GRenderRect::TM2GRenderRect
// -----------------------------------------------------------------------------
TM2GRenderRect::TM2GRenderRect(
    TInt aAnchorX, TInt aAnchorY,
    TInt aClipX, TInt aClipY,
    TInt aClipW, TInt aClipH)
        : TRect(
            TPoint(aClipX, aClipY),
            TSize(aClipW, aClipH)),
        iAnchorX(aAnchorX),
        iAnchorY(aAnchorY)
{
}

// -----------------------------------------------------------------------------
// TM2GRenderRect::TM2GRenderRect
// -----------------------------------------------------------------------------
TM2GRenderRect::TM2GRenderRect(TInt* aDimensions, TInt /*aLength*/)
        : TRect(
            TPoint(aDimensions[EClipX], aDimensions[EClipY]),
            TSize(aDimensions[EClipW], aDimensions[EClipH])),
        iAnchorX(aDimensions[EAnchorX]),
        iAnchorY(aDimensions[EAnchorY])
{
}

// -----------------------------------------------------------------------------
// TM2GRenderRect::~TM2GRenderRect
// -----------------------------------------------------------------------------
TM2GRenderRect::~TM2GRenderRect()
{
}

// -----------------------------------------------------------------------------
// TM2GRenderRect::TM2GRenderRect
// -----------------------------------------------------------------------------
TM2GRenderRect::TM2GRenderRect(const TM2GRenderRect& aRd)
{
    (*this = aRd);
}

// -----------------------------------------------------------------------------
// TM2GRenderRect::operator=
// -----------------------------------------------------------------------------
TM2GRenderRect& TM2GRenderRect::operator=(const TM2GRenderRect& aRd)
{
    if (this != &aRd)
    {
        iAnchorX = aRd.iAnchorX;
        iAnchorY = aRd.iAnchorY;
        SetRect(aRd.iTl.iX, aRd.iTl.iY, aRd.iBr.iX, aRd.iBr.iY);
    }
    return *this;
}

// -----------------------------------------------------------------------------
// TM2GRenderRect::GetRegionSizeInPixels
// -----------------------------------------------------------------------------
TSize TM2GRenderRect::GetRegionSizeInPixels(
    TM2GRenderRect& aRect,
    const TSize& aSz)
{
    return TSize(
               // determine the width of the region to be paint
               M2GGeneral::Min< TInt >(aRect.GetAnchorX() + aSz.iWidth, aRect.GetClipX() + aRect.GetClipW()) -
               M2GGeneral::Max< TInt >(aRect.GetAnchorX(), aRect.GetClipX()),
               // determine the height of the region to be paint
               M2GGeneral::Min< TInt >(aRect.GetAnchorY() + aSz.iHeight, aRect.GetClipY() + aRect.GetClipH()) -
               M2GGeneral::Max< TInt >(aRect.GetAnchorY(), aRect.GetClipY()));
}


// -----------------------------------------------------------------------------
// TM2GBitmapLock::TM2GBitmapLock
// -----------------------------------------------------------------------------
TM2GBitmapLock::TM2GBitmapLock(const CFbsBitmap* aBitmap, TBool aLock)
        : iBitmap(aBitmap), iIsLocked(EFalse)
{
    if (aLock)
    {
        Lock();
    }
}

// -----------------------------------------------------------------------------
// TM2GBitmapLock::~TM2GBitmapLock
// -----------------------------------------------------------------------------
TM2GBitmapLock::~TM2GBitmapLock()
{
    Unlock();
}

// -----------------------------------------------------------------------------
// TM2GBitmapLock::Lock()
// -----------------------------------------------------------------------------
void TM2GBitmapLock::Lock()
{
    if (iBitmap && !iIsLocked)
    {
        iBitmap->LockHeap();
        iIsLocked = ETrue;
    }
}

// -----------------------------------------------------------------------------
// TM2GBitmapLock::Unlock()
// -----------------------------------------------------------------------------
void TM2GBitmapLock::Unlock()
{
    if (iBitmap && iIsLocked)
    {
        iBitmap->UnlockHeap();
        iIsLocked = EFalse;
    }
}

TSWTBitBlt::TSWTBitBlt(CBitmapContext& aTargetContext,
                       const TPoint& aPoint,
                       const CFbsBitmap* aBitmap,
                       const TRect* aSourceRect,
                       const CFbsBitmap* aMaskBitmap,
                       TBool aUseNativeClear)
        : iTargetContext(aTargetContext),
        iPoint(aPoint),
        iUseNativeClear(aUseNativeClear)
{
    iBitmap = aBitmap;
    iRect = aSourceRect;
    iMaskBitmap = aMaskBitmap;
}
void TSWTBitBlt::operator()() const
{
    M2G_DEBUG_0("TSWTBitBlt()+");
    CFbsBitmap* tempBitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(tempBitmap);
    User::LeaveIfError(tempBitmap->Duplicate(iBitmap->Handle()));
    if (iRect)
    {
        if (iUseNativeClear)
        {
            iTargetContext.SetBrushColor(KRgbWhite);
            iTargetContext.Clear(*iRect);
        }
        if (iMaskBitmap)
        {
            CFbsBitmap* tempMask = new(ELeave) CFbsBitmap;
            CleanupStack::PushL(tempMask);
            User::LeaveIfError(tempMask->Duplicate(iMaskBitmap->Handle()));
            iTargetContext.BitBltMasked(
                iPoint, tempBitmap, *iRect, tempMask, ETrue);
            CleanupStack::PopAndDestroy(tempMask);
        }
        else
        {
            iTargetContext.BitBlt(iPoint, tempBitmap, *iRect);
        }
    }
    else
    {
        if (iUseNativeClear)
        {
            iTargetContext.SetBrushColor(KRgbWhite);
            iTargetContext.Clear();
        }
        iTargetContext.BitBlt(iPoint, tempBitmap);
    }
    CleanupStack::PopAndDestroy(tempBitmap);
    M2G_DEBUG_0("TSWTBitBlt()-");
}
M2G_NS_END
