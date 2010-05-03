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


#include "CMIDBitmapImage.h"
#include "LcdFbsImage.h"

CMIDBitmapImage::CMIDBitmapImage(CLcdFbsImage& aImageRep)
        : iImageRep(aImageRep)
        , iRefCount(0)
{
    iImageRep.AddRef();
}

CMIDBitmapImage::~CMIDBitmapImage()
{
    iImageRep.RemoveRef();
}

void CMIDBitmapImage::AddRef()
{
    ++iRefCount;
}

void CMIDBitmapImage::RemoveRef()
{
    if (--iRefCount == 0)
    {
        delete this;
    }
}

CFbsBitmap* CMIDBitmapImage::ColorBitmap() const
{
    return iImageRep.ColorBitmap();
}

CFbsBitmap* CMIDBitmapImage::AlphaBitmap() const
{
    return iImageRep.AlphaBitmap();
}

CFbsBitmap* CMIDBitmapImage::CreateColorBitmapL(TDisplayMode aMode) const
{
    return CLcdFbsImage::CopyBitmapL(iImageRep.ColorBitmap(), aMode, EFalse);
}

CFbsBitmap* CMIDBitmapImage::CreateAlphaBitmapL(TDisplayMode aMode, TBool aInvert) const
{
    CFbsBitmap* alphaBitmap = iImageRep.AlphaBitmap();
    if (!alphaBitmap)
    {
        return NULL;
    }
    return CLcdFbsImage::CopyBitmapL(alphaBitmap, aMode, aInvert);
}

/**
 * Create a duplicate bitmap of the color bitmap. The duplicate
 * bitmap will refer to the same pixel data.
 * Caller takes ownership.
 */
CFbsBitmap* CMIDBitmapImage::DuplicateColorBitmapL() const
{
    return CLcdFbsImage::DuplicateBitmapL(iImageRep.ColorBitmap());
}

/**
 * Create a duplicate bitmap of the alpha bitmap. The duplicate
 * bitmap will refer to the same pixel data.
 * Caller takes ownership.
 * May return NULL if no alpha bitmap is present.
 */
CFbsBitmap* CMIDBitmapImage::DuplicateAlphaBitmapL() const
{
    CFbsBitmap* alphaBitmap = iImageRep.AlphaBitmap();
    if (!alphaBitmap)
    {
        return NULL;
    }
    return CLcdFbsImage::DuplicateBitmapL(alphaBitmap);
}

void CMIDBitmapImage::Draw(CBitmapContext& aContext, const TPoint& aPoint) const
{
    TRect rect(ColorBitmap()->SizeInPixels());
    Draw(aContext, aPoint, rect);
}

void CMIDBitmapImage::Draw(CBitmapContext& aContext, const TPoint& aPoint, const TRect& aRect) const
{
    CFbsBitmap* colorBitmap = ColorBitmap();

    if (colorBitmap)
    {
        CFbsBitmap* alphaBitmap = AlphaBitmap();

        if (alphaBitmap)
        {
            aContext.BitBltMasked(aPoint, colorBitmap, aRect, alphaBitmap, EFalse);
        }
        else
        {
            aContext.BitBlt(aPoint, colorBitmap, aRect);
        }
    }
}
