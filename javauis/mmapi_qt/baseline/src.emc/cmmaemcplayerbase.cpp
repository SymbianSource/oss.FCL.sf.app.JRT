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
* Description:  This class is used for playing sounds
*
*/


//  INCLUDE FILES
//#include <mmf/server/mmfdes.h>
#include <AudioPreference.h>
#include <logger.h>
#include "cmmaemcplayerbase.h"
#include "cmmaemcresolver.h"

using namespace multimedia;

using multimedia::MSourceControlObserver;
using multimedia::MStreamControlObserver;

CMMAEMCPlayerBase::~CMMAEMCPlayerBase()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::~CMMAEMCPlayerBase +");

    if (iMStreamControl)
    {
        iMStreamControl->RemoveObserver(*this);
        if (iMStreamControl->GetState() > 0)
        {
            LOG1(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::~CMMAEMCPlayerBase: iMStreamControl's state = %d",iMStreamControl->GetState());
            TInt err = iMStreamControl->Close();
            ELOG1(EJavaMMAPI, "MMA::CMMAEMCPlayerBase::~CMMAEMCPlayerBase: Close() err = %d",err);
        }
    }

    if (iFactory)
    {
        CleanupSource();

        MSinkControl* objPtr1 = iMAudioSink;
        iMStreamControl->RemoveSink(*objPtr1);

        iFactory->DeleteSinkControl(objPtr1);
        iMAudioSink = NULL;

        //before deleting Stream Control all the controls need to be deleted
        if (iControls.Count() > 0)
            iControls.ResetAndDestroy();

        iFactory->DeleteStreamControl(iMStreamControl);
        delete iFactory;
    }

    delete iFileName;

    delete iActiveSchedulerWait;

    delete iMimeType;

    delete iDescData;

    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::~CMMAEMCPlayerBase -");
}


CMMAEMCPlayerBase::CMMAEMCPlayerBase(
    CMMAEMCResolver* aResolver) :
        iMediaTime(KTimeUnknown), iStartedEventTime(0)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase +");
    // content type ownership is transferred
    iDescData = NULL;
    iContentType = aResolver->ContentTypeOwnership();
    iMimeType = aResolver->MimeTypeOwnership();   // 8-bit version of iContentType

    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::ContentType-- %S" ,iContentType->Des().PtrZ());

    // file name ownership is transferred
    iFileName = aResolver->FileNameOwnership();
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::FileName-- %S" ,iFileName->Des().PtrZ());
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase -");
}

void CMMAEMCPlayerBase::ConstructL()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::ConstructL +");
    CMMAPlayer::ConstructL();
    iSourceType = EDATABUFFERSOURCE;    //Currently it only support Http

    TInt status=CMultimediaFactory::CreateFactory(iFactory);
    CreateStreamL();
    AddDataSourceToStreamL();

    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::ConstructL -");
}

void CMMAEMCPlayerBase::CreateStreamL()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::CreateStreamL +");
    if (iMStreamControl)
        User::Leave(KErrAlreadyExists);
    TInt err = iFactory->CreateStreamControl(KStreamControl,iMStreamControl);
    User::LeaveIfError(err);
    //If the state of iMStreamControl changes,iPrevStreamControlState will keep its previous state
    iPrevStreamControlState = MStreamControl::CLOSED;
    iMStreamControl->AddObserver(*this);

    MSinkControl* tempSinkCtrl(NULL);
    err = iFactory->CreateSinkControl(KMMFAudioOutputSinkControl,tempSinkCtrl);
    User::LeaveIfError(err);
    iMAudioSink = tempSinkCtrl;
    iMStreamControl->AddSink(*iMAudioSink);
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::CreateStreamL -");
}

void CMMAEMCPlayerBase::AddDataSourceToStreamL()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::AddDataSourceToStreamL +");
    if (iMStreamControl == NULL)
    {
        User::Leave(KErrNotReady);
    }

    TInt err(KErrNone);
    MSourceControl* tempCtrl(NULL);

    switch (iSourceType)
    {
    case EFILESOURCE:
    {
        err = iFactory->CreateSourceControl(KFileSourceControl, tempCtrl);
        User::LeaveIfError(err);
        iMFileSource = static_cast<MFileSource*>(tempCtrl);
        iMimeType->Des().FillZ();
        User::LeaveIfError(iMFileSource->Open(*iFileName, *iMimeType));
        iMFileSource->AddObserver(*this);
        break;
    }
    case EDATABUFFERSOURCE:
    {
        err = iFactory->CreateSourceControl(KDataBufferSourceControl, tempCtrl);
        User::LeaveIfError(err);

        iMDataBufferSource = static_cast<MDataBufferSource*>(tempCtrl);
        MDataBuffer* headerData = NULL;
        User::LeaveIfError(iMDataBufferSource->Open(*iMimeType, *headerData));
        iMDataBufferSource->AddObserver(*this);
        break;
    }
    case EDESCRIPTORSOURCE:
    {
        err = iFactory->CreateSourceControl(KDescriptorSourceControl, tempCtrl);
        User::LeaveIfError(err);
        iMDescriptorSource = static_cast<MDescriptorSource*>(tempCtrl);
        TInt size = 0;
        // iFile.Size(size);   will be used when Descriptor support will be implemented

        if (iDescData)
        {
            delete iDescData;
            iDescData = NULL;
        }
        iDescData = HBufC8::NewL(size);
        TPtr8 des = iDescData->Des();
        //iFile.Read(des);
        User::LeaveIfError(iMDescriptorSource->Open(*iMimeType,*iDescData));
        iMDescriptorSource->AddObserver(*this);
        break;
    }
    default:
        break;
    }

    iMStreamControl->AddSource(*tempCtrl);
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::AddDataSourceToStreamL -");
}

void CMMAEMCPlayerBase::CleanupSource()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::CleanupSource +");
    if (iMDataBufferSource)
    {
        iMDataBufferSource->RemoveObserver(*this);
        MSourceControl* objPtr = iMDataBufferSource;
        iMStreamControl->RemoveSource(*objPtr);    //added
        objPtr->Close();
        iFactory->DeleteSourceControl(objPtr);
        iMDataBufferSource = NULL;
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::CleanupSource -");
}

EXPORT_C MStreamControl* CMMAEMCPlayerBase::StreamControl()
{
    return iMStreamControl;
}

EXPORT_C CMultimediaFactory* CMMAEMCPlayerBase::MMFactory()
{
    return iFactory;
}

TBool CMMAEMCPlayerBase::IsFilePlayer()
{
    return EFalse;
}

void CMMAEMCPlayerBase::StartL()
{
    // empty implementation
    // should be overwritten in derived class
}

void CMMAEMCPlayerBase::StopL(TBool /*aPostEvent*/)
{
    // empty implementation
    // should be overwritten in derived class
}

void CMMAEMCPlayerBase::DeallocateL()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::DeallocateL +");
    if (iState == EPrefetched)
    {
        // Change state first to enable AMMS to delete Effect API classes
        ChangeState(ERealized);

        TInt err = iMStreamControl->Stop();
        ELOG1(EJavaMMAPI, "CMMAEMCPlayerBase::DeallocateL iMStreamControl->Stop = %d", err);
        ResetSourceStreams();
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::DeallocateL -");
}


void CMMAEMCPlayerBase::GetDuration(TInt64* aDuration)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::GetDuration +");
    if (iDuration == KTimeUnknown)
    {
        TInt64 duration(0);
        TInt err = iMStreamControl->GetDuration(duration);
        if (!err)
        {
            iDuration = duration;
        }
    }
    *aDuration = iDuration;
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAEMCPlayerBase::GetDuration -");
}

void CMMAEMCPlayerBase::SetMediaTimeL(TInt64* aTime)
{
    LOG(EJavaMMAPI, EInfo, "CMMAEMCPlayerBase::SetMediaTimeL +");

    // Negative values are not checked here
    // because it's done already in Java side.

    // Get clip duration
    TInt64 duration;
    User::LeaveIfError(iMStreamControl->GetDuration(duration));
    LOG1(EJavaMMAPI, EInfo, "CMMAEMCPlayerBase::SetMediaTimeL iMStreamControl->GetDuration=%d", duration);

    TInt64 position;

    // If the desired media time is beyond the duration,
    // the time is set to the end of the media.
    if (*aTime > duration)
    {
        position = duration;
    }
    else
    {
        position = *aTime;
    }

    // The controller must be in the PRIMED or PLAYING state
    User::LeaveIfError(iMStreamControl->SetPosition(position));

    // Reset cached media time, because actual set position may be
    // something else than aTime.
    iMediaTime = KTimeUnknown;

    // Inform about the position change to the StateListeners
    ChangeState(iState);

    // Get the actual media time
    GetMediaTime(aTime);

    iStartedEventTime = iMediaTime;
    LOG(EJavaMMAPI, EInfo, "CMMAEMCPlayerBase::SetMediaTimeL -");
}

void CMMAEMCPlayerBase::GetMediaTime(TInt64* aMediaTime)
{
    LOG(EJavaMMAPI, EInfo, "CMMAEMCPlayerBase::GetMediaTimeL +");
    TInt64 position(0);

    if (iMediaTime == KTimeUnknown || iState == EStarted)
    {

        TInt error(iMStreamControl->GetPosition(position));

        if (error == KErrNone)
        {
            TInt64 newTime = position;

            // Sanity check for media time going backwards or beyond the
            // duration.
            // Some native controls may return zero media time for
            // a few moments just before playback will complete.
            if (newTime < iMediaTime ||
                    (iDuration > 0 && newTime > iDuration))
            {
                GetDuration(&iMediaTime);
            }
            else
            {
                // set return value
                iMediaTime = newTime;
            }
        }
        else
        {
            ELOG1(EJavaMMAPI, "CMMAEMCPlayerBase::GetMediaTimeL: error=%d, returning TIME_UNKNOWN", error);
            // cannot get media time
            iMediaTime = KTimeUnknown;
        }
    }
    *aMediaTime = iMediaTime;
    LOG(EJavaMMAPI, EInfo, "CMMAEMCPlayerBase::GetMediaTimeL -");
}

void CMMAEMCPlayerBase::CloseL()
{
    LOG(EJavaMMAPI, EInfo, "CMMAEMCPlayerBase::CloseL +");
    CMMAPlayer::CloseL();

    iMStreamControl->Close();
    LOG(EJavaMMAPI, EInfo, "CMMAEMCPlayerBase::CloseL -");
}

void CMMAEMCPlayerBase::Event(MControl* /*aControl*/, TUint /*aEventType*/, TAny* /*aEventObject*/)
{
    //Not Called
}

//  END OF FILE
