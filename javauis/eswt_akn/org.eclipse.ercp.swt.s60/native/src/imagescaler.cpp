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


#include <eikenv.h>
#include <BitmapTransforms.h>
#include "imagescaler.h"


CImageScaler::CImageScaler()
        : CActive(EPriorityStandard)
{
    CActiveScheduler::Add(this);
}

CImageScaler::~CImageScaler()
{
    Cancel();
    if (iBitmapScaler)
        iBitmapScaler->Cancel();
    delete iBitmapScaler;
    delete iWait;
}

CImageScaler* CImageScaler::NewL()
{
    CImageScaler* self = new(ELeave) CImageScaler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CImageScaler::ConstructL()
{
    iBitmapScaler = CBitmapScaler::NewL();
    iBitmapScaler->SetQualityAlgorithm(CBitmapScaler::EMaximumQuality);
    iWait = new(ELeave) CActiveSchedulerWait;
}

void CImageScaler::Scale(CFbsBitmap& aSrcBitmap, CFbsBitmap& aDestBitmap, TBool aKeepAspectRatio)
{
    iBitmapScaler->Scale(&iStatus, aSrcBitmap, aDestBitmap, aKeepAspectRatio);
    SetActive();
    iWait->Start();
}

void CImageScaler::RunL()
{
    iWait->AsyncStop();
}

void CImageScaler::DoCancel()
{
}

// End of File
