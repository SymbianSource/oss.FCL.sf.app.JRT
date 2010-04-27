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


#ifndef LCDFBSIMAGECACHE_H
#define LCDFBSIMAGECACHE_H

#include <e32base.h>
#include <gdi.h>
#include <fbs.h>
#include <lcdgr.h>
#include <lcdgdrv.h>
#include "LcdFbsImage.h"

struct TLcdFbsImageEntry
{
    MMIDImage*       iImage;
    TBool            iPrimary;
    TDisplayMode     iColorMode;
    TDisplayMode     iAlphaMode;
    TBool            iInvertMask;
    MMIDBitmapImage* iBitmapRep;
};

/**
 *
 */
NONSHARABLE_CLASS(CLcdFbsImageCache) : public CBase
{
public:
    static CLcdFbsImageCache* NewL(CLcdGraphicsDriver& aDriver, TDisplayMode aDefaultColorMode, TDisplayMode aDefaultAlphaMode, TBool aDefaultInverted);
    CLcdFbsImageCache(CLcdGraphicsDriver& aDriver);
    ~CLcdFbsImageCache();


    /**
     * Find the primary bitmap image representation of aImage.
     *
     *@return a bitmap image or NULL if no appropriate bitmap representation of aImage exists.
     */
    MMIDBitmapImage* GetBitmapImage(MMIDImage* aImage);

    /**
     * Find a bitmap image representation of aImage with color bitmap in displaymode aColorMode, and alpha
     * bitmap (if present) in displaymode aAlphaMode. If aColorMode or aAlphaMode are equal to
     * MMIDBitmapImage::EDefaultDisplayMode, then the implementation will select a mode based on the image
     * pixel format.
     *
     *@return a bitmap image or NULL if no appropriate bitmap representation of aImage exists.
     */
    MMIDBitmapImage* GetBitmapImage
    (
        MMIDImage* aImage,
        TInt aColorMode,
        TInt aAlphaMode,
        TBool aInvertMask
    );

    /**
     * Find or create a bitmap image representation of aImage with a color bitmap in displaymode aColorMode and
     * alpha bitmap (if present) in displaymode  aAlphaMode.
     *
     *@return a bitmap image representation.
     *
     * Will leave with one of the system error codes if a bitmap representation does not exist and one cannot
     * be created.
     */
    MMIDBitmapImage* GetBitmapImageL
    (
        MMIDImage* aImage,
        TInt aColorMode=MMIDBitmapImage::EDefaultDisplayMode,
        TInt aAlphaMode=MMIDBitmapImage::EDefaultDisplayMode,
        TBool aInvertMask=EFalse
    );

    void RegisterL(MMIDImage* aImage, TBool aInverted, MMIDBitmapImage* aImageRep);
    void Purge(MMIDImage* aImage);

private:
    MMIDBitmapImage* CreateBitmapImageRepLC(MMIDImage& aImage, TDisplayMode aColorMode, TDisplayMode aAlphaMode, TBool aInvertMask);
    void RemoveEntry(TInt);

private:
    CLcdGraphicsDriver& iDriver;
    TDisplayMode iDefaultColorMode;
    TDisplayMode iDefaultAlphaMode;
    TBool iDefaultInverted;
    RArray<TLcdFbsImageEntry>       iEntries;
    TLinearOrder<TLcdFbsImageEntry> iOrder;
};

#endif // LCDFBSIMAGECACHE_H
