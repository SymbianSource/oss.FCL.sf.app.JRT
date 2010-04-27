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


#include "LcdFbsImageCache.h"
#include "CMIDImage.h"
#include "CMIDBitmapImage.h"
#include "LcdImage.h"

/**
 * Copy bitmap data from (aColorBitmap,aAlphaBitmap) to aFbsImage.
 */
extern void CopyBitmapsL
(
    CLcdGraphicsDriver& aDriver,
    CLcdFbsImage&       aFbsImage,
    CFbsBitmap*         aColorBitmap,
    CFbsBitmap*         aAlphaBitmap,
    TBool               aInvertMask
);

TInt CompareImageAddress(const TLcdFbsImageEntry& aLhs, const TLcdFbsImageEntry& aRhs)
{
    return (aLhs.iImage - aRhs.iImage);
}

TBool MatchEntry(const TLcdFbsImageEntry& aLhs, const TLcdFbsImageEntry& aRhs)
{
    return aLhs.iImage       == aRhs.iImage     &&
           aLhs.iColorMode  == aRhs.iColorMode &&
           aLhs.iAlphaMode  == aRhs.iAlphaMode &&
           aLhs.iInvertMask == aRhs.iInvertMask;
}

CLcdFbsImageCache* CLcdFbsImageCache::NewL(CLcdGraphicsDriver& aDriver, TDisplayMode aDefaultColorMode, TDisplayMode aDefaultAlphaMode, TBool aDefaultInverted)
{
    CLcdFbsImageCache* cache = new(ELeave) CLcdFbsImageCache(aDriver);
    cache->iDefaultColorMode = aDefaultColorMode;
    cache->iDefaultAlphaMode = aDefaultAlphaMode;
    cache->iDefaultInverted = aDefaultInverted;
    return cache;
}

CLcdFbsImageCache::CLcdFbsImageCache(CLcdGraphicsDriver& aDriver)
        : iDriver(aDriver)
        , iEntries(8, _FOFF(TLcdFbsImageEntry, iImage))
        , iOrder(CompareImageAddress)
{
}

CLcdFbsImageCache::~CLcdFbsImageCache()
{
    for (TInt i = iEntries.Count(); --i>=0;)
    {
        RemoveEntry(i);
    }
    iEntries.Reset();
    iEntries.Close();
}

TDisplayMode ValidateMode(TInt aMode, TDisplayMode aDefault)
{
    if (aMode == MMIDBitmapImage::EDefaultDisplayMode)
    {
        return aDefault;
    }
    return (TDisplayMode)aMode;
}


/**
 * Gets the primary bitmap representation of this image.
 */
MMIDBitmapImage* CLcdFbsImageCache::GetBitmapImage(MMIDImage* aImage)
{
    TLcdFbsImageEntry example;
    example.iImage      = aImage;
    example.iPrimary    = ETrue;    // not used in Find.
    example.iColorMode  = ENone;
    example.iAlphaMode  = ENone;
    example.iInvertMask = EFalse;
    example.iBitmapRep  = NULL;

    //
    // Get a start point for the search. Assuming that FindInOrder
    // does a binary search of some sort, then it will probably
    // end up in the middle of the range of entries corresponding
    // to aImage. Note that iEntries is sorted by aImage address.
    //
    TInt start = iEntries.FindInOrder(example, iOrder);
    TInt count = iEntries.Count();
    TInt index;

    ASSERT(start >= 0);

    //
    // Search backwards from start point
    //
    for (index=start; index>=0; --index)
    {
        TLcdFbsImageEntry& entry = iEntries[index];
        if (entry.iImage != aImage)
        {
            break;
        }
        if (entry.iPrimary)
        {
            entry.iBitmapRep->AddRef();
            return entry.iBitmapRep;
        }
    }

    //
    // We didn't find it searching backwards - try forwards
    //
    for (index=start+1; index<count; index++)
    {
        TLcdFbsImageEntry& entry = iEntries[index];
        if (entry.iImage != aImage)
        {
            break;
        }
        if (entry.iPrimary)
        {
            entry.iBitmapRep->AddRef();
            return entry.iBitmapRep;
        }
    }

    return NULL;
}

/**
 * Find A bitmap image representation of aImage with color bitmap in displaymode aColorMode, and alpha
 * bitmap (if present) in displaymode aAlphaMode. If aColorMode or aAlphaMode are EDefaultDisplayMode,
 * then this method will select a mode based on the default image pixel format.
 *
 *@return a bitmap image or NULL if no appropriate bitmap representation of aImage exists.
 */
MMIDBitmapImage* CLcdFbsImageCache::GetBitmapImage(MMIDImage* aImage, TInt aColorMode, TInt aAlphaMode, TBool aInvertMask)
{
    TDisplayMode defaultAlphaMode = (aImage->TransparencyType() == MMIDImage::ENone ? ENone : iDefaultAlphaMode);

    TIdentityRelation<TLcdFbsImageEntry> relation(MatchEntry);
    TLcdFbsImageEntry entry;

    entry.iImage      = aImage;
    entry.iColorMode  = ValidateMode(aColorMode, iDefaultColorMode);;
    entry.iAlphaMode  = ValidateMode(aAlphaMode, defaultAlphaMode);
    entry.iInvertMask = aInvertMask;
    entry.iPrimary    = EFalse;
    entry.iBitmapRep  = NULL;

    // linear search starting at
    TInt index = iEntries.Find(entry, relation);
    if (index >= 0)
    {
        MMIDBitmapImage* bitmapRep = iEntries[index].iBitmapRep;
        bitmapRep->AddRef();
        return bitmapRep;
    }

    return NULL;
}


/**
 * Find or create a bitmap image representation of aImage with a color bitmap in displaymode aColorMode and
 * alpha bitmap (if present) in displaymode  aAlphaMode.
 *
 *@return a bitmap image representation.
 *
 * Will leave with one of the system error codes if a bitmap representation does not exist and one cannot
 * be created.
 */
MMIDBitmapImage* CLcdFbsImageCache::GetBitmapImageL(MMIDImage* aImage, TInt aColorMode, TInt aAlphaMode, TBool aInvertMask)
{
    TDisplayMode colorMode = ValidateMode(aColorMode, iDefaultColorMode);

    TDisplayMode defaultAlphaMode = (aImage->TransparencyType() == MMIDImage::ENone ? ENone : iDefaultAlphaMode);

    TDisplayMode alphaMode = ValidateMode(aAlphaMode, defaultAlphaMode);

    MMIDBitmapImage* image = GetBitmapImage(aImage, colorMode, alphaMode, aInvertMask);
    if (NULL == image)
    {
        image = CreateBitmapImageRepLC(*aImage, colorMode, alphaMode, aInvertMask);
        TLcdFbsImageEntry entry;
        entry.iImage      = aImage;
        entry.iPrimary    = EFalse;
        entry.iColorMode  = colorMode;
        entry.iAlphaMode  = alphaMode;
        entry.iInvertMask = aInvertMask;
        entry.iBitmapRep  = image;
        iEntries.InsertInOrderAllowRepeatsL(entry, iOrder);
        image->AddRef();        // 1 for the cache
        CleanupStack::Pop();    // image
        image->AddRef();        // 1 for the caller
    }

    ASSERT(image);
    return image;
}

TDisplayMode BitmapMode(CFbsBitmap* aBitmapOrNull)
{
    TDisplayMode mode = ENone;
    if (aBitmapOrNull)
    {
        mode = aBitmapOrNull->DisplayMode();
    }
    return mode;
}

void CLcdFbsImageCache::RegisterL(MMIDImage* aImage, TBool aInverted, MMIDBitmapImage* aBitmapImageRep)
{
    TLcdFbsImageEntry entry;
    entry.iImage      = aImage;
    entry.iPrimary    = ETrue;
    entry.iColorMode  = BitmapMode(aBitmapImageRep->ColorBitmap());
    entry.iAlphaMode  = BitmapMode(aBitmapImageRep->AlphaBitmap());
    entry.iInvertMask = aInverted;
    entry.iBitmapRep  = aBitmapImageRep;
    iEntries.InsertInOrderAllowRepeatsL(entry, iOrder);
    aBitmapImageRep->AddRef();
}

//
// purge all entries corresponding to aImage.
//
void CLcdFbsImageCache::Purge(MMIDImage* aImage)
{
    TLcdFbsImageEntry entry;
    entry.iImage = aImage;
    TInt index;
    while ((index = iEntries.FindInOrder(entry, iOrder)) >= 0)
    {
        RemoveEntry(index);
    }
}

void CLcdFbsImageCache::RemoveEntry(TInt aIndex)
{
    iEntries[aIndex].iBitmapRep->RemoveRef();
    iEntries.Remove(aIndex);
}

MMIDBitmapImage* CLcdFbsImageCache::CreateBitmapImageRepLC(MMIDImage& aImage, TDisplayMode aColorMode, TDisplayMode aAlphaMode, TBool aInvertMask)
{
    MMIDBitmapImage* bitmapImage = aImage.BitmapImage();
    if (!bitmapImage)
    {
        User::Leave(KErrGeneral);
    }

    CMIDImage& proxy = static_cast<CMIDImage&>(aImage);
    CLcdImage& image = proxy.Image();
    const TSize size = proxy.Size();
    ASSERT(aColorMode != ENone);

    CMIDBitmapImage* bitmapRep = NULL;

    CLcdFbsImage* fbsImage = CLcdFbsImage::NewL(size, aColorMode, aAlphaMode);
    CleanupStack::PushL(fbsImage);
    CopyBitmapsL(iDriver, *fbsImage, bitmapImage->ColorBitmap(), bitmapImage->AlphaBitmap(), aInvertMask);
    bitmapRep = new(ELeave) CMIDBitmapImage(*fbsImage);
    CleanupStack::Pop(fbsImage);
    CleanupStack::PushL(bitmapRep);

    return bitmapRep;
}
