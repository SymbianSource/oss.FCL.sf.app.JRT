/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manages popups
*
*/


// using CAknInfoPopupNoteController API for iPopupController
#include <AknInfoPopupNoteController.h>

#include <j2me/jdebug.h>

#include "CMIDPopupNoteController.h"

CMIDPopupNoteController* CMIDPopupNoteController::NewL()
{
    CMIDPopupNoteController* self = new(ELeave) CMIDPopupNoteController();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CMIDPopupNoteController::ConstructL()
{
    iPopupController = CAknInfoPopupNoteController::NewL();
}

CMIDPopupNoteController::~CMIDPopupNoteController()
{
    delete iPopupController;
}


void CMIDPopupNoteController::ShowPopupL(const TDesC& aText, const TRect aRect, TBool aShortPopup)
{
    if (!iPopupController->IsActive())
    {
        iPopupController->SetTextL(aText);
        iPopupController->SetPositionByHighlight(aRect);
        iPopupController->SetTimeDelayBeforeShow((aShortPopup?KPopupNoteShortVisibleDelayMs:KPopupNoteVisibleDelayMs));
        iPopupController->SetTimePopupInView((aShortPopup?KPopupNoteShortVisibleTimeMs:KPopupNoteVisibleTimeMs));
        iPopupController->ShowInfoPopupNote();
    }
    else
    {
        DEBUG("CMIDPopupNoteController::ShowPopupL IGNORED");
    }
}

void CMIDPopupNoteController::HidePopup()
{
    iPopupController->HideInfoPopupNote();
}


CMIDPopupNoteController::CMIDPopupNoteController()
{
    // nothing here
}


