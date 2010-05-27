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
#include <mmf/server/mmfdes.h>
#include <audiopreference.h>
#include <logger.h>

#include "CMMAStreamHandler.h"
#include "CMMADataSourceStream.h"
#include "MMAFunctionServer.h"

CMMAStreamHandler* CMMAStreamHandler::NewL(
    MMMAStreamHandlerListener& aListener,
    RMMFController& aController)
{
    CMMAStreamHandler* self = new(ELeave) CMMAStreamHandler(aListener,
            aController);
    self->ConstructL();
    return self;
}

CMMAStreamHandler::~CMMAStreamHandler()
{
    iRequests.ResetAndDestroy();
}

CMMAStreamHandler::CMMAStreamHandler(MMMAStreamHandlerListener& aListener,
                                     RMMFController& aController):
        iController(aController),
        iListener(aListener)
{
    // ignore read/write completed before stream is prepared
    iState = EMMAStreamPaused;
}

void CMMAStreamHandler::ConstructL()
{
    // create requests
    for (TInt i = 0; i < KMMAStreamHandlerBufferCount; i++)
    {
        CMMAStreamRequest* requestToAppend = CMMAStreamRequest::NewLC(this);
        iRequests.AppendL(requestToAppend);
        CleanupStack::Pop(requestToAppend);
    }
}

void CMMAStreamHandler::PrepareL()
{
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::PrepareL state %d", iState);
    iState = EMMAStreamPrepare;

    //reset request data for reading again from beginning
    iRequests[0]->DataPtr().SetLength(0);

    // when read completes iListerner.PrepareComplete will be called
    iSourceStream->Read(iRequests[ 0 ]);
}

void CMMAStreamHandler::StartL()
{
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::StartL state %d", iState);
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::StartL data source request=%d",
         (TInt)iSourceStream->Request());

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
            LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::StartL write request to server");
            LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::StartL request length=%d",
                 r->DataPtr().Length());
            LOG1(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::StartL last buffer %d",
                 r->RequestBuffer()());

            // data was not yet written to server
            WriteRequest(r);
            started = ETrue;
        }
        else if (r->IsActive())  // data is in server
        {
            LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamHandler::StartL data is in server");
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
}

void CMMAStreamHandler::Pause()
{
    // ignore read/write completes
    iState = EMMAStreamPaused;
}

TMMFMessageDestination& CMMAStreamHandler::MessageDestination()
{
    return iDataSourceHandle;
}

void CMMAStreamHandler::SetSourceStream(CMMADataSourceStream* aSourceStream)
{
    iSourceStream = aSourceStream;
}

void CMMAStreamHandler::WriteComplete(CMMAStreamRequest* aRequest)
{
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
}

void CMMAStreamHandler::ReadComplete(CMMAStreamRequest* aRequest)
{
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
    // else, all other states ignore read complete
}

void CMMAStreamHandler::HandleError(CMMAStreamRequest* /*aRequest*/,
                                    TInt aError)
{
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
}

void CMMAStreamHandler::WriteRequest(CMMAStreamRequest* aRequest)
{
    if (aRequest->RequestBuffer()() == 1)
    {
        iState = EMMAStreamEof;
    }

    if (!aRequest->IsActive())
    {
        aRequest->SetActive();
    }

    // Send write request to server, HandleRequestCompleteL is called when finished
    iController.CustomCommandAsync(iDataSourceHandle,
                                   1, /*EProcessBuffer*/
                                   aRequest->DataPtr(),
                                   aRequest->RequestBuffer(),
                                   aRequest->iStatus);
}
//  END OF FILE
