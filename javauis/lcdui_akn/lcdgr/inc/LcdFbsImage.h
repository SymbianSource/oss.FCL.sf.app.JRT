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

#ifndef LCDFBSIMAGE_H
#define LCDFBSIMAGE_H

#include <fbs.h>
#include <lcdgr.h>
#include <lcdgdrv.h>

/**
 * Image represented by SymbianOS color bitmap + mask/alpha bitmap where
 * transparency is present.
 *
 */
NONSHARABLE_CLASS(CLcdFbsImage) : public CBase
{
public:
    /**
     * Construct a new image of size aSize and color bitmap of mode aColorMode with optional mask/alpha
     * bitmap of mode aAlphaMode. An alpha bitmap mode of ENone indicates no alpha bitmap should be
     * created.
     */
    static CLcdFbsImage* NewL(const TSize& aSize, TDisplayMode aColorMode, TDisplayMode aAlphaMode);

    /**
     * Construct a new image, duplicating the bitmap handles aColorBitmap and optional aAlphaBitmap.
     */
    static CLcdFbsImage* NewL(CFbsBitmap* aColorBitmap, CFbsBitmap* aAlphaBitmap);

    /**
     * Dup a bitmap handle. Allocates a new CFbsBitmap instance to hold the handle. Requires an FBSERV ipc.
     */
    static CFbsBitmap* DuplicateBitmapL(CFbsBitmap*);

    /**
     * Create a new bitmap in a given mode and copy the data, optionally inverted.
     */
    static CFbsBitmap* CopyBitmapL(CFbsBitmap*, TDisplayMode, TBool);

    /**
     * Copies pixel data from aSource to aTarget, optionally inverting. Uses BITGDI
     * for closest match.
     */
    static void CopyBitmapL(CFbsBitmap* aTarget, CFbsBitmap* aSource, TBool aInvert);

    ~CLcdFbsImage();

    /**
     *
     */
    TSize Size() const;

    /**
     * Increment ref count
     */
    void  AddRef();

    /**
     * Decrement ref count
     */
    void  RemoveRef();

    /**
     *@return the color bitmap
     */
    CFbsBitmap* ColorBitmap() const;

    /**
     *@return the alpha bitmap if any
     */
    CFbsBitmap* AlphaBitmap() const;

    /**
     * Create an alpha bitmap for this image.
     *
     * Temporarily required to support setTransparencyType.
     */
    void CreateAlphaBitmapL();

private:
    CFbsBitmap* iColorBitmap;
    CFbsBitmap* iAlphaBitmap;
    TInt iRefCount;
};

#endif // LCDFBSIMAGE_H
