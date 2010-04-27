/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  RecordControl for AudioRecorder
*
*/


//  INCLUDE FILES
#include <jdebug.h>

#include "cmmaaudiorecordcontrol.h"


CMMAAudioRecordControl* CMMAAudioRecordControl::NewL(
    CMMAAudioRecorder* aRecorder)
{
    CMMAAudioRecordControl* self = new(ELeave) CMMAAudioRecordControl(aRecorder);
    CleanupStack::PushL(self);
    self->ConstructL();
    aRecorder->InitializeL(&self->iFile, self);
    CleanupStack::Pop(self);
    return self;
}

CMMAAudioRecordControl::~CMMAAudioRecordControl()
{
    // close controller to free temp file
    if (iRecorder)
    {
        iRecorder->Deinitialize();
    }
}


CMMAAudioRecordControl::CMMAAudioRecordControl(CMMAAudioRecorder* aRecorder):
        CMMARecordControl(aRecorder)
{
    iRecorder = aRecorder;
    DEBUG("CMMAAudioRecordControl()");
}

void CMMAAudioRecordControl::DoStartRecordL()
{
    iRecorder->StartRecordL();
}

void CMMAAudioRecordControl::DoStopRecordL()
{
    iRecorder->StopRecordL();
}

void CMMAAudioRecordControl::DoResetL()
{
    iRecorder->ResetL();
}

void CMMAAudioRecordControl::InitializeL()
{
    // already initialized, empty implementation
}

TInt CMMAAudioRecordControl::SetRecordSizeLimitL(TInt aSize)
{
    return iRecorder->SetRecordSizeLimitL(aSize);
}

void CMMAAudioRecordControl::HandleEvent(const TMMFEvent& aEvent)
{
    DEBUG_INT2("MMA:CMMAAudioRecordControl::HandleEvent(type=%x,err=%d)",
               aEvent.iEventType.iUid, aEvent.iErrorCode);
    if (iState == ERecordRecording)
    {
        // Event's error code is KErrEof or KErrNone when record size limit
        // is reached. When controller is stopped from java error code is
        // KErrNone.
        if ((aEvent.iEventType == KMMFEventCategoryPlaybackComplete) &&
                ((aEvent.iErrorCode == KErrNone) ||
                 (aEvent.iErrorCode == KErrEof)))
        {
            HandleRecordSizeLimit();
        }
        else if (aEvent.iErrorCode != KErrNone)
        {
            Error(aEvent.iErrorCode);
        }
    }
}

//  END OF FILE


