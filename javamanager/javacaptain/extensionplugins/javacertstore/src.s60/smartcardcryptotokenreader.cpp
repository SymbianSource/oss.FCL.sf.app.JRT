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


#include "smartcardcryptotokenreader.h"

namespace java
{
namespace security
{

SmartCardCryptoTokenReader* SmartCardCryptoTokenReader::NewL()
{
    SmartCardCryptoTokenReader* self = new(ELeave) SmartCardCryptoTokenReader();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void SmartCardCryptoTokenReader::RetrieveCerts(RArray<HBufC8*>& aSmartCardCerts)
{
    // keep a pointer to the provided array (this will be
    // filled with the smart card certificates)
    iSmartCardCerts = &aSmartCardCerts;

    // issue& complete the first request, so that we enter the state machine
    //(implemented inside RunL method)
    SetActive();
    CompleteRequest(KErrNone);

    // start the active scheduler
    CActiveScheduler::Start();
}

void SmartCardCryptoTokenReader::RunL()
{
    if (iStatus.Int() != KErrNone)
    {
        iState = EReleaseToken;
    }
    switch (iState)
    {
    case EStart:
        NextState(Initialize());
        break;
    case EListTokenTypes:
        NextState(ListTokenTypesL());
        break;
    case EOpenTokenType:
        NextState(OpenTokenType());
        break;
    case EOpenToken:
        NextState(OpenToken());
        break;
    case EGetTokenInterface:
        NextState(GetTokenInterface());
        break;
    case EListCertificates:
        NextState(ListCertificates());
        break;
    case EFilterCertificates:
        NextState(FilterCertificates());
        break;
    case ERetrieveCertificates:
        NextState(RetrieveCertificates());
        break;
    case EReleaseToken:
        NextState(ReleaseToken());
        break;
    case EFinish:
        // this is the final state
        CActiveScheduler::Stop();
        return;
    }
    // re-issue a new request
    SetActive();
}

void SmartCardCryptoTokenReader::DoCancel()
{
    switch (iState)
    {
    case EStart:
    case EListTokenTypes:
    case EListCertificates:
    case ERetrieveCertificates:
    case EReleaseToken:
        // these were all self-completed operations
        // -> nothing to cancel
        break;
    case EOpenTokenType:
        CancelOpenTokenType();
        break;
    case EOpenToken:
        CancelOpenToken();
        break;
    case EGetTokenInterface:
        CancelGetTokenInterface();
        break;
    }
    DoReleaseToken();
}

TInt SmartCardCryptoTokenReader::RunError(TInt /*aError*/)
{
    // cleanup
    DoReleaseToken();
    CActiveScheduler::Stop();
    return KErrNone;
}


bool SmartCardCryptoTokenReader::Initialize()
{
    CompleteRequest(KErrNone);
    // the operation did succeed
    return true;
}

bool SmartCardCryptoTokenReader::ListTokenTypesL()
{
    TCTTokenTypeAttribute att = { KCTRemovable , 1 };
    iSmartCardTokensAttributes.AppendL(att);
    iSmartCardTokensInterfaces.AppendL(TUid::Uid(KInterfaceCertStore));
    TCTFindTokenTypesByInterfaceAndAttribute findByIAndA(
        iSmartCardTokensInterfaces.Array(), iSmartCardTokensAttributes.Array());
    CCTTokenTypeInfo::ListL(iSmartCardTokenTypes, findByIAndA);
    if (iSmartCardTokenTypes.Count() > 0)
    {
        // the operation did succeed
        CompleteRequest(KErrNone);
        return true;
    }
    else
    {
        // the operation did not succeed
        return false;
    }
}

bool SmartCardCryptoTokenReader::OpenTokenType()
{
    iSmartCardTokenType = CCTTokenType::NewL(*iSmartCardTokenTypes[iCurrentTokenType], iFs);
    iSmartCardTokenType->List(iSmartCardTokenInfo, iStatus);
    iCurrentTokenType++;
    // the operation did succeed
    return true;
}

bool SmartCardCryptoTokenReader::OpenToken()
{
    if (iSmartCardTokenInfo.Count() > 0)
    {
        iSmartCardTokenType->OpenToken(*iSmartCardTokenInfo[0], iSmartCardToken, iStatus);
        // the operation did succeed
        return true;
    }
    else
    {
        // the operation did not succeed
        return false;
    }
}

bool SmartCardCryptoTokenReader::GetTokenInterface()
{
    iSmartCardToken->GetInterface(TUid::Uid(KInterfaceCertStore), iSmartCardTokenInterface, iStatus);
    // the operation did succeed
    return true;
}

bool SmartCardCryptoTokenReader::ListCertificates()
{
    if (iSmartCardTokenInterface)
    {
        iSmartCardStore = static_cast<MCTCertStore*>(iSmartCardTokenInterface);
        iSmartCardCertsFilter = CCertAttributeFilter::NewL();
        iSmartCardCertsFilter->SetOwnerType(ECACertificate);
        iSmartCardCertsFilter->SetFormat(EX509Certificate);
        iSmartCardStore->List(iSmartCardCertInfos, *iSmartCardCertsFilter, iStatus);
        // the operation did succeed
        return true;
    }
    else
    {
        // the operation did not succeed
        return false;
    }
}

bool SmartCardCryptoTokenReader::FilterCertificates()
{
    if (iCurrentRetrievedSmartCardCert < iSmartCardCertInfos.Count())
    {
        iSmartCardStore->Trusted(*iSmartCardCertInfos[iCurrentRetrievedSmartCardCert], iCurrentSmartCardCertIsTrusted, iStatus);
        // the operation did succeed
        return true;
    }
    else
    {
        // the operation did not succeed
        return false;
    }
}

bool SmartCardCryptoTokenReader::RetrieveCertificates()
{
    if (iCurrentRetrievedSmartCardCert < iSmartCardCertInfos.Count())
    {
        // retrieve only the trusted certs
        if (iCurrentSmartCardCertIsTrusted)
        {
            iTmpBuf = HBufC8::NewMaxL(iSmartCardCertInfos[iCurrentRetrievedSmartCardCert]->Size());
            iTmpBufPtr.Set(iTmpBuf->Des());
            iSmartCardStore->Retrieve(*iSmartCardCertInfos[iCurrentRetrievedSmartCardCert], iTmpBufPtr, iStatus);
            iSmartCardCerts->Append(iTmpBuf);
        }
        else
        {
            // complete the outstanding request
            CompleteRequest(KErrNone);
        }
        // move on to the next certificate
        iCurrentRetrievedSmartCardCert++;
        // the operation did succeed
        return true;
    }
    else
    {
        // the operation did not succeed
        return false;
    }
}

bool SmartCardCryptoTokenReader::ReleaseToken()
{
    DoReleaseToken();
    // complete the outstanding request
    CompleteRequest(KErrNone);
    // the operation did succeed
    return true;
}

void SmartCardCryptoTokenReader::CancelOpenTokenType()
{
    iSmartCardTokenType->CancelList();
}

void SmartCardCryptoTokenReader::CancelOpenToken()
{
    iSmartCardTokenType->CancelOpenToken();
}

void SmartCardCryptoTokenReader::CancelGetTokenInterface()
{
    iSmartCardToken->CancelGetInterface();
}

void SmartCardCryptoTokenReader::ConstructL()
{
    User::LeaveIfError(iFs.Connect());
    CActiveScheduler* CurrentActiveScheduler = CActiveScheduler::Current();
    if (CurrentActiveScheduler == NULL)
    {
        iActiveScheduler = new(ELeave) CActiveScheduler;
        CActiveScheduler::Install(iActiveScheduler);
    }
    CActiveScheduler::Add(this);
}

SmartCardCryptoTokenReader::SmartCardCryptoTokenReader()
        : CActive(EPriorityNormal), iState(EStart), iCurrentTokenType(0), iSmartCardToken(NULL), iSmartCardTokenType(NULL), iSmartCardStore(NULL), iSmartCardCertsFilter(NULL), iCurrentRetrievedSmartCardCert(0), iSmartCardCerts(NULL), iCurrentSmartCardCertIsTrusted(false), iTmpBuf(NULL), iTmpBufPtr(TPtr8(0,0)), iActiveScheduler(NULL)
{
}

SmartCardCryptoTokenReader::~SmartCardCryptoTokenReader()
{
    iSmartCardTokensInterfaces.Close();
    iSmartCardTokensAttributes.Close();
    iSmartCardTokenInfo.Close();
    iSmartCardCertInfos.Close();
    iFs.Close();
    if (iSmartCardCertsFilter)
    {
        delete iSmartCardCertsFilter;
        iSmartCardCertsFilter = NULL;
    }
    if (iActiveScheduler)
    {
        delete iActiveScheduler;
        iActiveScheduler = NULL;
    }
}

void SmartCardCryptoTokenReader::CompleteRequest(TInt aCompletionCode)
{
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status,aCompletionCode);
}

void SmartCardCryptoTokenReader::NextState(bool aCurrentOperationSucceeded)
{
    if (!aCurrentOperationSucceeded)
    {
        // cancel the outstanding request
        CompleteRequest(KErrCancel);
        // move on to the last state
        iState = EReleaseToken;
        return;
    }
    switch (iState)
    {
    case EStart:
        iState = EListTokenTypes;
        break;
    case EListTokenTypes:
        iState = EOpenTokenType;
        break;
    case EOpenTokenType:
        iState = EOpenToken;
        break;
    case EOpenToken:
        iState = EGetTokenInterface;
        break;
    case EGetTokenInterface:
        iState = EListCertificates;
        break;
    case EListCertificates:
        iState = EFilterCertificates;
        break;
    case EFilterCertificates:
        iState = EFilterCertificates;
        if (iCurrentRetrievedSmartCardCert >= iSmartCardCertInfos.Count())
        {
            // change the state only if we are done with reading all
            // the certs
            iState = EReleaseToken;
        }
        else
        {

            iState = ERetrieveCertificates;
        }
        break;
    case ERetrieveCertificates:
        if (iCurrentRetrievedSmartCardCert >= iSmartCardCertInfos.Count())
        {
            // change the state only if we are done with reading all
            // the certs
            iState = EReleaseToken;
        }
        else
        {
            iState = EFilterCertificates;
        }
        break;
    case EReleaseToken:
        if (iCurrentTokenType >= iSmartCardTokenTypes.Count())
        {
            // if we are done going through all the token types,
            // move to finish state
            iState = EFinish;
        }
        else
        {
            // if there are still token types to be processed, go back
            // to EOpenTokenType
            iState = EOpenTokenType;
        }
        break;
    }
}

void SmartCardCryptoTokenReader::DoReleaseToken()
{
    if (iSmartCardTokenType)
    {
        iSmartCardTokenType->Release();
        iSmartCardTokenType = NULL;
    }
    if (iSmartCardToken)
    {
        iSmartCardToken->Release();
        iSmartCardToken = NULL;
    }
    if (iSmartCardTokenInterface)
    {
        iSmartCardTokenInterface->Release();
        iSmartCardTokenInterface = NULL;
    }
    iSmartCardTokenInfo.Close();

}

} //end namespace security
} //end namespace java
