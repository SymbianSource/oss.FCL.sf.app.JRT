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
#include <jdebug.h>

#include "cmmastreamhandler.h"
#include "cmmadatasourcestream.h"
#include "cmmaeventsource.h"


CMMAStreamHandler* CMMAStreamHandler::NewL(
    MMMAStreamHandlerListener& aListener,
    MStreamControl& aMStreamControl,
    MDataBufferSource& aMDataBufferSource,
    CMultimediaFactory& aFactory,
    CMetaDataUtility& aMetaDataUtility)
{
    DEBUG("MMA::CMMAStreamHandler::NewL +");
    CMMAStreamHandler* self = new(ELeave) CMMAStreamHandler(aListener,
            aMStreamControl,
            aMDataBufferSource,
            aFactory,
            aMetaDataUtility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    DEBUG("MMA::CMMAStreamHandler::NewL -");
    return self;
}

CMMAStreamHandler::~CMMAStreamHandler()
{
    DEBUG("MMA::CMMAStreamHandler::~CMMAStreamHandler() +");
    iMDataBufferSource.RemoveObserver(*this);
    if (iBuffer != NULL)
    {
        iFactory.DeleteDataBuffer(iBuffer);
        iBuffer = NULL;
    }
    iRequests.ResetAndDestroy();
    DEBUG("MMA::CMMAStreamHandler::~CMMAStreamHandler() -");
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
    DEBUG("MMA::CMMAStreamHandler::ConstructL +");
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
    DEBUG("MMA::CMMAStreamHandler::ConstructL -");
}

void CMMAStreamHandler::Prepare(const TDesC8& aMimeType)
{
    DEBUG("MMA::CMMAStreamHandler::Prepare +");
    DEBUG_INT("MMA::CMMAStreamHandler::Prepare state %d", iState);
    iState = EMMAStreamPrepare;
    // initialize MetaDataUtility
    TInt error = iMetaDataUtility.InitChunkData(aMimeType, *this);
    DEBUG_INT("MMA::CMMAStreamHandler::Prepare, error = %d", error);
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
    DEBUG("MMA::CMMAStreamHandler::Prepare -");
}

void CMMAStreamHandler::Start()
{
    DEBUG("MMA::CMMAStreamHandler::Start +");
    DEBUG_INT("MMA::CMMAStreamHandler::Start state %d", iState);
    DEBUG_INT("MMA::CMMAStreamHandler::Start data source request=%d",(TInt)iSourceStream->Request());
    iState = EMMAStreamStart;
    if (iSourceStream->Request())
    { // when read request is completed it will be written to server
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
            DEBUG("MMA::CMMAStreamHandler::Start write request to server");
            DEBUG_INT("MMA::CMMAStreamHandler::Start request length=%d",
                      r->DataPtr().Length());
            DEBUG_INT("MMA::CMMAStreamHandler::Start last buffer %d",
                      r->RequestBuffer()());

            // data was not yet written to server
            WriteRequest(r);
            started = ETrue;
        }
        else if (r->IsActive())  // data is in server
        {
            DEBUG("MMA::CMMAStreamHandler::Start data is in server");
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
    DEBUG("MMA::CMMAStreamHandler::Start -");
}

void CMMAStreamHandler::Pause()
{
    DEBUG("MMA::CMMAStreamHandler::Pause +");
    // ignore read/write completes
    iState = EMMAStreamPaused;
    DEBUG("MMA::CMMAStreamHandler::Pause -");
}

void CMMAStreamHandler::SetSourceStream(CMMADataSourceStream* aSourceStream)
{
    iSourceStream = aSourceStream;
}

void CMMAStreamHandler::WriteComplete(CMMAStreamRequest* aRequest)
{
    DEBUG("MMA::CMMAStreamHandler::WriteComplete +");
    DEBUG_INT("MMA::CMMAStreamHandler::WriteComplete state=%d", iState);
    DEBUG_INT("MMA::CMMAStreamHandler::WriteComplete request length=%d",
              aRequest->DataPtr().Length());
    DEBUG_INT("MMA::CMMAStreamHandler::WriteComplete last buffer %d",
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
    DEBUG("MMA::CMMAStreamHandler::WriteComplete -");
}

void CMMAStreamHandler::ReadComplete(CMMAStreamRequest* aRequest)
{

    DEBUG("MMA::CMMAStreamHandler::ReadComplete +");

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
    DEBUG("MMA::CMMAStreamHandler::ReadComplete -");
    // else, all other states ignore read complete
}

void CMMAStreamHandler::HandleError(CMMAStreamRequest* /*aRequest*/,
                                    TInt aError)
{
    DEBUG("MMA::CMMAStreamHandler::HandleError +");
    DEBUG_INT("MMA::CMMAStreamHandler::HandleError state=%d", iState);
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
    DEBUG("MMA::CMMAStreamHandler::HandleError -");
}

void CMMAStreamHandler::WriteRequest(CMMAStreamRequest* aRequest)
{

    DEBUG("MMA::CMMAStreamHandler::WriteRequest +");
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
    DEBUG_INT("MMA::CMMAStreamHandler::WriteRequest: Size of the Data to be written is %d ",aRequest->DataPtr().Length());
    if (aRequest->RequestBuffer()() == 1)  //set that it is the last buffer
    {
        iState = EMMAStreamEof;
        iBuffer->SetLastBuffer(ETrue);
        iLastBufferWritten = ETrue;
        DEBUG("MMA::CMMAStreamHandler::WriteRequest: LastBuffer");
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
    DEBUG_INT("MMA::CMMAStreamHandler::WriteRequest, err = %d", err);
    DEBUG("MMA::CMMAStreamHandler::WriteRequest -");
}

void CMMAStreamHandler::Event(MControl* /*aControl*/, TUint aEventType, TAny* aEventObject)
{
    DEBUG("MMA::CMMAStreamHandler::Event ");
    switch (aEventType)
    {

    case MSourceControlObserver::KBufferProcessedEvent:
    {
        DEBUG("MMA::CMMAStreamHandler::Event:KBufferProcessedEvent");
        MBufferProcessedEvent* evt = (MBufferProcessedEvent*)aEventObject;
        DEBUG_INT("MMA::CMMAStreamHandler::Event:KBufferProcessedEvent:ErrorCode = %d ",evt->GetErrorCode());
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
    DEBUG("MMA::CMMAStreamHandler::HandleChunkDataProcessed + ");
    DEBUG_INT("MMA::CMMAStreamHandler::HandleChunkDataProcessed, aError = %d", aError);
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
    DEBUG("MMA::CMMAStreamHandler::HandleChunkDataProcessed - ");
}

void CMMAStreamHandler::HandleChunkDataReadyToBeParsed()
{
    DEBUG("MMA::CMMAStreamHandler::HandleChunkDataReadyToBeParsed + ");
    iMetaDataReadyToBeParsed = ETrue;
    TInt error = iMetaDataUtility.ParseChunkData();
    DEBUG_INT("MMA::CMMAStreamHandler::HandleChunkDataReadyToBeParsed, error = %d ", error);
    DEBUG("MMA::CMMAStreamHandler::HandleChunkDataReadyToBeParsed - ");
}

void CMMAStreamHandler::HandleChunkDataComplete(TInt aError)
{
    DEBUG("MMA::CMMAStreamHandler::HandleChunkDataComplete");
    if (KErrNone != aError)
    {
        DEBUG_INT("MMA::CMMAStreamHandler::HandleChunkDataComplete, aError = %d ", aError);
        iListener.HandleError(aError);
    }

}

TBool CMMAStreamHandler::LastBufferWritten()
{
    return iLastBufferWritten;
}

void CMMAStreamHandler::Stop()
{
    DEBUG("MMA::CMMAStreamHandler::Stop + ");
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
    DEBUG("MMA::CMMAStreamHandler::Stop - ");
}


//  END OF FILE
