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


#include "ocspclient.h"

using namespace java::security;
using namespace std;
using namespace java::util;

const TInt KCertStoreUIDForSWInstallOCSPSigning = 268478646;

OcspClient* OcspClient::createInstance(long iap, const char* defaultUrl)
{
    OcspClient* self = new OcspClient(iap);
    TRAPD(err, self->ConstructL(defaultUrl));
    if (err != KErrNone)
    {
        delete self;
        self = NULL;
    }
    return self;
}

OcspClient::OcspClient(long iap)
        : CActive(EPriorityNormal), iIap(TUint32(iap)), iDefaultUrl(NULL), iOcspClient(NULL), iCertStore(NULL), iInitialized(false), iOcspResponse(NULL), iCertArray(NULL), iMonitor(NULL)
{
    CActiveScheduler::Add(this);
}

void OcspClient::ConstructL(const char* defaultUrl)
{
    User::LeaveIfError(iRfs.Connect());
    iCertStore = CUnifiedCertStore::NewL(iRfs, false /*aOpenForWrite*/);
    iOcspResponse = OcspResponse::NewL();
    // create the monitor
    iMonitor = Monitor::createMonitor();
    if (defaultUrl)
    {
        int len = strlen(defaultUrl);
        iDefaultUrl = HBufC8::NewL(len);
        TPtr8 defaultUrlPtr = iDefaultUrl->Des();
        TPtr8 ptr((TUint8 *)defaultUrl, len);
        ptr.SetLength(len);
        defaultUrlPtr.Copy(ptr);
    }
}

void OcspClient::RunL()
{
    switch (iState)
    {
    case ESendOcspRequest:
        SendOcspRequestL();
        iState = EProcessOcspResponse;
        break;
    case EProcessOcspResponse:
        ProcessOcspResponse();
        iMonitor->notify();
        return;
    }
    // re-issue a new request
    SetActive();
}

void OcspClient::DoCancel()
{
    if (iCertStore)
    {
        iCertStore->CancelInitialize();
    }
    if (iOcspClient)
    {
        iOcspClient->CancelCheck();
    }
}

TInt OcspClient::RunError(TInt /*aError*/)
{
    return KErrNone;
}

OcspClient::~OcspClient()
{
    if (IsActive())
    {
        Cancel();
    }
    if (iCertStore)
    {
        delete iCertStore;
        iCertStore = NULL;
    }
    if (iOcspClient)
    {
        delete iOcspClient;
        iOcspClient = NULL;
    }
    if (iOcspResponse)
    {
        delete iOcspResponse;
        iOcspResponse = NULL;
    }
    if (iCertArray)
    {
        iCertArray->ResetAndDestroy();
        delete iCertArray;
    }
    if (iDefaultUrl)
    {
        delete iDefaultUrl;
        iDefaultUrl = NULL;
    }
    if (iMonitor)
    {
        delete iMonitor;
        iMonitor = NULL;
    }
    iRfs.Close();
}

void OcspClient::startOcspCheck(const char ** aCertChain, int aCertChainLen)
{

    // clear the response
    iOcspResponse->Clear();
    // build up the chain
    iCertChain = aCertChain;
    iCertChainLen = aCertChainLen;
    // kick off the state machine
    Start();
}

OcspResponse OcspClient::getOcspCheckResponse()
{
    // wait for completion
    iMonitor->wait();
    // return the result
    return *iOcspResponse;
}

void OcspClient::cancelOcspCheck(bool doCleanup)
{
    // cancel the outstanding request (if any)
    if (doCleanup)
    {
        if (IsActive())
        {
            Cancel();
        }
    }
    iMonitor->notify();
}

void OcspClient::Start()
{
    // cancel the outstanding request (if any)
    if (IsActive())
    {
        Cancel();
    }

    iState = ESendOcspRequest;
    SetActive();
    if (iInitialized)
    {
        // already initialized
        CompleteRequest();
        return;
    }

    // do the initialization
    iCertStore->Initialize(iStatus);
    iInitialized = true;
}

void OcspClient::SendOcspRequestL()
{
    InitOcspClientL();
    iOcspClient->Check(iStatus);
}

void OcspClient::ProcessOcspResponse()
{
    int summary = RESPONSE_CANNOT_OBTAIN_CERT_STATUS;
    if (iStatus.Int() == KErrNone)
    {
        // set the summary
        switch (iOcspClient->SummaryResult())
        {
        case OCSP::EGood:
            summary = RESPONSE_GOOD;
            break;
        case OCSP::EUnknown:
            summary = RESPONSE_UNKNOWN;
            break;
        case OCSP::ERevoked:
            summary = RESPONSE_REVOKED;
            break;
        }
        // set the individual responses
        for (TInt index = 0 ; index < iOcspClient->TransactionCount() ; ++index)
        {
            const TOCSPOutcome& outcome = iOcspClient->Outcome(index);
            switch (outcome.iStatus)
            {
            case OCSP::ETransportError:
            case OCSP::EClientInternalError:
            case OCSP::EMalformedRequest:
            case OCSP::EServerInternalError:
            case OCSP::ETryLater:
            case OCSP::ESignatureRequired:
            case OCSP::EClientUnauthorised:
            case OCSP::EUnknownResponseType:
                iOcspResponse->iIndividualResponses.push_back(RESPONSE_CANNOT_OBTAIN_CERT_STATUS);
                break;
            case OCSP::ENoServerSpecified:
            case OCSP::EInvalidURI:
                iOcspResponse->iIndividualResponses.push_back(RESPONSE_INVALID_REVOCATION_SERVER_URI);
                break;
            case OCSP::EResponseSignatureValidationFailure:
                iOcspResponse->iIndividualResponses.push_back(RESPONSE_SIGNATURE_VALIDATION_FAILURE);
                break;
            case OCSP::EThisUpdateTooLate:
            case OCSP::EThisUpdateTooEarly:
            case OCSP::ENextUpdateTooEarly:
            case OCSP::ENonceMismatch:
            case OCSP::EMalformedResponse:
            case OCSP::EUnknownCriticalExtension:
            case OCSP::EMissingCertificates:
                iOcspResponse->iIndividualResponses.push_back(RESPONSE_INVALID_REVOCATION_SERVER_RESPONSE);
                break;
            case OCSP::EMissingNonce:
                iOcspResponse->iIndividualResponses.push_back(RESPONSE_MISSING_NONCE);
                break;
            case OCSP::ECertificateNotValidAtValidationTime:
                iOcspResponse->iIndividualResponses.push_back(RESPONSE_INVALID_CERT_STATUS_INFO);
                break;
            case OCSP::EValid:
                switch (outcome.iResult)
                {
                case OCSP::EGood:
                    iOcspResponse->iIndividualResponses.push_back(RESPONSE_GOOD);
                    break;
                case OCSP::EUnknown:
                    iOcspResponse->iIndividualResponses.push_back(RESPONSE_UNKNOWN);
                    break;
                case OCSP::ERevoked:
                    iOcspResponse->iIndividualResponses.push_back(RESPONSE_REVOKED);
                    break;
                default:
                    ASSERT(EFalse);
                }
                break;
            default:
                ASSERT(EFalse);
                break;
            }
        }
    }
    iOcspResponse->iSummary = summary;
}

void OcspClient::CompleteRequest()
{
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status,KErrNone);
}

void OcspClient::InitOcspClientL()
{
    if (iOcspClient)
    {
        delete iOcspClient;
        iOcspClient = NULL;
    }
    if (iCertArray)
    {
        iCertArray->ResetAndDestroy();
        delete iCertArray;
    }
    COCSPParameters* ocspParams = COCSPParameters::NewL();
    CleanupStack::PushL(ocspParams);
    if (iDefaultUrl)
    {
        ocspParams->SetURIL(*iDefaultUrl, ETrue);
    }
    ocspParams->SetTransport(COCSPTransportDefault::NewL(iIap));
    ocspParams->AddAllAuthorisationSchemesL(
        TUid::Uid(KCertStoreUIDForSWInstallOCSPSigning), *iCertStore);
    // add the certs
    if (iCertChainLen >= 2)
    {
        iCertArray = new(ELeave) CArrayPtrFlat<CX509Certificate>(1);
        std::string cert = JavaCommonUtils::base64decode(std::string(iCertChain[0], strlen(iCertChain[0])));
        TPtr8 ptr8((TUint8 *)(cert.c_str()), cert.size());
        ptr8.SetLength(cert.size());
        CX509Certificate* issuerCert = CX509Certificate::NewL(ptr8);
        iCertArray->AppendL(issuerCert);
        CX509Certificate* subjectCert = NULL;
        for (int i = 1; i < iCertChainLen; i++)
        {
            subjectCert = issuerCert;
            cert = JavaCommonUtils::base64decode(std::string(iCertChain[i], strlen(iCertChain[i])));
            TPtr8 ptr((TUint8 *)(cert.c_str()), cert.size());
            ptr.SetLength(cert.size());
            issuerCert = CX509Certificate::NewL(ptr);
            iCertArray->AppendL(issuerCert);
            ocspParams->AddCertificateL(*subjectCert, *issuerCert);
        }
    }
    iOcspClient = COCSPClient::NewL(ocspParams);
    CleanupStack::Pop(ocspParams);
}

OcspResponse* OcspResponse::NewL()
{
    OcspResponse* self = new(ELeave) OcspResponse();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

OcspResponse::OcspResponse()
{
    iIndividualResponses.clear();
    iSummary = RESPONSE_CANNOT_OBTAIN_CERT_STATUS;
}

void OcspResponse::ConstructL()
{
}

OcspResponse::~OcspResponse()
{
}

void OcspResponse::Clear()
{
    iIndividualResponses.clear();
    iSummary = RESPONSE_CANNOT_OBTAIN_CERT_STATUS;
}

OcspResponse& OcspResponse::operator=(const OcspResponse& x)
{
    iSummary = x.iSummary;
    iIndividualResponses = x.iIndividualResponses;
    return *this;
}

OcspResponse::OcspResponse(const OcspResponse& x)
{
    *this = x;
}

