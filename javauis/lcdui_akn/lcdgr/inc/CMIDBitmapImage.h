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

#ifndef CMIDBITMAPIMAGE_H
#define CMIDBITMAPIMAGE_H

#include <e32base.h>
#include <lcdgr.h>

class CLcdFbsImage;
class CFbsBitmap;
class CBitmapContext;
class TPoint;
class TRect;

/**
 * Bridge to MMIDBitmapImage.
 *
 */
NONSHARABLE_CLASS(CMIDBitmapImage) : public CBase, public MMIDBitmapImage
{
public:
    CMIDBitmapImage(CLcdFbsImage& aImageRep);
    ~CMIDBitmapImage();

public:
    /* MMIDBitmapImage interface */
    void AddRef();
    void RemoveRef();
    CFbsBitmap* ColorBitmap() const;
    CFbsBitmap* AlphaBitmap() const;
    CFbsBitmap* CreateColorBitmapL(TDisplayMode aMode) const;
    CFbsBitmap* CreateAlphaBitmapL(TDisplayMode aMode, TBool aInvert) const;
    CFbsBitmap* DuplicateColorBitmapL() const;
    CFbsBitmap* DuplicateAlphaBitmapL() const;
    void Draw(CBitmapContext& aContext, const TPoint& aPoint) const;
    void Draw(CBitmapContext& aContext, const TPoint& aPoint, const TRect& aRect) const;

private:
    CLcdFbsImage& iImageRep;
    TInt iRefCount;
};

#endif // CMIDBITMAPIMAGE_H
