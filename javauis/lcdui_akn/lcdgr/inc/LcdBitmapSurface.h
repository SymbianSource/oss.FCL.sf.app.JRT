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


#ifndef LCDBITMAPSURFACE_H
#define LCDBITMAPSURFACE_H

#include "LcdSurface.h"
#include <gdi.h>
#include <fbs.h>
#include <bitdev.h>
#include <bitstd.h>

class CFbsBitmap;
class CFbsBitmapDevice;
class CFbsBitGc;

NONSHARABLE_CLASS(CLcdBitmapSurface) : public CLcdSurface
{
public:
    static CLcdBitmapSurface* NewL(const CFbsBitmap* aTarget);
    ~CLcdBitmapSurface();

    // CLcdBitmapSurface
    CFbsBitmap* Bitmap() const;

    // CLcdSurface
    virtual TSurfaceType Type() const;
    virtual void Update(const TRect& aRect);
    virtual void Begin(TAcceleratedBitmapInfo& aInfo, TBitmapLockCount& aCount);
    virtual void End(TBitmapLockCount& aCount);
    virtual TRect Bounds() const;
    virtual RRegion* VisibleRegion() const;
    virtual CBitmapContext* Context() const;
    virtual TImageType ImageType() const;

private:
    void ConstructL(const CFbsBitmap& aTarget);

private:
    TAcceleratedBitmapSpec iSpec;
    TRect               iBounds;
    TImageType          iImageType;
    CFbsBitmap*         iBitmap;
    CFbsBitmapDevice*   iDevice;
    CFbsBitGc*          iContext;
    RRegion             iRegion;
};

#endif // LCDBITMAPSURFACE_H
