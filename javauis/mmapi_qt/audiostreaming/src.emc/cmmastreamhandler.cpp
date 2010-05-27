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
* Description:  Streams data from Java to controller
*
*/


//  INCLUDE FILES
#include <AudioPreference.h>
#include <logger.h>

#include "cmmastreamhandler.h"
#include "cmmadatasourcestream.h"
#include "mmafunctionserver.h"


CMMAStreamHandler* CMMAStreamHandler::NewL(
    MMMAStreamHandlerListener& aListener,
    MStreamControl& aMStreamControl,
    MDataBufferSource& aMDataBufferSource,
    CMultimediaFactory& aFactory,
    CMetaDataUtility& aMetaDataUtility)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::NewL +");
    CMMAStreamHandler* self = new(ELeave) CMMAStreamHandler(aListener,
            aMStreamControl,
            aMDataBufferSource,
            aFactory,
            aMetaDataUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::NewL -");
    return self;
}

CMMAStreamHandler::~CMMAStreamHandler()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::~CMMAStreamHandler() +");
    iMDataBufferSource.RemoveObserver(*this);
    if (iBuffer != NULL)
    {
        iFactory.DeleteDataBuffer(iBuffer);
        iBuffer = NULL;
    }
    iRequests.ResetAndDestroy();
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::~CMMAStreamHandler() -");
}

CMMAStreamHandler::CMMAStreamHandler(MMMAStreamHandlerListener& aListener,
                                     MStreamControl& aMStreamControl,
                                     MDataBufferSource& aMDataBufferSource,
                                     CMultimediaFactory& aFactory,
                                     CMetaDataUtility& aMetaDataUtility)
        :iMStreamControl(aMStreamControl),
        iMDataBufferSource(aMDataBufferSource),
        iFactory(aFactory),
        iListener(aListener),
        iMetaDataUtility(aMetaDataUtility),
        // to be removed once MDU supports all the reqd formats
        iMimeTypeSupportedByMDU(ETrue)
{
    // ignore read/write completed before stream is prepared
    iState = EMMAStreamPaused;
    iMetaDataReadyToBeParsed = EFalse;
}

void CMMAStreamHandler::ConstructL()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::ConstructL +");
    //Register source control to control observer to get the event
    iMDataBufferSource.AddObserver(*this);
    iLastBufferWritten = EFalse;
    iBuffer = NULL; // initialize MDataBuffer type pointer
    // create requests
    for (TInt i = 0; i < KMMAStreamHandlerBufferCount; i++)
    {
        CMMAStreamRequest* requestToAppend = CMMAStreamRequest::NewLC(this);
        iRequests.AppendL(requestToAppend);
        CleanupStack::Pop(requestToAppend);
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::ConstructL -");
}

void CMMAStreamHandler::Prepare(const TDesC8& aMimeType)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Prepare +");
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Prepare state %d", iState);
    iState = EMMAStreamPrepare;
    // initialize MetaDataUtility
    TInt error = iMetaDataUtility.InitChunkData(aMimeType, *this);
    ELOG1(EJavaMMAPI, "MMA::CMMAStreamHandler::Prepare, error = %d", error);
    if (error != KErrNone)
    {
        if (error == KErrArgument)
        {
            // MDU doesn't support aMimeType
            // can be removed once MDU supports all the reqd formats
            iMimeTypeSupportedByMDU = EFalse;
        }
        else
        {
            // MDU supports but some other error occured
            iListener.PrepareComplete(error);
            return;
        }
    }
    else
    {
        // to be removed once MDU supports all the reqd formats
        iMimeTypeSupportedByMDU = ETrue;
    }
    //reset request data for reading again from beginning
    iRequests[0]->DataPtr().SetLength(0);

    // when read completes iListerner.PrepareComplete will be called
    iSourceStream->Read(iRequests[ 0 ]);
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Prepare -");
}

void CMMAStreamHandler::Start()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Start +");
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Start state %d", iState);
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Start data source request=%d",(TInt)iSourceStream->Request());
    iState = EMMAStreamStart;
    if (iSourceStream->Request())
    {
        // when read request is completed it will be written to server
        iState = EMMAStreamStarted;
        iListener.StartComplete(KErrNone);
        return;
    }

    TInt count = iRequests.Count();
    TBool started = EFalse;
    for (TInt i = 0; i < count && !started; i++)
    {
        CMMAStreamRequest* r = iRequests[ i ];
        if (!r->IsActive() && r->DataPtr().Length() > 0)
        {
            LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Start write request to server");
            LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Start request length=%d",
                 r->DataPtr().Length());
            LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Start last buffer %d",
                 r->RequestBuffer()());

            // data was not yet written to server
            WriteRequest(r);
            started = ETrue;
        }
        else if (r->IsActive())  // data is in server
        {
            LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Start data is in server");
            // atleast one request is not processed
            started = ETrue;
        }
    }
    if (started)  // If allready started complete start
    {
        iListener.StartComplete(KErrNone);
        iState = EMMAStreamStarted;
    }
    else
    {
        // Need to read data before start
        iSourceStream->Read(iRequests[ 0 ]);
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Start -");
}

void CMMAStreamHandler::Pause()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Pause +");
    // ignore read/write completes
    iState = EMMAStreamPaused;
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Pause -");
}

void CMMAStreamHandler::SetSourceStream(CMMADataSourceStream* aSourceStream)
{
    iSourceStream = aSourceStream;
}

void CMMAStreamHandler::WriteComplete(CMMAStreamRequest* aRequest)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::WriteComplete +");
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::WriteComplete state=%d", iState);
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::WriteComplete request length=%d",
         aRequest->DataPtr().Length());
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::WriteComplete last buffer %d",
         aRequest->RequestBuffer()());
    if (iState == EMMAStreamStarted)
    {
        if (aRequest->RequestBuffer()() == 1)
        {
            iState = EMMAStreamEof;
        }
        else
        {
            iSourceStream->Read(aRequest);
        }
    }
    // else, all other states ignore write complete
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::WriteComplete -");
}

void CMMAStreamHandler::ReadComplete(CMMAStreamRequest* aRequest)
{

    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::ReadComplete +");

    if (iState == EMMAStreamPrepare)
    {

        WriteRequest(aRequest);
        iListener.PrepareComplete(KErrNone);
        iState = EMMAStreamPaused;
    }
    else if (iState == EMMAStreamStart)
    {
        iState = EMMAStreamStarted;
        // write first request to server
        WriteRequest(aRequest);
        iListener.StartComplete(KErrNone);
    }
    else if (iState == EMMAStreamStarted)
    {
        WriteRequest(aRequest);
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::ReadComplete -");
    // else, all other states ignore read complete
}

void CMMAStreamHandler::HandleError(CMMAStreamRequest* /*aRequest*/,
                                    TInt aError)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::HandleError +");
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::HandleError state=%d", iState);
    if (iState == EMMAStreamPrepare)
    {
        iListener.PrepareComplete(aError);
        iState = EMMAStreamPaused;
    }
    else if (iState == EMMAStreamStart)
    {
        iListener.StartComplete(aError);
        iState = EMMAStreamStarted;
    }
    else
    {
        iListener.HandleError(aError);
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::HandleError -");
}

void CMMAStreamHandler::WriteRequest(CMMAStreamRequest* aRequest)
{

    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::WriteRequest +");
    iCurrentRequest = aRequest; //initialize the current StreamRequest for use in callback function
    iProcessedState = EMMANoneProcessed;

    TInt state = iMStreamControl.GetState();
    if (!aRequest->IsActive())
    {
        aRequest->SetActive(ETrue);
    }

    //Create MDataBuffer and then pass it as a parameter of WriteData
    if (iBuffer != NULL)
    {
        iFactory.DeleteDataBuffer(iBuffer);
        iBuffer= NULL;
    }
    iFactory.CreateDataBuffer(KDataBufferSourceControl, KMMAStreamRequestBufferSize, iBuffer);
    iBuffer->GetBufferPtr().Set(aRequest->DataPtr());
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::WriteRequest: Size of the Data to be written is %d ",aRequest->DataPtr().Length());
    if (aRequest->RequestBuffer()() == 1)  //set that it is the last buffer
    {
        iState = EMMAStreamEof;
        iBuffer->SetLastBuffer(ETrue);
        iLastBufferWritten = ETrue;
        LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::WriteRequest: LastBuffer");
    }
    else
    {
        iBuffer->SetLastBuffer(EFalse);
        iLastBufferWritten = EFalse;
    }

    // When the buffer is processed by framework KBufferProcessedEvent
    // will be delivered to registered observers
    TInt err = iMDataBufferSource.WriteData(*iBuffer);
    // to be removed once MDU supports all the reqd formats
    if (iMimeTypeSupportedByMDU && !iMetaDataReadyToBeParsed)
    {
        err = iMetaDataUtility.ProcessChunkData(aRequest->DataPtr(), iLastBufferWritten);
    }
    ELOG1(EJavaMMAPI, "MMA::CMMAStreamHandler::WriteRequest, err = %d", err);
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::WriteRequest -");
}

void CMMAStreamHandler::Event(MControl* /*aControl*/, TUint aEventType, TAny* aEventObject)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Event ");
    switch (aEventType)
    {

    case MSourceControlObserver::KBufferProcessedEvent:
    {
        LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Event:KBufferProcessedEvent");
        MBufferProcessedEvent* evt = (MBufferProcessedEvent*)aEventObject;
        ELOG1(EJavaMMAPI, "MMA::CMMAStreamHandler::Event:KBufferProcessedEvent:ErrorCode = %d ",evt->GetErrorCode());
        // can be removed once MDU supports all the reqd formats
        if (!iMimeTypeSupportedByMDU ||
                (iProcessedState == EMMAMetaDataProcessed || iMetaDataReadyToBeParsed))
        {
            iCurrentRequest->WriteRequestComplete(evt->GetErrorCode());
            iCurrentRequest->SetActive(EFalse);
            iProcessedState = EMMABothProcessed;
        }
        else
        {
            iProcessedState = EMMABufferProcessed;
        }
    }
    break;

    case MSourceControlObserver::KBitRateChangedEvent:
    {
    }
    break;

    case MStreamControlObserver::KDurationChangedEvent:
    {
    }
    break;

    default:
        break;

    };
}

void CMMAStreamHandler::HandleChunkDataProcessed(TInt aError)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::HandleChunkDataProcessed + ");
    ELOG1(EJavaMMAPI, "MMA::CMMAStreamHandler::HandleChunkDataProcessed, aError = %d", aError);
    if (iProcessedState == EMMABufferProcessed)
    {
        iCurrentRequest->WriteRequestComplete(aError);
        iCurrentRequest->SetActive(EFalse);
        iProcessedState = EMMABothProcessed;
    }
    else
    {
        iProcessedState = EMMAMetaDataProcessed;
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::HandleChunkDataProcessed - ");
}

void CMMAStreamHandler::HandleChunkDataReadyToBeParsed()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::HandleChunkDataReadyToBeParsed + ");
    iMetaDataReadyToBeParsed = ETrue;
    TInt error = iMetaDataUtility.ParseChunkData();
    ELOG1(EJavaMMAPI, "MMA::CMMAStreamHandler::HandleChunkDataReadyToBeParsed, error = %d ", error);
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::HandleChunkDataReadyToBeParsed - ");
}

void CMMAStreamHandler::HandleChunkDataComplete(TInt aError)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::HandleChunkDataComplete");
    if (KErrNone != aError)
    {
        ELOG1(EJavaMMAPI, "MMA::CMMAStreamHandler::HandleChunkDataComplete, aError = %d ", aError);
        iListener.HandleError(aError);
    }

}

TBool CMMAStreamHandler::LastBufferWritten()
{
    return iLastBufferWritten;
}

void CMMAStreamHandler::Stop()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Stop + ");
    // forcefully complete all requests
    // and release already read data.
    Pause();
    iMetaDataUtility.CloseChunkData();
    TInt count = iRequests.Count();
    for (TInt i = 0; i < count; i++)
    {
        CMMAStreamRequest* r = iRequests[ i ];
        r->WriteRequestComplete(KErrNone);
        r->SetActive(EFalse);
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::Stop - ");
}


//  END OF FILE
