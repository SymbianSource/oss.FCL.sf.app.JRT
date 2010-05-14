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

#include "cmmavideorecordcontrol.h"
#include "cmmaplayer.h"
#include "cmmammfresolver.h"
#include "cmmacamerasound.h"

const TInt KUnlimitedSize = KMaxTInt32;

const TReal32 KNoUserDefinedFps = -1;
const TInt KNoUserDefinedSize = -1;
const TInt KNoUserDefinedRecordSize = -1;

_LIT(KVideoMp4MimeType, "video/mp4");
_LIT(KVideoH264MimeType, "video/H264");

_LIT8(KMimeTypeMPEG4VSPL3, "video/mp4v-es; profile-level-id=3");
_LIT8(KMimeTypeH264AVCBPL12, "video/H264; profile-level-id=42800C");     // Level 1.2

CMMAVideoRecordControl* CMMAVideoRecordControl::NewL(CMMAPlayer* aPlayer,
        CMMAMMFResolver* aResolver,
        const TMMAVideoSettings& aVideoSettings,
        CMMAAudioSettings* aAudioSettings,
        TInt aCameraHandle)
{
    CMMAVideoRecordControl* self =
        new(ELeave)CMMAVideoRecordControl(aPlayer,
                                          aCameraHandle);
    CleanupStack::PushL(self);

    // SetContentType takes ownership
    aPlayer->SetContentType(aResolver->ContentTypeOwnership());
    self->ConstructL(*aResolver->Implementations(),
                     aVideoSettings,
                     aAudioSettings);

    CleanupStack::Pop();
    return self;
}

CMMAVideoRecordControl::CMMAVideoRecordControl(CMMAPlayer* aPlayer,
        TInt aCameraHandle):
        CMMARecordControl(aPlayer),
        iRecordSizeLimit(KNoUserDefinedRecordSize)
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::CMMAVideoRecordControl");

    __ASSERT_DEBUG(aCameraHandle > 0, User::Panic(
                       _L("CMMAVideoRecordControl::Invalid camera"),
                       KErrArgument));
    __ASSERT_DEBUG(aPlayer, User::Panic(
                       _L("CMMAVideoRecordControl::Invalid player"),
                       KErrArgument));
    iCameraHandle = aCameraHandle;
}

CMMAVideoRecordControl::~CMMAVideoRecordControl()
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::~CMMAVideoRecordControl");

    delete iActiveSchedulerWait;

    if (iRecorder)
    {
        iRecorder->Close();
        delete iRecorder;
    }
}

void CMMAVideoRecordControl::ConstructL(
    RMMFControllerImplInfoArray& aControllerInfos,
    const TMMAVideoSettings& aVideoSettings,
    CMMAAudioSettings* aAudioSettings)
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::ConstructL +");
    CMMARecordControl::ConstructL();

    iVideoSettings = aVideoSettings;

    iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait();

    iRecorder = CVideoRecorderUtility::NewL(*this, EMdaPriorityMin, EMdaPriorityPreferenceNone);

    TUid videoControllerUid = KNullUid;
    TUid videoFormatUid = KNullUid;

    HBufC8* encoding;
    // if wanted video encoding is H264, we must find format uid for video/mp4
    // and set encoding later because mmf doesn't have controller for video/H264
    if (!(iVideoSettings.iEncoding.Compare(KVideoH264MimeType) == 0))
    {
        encoding = HBufC8::NewLC(iVideoSettings.iEncoding.Length());
        encoding->Des().Copy(iVideoSettings.iEncoding);
    }
    else
    {
        encoding = HBufC8::NewLC(KVideoMp4MimeType().Length());
        encoding->Des().Copy(KVideoMp4MimeType());
    }

    // Go through controller infos to find format UID
    TInt controllerCount = aControllerInfos.Count();
    for (TInt cIndex = 0; cIndex < controllerCount; cIndex++)
    {
        const RMMFFormatImplInfoArray infoArray =
            aControllerInfos[ cIndex ]->RecordFormats();

        // Find correct format uid
        TInt formatCount = infoArray.Count();
        for (TInt formatIndex = 0; formatIndex < formatCount; formatIndex++)
        {
            if (infoArray[ formatIndex ]->SupportsMimeType(*encoding))
            {
                // Uids will be used to open controller
                videoFormatUid = infoArray[ formatIndex ]->Uid();
                videoControllerUid = aControllerInfos[ cIndex ]->Uid();
            }
        }
    }
    CleanupStack::PopAndDestroy(encoding);

    if (videoFormatUid == KNullUid ||
            videoControllerUid == KNullUid)
    {
        // Controller and / or format for mime was not found.
        User::Leave(KErrNotSupported);
    }

    iFile.Close();
    iRecorder->OpenFileL(iFilename,
                         iCameraHandle,
                         videoControllerUid,
                         videoFormatUid,
                         KNullDesC8,
                         aAudioSettings->iDataType);

    // wait until MvruoOpenComplete is called
    if (!iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->Start();
    }
    User::LeaveIfError(iError);

    // if wanted video encoding is mp4, set it
    if (iVideoSettings.iEncoding.Compare(KVideoMp4MimeType) == 0)
    {
        iRecorder->SetVideoTypeL(KMimeTypeMPEG4VSPL3);
        // audio encoding with mp4 is AAC by default
        iRecorder->SetAudioTypeL(KMMFFourCCCodeAAC);
    }

    // if wanted video encoding is H264, set it
    if (iVideoSettings.iEncoding.Compare(KVideoH264MimeType) == 0)
    {
        iRecorder->SetVideoTypeL(KMimeTypeH264AVCBPL12);
        // audio encoding with mp4 is AAC by default
        iRecorder->SetAudioTypeL(KMMFFourCCCodeAAC);
    }

    // setting size if it is set by user
    TSize videoSize;
    iRecorder->GetVideoFrameSizeL(videoSize);
    if (iVideoSettings.iHeight != KNoUserDefinedSize)
    {
        videoSize.iHeight = iVideoSettings.iHeight;
    }

    if (iVideoSettings.iWidth != KNoUserDefinedSize)
    {
        videoSize.iWidth = iVideoSettings.iWidth;
    }
    iRecorder->SetVideoFrameSizeL(videoSize);

    LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::ConstructL()-");
}

void CMMAVideoRecordControl::InitializeL()
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::InitializeL()");
    iRecorder->Prepare();
    if (!iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->Start();
    }
    User::LeaveIfError(iError);

    // setting frame rate if it is set by user
    // record utility must be prepared to do this
    if (iVideoSettings.iFps != KNoUserDefinedFps)
    {
        LOG1( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::InitializeL() SetVideoFrameRateL = %d", iVideoSettings.iFps);
        iRecorder->SetVideoFrameRateL(iVideoSettings.iFps);
    }

    LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::InitializeL()-");
}

void CMMAVideoRecordControl::DoStartRecordL()
{
    // if reset is called then file is reopened, closing it in order to prevent
    // KErrInUse
    iFile.Close();
    ELOG1( EJavaMMAPI, "MMA:CMMAVideoRecordControl::DoStartRecordL() err=%d", iError);
    User::LeaveIfError(iError);

    // play sound when recording starts
    CMMACameraSound::PlayVideoRecordSoundL();

    // Set recordsize limit if needed
    if (iRecordSizeLimit != KNoUserDefinedRecordSize)
    {
        DoSetRecordSizeLimitL(iRecordSizeLimit);
    }

    iRecorder->Record();
}

void CMMAVideoRecordControl::DoStopRecordL()
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::DoStopRecordL");
    // Just paused that recording can continue
    iRecorder->PauseL();
}

void CMMAVideoRecordControl::DoResetL()
{
    LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::ResetL");
    TInt err = iRecorder->Stop();
    // returns KErrCompletion when recording is finished when size limit is reached
    // and KErrAlreadyExists if we have created file (ignoring both)
    if ((err != KErrCompletion) &&
            (err != KErrAlreadyExists))
    {
        User::LeaveIfError(err);
    }
    TBool isOpen(EFalse);
    err = iFs.IsFileOpen(iFilename, isOpen);
    if (!isOpen && (err != KErrNotFound))
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::ResetL - Opening file");
        // opening file for resetting (or commit)
        User::LeaveIfError(iFile.Open(iFs, iFilename, EFileRead |
                                      EFileWrite |
                                      EFileShareAny));
    }
    else if (err == KErrNotFound)
    {
        LOG( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::ResetL - Recreating file");
        // record utility deletes file if nothing is recorded, creating it again
        User::LeaveIfError(iFile.Create(iFs,
                                        iFilename,
                                        EFileRead |
                                        EFileWrite |
                                        EFileShareAny));
    }
    else
    {
        User::LeaveIfError(err);
    }
}

void CMMAVideoRecordControl::DoSetRecordSizeLimitL(TInt aSize)
{
    if (aSize == KUnlimitedSize)
    {
        iRecorder->SetMaxClipSizeL(KMMFNoMaxClipSize);
    }
    else
    {
        LOG1( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::DoSetRecordSizeLimitL %d", aSize);
        iRecorder->SetMaxClipSizeL(aSize);
    }
    // reset local value
    iRecordSizeLimit = KNoUserDefinedRecordSize;
    // have to reprepare after setting clipsize
    iRecorder->Prepare();
    if (!iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->Start();
    }
    User::LeaveIfError(iError);
}

TInt CMMAVideoRecordControl::SetRecordSizeLimitL(TInt aSize)
{
    LOG1( EJavaMMAPI, EInfo, "MMA:CMMAVideoRecordControl::SetRecordSizeLimitL %d", aSize);

    // if already recording, try to set immediately
    if (iState == ERecordRecording)
    {
        DoSetRecordSizeLimitL(aSize);
    }
    else
    {
        // set record size limit when recording is started
        iRecordSizeLimit = aSize;
    }
    return aSize;
}

void CMMAVideoRecordControl::MvruoOpenComplete(TInt aError)
{
    ELOG1( EJavaMMAPI, "MMA:CMMAVideoRecordControl::MvruoOpenComplete aError = %d", aError);

    // Error situation is handled in ConstructL,
    iError = aError;
    if (iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->AsyncStop();
    }
}

void CMMAVideoRecordControl::MvruoPrepareComplete(TInt aError)
{
    ELOG1( EJavaMMAPI, "MMA:CMMAVideoRecordControl::MvruoPrepareComplete aError = %d", aError);
    // Error situation is handled in InitializeL
    iError = aError;
    if (iActiveSchedulerWait->IsStarted())
    {
        iActiveSchedulerWait->AsyncStop();
    }
}

void CMMAVideoRecordControl::MvruoRecordComplete(TInt aError)
{
    ELOG1( EJavaMMAPI, "MMA:CMMAVideoRecordControl::MvruoRecordComplete aError = %d", aError);

    // iState = ERecordComplete

    if ((aError == KErrNone) ||
            (aError == KErrCompletion))  // Size limit reached
    {
        if (aError == KErrCompletion)
        {
            HandleRecordSizeLimit();
        }
    }
    else
    {
        Error(aError);
    }
}


void CMMAVideoRecordControl::MvruoEvent(const TMMFEvent& aEvent)
{
    ELOG1( EJavaMMAPI, "MMA:CMMAVideoRecordControl::MvruoEvent event error code = %d", aEvent.iErrorCode);
    // Only error situations needs to be considered.
    if (aEvent.iErrorCode != KErrNone)
    {
        Error(aEvent.iErrorCode);
    }
}

//  END OF FILE
