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

#ifdef RD_SCALABLE_UI_V2
#include "swtpointereventsuppressor.h"
#include "eswtwidgetscore.h"

//Pointer event suppressor default values
const TInt KPESPointerMovementInTwips = 200; //1 twip == 1/1440 of an inch
const TInt KPESTimeInMilliseconds = 500; //default time 0.5s

/**
 * 1st & 2nd phase constructors wrapper
 */
CSwtPointerEventSuppressor::CSwtPointerEventSuppressor()
        :
        iIsSuppressionOngoing(EFalse)
{
}

CSwtPointerEventSuppressor* CSwtPointerEventSuppressor::NewL(
    const MSwtControl* aControl)
{
    CSwtPointerEventSuppressor* self = new(ELeave) CSwtPointerEventSuppressor();
    CleanupStack::PushL(self);
    self->ConstructL(aControl);
    CleanupStack::Pop(self);
    return self;
}

/**
 * 2nd phase constructor
 */
void CSwtPointerEventSuppressor::ConstructL(const MSwtControl* aControl)
{
    iPointerEventSuppressor = CAknPointerEventSuppressor::NewL();
    iPointerEventSuppressor->SetMaxTapDuration(
        KPESTimeInMilliseconds * 1000);
    TInt pointerMovementInPixels = aControl->CoeControl().ControlEnv()
                                   ->ScreenDevice()
                                   ->HorizontalTwipsToPixels(KPESPointerMovementInTwips);
    iPointerEventSuppressor->SetMaxTapMove(TSize(pointerMovementInPixels,
                                           pointerMovementInPixels));
}

/**
 * Destructor
 */
CSwtPointerEventSuppressor::~CSwtPointerEventSuppressor()
{
    if (iPointerEventSuppressor)
    {
        delete iPointerEventSuppressor;
        iPointerEventSuppressor = NULL;
    }
}

TBool CSwtPointerEventSuppressor::IsSuppressionEnabled() const
{
    return NULL != iPointerEventSuppressor;
}

TBool CSwtPointerEventSuppressor::SuppressPointerEvent(
    const TPointerEvent& aPointerEvent)
{
    iPointerEventPosition = aPointerEvent.iPosition;
    if (!IsSuppressionEnabled())
    {
        return EFalse;
    }

    switch (aPointerEvent.iType)
    {
    case TPointerEvent::EButton1Down:
    case TPointerEvent::EButton2Down:
    case TPointerEvent::EButton3Down:
        iLastButtonDownPointerEventPosition = iPointerEventPosition;
        break;
    case TPointerEvent::EButton1Up:
    case TPointerEvent::EButton2Up:
    case TPointerEvent::EButton3Up:
        if (iIsSuppressionOngoing)
        {
            iPointerEventPosition = iLastButtonDownPointerEventPosition;
        }
        break;
    default:
        //Do nothing
        break;
    }

    iIsSuppressionOngoing = iPointerEventSuppressor
                            ->SuppressPointerEvent(aPointerEvent);
    return iIsSuppressionOngoing;
}

TPoint CSwtPointerEventSuppressor::PointerEventPosition() const
{
    return iPointerEventPosition;
}

#endif //RD_SCALABLE_UI_V2
