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
* Description:  Class for reading data from Java SourceStream to native side
*
*/

#include <jdebug.h>
#include "cmmaeventsource.h"
#include "cmmadatasourcestream.h"
#include "mmmasourcestreamlistener.h"
#include "cmmastreamrequest.h"
#include "cmmasourcestreamevent.h"


// CONSTRUCTION
CMMADataSourceStream* CMMADataSourceStream::NewLC(JNIEnv* aJNIEnv,
        MMMAEventPoster* aEventPoster,
        jobject aJavaSourceStream,
        MMMASourceStreamListener* aListener
                                                 )
{
    CMMADataSourceStream* self = new(ELeave) CMMADataSourceStream(aEventPoster,
            aListener);
    CleanupStack::PushL(self);
    self->ConstructL(aJNIEnv, aJavaSourceStream);
    return self;
}


CMMADataSourceStream::~CMMADataSourceStream()
{
    DEBUG("MMA::CMMADataSourceStream::~");
}


// Default constructor
CMMADataSourceStream::CMMADataSourceStream(MMMAEventPoster* aEventPoster,
        MMMASourceStreamListener* aListener):
        CMMASourceStream(aEventPoster, aListener)
{
}


inline void CMMADataSourceStream::ConstructL(JNIEnv* aJNIEnv,
        jobject aJavaSourceStream)
{
    CMMASourceStream::ConstructL(aJNIEnv, aJavaSourceStream);
    CreateDataBufferL(KMMAStreamRequestBufferSize);
}

void CMMADataSourceStream::WriteL(const TUint8* aData,
                                  TInt aLength,
                                  TInt aState)
{
    DEBUG_INT("MMA::CMMADataSourceStream::WriteL data length %d", aLength);
    DEBUG_INT("MMA::CMMADataSourceStream::WriteL state %d", aState);

    if (!iRequest)
    {
        // there is no request to write
        return;
    }

    if (aState >= KErrNone)
    {
        if ((aState == CMMASourceStream::ECompleted) &&
                aLength == -1)
        {
            // Stream ended
            iRequest->RequestBuffer() = 1;
        }
        else if ((iData->Length() == 0) &&
                 (iRequest->DataPtr().Length() == 0))
        {
            iRequest->DataPtr().Append(aData, aLength);
            // read next
            iEventPoster->PostEvent(iReadEvent,
                                    CMMAEvent::ENotifyPriority);
            return;
        }
        else if (iData->Length() == 0)
        {
            iData->Des().Append(aData, aLength);
        }
    }

    // complete read operation
    CMMAStreamRequest* r = iRequest;
    iRequest = NULL;
    r->CompleteRead(aState);
    DEBUG("MMA::CMMADataSourceStream::WriteL completed");
}

void CMMADataSourceStream::ResetData()
{
    DEBUG("MMA::CMMADataSourceStream::ResetData");
}

CMMAStreamRequest* CMMADataSourceStream::Request()
{
    return iRequest;
}

void CMMADataSourceStream::Read(CMMAStreamRequest* aRequest)
{
    // Setting NULL, means that all read completes are ignored
    if (!aRequest)
    {
        iRequest = NULL;
        return;
    }

    if (!iRequest)
    {
        iRequest = aRequest;
        iRequest->RequestBuffer() = 0;
        iReadEvent->SetLength(aRequest->DataPtr().MaxLength());

        if (iData->Length() > 0)
        {
            // Previous read buffered data
            iRequest->DataPtr().Append(*iData);

            // re-use buffer
            iData->Des().SetLength(0);
        }

        // data has been requested, note will be sent
        iEventPoster->PostEvent(iReadEvent, CMMAEvent::ENotifyPriority);
    }
    // else java is already reading
}

//  END OF FILE
