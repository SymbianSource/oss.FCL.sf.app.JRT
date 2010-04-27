/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include <w32std.h>
#include <coecntrl.h>
#include <bitmaptransforms.h>
#include "swtrotateimage.h"
#include "eswtwidgetscore.h"


// ======== MEMBER FUNCTIONS ========


CAORotateImage* CAORotateImage::NewL()
{
    CAORotateImage* self = new(ELeave) CAORotateImage();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CAORotateImage::CAORotateImage()
        : CActive(CActive::EPriorityHigh)
{
    CActiveScheduler::Add(this);
    iStep       = -1;
    iIsFinish   = EFalse;
}

CAORotateImage::~CAORotateImage()
{
    Cancel();

    iImages.Reset();

    delete iBitmapRotator;

    iControl    = NULL;
}

void CAORotateImage::ConstructL()
{
    iBitmapRotator      = CBitmapRotator::NewL();
}

void CAORotateImage::Start(MSwtControl* aControl)
{
    Cancel();
    iIsFinish   = EFalse;

    iControl    = aControl;
    if (aControl)
    {
        iRedraw = ETrue;
    }

    iStep = 0;
    Queue();
}

void CAORotateImage::RemoveAllImages()
{
    iStep = -1;
    iIsFinish   = EFalse;
    iRedraw = EFalse;
    Cancel();

    iImages.Reset();
}

void CAORotateImage::AddImage(CFbsBitmap* aImage)
{
    ASSERT(aImage);
    ASSERT(!aImage->IsCompressedInRAM());

    Cancel();
    if (!aImage->IsCompressedInRAM())
    {
        iImages.Append(aImage);
    }
}

void CAORotateImage::AddImages(const RArray<CFbsBitmap*>& aImages)
{
    Cancel();
    for (TInt i = 0; i < aImages.Count(); i++)
    {
        if (aImages[i])
        {
            ASSERT(!aImages[i]->IsCompressedInRAM());

            if (!aImages[i]->IsCompressedInRAM())
            {
                iImages.Append(aImages[i]);
            }
        }
    }
}

void CAORotateImage::SetRedrawAfterRotation(TBool aRedrawAfterRotation)
{
    iRedraw = aRedrawAfterRotation;
}

TBool CAORotateImage::IsFinish() const
{
    return iIsFinish;
}

void CAORotateImage::DoCancel()
{
    iBitmapRotator->Cancel();

    iImages.Reset();

    iStep       = -1;
    iIsFinish   = EFalse;
}

void CAORotateImage::Queue()
{
    if (iStep < iImages.Count() && iStep != -1)
    {
        iIsFinish   = EFalse;
        iBitmapRotator->Rotate(&iStatus, *iImages[iStep++],
                               CBitmapRotator::ERotation270DegreesClockwise);

        SetActive();
    }
    else
    {
        iIsFinish   = ETrue;

        if (iRedraw && iControl)
        {
            if (iControl->IsPaintingUrgently())
            {
                iControl->PaintUrgently(iControl->CoeControl().Rect());
            }
            else
            {
                iControl->Paint(TRect((iControl->CoeControl()).PositionRelativeToScreen(),
                                      iControl->GetWidgetSize()));
            }
        }
    }
}

void CAORotateImage::RunL()
{
    Queue();
}
