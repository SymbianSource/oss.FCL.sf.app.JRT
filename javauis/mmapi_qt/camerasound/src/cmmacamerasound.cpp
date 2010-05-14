/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used to play sound from file
*
*/



// INCLUDE FILES
#include    "cmmacamerasound.h"

#include    <mdaaudiosampleplayer.h>
#include <logger.h>
#include <AudioPreference.h>

//CONSTANTS
_LIT(KMMASnapshotSound, "Z:\\System\\Sounds\\Digital\\CamcorderJavaCapture.wav");
_LIT(KMMAVideoSound, "Z:\\System\\Sounds\\Digital\\CamcorderJavaStart.wav");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMMACameraSound::CMMACameraSound
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMMACameraSound::CMMACameraSound()
{
}

// -----------------------------------------------------------------------------
// CMMACameraSound::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMMACameraSound* CMMACameraSound::NewLC()
{
    CMMACameraSound* self = new(ELeave) CMMACameraSound;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}


// Destructor
CMMACameraSound::~CMMACameraSound()
{
    delete iActiveSchedulerWait;
}

// -----------------------------------------------------------------------------
// CMMACameraSound::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMMACameraSound::ConstructL()
{
    iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait();
}

// -----------------------------------------------------------------------------
// PlayImageCaptureSoundL()
// -----------------------------------------------------------------------------
//
void CMMACameraSound::PlayImageCaptureSoundL()
{
    CMMACameraSound* cameraSound = NewLC();
    cameraSound->PlaySoundL(KMMASnapshotSound());
    CleanupStack::PopAndDestroy(cameraSound);
}

// -----------------------------------------------------------------------------
// PlayVideoRecordSoundL()
// -----------------------------------------------------------------------------
//
void CMMACameraSound::PlayVideoRecordSoundL()
{
    CMMACameraSound* cameraSound = NewLC();
    cameraSound->PlaySoundL(KMMAVideoSound());
    CleanupStack::PopAndDestroy(cameraSound);
}

// -----------------------------------------------------------------------------
// PlaySoundL()
// -----------------------------------------------------------------------------
//
void CMMACameraSound::PlaySoundL(const TDesC& aFileName)
{
    iError = KErrNone;
    CMdaAudioPlayerUtility* player =
        CMdaAudioPlayerUtility::NewFilePlayerL(
            aFileName, *this, KAudioPriorityVideoRecording,
            TMdaPriorityPreference(KAudioPrefCamera), NULL);
    CleanupStack::PushL(player);

    //wait until init is completed
    if (!iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->Start();
    }
    //leave if there was error in init
    User::LeaveIfError(iError);

    player->Play();
    //wait until play is completed
    if (!iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->Start();
    }
    //leave if there was error in play
    User::LeaveIfError(iError);

    //close player
    player->Close();
    CleanupStack::PopAndDestroy(player);
}

void CMMACameraSound::MapcInitComplete(TInt aError,
                                       const TTimeIntervalMicroSeconds& /*aDuration*/)
{
    iError = aError;
    if (iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->AsyncStop();
    }
}

void CMMACameraSound::MapcPlayComplete(TInt aError)
{
    iError = aError;
    if (iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->AsyncStop();
    }
}

//  End of File
