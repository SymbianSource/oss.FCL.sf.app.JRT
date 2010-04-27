/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/



#include "swtmaskhandler.h"

CSwtMaskHandler::CSwtMaskHandler(TDisplayMode aDisplayMode)
        : iDisplayMode(aDisplayMode)
{
}

CSwtMaskHandler::~CSwtMaskHandler()
{
    iMasks.ResetAndDestroy();
}

CFbsBitmap* CSwtMaskHandler::GetMask(TSize aSize)
{
    // Search for suitable mask, if exists then use it
    // otherwise crate new with correct size
    CFbsBitmap* mask = NULL;
    TInt count = iMasks.Count();
    for (TInt i = 0; i < count; i++)
    {
        mask = iMasks[i];
        if (mask->SizeInPixels() == aSize)
        {
            return mask;
        }
    }
    TRAPD(err, mask = NewMaskL(aSize));
    if (err != KErrNone)
    {
        mask = NULL;
    }
    return mask;
}

CFbsBitmap* CSwtMaskHandler::NewMaskL(TSize aSize)
{
    // Creating new mask according to aSize
    // Disposing mask is done in destructor
    CFbsBitmap* newMask = new(ELeave) CFbsBitmap;
    User::LeaveIfError(newMask->Create(aSize, EGray2));

    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL(newMask);
    CleanupStack::PushL(device);
    CFbsBitGc* gc = CFbsBitGc::NewL();
    CleanupStack::PushL(gc);

    gc->Activate(device);
    gc->SetPenStyle(CGraphicsContext::ENullPen);
    gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc->SetBrushColor(KRgbBlack);
    gc->DrawRect(TRect(aSize));

    CleanupStack::PopAndDestroy(gc);
    CleanupStack::PopAndDestroy(device);

    iMasks.AppendL(newMask);
    return newMask;
}
