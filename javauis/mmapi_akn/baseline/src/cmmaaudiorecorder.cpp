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
* Description:  This class is used for recording Audio
*
*/


#include <jdebug.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmfformatimplementationuids.hrh>
#include "cmmaaudiorecorder.h"
#include "cmmaoutputstream.h"

const TInt KDefaultSize = KMaxTInt32;   // Default record size limit

CMMAAudioRecorder* CMMAAudioRecorder::NewLC(
    CMMAMMFResolver* aResolver,
    CMMAAudioSettings* aAudioSettings,
    TInt aMIDletSuiteID)
{
    // Not using ELeave to avoid leave before owneship change of settings
    CMMAAudioRecorder* self = new CMMAAudioRecorder(aResolver, aMIDletSuiteID);
    if (!self)
    {
        delete aAudioSettings;
        User::Leave(KErrNoMemory);
    }
    // Take the ownership of setting before any possible leaves
    self->iSettings = aAudioSettings;
    CleanupStack::PushL(self);
    self->iWait = new(ELeave) CActiveSchedulerWait();
    self->ConstructL();
    return self;
}


CMMAAudioRecorder::~CMMAAudioRecorder()
{
    iFile.Close();
    delete iSettings;
    delete iWait;
}


CMMAAudioRecorder::CMMAAudioRecorder(
    CMMAMMFResolver* aResolver,
    TInt /*aMIDletSuiteID*/):
        CMMAMMFPlayerBase(aResolver),
        iAudioRecordControllerCustomCommands(iController),
        iAudioControllerRecCustomCommands(iController),
        iRecordSizeLimit(KDefaultSize),
        iPauseError(KErrNone)
{
}

/**
 * start the play
 */
void CMMAAudioRecorder::StartL(TBool aPostEvent)
{
    if (aPostEvent)
    {
        TInt64 time;
        GetMediaTime(&time);
        PostLongEvent(CMMAPlayerEvent::EStarted, time);
    }

    ChangeState(EStarted);
    PostActionCompleted(KErrNone);   // java start return
}


void CMMAAudioRecorder::StopL(TBool aPostEvent)
{
    if (iState == EStarted)
    {
        ChangeState(EPrefetched);
    }

    if (aPostEvent)
    {
        TInt64 time;
        GetMediaTime(&time);
        PostLongEvent(CMMAPlayerEvent::EStopped, time);
    }
}

void CMMAAudioRecorder::DoOpenL()
{
    // Make sure any existing controller is closed.
    iEventMonitor->Cancel();
    iController.Close();

    // player priority settings
    TMMFPrioritySettings prioritySettings;
    prioritySettings.iPriority = EMdaPriorityMax;
    prioritySettings.iPref = EMdaPriorityPreferenceTimeAndQuality;
    prioritySettings.iState = EMMFStateRecording;

    // Try opening and configuring each controller in turn
    TInt error = KErrNotSupported;
    TInt index = 0;

    // Try controllers until found a good controller or out of list
    while ((error != KErrNone) &&
            (index < iControllerInfos->Count()))
    {
        // Open the controller
        error = iController.Open((*iControllerInfos)[ index ]->Uid(),
                                 prioritySettings);

        // If the controller was opened without error, start receiving events from it.
        if (!error)
        {
            iEventMonitor->Start();

            // Add the data source to the controller.
            error = iController.AddDataSource(KUidMmfAudioInput, KNullDesC8);
        }

        // Add the data sink
        if (!error)
        {
            TMMFFileHandleParams params(&iFile);
            TPckg<TMMFFileHandleParams> paramPckg = params;
            error = iController.AddDataSink(KUidMmfFileSink, paramPckg, iSinkInfo);
        }

        // If an error occurred in any of the above, close the controller.
        if (error)
        {
            iEventMonitor->Cancel();
            iController.Close();
        }

        index++;
    }
    User::LeaveIfError(error);

    HBufC8* contentType8 = HBufC8::NewLC(iContentType->Length());
    contentType8->Des().Copy(*iContentType);

    TInt cInfoCount = iControllerInfos->Count();
    for (TInt i = 0; i < cInfoCount; i++)
    {
        CMMFControllerImplementationInformation* contrInfo = (*iControllerInfos)[ i ];
        const RMMFFormatImplInfoArray& recordFormats = contrInfo->RecordFormats();

        for (TInt formatIndex = 0; formatIndex < recordFormats.Count(); formatIndex++)
        {
            CMMFFormatImplementationInformation* formatInfo = recordFormats[ formatIndex ];

            if (formatInfo->SupportsMimeType(*contentType8))
            {
                iSupplier.Set(formatInfo->Supplier());
                // Some of the controllers those support only one format can return KErrNotSupported
                // error when setting the format, but still they can record that only supported format
                // Ignoring this error
                TInt err = iAudioControllerRecCustomCommands.SetSinkFormat(formatInfo->Uid());
                if ((err != KErrNotSupported))
                {
                    User::LeaveIfError(err);
                }
                break;
            }
        }
    }
    CleanupStack::PopAndDestroy(contentType8);

    // Do not set codec if it is default
    if (iSettings->iDataType != TFourCC(KFourCCNULL))
    {
        User::LeaveIfError(
            iAudioControllerRecCustomCommands.SetCodec(KFourCCNULL, iSettings->iDataType));
    }

    // set the audio data settings ie sample rate & channels, if not defaults
    if (iSettings->iRate != KDefaultRate)
    {
        User::LeaveIfError(
            iAudioControllerRecCustomCommands.SetSinkSampleRate(iSettings->iRate));
    }

    if (iSettings->iChannels != KDefaultChannels)
    {
        User::LeaveIfError(
            iAudioControllerRecCustomCommands.SetSinkNumChannels(iSettings->iChannels));
    }
}

void CMMAAudioRecorder::PrefetchL()
{
    if (iResetController)
    {
        DoOpenL();
    }
    User::LeaveIfError(iController.Prime());
    ChangeState(EPrefetched);
    PostActionCompleted(KErrNone);
}

void CMMAAudioRecorder::InitializeL(RFile* aFile,
                                    MMMFControllerEventMonitorObserver* aObserver)
{
    iFile.Duplicate(*aFile);
    iObserver = aObserver;
    DoOpenL();
}

void CMMAAudioRecorder::Deinitialize()
{
    DEBUG("CMMAAudioRecorder::Deinitialize++");
    if (iEventMonitor)
    {
        iEventMonitor->Cancel();
    }
    iController.Close();
    iFile.Close();
    DEBUG("CMMAAudioRecorder::Deinitialize--");
}

const TDesC& CMMAAudioRecorder::Type()
{
    return KMMAAudioRecorder;
}

void CMMAAudioRecorder::StartRecordL()
{
    if (iResetController)
    {
        iResetController = EFalse;
        DEBUG("CMMAAudioRecorder::StartRecordL reopen controller");
        DoOpenL();
        DEBUG("CMMAAudioRecorder::StartRecordL reopen done");
    }

    User::LeaveIfError(
        iAudioRecordControllerCustomCommands.SetMaxFileSize(iRecordSizeLimit));

    DEBUG_INT("CMMAAudioRecorder::StartRecordL Max File size set %d", iRecordSizeLimit);

    User::LeaveIfError(iController.Prime());
    User::LeaveIfError(iController.Play());
}


void CMMAAudioRecorder::StopRecordL()
{
    // ! here is the workaround for pause
    // Pause() doesn't work with all formats.
    DEBUG("CMMAAudioRecorder::StopRecordL Stopping");

    TInt pauseError = iController.Pause();
    // if EOF is already reached return without
    // doing any operation because controller is
    // already in Stopped state
    if (pauseError == KErrNotSupported)
    {
        DEBUG("CMMAAudioRecorder::StopRecordL Stopped instead pause");
        User::LeaveIfError(iController.Stop());
    }
    else
    {
        User::LeaveIfError(pauseError);
// wait only in HW
#ifndef __WINS__
        DEBUG("CMMAAudioRecorder::StopRecordL Stopped waiting");
        // wait for playback complete event
        if (!iWait->IsStarted())
        {
            iWait->Start();
        }
        User::LeaveIfError(iError);
#endif
    }


    // prime controller and set position back to end (autorewind in controller)
    User::LeaveIfError(iController.Prime());

    TTimeIntervalMicroSeconds position(0);
    User::LeaveIfError(iController.GetDuration(position));
    User::LeaveIfError(iController.SetPosition(position));
}

void CMMAAudioRecorder::GetDuration(TInt64* aDuration)
{
    // Return -1 in realized state in order to pass END_OF_MEDIA
    // tests in TCK. MMFPlayerbase would return 0.
    if (iState == ERealized)
    {
        *aDuration = KTimeUnknown;
    }
    else
    {
        CMMAMMFPlayerBase::GetDuration(aDuration);
    }
}

void CMMAAudioRecorder::DeallocateL()
{
    // set control to be reopened since
    // deallocate closes it
    if (iState == EPrefetched)
    {
        iResetController = ETrue;
    }
    CMMAMMFPlayerBase::DeallocateL();
}

void CMMAAudioRecorder::ResetL()
{
    iResetController = ETrue;
}

const TInt KMMASymbianControllerLimit = 4096 + 44; // buffer size + header
_LIT(KSymbian, "Symbian");

TInt CMMAAudioRecorder::SetRecordSizeLimitL(TInt aSize)
{
    iRecordSizeLimit = aSize;

    // Wav and AU controller (supplied from symbian) is recording in 4kb buffers
    // this controller does not record at all if size is smaller than 4kb and
    // goes even in wierd state.
    DEBUG_STR("CMMAAudioRecorder::SetRecordSizeLimitL Supplier = %S", iSupplier);
    if ((iSupplier == KSymbian) &&
            (aSize < KMMASymbianControllerLimit))
    {
        iRecordSizeLimit = KMMASymbianControllerLimit;
    }

    // normal case is that recordsize is set just before starting
    if (iState == EStarted)
    {
        DEBUG_INT("CMMAAudioRecorder::SetRecordSizeLimitL Setting while playing limit: %d", iRecordSizeLimit);
        // trying to set max file size while recording
        User::LeaveIfError(
            iAudioRecordControllerCustomCommands.SetMaxFileSize(iRecordSizeLimit));
    }
    return iRecordSizeLimit;
}

void CMMAAudioRecorder::HandleEvent(const TMMFEvent& aEvent)
{
    DEBUG_INT("CMMAAudioRecorder::HandleEvent event error: %d", aEvent.iErrorCode);

    if (iWait->IsStarted())
    {
        iError = aEvent.iErrorCode;
        iWait->AsyncStop();
        return;
    }
    TMMFEvent event = aEvent;
    if ((event.iEventType == KMMFEventCategoryPlaybackComplete) &&
            ((event.iErrorCode == KErrNone) ||
             (event.iErrorCode == KErrEof)))
    {
        // Prime controller after playback complete, in order to get position/duration
        event.iErrorCode = iController.Prime();
    }
    if (iObserver)
    {
        iObserver->HandleEvent(event);
    }
}

//  END OF FILE
