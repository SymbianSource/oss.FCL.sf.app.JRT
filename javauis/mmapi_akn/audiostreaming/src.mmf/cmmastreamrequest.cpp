/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Request used to write or read data
*
*/


//  INCLUDE FILES
#include <jdebug.h>
#include "CMMAStreamRequest.h"
#include "MMMAStreamRequestListener.h"

CMMAStreamRequest* CMMAStreamRequest::NewLC(MMMAStreamRequestListener* aListener)
{
    CMMAStreamRequest* self = new(ELeave)CMMAStreamRequest(aListener);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CMMAStreamRequest::~CMMAStreamRequest()
{
    Cancel();
    delete iData;
}

TPtr8& CMMAStreamRequest::DataPtr()
{
    return iDataPtr;
}

void CMMAStreamRequest::CompleteRead(TInt aError)
{
    if (aError < KErrNone)
    {
        iListener->HandleError(this, aError);
    }
    else // OK
    {
        iListener->ReadComplete(this);
    }
}

void CMMAStreamRequest::SetActive()
{
    CActive::SetActive();
}

TPckgBuf< TInt >& CMMAStreamRequest::RequestBuffer()
{
    return iRequestBuffer;
}

void CMMAStreamRequest::RunL()
{
    if (iStatus.Int() == KErrNone)
    {
        // data is processed, set ready for reuse
        iDataPtr.SetLength(0);
        iListener->WriteComplete(this);
    }
    else // error
    {
        iListener->HandleError(this, iStatus.Int());
    }
}

TInt CMMAStreamRequest::RunError(TInt aError)
{
    iListener->HandleError(this, aError);
    return KErrNone;
}

void CMMAStreamRequest::DoCancel()
{
    // Complete this request
    TRequestStatus* s = &iStatus;
    User::RequestComplete(s, KErrCancel);
}

CMMAStreamRequest::CMMAStreamRequest(MMMAStreamRequestListener* aListener):
        CActive(EPriorityStandard),
        iListener(aListener),
        iDataPtr(NULL, 0)
{
    CActiveScheduler::Add(this);
}

void CMMAStreamRequest::ConstructL()
{
    iData = HBufC8::NewL(KMMAStreamRequestBufferSize);
    iDataPtr.Set(iData->Des());
}

//  END OF FILE
