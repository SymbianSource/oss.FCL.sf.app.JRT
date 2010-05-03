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
* Description:  This class is used for playing video.
*
*/


//  INCLUDE FILES
#include <mmf/server/mmfurl.h>
#include <es_enum.h>
#include <jdebug.h>

#include "cmmavideourlplayer.h"
#include "mmmadisplay.h"
#include "cmmasourcestream.h"

const TUid KUrlSourceUid = { KMmfUidUrlSource };
_LIT(KMMALiveStreamMetaDataKeyword, "LiveStream");
_LIT(KMMALiveStreamMetaDataValue, "1");

CMMAVideoUrlPlayer* CMMAVideoUrlPlayer::NewLC(
    CMMAMMFResolver* aResolver,
    const TDesC& aUrl)
{
    CMMAVideoUrlPlayer* self = new(ELeave) CMMAVideoUrlPlayer(
        aResolver);
    CleanupStack::PushL(self);
    self->ConstructL(aUrl);
    return self;
}

EXPORT_C CMMAVideoUrlPlayer::~CMMAVideoUrlPlayer()
{
    iConnection.Close();
    iSocketServ.Close();
    delete iUrl;
    delete iPlayerDelegate;
}

EXPORT_C void CMMAVideoUrlPlayer::ConstructL(const TDesC& aUrl)
{
    CMMAVideoPlayer::ConstructL();
    iUrl = aUrl.AllocL();

    User::LeaveIfError(iSocketServ.Connect());
    User::LeaveIfError(iConnection.Open(iSocketServ));
}

EXPORT_C CMMAVideoUrlPlayer::CMMAVideoUrlPlayer(
    CMMAMMFResolver* aResolver):
        CMMAVideoPlayer(aResolver)
{
}

EXPORT_C void CMMAVideoUrlPlayer::DeallocateL()
{
    __ASSERT_DEBUG(iPlayerDelegate != NULL, User::Invariant());
    delete iPlayerDelegate;
    iPlayerDelegate = NULL;
    CMMAMMFPlayerBase::DeallocateL();
}

EXPORT_C void CMMAVideoUrlPlayer::CloseL()
{
    // Do not call the CMMAVideoPlayer CloseL because it deletes
    // file source stream.
    CMMAAudioPlayer::CloseL();
}

EXPORT_C void CMMAVideoUrlPlayer::StartL()
{
    __ASSERT_DEBUG(iPlayerDelegate != NULL, User::Invariant());
    DEBUG("CMMAVideoUrlPlayer::StartL() +");
    iPlayerDelegate->StartL();
    DEBUG("CMMAVideoUrlPlayer::StartL() -");
}

EXPORT_C void CMMAVideoUrlPlayer::StopL(TBool aPostEvent)
{
    DEBUG("MMA::CMMAVideoUrlPlayer::StopL ");
    __ASSERT_DEBUG(iPlayerDelegate != NULL, User::Invariant());
    iPlayerDelegate->StopL(aPostEvent);
}

EXPORT_C void CMMAVideoUrlPlayer::GetMediaTime(TInt64* aMediaTime)
{
    DEBUG("MMA::CMMAVideoUrlPlayer::GetMediaTime +");
    if (iPlayerDelegate)
    {
        DEBUG("MMA::CMMAVideoUrlPlayer::GetMediaTime .iPlayerDelegate->GetMediaTime ");
        iPlayerDelegate->GetMediaTime(aMediaTime);
    }
    else
    {
        DEBUG("MMA::CMMAVideoUrlPlayer::GetMediaTime Not found ");
        *aMediaTime = KErrNotFound;
    }
    DEBUG("MMA::CMMAVideoUrlPlayer::GetMediaTime -");
}

EXPORT_C void CMMAVideoUrlPlayer::RealizeL()
{
    CMMAPlayer::RealizeL(); // Changes state to ERealized
}


EXPORT_C void CMMAVideoUrlPlayer::PrefetchL()
{
    const TUint KConnectionTreshold(1);
    TUint connectIap((TUint)KUseDefaultIap);
    TUint connectionCount(0);
    User::LeaveIfError(iConnection.EnumerateConnections(connectionCount));
    DEBUG_INT("MMA::CMMAVideoUrlPlayer::PrefetchL: connectionCount = %d", connectionCount);
    if (connectionCount == KConnectionTreshold)
    {
        // One active connection - find it and try using it
        FindActiveIap(connectionCount, connectIap);
    }
    // else No active connections try using the default one
    DEBUG_INT("MMA::CMMAVideoUrlPlayer::PrefetchL: connectIap = %d", connectIap);

    CMMFUrlParams* urlCfg = CMMFUrlParams::NewLC(*iUrl, (TInt)connectIap);
    CBufFlat* urlCfgBuffer = urlCfg->ExternalizeToCBufFlatLC();

    // Store current thread priority to give
    // more CPU time to Java threads i.e. this
    // is work around for not locking the Java
    // thread while RTSP connection is being
    // opened
    iOrigPriority = RThread().Priority();

    // player priority settings
    TMMFPrioritySettings prioritySettings;
    prioritySettings.iPriority = EMdaPriorityMax;
    prioritySettings.iPref = EMdaPriorityPreferenceTimeAndQuality;
    prioritySettings.iState = EMMFStatePlaying;

    User::LeaveIfError(DoOpen(KUrlSourceUid,
                              urlCfgBuffer->Ptr(0),
                              KUidMmfAudioOutput,
                              KNullDesC8,
                              prioritySettings));

    User::LeaveIfError(iController.Prime());

    CleanupStack::PopAndDestroy(urlCfgBuffer);
    CleanupStack::PopAndDestroy(urlCfg);

    // Prefetch will be completed in HandleEvent method.
    // Next event KMMFEventCategoryVideoOpenComplete will be handled in
    // CMMAVideoPlayer.
}

void CMMAVideoUrlPlayer::FindActiveIap(const TUint aConnectionCount, TUint& aActiveIap)
{
    DEBUG_INT("MMA:CMMAVideoUrlPlayer::FindActiveIap: aConnectionCount = %d +", aConnectionCount);

    TPckgBuf<TConnectionInfo> connectionInfo;
    for (TUint i = 1; i <= aConnectionCount; ++i)
    {
        if (iConnection.GetConnectionInfo(i, connectionInfo) == KErrNone)
        {
            aActiveIap = connectionInfo().iIapId;
            break;
        }
    }

    DEBUG_INT("MMA:CMMAVideoUrlPlayer::FindActiveIap: aActiveIap = %d -", aActiveIap);
}

TBool CMMAVideoUrlPlayer::IsLiveStreamL()
{
    DEBUG("CMMAVideoUrlPlayer::IsLiveStream: Checking if this is a live stream..");

    CMMFMetaDataEntry* currEntry = NULL;

    TInt nEntries = 0;
    User::LeaveIfError(iController.GetNumberOfMetaDataEntries(nEntries));

    for (TInt i = 0; i < nEntries; ++i)
    {
        currEntry = iController.GetMetaDataEntryL(i);

        if ((0 == currEntry->Name().Compare(KMMALiveStreamMetaDataKeyword)) &&
                (0 == currEntry->Value().Compare(KMMALiveStreamMetaDataValue)))
        {
            DEBUG("CMMAVideoUrlPlayer::IsLiveStream: Stream is a live stream");
            delete currEntry;
            return ETrue;
        }

        delete currEntry;
    }

    DEBUG("CMMAVideoUrlPlayer::IsLiveStream: Not a live stream");
    return EFalse;
}

EXPORT_C void CMMAVideoUrlPlayer::HandleEvent(const TMMFEvent& aEvent)
{
    DEBUG_INT("MMA:CMMAVideoUrlPlayer::HandleEvent 0x%X", aEvent.iEventType.iUid);
    DEBUG_INT("MMA:CMMAVideoUrlPlayer::HandleEvent error code: %d", aEvent.iErrorCode);
    RThread currentThread;

    if (iPlayerDelegate != NULL)
    {
        DEBUG("MMA::CMMAVideoUrlPlayer::HandleEvent: iPlayerDelegate != NULL");
        iPlayerDelegate->HandleEvent(aEvent);
    }
    else
    {
        DEBUG("MMA::CMMAVideoUrlPlayer::HandleEvent: else");

        __ASSERT_DEBUG(iPlayerDelegate == NULL, User::Invariant());
        TInt err = aEvent.iErrorCode;

        DEBUG_INT("MMA:CMMAVideoUrlPlayer::HandleEvent: currentThread.Priority() I = %d", currentThread.Priority());

        // Lower thread priority to give more CPU time to Java-threads
        // i.e. current thread has one increment higher priority than
        // EPriorityNormal
        if (aEvent.iEventType == KMMFEventCategoryVideoOpenComplete &&
                err == KErrNone)
        {
            currentThread.SetPriority(EPriorityNormal);
        }

        DEBUG_INT("MMA:CMMAVideoUrlPlayer::HandleEvent: currentThread.Priority() II = %d", currentThread.Priority());

        if (aEvent.iEventType == KMMFEventCategoryVideoPrepareComplete)
        {
            // Restore original thread priority
            currentThread.SetPriority(iOrigPriority);
            DEBUG_INT("MMA:CMMAVideoUrlPlayer::HandleEvent: currentThread.Priority() III = %d", currentThread.Priority());

            // NotCompleteVideoError is not considered as an error condition, instead it indicates
            // that some elements of the media cannot be played (e.g. video OR audio)
            if (err != KErrNone && err != KNotCompleteVideoError)
            {
                CompletePrefetch(err);
                return;
            }

            // Find out of this is a live stream
            TBool liveStream = EFalse;
            TRAP(err, liveStream = IsLiveStreamL());

            if (err != KErrNone)
            {
                CompletePrefetch(err);
                return;
            }

            if (liveStream)
            {
                TRAP(err, iPlayerDelegate =
                         CMMAVideoUrlPlayerLiveStreamDelegate::NewL(*this));
            }
            else
            {
                iPlayerDelegate = new CMMAVideoUrlPlayerClipStreamDelegate(*this);
                err = iPlayerDelegate ? KErrNone : KErrNoMemory;
            }

            if (err != KErrNone)
            {
                // could not create delegate
                CompletePrefetch(err);
                return;
            }

            iPlayerDelegate->HandleEvent(aEvent);
        }
        else
        {
            // All other events.
            DEBUG("MMA::CMMAVideoUrlPlayer::HandleEvent: Calling CMMAVideoPlayer::HandleEvent()");
            CMMAVideoPlayer::HandleEvent(aEvent);
        }
    }
}

void CMMAVideoUrlPlayer::HandleEventToParent(const TMMFEvent& aEvent)
{
    CMMAVideoPlayer::HandleEvent(aEvent);
}

// ----------------------------------------------------------------------------
//
// CMMAVideoUrlPlayerClipStreamDelegate
//
// ----------------------------------------------------------------------------

CMMAVideoUrlPlayer::CMMAVideoUrlPlayerDelegate::
CMMAVideoUrlPlayerDelegate(CMMAVideoUrlPlayer& aPlayer) :
        iPlayer(aPlayer)
{
    // Nothing to be done.
}

CMMAVideoUrlPlayer::CMMAVideoUrlPlayerDelegate::~CMMAVideoUrlPlayerDelegate()
{
    // Nothing to be done.
}

//  END OF FILE
