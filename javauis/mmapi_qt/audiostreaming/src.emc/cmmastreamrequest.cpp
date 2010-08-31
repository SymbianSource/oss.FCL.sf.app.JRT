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
#include <logger.h>
#include "cmmastreamrequest.h"
#include "mmmastreamrequestlistener.h"

CMMAStreamRequest* CMMAStreamRequest::NewLC(MMMAStreamRequestListener* aListener)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamRequest::NewLC +");
    CMMAStreamRequest* self = new(ELeave)CMMAStreamRequest(aListener);
    CleanupStack::PushL(self);
    self->ConstructL();
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamRequest::NewLC -");
    return self;
}

CMMAStreamRequest::~CMMAStreamRequest()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamRequest::~CMMAStreamRequest +");
    delete iData;
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamRequest::~CMMAStreamRequest -");
}

TPtr8& CMMAStreamRequest::DataPtr()
{
    return iDataPtr;
}

void CMMAStreamRequest::CompleteRead(TInt aError)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamRequest::CompleteRead +");
    if (aError < KErrNone)
    {
        iListener->HandleError(this, aError);
    }
    else // OK
    {
        iListener->ReadComplete(this);
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamRequest::CompleteRead -");
}

void CMMAStreamRequest::SetActive(TBool aActive)
{
    iActive = aActive;
}

TBool CMMAStreamRequest::IsActive()
{
    return iActive;
}

TPckgBuf< TInt >& CMMAStreamRequest::RequestBuffer()
{
    return iRequestBuffer;
}

void CMMAStreamRequest::WriteRequestComplete(TInt Err)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamRequest::WriteRequestComplete +");
    if (Err == KErrNone)
    {
        // data is processed, set ready for reuse
        iDataPtr.SetLength(0);
        iListener->WriteComplete(this);
    }
    else // error
    {
        iListener->HandleError(this, Err);
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamRequest::WriteRequestComplete -");
}

CMMAStreamRequest::CMMAStreamRequest(MMMAStreamRequestListener* aListener):
        iListener(aListener),
        iDataPtr(NULL, 0),iActive(EFalse)
{
    //Do Nothing
}

void CMMAStreamRequest::ConstructL()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamRequest::ConstructL +");
    iData = HBufC8::NewL(KMMAStreamRequestBufferSize);
    iDataPtr.Set(iData->Des());
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAStreamRequest::ConstructL -");
}

//  END OF FILE
