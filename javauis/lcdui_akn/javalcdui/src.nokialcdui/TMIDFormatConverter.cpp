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
* Description:  Abstract base class for format converters.
*
*/



//  INCLUDE FILES
#include "TMIDFormatConverter.h"
#include "TMIDGray1.h"
#include <fbs.h>

namespace
{
const TInt KAnchorHCenter = 1;
const TInt KAnchorVCenter = 2;
const TInt KAnchorRight = 8;
const TInt KAnchorBottom = 32;
const TInt KScanLengthOffset = 31;
const TInt KScanLengthDivider = 32;
}

TMIDFormatConverter::TMIDFormatConverter()
{
    iTransformer = TMIDTransformer();
    iBitmapRect = TRect();
    iOffset = 0;
    iScanlength = 0;
    iAlphaBitmap = NULL;
    iAlphaMode = ENone;
    iTransparency = ETrue;
}

void TMIDFormatConverter::InitializeL(const TMIDBitmapParameters& aParameters)
{
    iOffset = aParameters.iOffset;
    iScanlength = aParameters.iScanLength;
    iAlphaBitmap = aParameters.iAlpha;
    iAlphaMode = aParameters.iAlphaMode;
    iTransparency = aParameters.iTransparency;

    switch (iAlphaMode)
    {
    case EGray2:
    {
        iAlphaScanLength = ((aParameters.iWidth + KScanLengthOffset) /
                            KScanLengthDivider);

        break;
    }
    default:
    {
        if (iAlphaBitmap)
        {
            iAlphaScanLength = aParameters.iBitmap->ScanLineLength(
                                   aParameters.iWidth, iAlphaMode);
        }
    }
    }

    TInt lastPixel = iScanlength * (aParameters.iHeight - 1) +
                     aParameters.iWidth - 1 + iOffset;

    // checking that bitmap is big enough
    if (iOffset < 0 ||
            !CheckSize(aParameters.iPixelsSize, lastPixel))
    {
        // out of bounds
        User::Leave(KErrArrayIndexOutOfBoundsException);
    }

    // checking alpha size
    if (iAlphaBitmap)
    {
        if (((iAlphaMode == EGray256) &&
                (aParameters.iAlphaSize <
                 (aParameters.iHeight * aParameters.iWidth + iOffset))) ||
                ((iAlphaMode == EGray2) &&
                 (aParameters.iAlphaSize <
                  (aParameters.iHeight * (aParameters.iWidth >> KMIDShift3Bits)
                   + iOffset))) ||
                ((iAlphaMode != EGray256) && (iAlphaMode != EGray2) &&
                 (!CheckSize(aParameters.iAlphaSize, lastPixel))))
        {
            // out of bounds
            User::Leave(KErrArrayIndexOutOfBoundsException);
        }
    }


    iBitmapRect = TRect(aParameters.iX, aParameters.iY,
                        aParameters.iX + aParameters.iWidth,
                        aParameters.iY + aParameters.iHeight);

    // initializing transformer, this will only rotate
    iTransformer.Transform(iBitmapRect, aParameters.iManipulation);

    // calculating new position from anchor point
    if (aParameters.iAnchor != 0)
    {
        if (aParameters.iAnchor & KAnchorRight)
        {
            iBitmapRect.Move(-iBitmapRect.Width(), 0);
        }
        if (aParameters.iAnchor & KAnchorBottom)
        {
            iBitmapRect.Move(0, -iBitmapRect.Height());
        }
        if (aParameters.iAnchor & KAnchorHCenter)
        {
            iBitmapRect.Move(-(iBitmapRect.Width() >> 1), 0);
        }
        if (aParameters.iAnchor & KAnchorVCenter)
        {
            iBitmapRect.Move(0, -(iBitmapRect.Height() >> 1));
        }
    }


    // second phase of initialization, this will do actual initalization of directions
    // for both flip and rotate. Movement value is for changing starting position if out
    // of clipping rect.
    TPoint move(0, 0);
    if (iBitmapRect.iTl.iX < aParameters.iClipRect.iTl.iX)
    {
        move.iX = aParameters.iClipRect.iTl.iX - iBitmapRect.iTl.iX;
    }
    if (iBitmapRect.iTl.iY < aParameters.iClipRect.iTl.iY)
    {
        move.iY = aParameters.iClipRect.iTl.iY - iBitmapRect.iTl.iY;
    }

    // This will rotate and flip bitmap, returns error code if manipulation is
    // invalid
    User::LeaveIfError(iTransformer.Transform(move));

    // calculating possible new size after clipping
    iBitmapRect.Intersection(aParameters.iClipRect);
    if (iBitmapRect.IsEmpty())
    {
        // we are not in clipping rect, cancelling the drawing
        return;
    }

    // iTransformers moves are two dimensional moves with following combinations
    // x-move y-move
    // (1, 0) (0, 1) Default (no rotations or flips)
    // (1, 0) (0,-1)
    // (-1,0) (0, 1)
    // (-1,0) (0,-1)
    // (0, 1) (1, 0)
    // (0, 1) (-1,0)
    // (0,-1) (1, 0)
    // (0,-1) (-1,0)

    // So precalculating moves

    // going right or left when advancing y
    // x(y) y(x)
    //(1)   (1)  -scan * (height - 1) + 1
    //(1)   (-1) -scan * (height - 1) - 1
    //(-1)  (1)   scan * (height - 1) + 1
    //(-1)  (-1)  scan * (height - 1) - 1
    if (iTransformer.iYMove.iX != 0)
    {
        iTransformer.iYMove.iX += (-(iTransformer.iXMove.iY) * iScanlength) *
                                  (iBitmapRect.Width() - 1);
    }

    // going up or down when advancing x
    iTransformer.iXMove.iY *= iScanlength;

    // going up or down (default) when advancing y
    // x    y
    //(1)   (1)  scan - width + 1
    //(-1)  (1)  scan + width - 1
    //(1)   (-1) -scan - width + 1
    //(-1)  (-1) -scan + width - 1
    iTransformer.iYMove.iY = iTransformer.iYMove.iY * iScanlength -
                             (iTransformer.iXMove.iX * iBitmapRect.Width()) + iTransformer.iXMove.iX;

    // combine x & y to one value
    iTransformer.iXMove.iX += iTransformer.iXMove.iY;
    iTransformer.iYMove.iX += iTransformer.iYMove.iY;
}


void TMIDFormatConverter::GetPixelWithAlpha(TMIDInternalARGB& aResult) const
{
    GetPixel(aResult);
    switch (iAlphaMode)
    {
    case(EGray256):
    {
        TInt line = iOffset / iScanlength;
        TInt column = iOffset % iScanlength;
        aResult.iA =
            ((TUint8*)iAlphaBitmap)[ line * iAlphaScanLength + column ];  // CSI: 2 Wrong index means implementation error #
        break;
    }
    case(EGray2):
    {
        TInt line = iOffset / iScanlength;
        TInt column = iOffset % iScanlength;
        TInt word = line * iAlphaScanLength + (column >> KMIDShift5Bits);
        aResult.iA = (((((TUint32*)iAlphaBitmap)[ word ] >>    // CSI: 2 Wrong index means implementation error #
                        (column % KScanLengthDivider)) & 1) * KAlphaFullOpaque);

        break;
    }
    default:
    {
        aResult.iA = GetAlpha();
    }
    }
}

void TMIDFormatConverter::PlotPixelWithAlpha(const TMIDInternalARGB& aInternal)
{
    PlotPixel(aInternal);

    // modifying alpha channel
    if (iAlphaBitmap)
    {
        switch (iAlphaMode)
        {
        case(EGray256):
        {
            TInt line = iOffset / iScanlength;
            TInt column = iOffset % iScanlength;
            ((TUint8*)iAlphaBitmap)[ line * iAlphaScanLength + column ] =  // CSI: 2 Wrong index means implementation error #
                aInternal.iA;
            break;
        }
        case(EGray2):
        {
            TInt line = iOffset / iScanlength;
            TInt column = iOffset % iScanlength;
            TInt word = line * iAlphaScanLength + (column >> KMIDShift5Bits);
            if ((aInternal.iR + aInternal.iG + aInternal.iB) <
                    KWhiteBlackSumRGBMidValue)
            {
                iAlphaBitmap[ word ] = (TUint32)(iAlphaBitmap[ word ] |      // CSI: 2 Wrong offset means implementation error #
                                                 KBits32[ column % KScanLengthDivider ]);   // CSI: 2 Wrong index means implementation error #
            }
            else
            {
                iAlphaBitmap[ word ] = (TUint32)(iAlphaBitmap[ word ] &     // CSI: 2 Wrong offset means implementation error #
                                                 ~KBits32[ column % KScanLengthDivider ]);   // CSI: 2 Wrong index means implementation error #
            }
        }
        default:
        {
            // otherwise we have same format as normal bitmap
            // getting real bitmap from converter
            TUint32* tempBitmap = Bitmap();
            SetBitmap(iAlphaBitmap);

            // It must be either white or black
            TMIDInternalARGB alphaIn;
            if (aInternal.iA == KAlphaFullOpaque)
            {
                alphaIn = KOpaque;
            }
            PlotPixel(alphaIn);

            // reverting to original bitmap
            SetBitmap(tempBitmap);
        }
        }
    }
}

TBool TMIDFormatConverter::IsAlpha()
{

    if (iAlphaBitmap)
    {
        return ETrue;
    }
    return EFalse;
}
