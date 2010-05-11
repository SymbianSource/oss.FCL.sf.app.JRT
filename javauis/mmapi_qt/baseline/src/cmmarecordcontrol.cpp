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
* Description:  This class is a RecordControl.
*
*/


//  INCLUDE FILES
#include <logger.h>
#include "cmmarecordcontrol.h"
#include "cmmaplayer.h"


_LIT(KControlName, "RecordControl");

CMMARecordControl::CMMARecordControl(CMMAPlayer* aPlayer):
        iState(ERecordUninitialized),
        iPlayer(aPlayer),
        iStartTime(0)
{
}


CMMARecordControl::~CMMARecordControl()
{
    iRecordPauseTimer.Cancel();
    iRecordPauseTimer.Close();

    if (iPlayer)
    {
        iPlayer->RemoveStateListener(this);
    }

    delete iOutputStream;

    iFile.Close();
    iFs.Delete(iFilename);
    iFs.Close();
}

void CMMARecordControl::WaitForPauseL(const TInt64& aMediaTime)
{
    // reusing time variable
    TInt64 time = iStartTime - aMediaTime + KMinRecordPauseInterval;
    iStartTime = 0;
    LOG1( EJavaMMAPI, EInfo, "CMMARecordControl::WaitForPauseL wait time = %d", (TInt)time);
    // if time > 0, time between record and pause is smaller than
    // KMinRecordPauseInterval and we have to wait.
    if (time > 0)
    {
        TRequestStatus status;
        iRecordPauseTimer.Cancel();
        iRecordPauseTimer.After(status, I64INT(time));
        User::WaitForRequest(status);
        // Status ignored because recording must be stopped.
    }
}

void CMMARecordControl::ConstructL()
{
    User::LeaveIfError(iRecordPauseTimer.CreateLocal());

    User::LeaveIfError(iFs.Connect());

    // create temporary file for recording
    User::LeaveIfError(iFile.Temp(iFs,
                                  KMMARecordTempDirectory,
                                  iFilename,
                                  EFileRead | EFileWrite | EFileShareAny));

    iPlayer->AddStateListenerL(this);
}

void CMMARecordControl::StartRecordL()
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::StartRecordL+");

    // Initialize needs to be done only once after setRecordLocation or
    // setRecordStream and before commit or reset. Although startRecord can be
    // called several times in the process, the initialization must happen
    // only in the first call to startRecord
    if (iState == ERecordUninitialized)
    {
        InitializeL();
    }

    iState = ERecordStandBy;
    // calling next state if player is already started
    NextStateL(iPlayer->State());

    iPlayer->GetMediaTime(&iStartTime);
    iPlayer->PostLongEvent(CMMAPlayerEvent::ERecordStarted, iStartTime);

    LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::StartRecordL-");
}


void CMMARecordControl::StopRecordL()
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::StopRecordL+");
    if (iState == ERecordRecording)
    {
        NextStateL(iPlayer->State());
    }

    iState = ERecordInitialized;

    // ask mediatime again for more accurate value
    TInt64 time;
    iPlayer->GetMediaTime(&time);
    iPlayer->PostLongEvent(CMMAPlayerEvent::ERecordStopped, time);

    LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::StopRecordL-");
}


void CMMARecordControl::CommitL()
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::CommitL+");
    // reset the recorder if already initialized
    if (iState > ERecordUninitialized)
    {
        DoResetL();
    }
    else
    {
        // not even initialized yet, just inform java
        iOutputStream->WriteL(KNullDesC8);
        return;
    }
    iState = ERecordUninitialized;

    // Create buffer for file context
    TInt fileSize;
    User::LeaveIfError(iFile.Size(fileSize));
    LOG1( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::CommitL size in bytes = %d", fileSize);

    HBufC8* data = HBufC8::NewLC(fileSize);

    // Read file context
    TPtr8 dataPtr = data->Des();
    TInt pos(0);
    User::LeaveIfError(iFile.Seek(ESeekStart, pos));
    User::LeaveIfError(iFile.Read(dataPtr));
    User::LeaveIfError(iFile.SetSize(0));
    User::LeaveIfError(iFile.Flush());

    // Write whole video data to stream
    iOutputStream->WriteL(dataPtr);
    CleanupStack::PopAndDestroy(data);

    LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::CommitL-");
}


void CMMARecordControl::ResetL()
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::ResetL+");
    // reset the recorder if already initialized
    if (iState > ERecordUninitialized)
    {
        DoResetL();
        LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::DoResetL done");
        iState = ERecordUninitialized;

        // Empty file that it can be reused
        User::LeaveIfError(iFile.SetSize(0));
        User::LeaveIfError(iFile.Flush());
    }

    LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::ResetL-");
}

void CMMARecordControl::SetRecordStream(CMMAOutputStream* aStream)
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::SetRecordStream");
    __ASSERT_DEBUG(aStream, User::Panic(
                       _L("CMMAVideoRecordControl:: Stream is NULL."),
                       KErrArgument));
    delete iOutputStream;
    iOutputStream = aStream;
}


HBufC* CMMARecordControl::GetContentTypeL()
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::GetContentTypeL");
    return iPlayer->ContentType();
}

const TDesC& CMMARecordControl::ClassName() const
{
    return KControlName;
}

void CMMARecordControl::StateChanged(TInt aState)
{
    TRAPD(err, NextStateL(aState));
    if (err)
    {
        Error(err);
    }
}

void CMMARecordControl::Error(TInt aErrorCode)
{
    ELOG1( EJavaMMAPI, "MMA:CMMARecordControl::Error %d", aErrorCode);
    TBuf<KRecordErrorMessageSize> errorMessage;
    errorMessage.Format(KMMARecordErrorMsg, aErrorCode);
    iPlayer->PostStringEvent(CMMAPlayerEvent::ERecordError,
                             errorMessage);
}

void CMMARecordControl::HandleRecordSizeLimit()
{
    // commit the stream
    iOutputStream->Commit();
    TInt64 time;
    iPlayer->GetMediaTime(&time);
    iPlayer->PostLongEvent(CMMAPlayerEvent::ERecordStopped, time);
}

void CMMARecordControl::NextStateL(TInt aPlayerState)
{
    LOG1( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::NextStateL Player state = %d", aPlayerState);
    LOG1( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::NextStateL internal state = %d", iState);
    switch (iState)
    {
    case ERecordUninitialized:
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::NextStateL: case ERecordUninitialized");
        // This is the initial state.
        // To getinto this state, commit or reset must be called
        // To get out of this state startRecord must be called

        break;
    }
    case ERecordInitialized:
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::NextStateL: case ERecordInitialized");
        // To get into this state stopRecord must be called.
        // To get out of this state startRecord, commit or reset must be
        // called
        break;
    }
    case ERecordStandBy:
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::NextStateL: case ERecordStandBy");
        // To get into this state startRecord must be called.
        // To get out of this state stopRecord, commit or reset must be
        // called, or state of the Player must change to STARTED


        // start the actual recording if player is started
        if (aPlayerState == CMMAPlayer::EStarted)
        {
            LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::NextStateL: ERecordStandBy -> ERecordRecording");
            DoStartRecordL();
            iState = ERecordRecording;
        }
        break;
    }
    case ERecordRecording:
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::NextStateL: case ERecordRecording");

        // To get into this state startRecord must have been called and
        // the Player must be in STARTED state.
        // To get out of this state, stopRecord, commit or reset must be
        // called or the Player must change to a state other than STARTED
        TInt64 time;
        iPlayer->GetMediaTime(&time);
        WaitForPauseL(time);
        DoStopRecordL();
        iState = ERecordStandBy;
        break;
    }
    default:
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMARecordControl::NextStateL: case default - code should not reach here!!");
        __ASSERT_DEBUG(KErrGeneral, User::Invariant());   // This will newer occur
        break;
    }
    }
}

//  END OF FILE
