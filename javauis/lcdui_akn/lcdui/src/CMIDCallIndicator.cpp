/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#include <e32base.h>
//using CTelephony class
#include <etel3rdparty.h>
#include "CMIDCallIndicator.h"
//using CMIDCanvas class - needed for CMIDNetworkIndicator
#include "CMIDCanvas.h"
//using CMIDNetworkIndicator class - iNetworkIndicator
#include "CMIDNetworkIndicator.h"
#include <j2me/jdebug.h>

/**
 * Constructor. Defines the priority for this active object and adds him to AS.
 */
CMIDCallIndicator::CMIDCallIndicator(
    CMIDNetworkIndicator *aNetworkIndicator):
        CActive(EPriorityStandard)
        , iNetworkIndicator(aNetworkIndicator)
        , iLineStatusPckg(iLineStatus)
{
    iNetworkIndicator->SetCallIndicator(this);
    CActiveScheduler::Add(this);
}

/**
 * Two-phased constructor.
 */
CMIDCallIndicator* CMIDCallIndicator::NewL(
    CMIDNetworkIndicator *aNetworkIndicator)
{
    DEBUG(">>> CMIDCallIndicator::NewL");
    CMIDCallIndicator* self = CMIDCallIndicator::NewLC(aNetworkIndicator);
    CleanupStack::Pop(self);
    DEBUG("<<< CMIDCallIndicator::NewL");
    return self;
}

/**
 * Two-phased constructor.
 */
CMIDCallIndicator* CMIDCallIndicator::NewLC(
    CMIDNetworkIndicator *aNetworkIndicator)
{
    CMIDCallIndicator* self = new(ELeave) CMIDCallIndicator(
        aNetworkIndicator);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

/**
 * 2nd phase constructor.
 */
void CMIDCallIndicator::ConstructL()
{
    iTelephony = CTelephony::NewL();
    Init();
}

/**
 * Destructor.
 */
CMIDCallIndicator::~CMIDCallIndicator()
{
    DEBUG(">>> CMIDCallIndicator::~CMIDCallIndicator");
    Cancel();
    delete iTelephony;
    DEBUG("<<< CMIDCallIndicator::~CMIDCallIndicator");
}

/**
 * Activates the AO and register for notification by the CTelephony
 */
void CMIDCallIndicator::SetActiveLocal()
{
    // XXX DEBUG(">>> CMIDCallIndicator::SetActiveLocal");
    if (!this->IsActive())
    {
        iTelephony->NotifyChange(iStatus,
                                 CTelephony::EVoiceLineStatusChange,
                                 iLineStatusPckg);
        SetActive();
    }
    // XXX DEBUG("<<< CMIDCallIndicator::SetActiveLocal");
}

/**
 * Finds out if call is ongoing when opening canvas and sets members.
 */
void CMIDCallIndicator::Init()
{
    CTelephony::TPhoneLine line = CTelephony::EVoiceLine;

    iTelephony->GetLineStatus(line, iLineStatusPckg);

    if (CallInProgress(iLineStatus.iStatus))
    {
        // Call in progress, show call indicator.
        TRAPD(ignoreError,
              iNetworkIndicator->CreateNetworkResourceL(EBearerInfoCSD, 0));
        if (ignoreError != KErrNone)
        { // Leave is ignored. It is not so important to pass leave only
            // because the icon wasnt created.
            DEBUG_INT("CMIDCallIndicator::SetActiveLocal - Exception from CreateNetworkResourceL. Error = %d", ignoreError);
        }
        iNetworkIndicator->Repaint();
        iView = ETrue;
    }
    else
    {
        iLineStatus.iStatus = CTelephony::EStatusUnknown;
        iView = EFalse;
    }
}

/*
* Check the call status. If there is call established
* return true orwise return false.
*
* @param   aCallStatus     call status.
* @return ETrue if calling else EFalse.
*/
TBool CMIDCallIndicator::CallInProgress(CTelephony::TCallStatus aCallStatus)
{
    switch (aCallStatus)
    {
    case CTelephony::EStatusDialling:
    case CTelephony::EStatusRinging:
    case CTelephony::EStatusAnswering:
    case CTelephony::EStatusConnecting:
    case CTelephony::EStatusConnected:
    case CTelephony::EStatusReconnectPending:
    case CTelephony::EStatusDisconnecting:
    case CTelephony::EStatusHold:
    case CTelephony::EStatusTransferring:
    case CTelephony::EStatusTransferAlerting:
    {
        return ETrue; // call is in progress
    }
    case CTelephony::EStatusIdle:
    case CTelephony::EStatusUnknown:
    default:
    {
        return EFalse; // no active call
    }
    }
}

/**
 * Returns the call activity.
 *
 * @return Member iView. ETrue when active call indicator is shown = call
 *         is ongoing and EFalse otherwise.
 */
TBool CMIDCallIndicator::GetCallActivity()
{
    return iView;
}

/**
 * From CActive.
 */
void CMIDCallIndicator::RunL()
{
    DEBUG(">>> CMIDCallIndicator::RunL");

    TBool callOn = CallInProgress(iLineStatus.iStatus);

    if (!iView && callOn)
    {
        // Draw active call indicator because the call just started.
        // Create the network resource.
        // XXX    DEBUG("CMIDCallIndicator::RunL() -Newcall");
        TRAPD(ignoreError,
              iNetworkIndicator->CreateNetworkResourceL(EBearerInfoCSD, 0));
        if (ignoreError != KErrNone)
        { // Leave is ignored. It is not so important to pass leave only
            // because the icon wasnt created.
            DEBUG_INT("CMIDCallIndicator::SetActiveLocal - Exception from CreateNetworkResourceL. Error = %d", ignoreError);
        }
        iView = ETrue;
        // Repaints the indicator
        iNetworkIndicator->Repaint(ETrue);
    }
    else if (iView && !callOn)
    {
        // XXX    DEBUG("CMIDCallIndicator::RunL() -Idle/unknown - iView=ETrue >>>");
        // Call just ended. Clean the bitmap. Fill it with network indicator
        // stored in temp bitmaps only if the network connection is active.
        // Used to repaints the active call indicator with network indicator
        // immediately after the call ends.
        iNetworkIndicator->CleanNetworkResource();
        if (iNetworkIndicator->IsNetworkActive())
        {
            // There is active network connection, show its indicator.
            // XXX DEBUG("CMIDCallIndicator::RunL() -Network is active - copybitmaps");
            iNetworkIndicator->CopyBitmaps();
            iNetworkIndicator->ResizeBitmap();
        }
        iView = EFalse;
        iNetworkIndicator->Repaint(ETrue);
        // XXX DEBUG("CMIDCallIndicator::RunL() -Idle/unknown - iView=ETrue <<<");
    }
    // Set active to get next voice line status change.
    SetActiveLocal();
    DEBUG("<<< CMIDCallIndicator::RunL");
}

/**
 * From CActive.
 */
TInt CMIDCallIndicator::RunError(TInt anError)
{
    return anError;
}

/**
 * From CActive.
 */
void CMIDCallIndicator::DoCancel()
{
    // The EVoiceLineStatusChange cancellation code.
    iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
}

