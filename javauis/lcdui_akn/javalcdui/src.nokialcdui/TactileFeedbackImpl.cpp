/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Layer to transfer calls from MTactileFeedbackInterface to native tactile feedback API.
*
*/



#ifdef RD_TACTILE_FEEDBACK

#include "CMIDToolkit.h"
#include <touchfeedback.h>
#include "CMIDCustomItem.h"

// eSWT implementation methods. Direct calls to native feedback engine.
void _CreateMTouchFeedback(MTouchFeedback**  aHandle)
{
    *aHandle = MTouchFeedback::Instance();
};

void _DoDirectFeedback(MTouchFeedback *aHandle, TTouchLogicalFeedback aStyle)
{
    aHandle->InstantFeedback(aStyle);
};

void _RegisterFeedbackArea(MTouchFeedback *aHandle, CCoeControl* aControl, TInt aAreaIndex, TRect aFeedbackRect, TTouchLogicalFeedback aStyle)
{
    aHandle->SetFeedbackArea(aControl, aAreaIndex, aFeedbackRect, aStyle, ETouchEventStylusDown);
};


void _UnregisterFeedbackArea(MTouchFeedback *aHandle, CCoeControl* aControl, TInt aAreaIndex)
{
    aHandle->RemoveFeedbackArea(aControl, aAreaIndex);
}

void _UnregisterFeedbackForControl(MTouchFeedback *aHandle, CCoeControl* aControl)
{
    aHandle->RemoveFeedbackForControl(aControl);
}

void _MoveAreaToFirstPriority(MTouchFeedback *aHandle, CCoeControl* aControl, TInt aAreaIndex)
{
    aHandle->MoveFeedbackAreaToFirstPriority(aControl, aAreaIndex);
}




// LCDUI implementation method. In LCDUI a component is responsible for storing/moving/updating a feedback areas.

void _RegisterFeedbackAreaForComponent(MMIDTactileFeedbackComponent* aComponent, TInt aAreaIndex, TRect aFeedbackRect, TTouchLogicalFeedback aStyle)
{
    //MMIDCustomItem* control = JavaUnhand<MMIDCustomItem>( (TInt)aControl );
    aComponent->RegisterFeedbackArea(aAreaIndex, aFeedbackRect, aStyle);
}

void _UnregisterFeedbackAreaForComponent(MMIDTactileFeedbackComponent* aComponent, TInt aAreaIndex)
{
    aComponent->UnregisterFeedbackArea(aAreaIndex);
}

void _UnregisterFeedbackForControlForComponent(MMIDTactileFeedbackComponent* aComponent)
{
    aComponent->UnregisterFeedbackForControl();
}

void _MoveAreaToFirstPriorityForComponent(MMIDTactileFeedbackComponent* aComponent, TInt aAreaIndex)
{
    aComponent->MoveAreaToFirstPriority(aAreaIndex);
}

TBool _IsTouchFeedbackSupported(MTouchFeedback *aHandle)
{
    return aHandle->TouchFeedbackSupported();
}
#endif
