/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#include "smartcardcertextrasreader.h"

using namespace std;

namespace java
{
namespace security
{

SmartCardCertExtrasReader* SmartCardCertExtrasReader::NewL()
{
    SmartCardCertExtrasReader* self = new(ELeave) SmartCardCertExtrasReader();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void SmartCardCertExtrasReader::GetCertTrustedUsages(TDesC8& aSmartCardCertId, RCPointerArray<HBufC>& aSmartCardCertTrustedUsages)
{
    // make a copy of the cert id
    if (iSmartCardCertId)
    {
        delete iSmartCardCertId;
        iSmartCardCertId = NULL;
    }
    iSmartCardCertId = aSmartCardCertId.Alloc();

    // keep a pointer to the provided array (this will be
    // filled with the smart card cert's trusted usages)
    iSmartCardCertTrustedUsages = &aSmartCardCertTrustedUsages;

    // issue& complete the first request, so that we enter the state machine
    //(implemented inside RunL method)
    SetActive();
    CompleteRequest(KErrNone);

    // start the active scheduler
    CActiveScheduler::Start();
}

void SmartCardCertExtrasReader::RunL()
{
    if (iStatus.Int() != KErrNone)
    {
        iState = EFinish;
    }
    switch (iState)
    {
    case EStart:
        NextState(Initialize());
        break;
    case EGetExtras:
        NextState(GetExtras());
        break;
    case EFinish:
        NextState(Finish());
        return;
    }
    // re-issue a new request
    SetActive();
}

void SmartCardCertExtrasReader::DoCancel()
{
    switch (iState)
    {
    case EStart:
        CancelInitialize();
        break;
    case EGetExtras:
        CancelGetExtras();
        break;
    }
}

TInt SmartCardCertExtrasReader::RunError(TInt /*aError*/)
{
    CActiveScheduler::Stop();
    return KErrNone;
}


bool SmartCardCertExtrasReader::Initialize()
{
    // do the initialization
    iSmartCardCertExtras->Initialize(iStatus);
    // the operation did succeed
    return true;
}

bool SmartCardCertExtrasReader::GetExtras()
{
    // retrieve the certificate's extra info
    iSmartCardCertExtras->GetExtras(iSmartCardCertId->Des(), *iSmartCardCertTrustedUsages, iSmartCardCertLocation, iStatus);
    // the operation did succeed
    return true;
}

bool SmartCardCertExtrasReader::Finish()
{
    // this is the final state
    CActiveScheduler::Stop();
    // the operation did succeed
    return true;
}



void SmartCardCertExtrasReader::CancelInitialize()
{
    iSmartCardCertExtras->CancelInitialize();
}

void SmartCardCertExtrasReader::CancelGetExtras()
{
    iSmartCardCertExtras->CancelGetExtras();
}

void SmartCardCertExtrasReader::ConstructL()
{
    CActiveScheduler* CurrentActiveScheduler = CActiveScheduler::Current();
    if (CurrentActiveScheduler == NULL)
    {
        iActiveScheduler = new(ELeave) CActiveScheduler;
        CActiveScheduler::Install(iActiveScheduler);
    }
    CActiveScheduler::Add(this);
    iSmartCardCertExtras = CWimCert::NewL();
}

SmartCardCertExtrasReader::SmartCardCertExtrasReader()
        : CActive(EPriorityNormal), iState(EStart), iSmartCardCertTrustedUsages(NULL), iSmartCardCertExtras(NULL), iSmartCardCertId(NULL), iActiveScheduler(NULL)
{
}

SmartCardCertExtrasReader::~SmartCardCertExtrasReader()
{
    if (iSmartCardCertExtras)
    {
        delete iSmartCardCertExtras;
        iSmartCardCertExtras = NULL;
    }
    if (iSmartCardCertId)
    {
        delete iSmartCardCertId;
        iSmartCardCertId = NULL;
    }
    if (iActiveScheduler)
    {
        delete iActiveScheduler;
        iActiveScheduler = NULL;
    }
}

void SmartCardCertExtrasReader::CompleteRequest(TInt aCompletionCode)
{
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status,aCompletionCode);
}

void SmartCardCertExtrasReader::NextState(bool aCurrentOperationSucceeded)
{
    if (!aCurrentOperationSucceeded)
    {
        // cancel the outstanding request
        CompleteRequest(KErrCancel);
        // move on to the last state
        iState = EFinish;
        return;
    }
    switch (iState)
    {
    case EStart:
        iState = EGetExtras;
        break;
    case EGetExtras:
        iState = EFinish;
        break;
    case EFinish:
        // reset the state to getextras
        // (not to EStart, since we are already initialized)
        iState = EGetExtras;
        break;
    }
}

} //end namespace security
} //end namespace java
