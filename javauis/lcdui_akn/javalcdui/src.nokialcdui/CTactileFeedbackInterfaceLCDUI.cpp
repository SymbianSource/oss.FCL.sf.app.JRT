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
* Description:  Implementation of MTactileFeedbackInterface of LCDUI midlets.
*
*/


#ifdef RD_TACTILE_FEEDBACK

#include "CMIDToolkit.h"
#include <touchfeedback.h>
#include "CTactileFeedbackInterfaceLCDUI.h"
#include "TactileFeedbackImpl.h"

//#include "CMIDTactileFeedbackExtension.h"

CTactileFeedbackInterfaceLCDUI::CTactileFeedbackInterfaceLCDUI(CMIDToolkit* aToolkit)
{
    iLcduiToolkit = aToolkit;
    aToolkit->ExecuteTrap(&_CreateMTouchFeedback, &iFeedback);
};


void CTactileFeedbackInterfaceLCDUI::DirectFeedback(TTouchLogicalFeedback aStyle)
{
    iLcduiToolkit->ExecuteTrap(&_DoDirectFeedback, iFeedback, aStyle);
};


void CTactileFeedbackInterfaceLCDUI::RegisterFeedbackArea(void* aControl, TInt aControlType, TInt aAreaIndex, TInt aX, TInt aY, TInt aWidth, TInt aHeight, TTouchLogicalFeedback aStyle)
{
    TRect feedbackRect(aX, aY, aX + aWidth, aY + aHeight);

    MMIDTactileFeedbackComponent*  tactileComponent = NULL;

    if (aControlType == 1)  // Canvas
    {
        MMIDCanvas* control = MIDUnhand<MMIDCanvas>((TInt)aControl);
        tactileComponent = control->TactileFeedbackComponent();
    }
    else if (aControlType == 2) //CustomItem
    {
        MMIDCustomItem* control = MIDUnhand<MMIDCustomItem>((TInt)aControl);
        tactileComponent = control->TactileFeedbackComponent();
    }

    iLcduiToolkit->ExecuteTrap(&_RegisterFeedbackAreaForComponent, tactileComponent, aAreaIndex, feedbackRect, aStyle);
};


void CTactileFeedbackInterfaceLCDUI::UnregisterFeedbackArea(void* aControl, TInt aControlType,  TInt aAreaIndex)
{
    MMIDTactileFeedbackComponent*  tactileComponent = NULL;

    if (aControlType == 1)  // Canvas
    {
        MMIDCanvas* control = MIDUnhand<MMIDCanvas>((TInt)aControl);
        tactileComponent = control->TactileFeedbackComponent();
    }
    else if (aControlType == 2) //CustomItem
    {
        MMIDCustomItem* control = MIDUnhand<MMIDCustomItem>((TInt)aControl);
        tactileComponent = control->TactileFeedbackComponent();
    }


    iLcduiToolkit->ExecuteTrap(&_UnregisterFeedbackAreaForComponent, tactileComponent, aAreaIndex);
}

void CTactileFeedbackInterfaceLCDUI::UnregisterFeedbackForControl(void* aControl, TInt aControlType)
{
    MMIDTactileFeedbackComponent*  tactileComponent = NULL;

    if (aControlType == 1)  // Canvas
    {
        MMIDCanvas* control = MIDUnhand<MMIDCanvas>((TInt)aControl);
        tactileComponent = control->TactileFeedbackComponent();
    }
    else if (aControlType == 2) //CustomItem
    {
        MMIDCustomItem* control = MIDUnhand<MMIDCustomItem>((TInt)aControl);
        tactileComponent = control->TactileFeedbackComponent();
    }


    iLcduiToolkit->ExecuteTrap(&_UnregisterFeedbackForControlForComponent, tactileComponent);
}

void CTactileFeedbackInterfaceLCDUI::MoveAreaToFirstPriority(void* aControl, TInt aControlType, TInt aAreaIndex)
{
    MMIDTactileFeedbackComponent*  tactileComponent = NULL;

    if (aControlType == 1)  // Canvas
    {
        MMIDCanvas* control = MIDUnhand<MMIDCanvas>((TInt)aControl);
        tactileComponent = control->TactileFeedbackComponent();
    }
    else if (aControlType == 2) //CustomItem
    {
        MMIDCustomItem* control = MIDUnhand<MMIDCustomItem>((TInt)aControl);
        tactileComponent = control->TactileFeedbackComponent();
    }

    iLcduiToolkit->ExecuteTrap(&_MoveAreaToFirstPriorityForComponent, tactileComponent, aAreaIndex);
}

TBool CTactileFeedbackInterfaceLCDUI::IsTouchFeedbackSupported()
{
    return iLcduiToolkit->Execute(&_IsTouchFeedbackSupported, iFeedback);
}

#endif
