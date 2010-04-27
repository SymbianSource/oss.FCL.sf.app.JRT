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


#include "LcdBitmapSurface.h"

CLcdBitmapSurface* CLcdBitmapSurface::NewL(const CFbsBitmap* aTarget)
{
    CLcdBitmapSurface* surface = new(ELeave) CLcdBitmapSurface;
    CleanupStack::PushL(surface);
    surface->ConstructL(*aTarget);
    CleanupStack::Pop(surface);
    return surface;
}

void CLcdBitmapSurface::ConstructL(const CFbsBitmap& aTarget)
{
    iBitmap = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iBitmap->Duplicate(aTarget.Handle()));
    iDevice = CFbsBitmapDevice::NewL(iBitmap);
    User::LeaveIfError(iDevice->CreateContext(iContext));
    iSpec = TAcceleratedBitmapSpec(iBitmap);
    iImageType.iColorMode = aTarget.DisplayMode();
    iImageType.iAlphaMode = ENone;
    iImageType.iTransparency = ETransparencyNone;
    iBounds = TRect(aTarget.SizeInPixels());
    iRegion.AddRect(iBounds);
}

CLcdBitmapSurface::~CLcdBitmapSurface()
{
    delete iContext;
    delete iDevice;
    delete iBitmap;
    iRegion.Close();
}

CFbsBitmap* CLcdBitmapSurface::Bitmap() const
{
    return iBitmap;
}

CLcdSurface::TSurfaceType CLcdBitmapSurface::Type() const
{
    return CLcdSurface::EBitmapSurface;
}

void CLcdBitmapSurface::Update(const TRect& /*aRect*/)
{
    // NOP. but could copy up from pbuffer here
}

void CLcdBitmapSurface::Begin(TAcceleratedBitmapInfo& aInfo, TBitmapLockCount& aCount)
{
    iSpec.Lock(aCount);
    iSpec.GetInfo(aInfo);
}

void CLcdBitmapSurface::End(TBitmapLockCount& aCount)
{
    iSpec.Unlock(aCount);
}

TRect CLcdBitmapSurface::Bounds() const
{
    return iBounds;
}

RRegion* CLcdBitmapSurface::VisibleRegion() const
{
    return const_cast<RRegion*>(&iRegion);
}

CBitmapContext* CLcdBitmapSurface::Context() const
{
    return iContext;
}

TImageType CLcdBitmapSurface::ImageType() const
{
    return iImageType;
}
