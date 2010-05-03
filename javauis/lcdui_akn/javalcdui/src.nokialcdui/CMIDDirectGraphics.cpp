/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Native side interface for DirectGraphics java class.
*
*/


#include <jutils.h> // MIDMakeHandle
#include <gdi.h> // CWsBitmap

#include "CMIDImage.h" // CMIDImage
#include "CMIDGraphics.h" // CMIDGraphics
#ifdef RD_JAVA_NGA_ENABLED
#include "lcdui.h"
#endif

#include "CMIDDirectGraphics.h"
#include "CMIDNativeConverter.h"
#include "CMIDConvertFactory.h"
#include "TMIDInternalARGB.h"
#include "nativecolors.h"
#include "TMIDBitmapParameters.h"

namespace
{
const TUint8 KDivByEight = 3;
const TInt KGray2Divider = 32;
const TInt KGray4Divider = 16;
const TInt KColor16Divider = 8;
const TInt KColor256Divider = 4;
const TInt KColor64KDivider = 2;
const TInt KMaxInteger = 2147483647;
}

// -----------------------------------------------------------------------------
// CMIDDirectGraphics::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMIDDirectGraphics* CMIDDirectGraphics::NewL(CMIDGraphics* aGraphics,
        MMIDImage* aImage)
{
    CMIDDirectGraphics* graphics = new(ELeave) CMIDDirectGraphics();
    CleanupStack::PushL(graphics);
    graphics->ConstructL(aGraphics, aImage);
    CleanupStack::Pop(graphics);
    return graphics;
}

CMIDDirectGraphics::CMIDDirectGraphics():
        iDottedLine(EFalse)
{
}

CMIDDirectGraphics::~CMIDDirectGraphics()
{
    delete iConverterFactory;
    delete iNativeConverter;
    delete iPolygonFiller;
}

// -----------------------------------------------------------------------------
// CMIDDirectGraphics::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMIDDirectGraphics::ConstructL(CMIDGraphics* aGraphics,
                                    MMIDImage *aImage)
{
    User::LeaveIfNull(aGraphics);
    iGraphics = aGraphics;

#ifdef RD_JAVA_NGA_ENABLED
    iCanvasTarget = iGraphics->GetTargetCanvas();
#endif

    CFbsBitmap* bitmap = iGraphics->Bitmap();
    User::LeaveIfNull(bitmap);

    iScreenSize = bitmap->SizeInPixels();

    TMIDBitmapParameters params;
    params.iOffset = 0;
    params.iX = 0;
    params.iY = 0;
    params.iWidth = iScreenSize.iWidth;
    params.iHeight = iScreenSize.iHeight;
    params.iManipulation = 0;
    params.iClipRect = TRect(iScreenSize);


    TDisplayMode displayMode = bitmap->DisplayMode();

    params.iScanLength = CalculateScanLength(iScreenSize.iWidth,
                         displayMode);


    bitmap->LockHeap(ETrue);
    params.iPixels = bitmap->DataAddress();
    bitmap->UnlockHeap(ETrue);

    params.iPixelsSize = (params.iScanLength *
                          iScreenSize.iHeight *
                          bitmap->Header().iBitsPerPixel) >> KDivByEight;

    if (aImage)
    {
        // note: getting bitmap image increases its reference count
        MMIDBitmapImage* bitmapImage = aImage->BitmapImage();
        CFbsBitmap* alphaBitmap = bitmapImage->AlphaBitmap();
        if (alphaBitmap)
        {
            params.iTransparency = ETrue;
            alphaBitmap->LockHeap();
            params.iAlpha = alphaBitmap->DataAddress();
            params.iAlphaMode = alphaBitmap->DisplayMode();
            TSize bitmapSize(alphaBitmap->SizeInPixels());
            TInt scanLineLength =
                CFbsBitmap::ScanLineLength(bitmapSize.iWidth,
                                           alphaBitmap->DisplayMode());
            TInt bitsPerPixel = alphaBitmap->Header().iBitsPerPixel;
            scanLineLength = (scanLineLength << KMIDShift3Bits) / bitsPerPixel;
            params.iAlphaSize = (scanLineLength * bitmapSize.iHeight *
                                 bitsPerPixel) >> KMIDShift3Bits;
            alphaBitmap->UnlockHeap();
        }
        bitmapImage->RemoveRef();
    }

    iNativePixelFormat = CMIDConvertFactory::GetPixelFormat(displayMode);
    params.iFormat = iNativePixelFormat;

    iNativeConverter = CMIDNativeConverter::NewL(params);

    iClipRect = params.iClipRect;

    iPolygonFiller = new(ELeave) CPolygonFiller();

    iConverterFactory = new(ELeave) CMIDConvertFactory();


}


void CMIDDirectGraphics::SetClippingRect(TRect aRect)
{
    TRect screenRect(iScreenSize);
    screenRect.Intersection(aRect);
    iClipRect = screenRect;
}

inline TBool IsFormatARGB(TInt aFormat)
{
    switch (aFormat)
    {
    case KMIDTypeInt8888ARGB:
    case KMIDTypeInt888ARGB:
    case KMIDTypeUshort4444ARGB:
    case KMIDTypeUshort1555ARGB:
        return ETrue;

    default:
        return EFalse;
    }
}

void CMIDDirectGraphics::SetColorL(TUint32 aColor, TInt aImageHandle)
{
    TMIDInternalARGB internalColor;
    iNativeConverter->Convert(internalColor, aColor);
    iNativeConverter->SetDrawRect(iClipRect);

    // if native pixel format is ARGB or converted internal color is fully opaque
    // there is no need to create separate alpha bitmap, just plot
    if (IsFormatARGB(iNativePixelFormat) || KAlphaFullOpaque == internalColor.iA)
    {
        iGraphics->Bitmap()->LockHeap(ETrue);

        // bitmaps can change so have to re-install them every time
        iNativeConverter->SetBitmaps(iGraphics->Bitmap()->DataAddress());

        do
        {
            do
            {
                iNativeConverter->PlotPixel(internalColor);
            }
            while (iNativeConverter->AdvanceX());
        }
        while (iNativeConverter->AdvanceY());

        iGraphics->Bitmap()->UnlockHeap(ETrue);
    }
    // otherwise create separate alpha bitmap and set possible alpha channel
    else
    {
        CMIDImage* image = MIDUnhandObject< CMIDImage >(aImageHandle);

        User::LeaveIfError(image->SetTransparencyType(MMIDImage::EAlpha));

        MMIDBitmapImage* bitmapImage = image->BitmapImage();
        CFbsBitmap* bitmap = bitmapImage->ColorBitmap();
        CFbsBitmap* alphaBitmap = bitmapImage->AlphaBitmap();
        __ASSERT_DEBUG(alphaBitmap->SizeInPixels() == bitmap->SizeInPixels(),
                       User::Invariant());
        TMIDBitmapParameters params;
        params.iOffset = 0;
        params.iX = 0;
        params.iY = 0;
        params.iWidth = iScreenSize.iWidth;
        params.iHeight = iScreenSize.iHeight;
        params.iManipulation = 0;
        params.iClipRect = TRect(iScreenSize);

        TDisplayMode displayMode = bitmap->DisplayMode();

        params.iScanLength = CalculateScanLength(iScreenSize.iWidth,
                             displayMode);

        bitmap->LockHeap(ETrue);
        params.iPixels = bitmap->DataAddress();
        bitmap->UnlockHeap(ETrue);

        params.iPixelsSize = (params.iScanLength *
                              iScreenSize.iHeight *
                              bitmap->Header().iBitsPerPixel) >> KDivByEight;
        if (alphaBitmap)
        {
            alphaBitmap->LockHeap(ETrue);
            params.iAlpha = alphaBitmap->DataAddress();
            alphaBitmap->UnlockHeap(ETrue);
            params.iAlphaMode = alphaBitmap->DisplayMode();
            params.iAlphaSize =
                CalculateScanLength(iScreenSize.iWidth,
                                    params.iAlphaMode) *
                iScreenSize.iHeight;
        }

        iNativePixelFormat = CMIDConvertFactory::GetPixelFormat(displayMode);
        params.iFormat = iNativePixelFormat;

        CMIDNativeConverter* nativeConverter =
            CMIDNativeConverter::NewL(params);
        delete iNativeConverter;
        iNativeConverter = nativeConverter;
        iNativeConverter->SetDrawRect(params.iClipRect);
        do
        {
            do
            {
                iNativeConverter->PlotPixel(internalColor);
            }
            while (iNativeConverter->AdvanceX());
        }
        while (iNativeConverter->AdvanceY());

        // 256 level alpha bitmap is the only one we can currently handle
        if (params.iAlphaMode == EGray256)
        {
            for (TInt i = 0; i < params.iAlphaSize; i++)
            {
                ((TUint8*)params.iAlpha)[ i ] = internalColor.iA;      // CSI: 2 Index already checked #
            }
        }
        bitmapImage->RemoveRef();
    }
}

// Looping through pixels, iNativeConverter takes care that looping is ended
// notice that bitmapConverter's Advance methods returns always ETrue, but
// they are located in while because advancing must not be done after native
// converter is gone through. So do not mess with order.
#define DRAWLOOP( get, plot ) \
    do \
        { \
        do \
            { \
            get; \
            plot; \
            } \
        while( iNativeConverter->AdvanceX() && bitmapConverter->AdvanceX() ); \
        } \
    while( iNativeConverter->AdvanceY() && bitmapConverter->AdvanceY() );


void CMIDDirectGraphics::DrawBitmapL(TMIDBitmapParameters* aParams)
{
    // Get converter for specified format (ownership will remain in factory)
    TMIDFormatConverter* bitmapConverter =
        iConverterFactory->ConverterL(aParams->iFormat);

    aParams->iClipRect = iClipRect;

    bitmapConverter->InitializeL(*aParams);

    if (bitmapConverter->iBitmapRect.IsEmpty())
    {
        //nothing to draw, probably outside cliprect
        return;
    }
    iNativeConverter->SetDrawRect(bitmapConverter->iBitmapRect);


    // bitmaps data address can change so have to re-install them every time
    iNativeConverter->SetBitmaps(iGraphics->Bitmap()->DataAddress());

    // choosing right operation for optimized drawing

    TMIDInternalARGB color;

    // are we ignoring all alpha or there is no transparency
    if (!(aParams->iTransparency) &&
            !(iNativeConverter->HasAlphaBitmap()))
    {
        do
        {
            do
            {
                bitmapConverter->GetPixel(color);
                color.iA = KAlphaFullOpaque;
                iNativeConverter->PlotPixel(color);
            }
            while (iNativeConverter->AdvanceX() && bitmapConverter->AdvanceX());
        }
        while (iNativeConverter->AdvanceY() && bitmapConverter->AdvanceY());
        return;
    }


    // canvases and images no longer have a separate transparency bitmap
    if (aParams->iAlpha)
    {
        if (iNativeConverter->HasAlphaBitmap())
        {
            DRAWLOOP(bitmapConverter->GetPixelWithAlpha(color),
                     iNativeConverter->PlotPixelWithAlphaBitmap(color));
        }
        else
        {
            DRAWLOOP(bitmapConverter->GetPixelWithAlpha(color),
                     iNativeConverter->PlotPixelWithAlpha(color));
        }
    }
    else if (!aParams->iAlpha &&
             bitmapConverter->IsAlpha())
    {
        DRAWLOOP(bitmapConverter->GetPixel(color),
                 iNativeConverter->PlotPixelWithAlpha(color));
    }
    else if (!(bitmapConverter->IsAlpha()) &&
             (iNativeConverter->HasAlphaBitmap()))
    {
        do
        {
            do
            {
                bitmapConverter->GetPixel(color);
                color.iA = KAlphaFullOpaque;
                iNativeConverter->PlotPixelWithAlphaBitmap(color);
            }
            while (iNativeConverter->AdvanceX() && bitmapConverter->AdvanceX());
        }
        while (iNativeConverter->AdvanceY() && bitmapConverter->AdvanceY());
    }

#ifdef RD_JAVA_NGA_ENABLED
    if (iCanvasTarget && iCanvasTarget->IsEglAvailable())
    {
        iCanvasTarget->UpdateRect(bitmapConverter->iBitmapRect);
    }
#endif
}


void CMIDDirectGraphics::GetBitmapL(TMIDBitmapParameters* aParams)
{

    if ((aParams->iX < 0) ||
            (aParams->iY < 0) ||
            (aParams->iWidth < 0) ||
            (aParams->iHeight < 0))
    {
        // throws java.lang.IllegalArgumentException if x, y, width or height is
        // negative
        User::Leave(KErrIllegalArgumentException);
    }
    // Get converter for specified format (ownership will remain in factory)
    TMIDFormatConverter* bitmapConverter =
        iConverterFactory->ConverterL(aParams->iFormat);

    // Whole bitmap as clipping rect
    aParams->iClipRect = TRect(iScreenSize);
    bitmapConverter->InitializeL(*aParams);


    // Set drawing rectangle to the native converter.
    // Use intersection with the screen size in order to handle correctly
    // areas that are outside of the screen.
    TRect drawRect(aParams->iX, aParams->iY,
                   aParams->iX + aParams->iWidth,
                   aParams->iY + aParams->iHeight);

    drawRect.Intersection(iScreenSize);

    // Do nothing if the asked area is not on the screen.
    if (!drawRect.IsEmpty())
    {
        iNativeConverter->SetDrawRect(drawRect);

#ifdef RD_JAVA_NGA_ENABLED
        if (iCanvasTarget && iCanvasTarget->IsEglAvailable())
        {
            iCanvasTarget->UpdateOffScreenBitmapL(ETrue);
        }
#endif
        // bitmaps data address can change so have to re-install them every time
        iNativeConverter->SetBitmaps(iGraphics->Bitmap()->DataAddress());

        TMIDInternalARGB color;
        DRAWLOOP(iNativeConverter->GetPixel(color),
                 bitmapConverter->PlotPixelWithAlpha(color));
    }
}

void CMIDDirectGraphics::DrawPolygonL(TPolygonParameters* aParams)
{
    // We don't have to test wheter xPoints or yPoints are null, as they
    // are already checked on the java side
    if (aParams->aNumberOfPoints < 0 ||
            aParams->aXPointsLength < (aParams->aXOffset + aParams->aNumberOfPoints) ||
            aParams->aYPointsLength < (aParams->aYOffset + aParams->aNumberOfPoints) ||
            aParams->aXOffset < 0 ||
            aParams->aYOffset < 0)
    {
        //ArrayIndexOutOfBoundsException - if requested to access
        //xPoints or yPoints beyond the
        //length of the arrays or with negative index
        User::Leave(KErrArrayIndexOutOfBoundsException);
    }


    // bitmaps can change so have to re-install them every time
    iNativeConverter->SetBitmaps(iGraphics->Bitmap()->DataAddress());

    TMIDInternalARGB colorNative;
    iNativeConverter->Convert(colorNative, aParams->aARGBColor);

    TInt* xPoints = aParams->aXPoints;
    TInt* yPoints = aParams->aYPoints;
    TInt numberOfPoints(aParams->aNumberOfPoints);

    xPoints += aParams->aXOffset; // move to first x point
    yPoints += aParams->aYOffset; // move to first y point

#ifdef RD_JAVA_NGA_ENABLED
    TRect updateRect;
#endif

    TInt i(0);
    for (; i < numberOfPoints; i++)
    {
        xPoints[ i ] += aParams->aTransX;   // CSI: 2 Index already checked #
        yPoints[ i ] += aParams->aTransY;   // CSI: 2 Index already checked #

#ifdef RD_JAVA_NGA_ENABLED
        if (i == 0)
        {
            updateRect.iTl = TPoint(xPoints[i], yPoints[i]);
            updateRect.iBr = updateRect.iTl;
        }
        else
        {
            updateRect.iTl.iX = Min(xPoints[i], updateRect.iTl.iX);
            updateRect.iTl.iY = Min(yPoints[i], updateRect.iTl.iY);
            updateRect.iBr.iX = Max(xPoints[i], updateRect.iBr.iX);
            updateRect.iBr.iY = Max(yPoints[i], updateRect.iBr.iY);
        }
#endif
    }

    for (i = 0; i < numberOfPoints - 1; i++)
    {
        DrawLine(TPoint(xPoints[ i ], yPoints[ i ]),     // CSI: 2 Index already checked #
                 TPoint(xPoints[ i + 1 ], yPoints[ i + 1 ]),    // CSI: 2 Index already checked #
                 colorNative);
    }
    // draw line from last point to first point
    DrawLine(TPoint(xPoints[ i ], yPoints[ i ]),     // CSI: 2 Index already checked #
             TPoint(xPoints[ 0 ], yPoints[ 0 ]),    // CSI: 2 Index already checked #
             colorNative);

    if (aParams->aFill)
    {

        TPoint* points = new(ELeave)TPoint[ numberOfPoints ];

        // CPolygonFiller chokes if coordinates are exactly 2147483647 (the integer
        // maximum value), so invalid coordinates are made smaller
        // api/com_nokia_mid/ui/DirectGraphics/index.html#FillPolygon wont pass
        // if this is not done.
        for (TInt i = 0; i < numberOfPoints; i++)
        {
            if ((xPoints[ i ] == KMaxInteger) ||    // CSI: 2 Index already checked #
                    (yPoints[ i ] == KMaxInteger))     // CSI: 2 Index already checked #
            {
                points[ i ].SetXY(xPoints[ i ] - 1, yPoints[ i ] - 1);    // CSI: 2 Index already checked #
            }
            else
            {
                points[ i ].SetXY(xPoints[ i ], yPoints[ i ]);    // CSI: 2 Index already checked #
            }
        }
        iPolygonFiller->Reset();
        iPolygonFiller->Construct(points, numberOfPoints,
                                  CGraphicsContext::EAlternate);
        TBool draw(ETrue);
        TPoint pos;
        TInt end;
        while (draw)
        {
            iPolygonFiller->GetNextPixelRun(draw, pos.iY, pos.iX, end);
            iNativeConverter->DrawScanLine(pos, end - pos.iX + 1, colorNative, iClipRect);
        }
        delete[] points;
    }

#ifdef RD_JAVA_NGA_ENABLED
    if (iCanvasTarget && iCanvasTarget->IsEglAvailable())
    {
        // Grow by one pixel to include bottom right end point
        ++updateRect.iBr.iX;
        ++updateRect.iBr.iY;
        updateRect.Intersection(iClipRect);
        iCanvasTarget->UpdateRect(updateRect);
    }
#endif
}

void CMIDDirectGraphics::DrawLine(const TPoint& aPointA,
                                  const TPoint& aPointB,
                                  const TMIDInternalARGB& aColor)
{
    TLinearDDA linear;
    // TLinearDDA results a bit different line depending on starting point.
    // We do not have any problems with CPolygonFiller if we draw like this.
    if (aPointA.iY < aPointB.iY)
    {
        linear.Construct(aPointA, aPointB, TLinearDDA::ECenter);
    }
    else
    {
        linear.Construct(aPointB, aPointA, TLinearDDA::ECenter);
    }

    TPoint pos(0, 0);
    if (iDottedLine)
    {
        TInt dot(0);
        while (!linear.SingleStep(pos))
        {
            if (dot == KDottedLineStep ||
                    pos == aPointA ||
                    pos == aPointB)
            {
                iNativeConverter->PlotPixel(pos, aColor, iClipRect);
                dot = 0;
            }
            dot++;
        }
    }
    else
    {
        while (!linear.SingleStep(pos))
        {
            iNativeConverter->PlotPixel(pos, aColor, iClipRect);
        }
        // plot last pixel
        iNativeConverter->PlotPixel(pos, aColor, iClipRect);
    }
}

TInt CMIDDirectGraphics::CalculateScanLength(
    const TInt aWidth,
    const TDisplayMode aDisplayMode)
{
    TInt retVal = 0;

    switch (aDisplayMode)
    {
    case EGray2:
    {
        retVal = ((aWidth + KGray2Divider - 1) / KGray2Divider) *
                 KGray2Divider;
        break;
    }
    case EGray4:
    {
        retVal = ((aWidth + KGray4Divider - 1) / KGray4Divider) *
                 KGray4Divider;
        break;
    }
    case EGray16:
    case EColor16:
    {
        retVal = ((aWidth + KColor16Divider - 1) / KColor16Divider) *
                 KColor16Divider;
        break;
    }
    case EGray256:
    case EColor256:
    {
        retVal = ((aWidth + KColor256Divider - 1) / KColor256Divider) *
                 KColor256Divider;
        break;
    }
    case EColor4K:
    case EColor64K:
    {
        retVal = ((aWidth + KColor64KDivider - 1) / KColor64KDivider) *
                 KColor64KDivider;
        break;
    }
    case EColor16M:
    {
        User::Invariant();
        break;
    }
    case EColor16MU:
    case ERgb:
    case EColor16MA:
    {
        retVal = aWidth;
        break;
    }
    default:
    {
        User::Invariant();
    }
    }

    return retVal;
}

#ifdef RD_JAVA_NGA_ENABLED
#define UPDATE_TRIANGLE_RECT \
if (iCanvasTarget && iCanvasTarget->IsEglAvailable()) \
{ \
    TRect updateRect(TPoint(Min(Min(aX1,aX2),aX3), Min(Min(aY1,aY2),aY3)), \
                     TPoint(Max(Max(aX1,aX2),aX3), Max(Max(aY1,aY2),aY3))); \
    ++updateRect.iBr.iX; \
    ++updateRect.iBr.iY; \
    updateRect.Intersection(iClipRect); \
    iCanvasTarget->UpdateRect(updateRect); \
}
#endif // RD_JAVA_NGA_ENABLED

void CMIDDirectGraphics::DrawTriangle(const TInt aX1, const TInt aY1,
                                      const TInt aX2, const TInt aY2,
                                      const TInt aX3, const TInt aY3,
                                      const TUint32 aColor)
{
    // bitmaps can change so have to re-install them every time
    iNativeConverter->SetBitmaps(iGraphics->Bitmap()->DataAddress());

    TMIDInternalARGB colorNative;
    iNativeConverter->Convert(colorNative, aColor);

    DrawLine(TPoint(aX1, aY1), TPoint(aX2, aY2), colorNative);
    DrawLine(TPoint(aX2, aY2), TPoint(aX3, aY3), colorNative);
    DrawLine(TPoint(aX3, aY3), TPoint(aX1, aY1), colorNative);

#ifdef RD_JAVA_NGA_ENABLED
    UPDATE_TRIANGLE_RECT;
#endif
}

void CMIDDirectGraphics::FillTriangle(const TInt aX1, const TInt aY1,
                                      const TInt aX2, const TInt aY2,
                                      const TInt aX3, const TInt aY3,
                                      const TUint32 aColor)
{
    iNativeConverter->SetBitmaps(iGraphics->Bitmap()->DataAddress());

    TMIDInternalARGB colorNative;
    iNativeConverter->Convert(colorNative, aColor);

    TMIDTriangleFiller triangleFiller;
    triangleFiller.Construct(aX1, aY1,
                             aX2, aY2,
                             aX3, aY3);
    TBool draw(ETrue);
    TPoint pos;
    TInt end;

    while (draw)
    {
        triangleFiller.GetNextPixelRun(draw, pos.iY,
                                       pos.iX, end);

        iNativeConverter->DrawScanLine(pos, end - pos.iX + 1, colorNative, iClipRect);
    }

#ifdef RD_JAVA_NGA_ENABLED
    UPDATE_TRIANGLE_RECT;
#endif
}



















