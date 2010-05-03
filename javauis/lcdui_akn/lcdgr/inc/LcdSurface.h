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


#ifndef LCDSURFACE_H
#define LCDSURFACE_H

#include <lcdgdrv.h>    // for TImageType

class CBitmapContext;
class TAcceleratedBitmapInfo;
class TBitmapLockCount;

NONSHARABLE_CLASS(CLcdSurface) : public CBase
{
public:
    enum TSurfaceType
    {
        EBitmapSurface,
        EWindowSurface
    };

public:
    virtual TSurfaceType Type() const = 0;
    virtual void Update(const TRect& aRect) = 0;
    virtual void Begin(TAcceleratedBitmapInfo& aInfo, TBitmapLockCount& aCount) = 0;
    virtual void End(TBitmapLockCount& aCount) = 0;
    virtual TRect Bounds() const = 0;
    virtual RRegion* VisibleRegion() const = 0;
    virtual CBitmapContext* Context() const = 0;
    virtual TImageType ImageType() const = 0;
};

#endif // LCDSURFACE_H
