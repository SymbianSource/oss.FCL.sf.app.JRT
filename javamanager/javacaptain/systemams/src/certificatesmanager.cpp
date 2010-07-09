/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "certificatesmanager.h"

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <securitydefs.h>
#else
#include <securitydefs.h>
#include <securitydefsconst.h>
#endif

#include "logger.h"

using namespace java::security::legacysupport;

const int STATE_ENABLED = 1;
const int STATE_DISABLED = 2;
const int STATE_DELETED = 3;

CertificatesManager* CertificatesManager::NewL()
{
    JELOG2(EJavaSystemAMS);
    CertificatesManager* self = new(ELeave) CertificatesManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CertificatesManager::CertificatesManager()
        : CActive(EPriorityNormal), iState(EStart), iCTToken(NULL), iCTTokenType(NULL), iCertStore(NULL), iCertsFilter(NULL), iBuffer(NULL), iBufferPos(0), iCanDisable(false), iEncodedCert(NULL), iCurrentRetrievedCert(0),  iCurrentCert(NULL), iCurrentCertInfo(NULL), iProtDomain(NULL), iCertCount(0), iInitialized(false)
{
    JELOG2(EJavaSystemAMS);
}

void CertificatesManager::ConstructL()
{
    JELOG2(EJavaSystemAMS);
    CActiveScheduler::Add(this);
}

void CertificatesManager::RunL()
{
    JELOG2(EJavaSystemAMS);
    switch (iState)
    {
    case EStart:
        InitL();
        iState = EOpenToken;
        break;
    case EOpenToken:
        OpenToken();
        iState = EGetTokenInterface;
        break;
    case EGetTokenInterface:
        GetTokenInterface();
        iState = EListCertificates;
        break;
    case EListCertificates:
        ListCertificatesL();
        iState = EInitCertsRetrieval;
        break;
    case EInitCertsRetrieval:
        InitCertsRetrievalL();
    case ERetrieveCertificates:
        RetrieveCertificates();
        CompleteRequest();
        break;
    case ERetrieveCertState:
        RetrieveCertState();
        iState = ERetrieveCertLength;
        break;
    case ERetrieveCertLength:
        RetrieveCertLengthL();
        iState = ECollectCertInfo;
        break;
    case ECollectCertInfo:
        CollectCertInfoL();
        iState = ERetrieveCertificates;
        CompleteRequest();
        break;
    case EFinish:
        // mark that the initialization was done
        CActiveScheduler::Stop();
        return;
    }
    // re-issue a new request
    SetActive();
}

void CertificatesManager::DoCancel()
{
    JELOG2(EJavaSystemAMS);
}

TInt CertificatesManager::RunError(TInt /*aError*/)
{
    JELOG2(EJavaSystemAMS);
    return KErrNone;
}

CertificatesManager::~CertificatesManager()
{
    JELOG2(EJavaSystemAMS);
    if (iCTTokenType)
    {
        iCTTokenType->Release();
        iCTTokenType = NULL;
    }
    if (iCTToken)
    {
        iCTToken->Release();
        iCTToken = NULL;
    }
    if (iCTTokenInterface)
    {
        iCTTokenInterface->Release();
        iCTTokenInterface = NULL;
    }
    if (iCertsFilter)
    {
        delete iCertsFilter;
        iCertsFilter = NULL;
    }
    REComSession::DestroyedImplementation(iJavaCertStoreEcomPluginId);
    iCertInfos.Close();
    iCTTokenInfo.Close();
    iCertInfos.Close();
    if (iBuffer)
    {
        delete iBuffer;
        iBuffer = NULL;
    }
    iApplications.Close();
    if (iEncodedCert)
    {
        delete iEncodedCert;
        iEncodedCert = NULL;
    }
    if (iCurrentCert)
    {
        delete iCurrentCert;
        iCurrentCert = NULL;
    }
    if (iProtDomain)
    {
        delete iProtDomain;
        iProtDomain = NULL;
    }
}

TUint32 CertificatesManager::TrustRootCount(const TDesC& aProtectionDomain)
{
    JELOG2(EJavaSystemAMS);
    LazyInit();
    if (iProtDomain == NULL || iProtDomain->Compare(aProtectionDomain) == 0)
    {
        iProtDomain = aProtectionDomain.Alloc();
        return iCertCount;
    }
    else
    {
        return 0;
    }
}

TPtr8 CertificatesManager::TrustRootInfo(const TDesC& /*aProtectionDomain*/)
{
    JELOG2(EJavaSystemAMS);
    LazyInit();
    return iBuffer->Ptr(0);
}

TPtr8 CertificatesManager::TrustRootInfoCertificateL(const TInt aId)
{
    JELOG2(EJavaSystemAMS);
    LazyInit();
    iCurrentCertInfo = getTrustRootL(aId);
    iCurrentCert = HBufC8::NewL(iCurrentCertInfo->Size());
    TPtr8 currentCertPtr = iCurrentCert->Des();
    iCertStore->Retrieve(*iCurrentCertInfo, currentCertPtr, iStatus);
    User::WaitForRequest(iStatus);
    TPtr8 ptr = iCurrentCert->Des();
    return ptr;
}

void CertificatesManager::DeleteTrustRootL(const TInt aId)
{
    JELOG2(EJavaSystemAMS);
    LazyInit();
    iCurrentCertInfo = getTrustRootL(aId);
    iCertStore->Remove(*iCurrentCertInfo, iStatus);
    User::WaitForRequest(iStatus);
    if (iStatus.Int() == KErrNone)
    {
        UpdateCacheL(aId, STATE_DELETED);
    }
}

void CertificatesManager::DisableTrustRootL(const TInt aId)
{
    JELOG2(EJavaSystemAMS);
    LazyInit();
    iCurrentCertInfo = getTrustRootL(aId);
    RArray<TUid> applications;
    iCertStore->SetApplicability(*iCurrentCertInfo, applications, iStatus);
    User::WaitForRequest(iStatus);
    if (iStatus.Int() == KErrNone)
    {
        UpdateCacheL(aId, STATE_DISABLED);
    }
}

void CertificatesManager::EnableTrustRootL(const TInt aId)
{
    JELOG2(EJavaSystemAMS);
    LazyInit();
    iCurrentCertInfo = getTrustRootL(aId);
    RArray<TUid> applications;
    applications.AppendL(KMidletInstallApplicabilityUid);
    iCertStore->SetApplicability(*iCurrentCertInfo, applications, iStatus);
    User::WaitForRequest(iStatus);
    if (iStatus.Int() == KErrNone)
    {
        UpdateCacheL(aId, STATE_ENABLED);
    }
}

CCTCertInfo* CertificatesManager::getTrustRootL(const TInt aId)
{
    JELOG2(EJavaSystemAMS);
    for (int i=0; i<iCertInfos.Count(); i++)
    {
        if (iCertInfos[i]->Handle().iObjectId == aId)
        {
            return iCertInfos[i];
        }
    }
    User::Leave(KErrNotFound);
    return 0; // keeps compiler happy
}

void CertificatesManager::CompleteRequest()
{
    JELOG2(EJavaSystemAMS);
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status,KErrNone);
}

void CertificatesManager::InitL()
{
    JELOG2(EJavaSystemAMS);
    const TUid javaCertStoreEcomPlugin = {0x200213A3};
    iCTTokenType = reinterpret_cast<CCTTokenType*>
                   (REComSession::CreateImplementationL(
                        javaCertStoreEcomPlugin,
                        iJavaCertStoreEcomPluginId));
    if (iCTTokenType)
    {
        iCTTokenType->List(iCTTokenInfo, iStatus);
    }
    else
    {
        CompleteRequest();
    }
}

void CertificatesManager::OpenToken()
{
    JELOG2(EJavaSystemAMS);
    if (iCTTokenType)
    {
        iCTTokenType->OpenToken(*iCTTokenInfo[0], iCTToken, iStatus);
    }
    else
    {
        CompleteRequest();
    }
}

void CertificatesManager::GetTokenInterface()
{
    JELOG2(EJavaSystemAMS);
    if (iCTToken)
    {
        iCTToken->GetInterface(TUid::Uid(KInterfaceCertStore), iCTTokenInterface, iStatus);
    }
    else
    {
        CompleteRequest();
    }
}

void CertificatesManager::ListCertificatesL()
{
    JELOG2(EJavaSystemAMS);
    iCertStore = static_cast<MCTWritableCertStore*>(iCTTokenInterface);
    if (iCertStore)
    {
        iCertsFilter = CCertAttributeFilter::NewL();
        iCertsFilter->SetOwnerType(ECACertificate);
        iCertsFilter->SetFormat(EX509Certificate);
        iCertStore->List(iCertInfos, *iCertsFilter, iStatus);
    }
    else
    {
        CompleteRequest();
    }
}

void CertificatesManager::InitCertsRetrievalL()
{
    JELOG2(EJavaSystemAMS);
    iCertCount = iCertInfos.Count();
    iBuffer = CBufFlat::NewL(4 /*trusts count*/ + iCertInfos.Count() * 12);
    RBufWriteStream writer(*iBuffer, iBufferPos);
    writer.WriteUint32L(iCertInfos.Count());
    iBufferPos +=4;
}

void CertificatesManager::RetrieveCertificates()
{
    JELOG2(EJavaSystemAMS);
    if (iCurrentRetrievedCert < iCertInfos.Count())
    {
        iState = ERetrieveCertState;
    }
    else
    {
        iState = EFinish;
    }
}

void CertificatesManager::RetrieveCertState()
{
    JELOG2(EJavaSystemAMS);
    iApplications.Reset();
    iCertStore->Applications(*iCertInfos[iCurrentRetrievedCert],iApplications,iStatus);
}

void CertificatesManager::RetrieveCertLengthL()
{
    JELOG2(EJavaSystemAMS);
    iEncodedCert = HBufC8::NewL(iCertInfos[iCurrentRetrievedCert]->Size());
    TPtr8 ptr = iEncodedCert->Des();
    iCertStore->Retrieve(*iCertInfos[iCurrentRetrievedCert], ptr, iStatus);
}

void CertificatesManager::CollectCertInfoL()
{
    JELOG2(EJavaSystemAMS);
    TBool canDelete = iCertInfos[iCurrentRetrievedCert]->IsDeletable();
    TBool isDisabled = (iApplications.Count() == 0);
    // flags
    TInt flags = 0;
    if (canDelete)
    {
        flags |= 0x4;
        flags |= 0x2;
    }
    if (isDisabled)
    {
        flags |= 0x1;
    }
    RBufWriteStream writer(*iBuffer, iBufferPos);
    writer.WriteUint32L(iCertInfos[iCurrentRetrievedCert]->Handle().iObjectId);
    writer.WriteUint32L(flags);
    writer.WriteUint32L(iEncodedCert->Length());
    iBufferPos +=12;
    iCurrentRetrievedCert++;
}

void CertificatesManager::UpdateCacheL(TInt aCertId, TInt aCertState)
{
    JELOG2(EJavaSystemAMS);
    CBufFlat* newBuffer;
    if (aCertState == STATE_DELETED)
    {
        iCertCount--;
    }
    newBuffer = CBufFlat::NewL(4 /*trusts count*/ + iCertCount * 12);
    CleanupStack::PushL(newBuffer);
    int bufferPos = 0;
    RBufReadStream reader(*iBuffer, bufferPos);
    RBufWriteStream writer(*newBuffer, bufferPos);
    TInt cnt = reader.ReadUint32L();
    writer.WriteUint32L(iCertCount);
    bufferPos += 4;
    for (int i=0; i<cnt; i++)
    {
        TInt id = reader.ReadUint32L();
        TInt flags = reader.ReadUint32L();
        TInt length = reader.ReadUint32L();
        if (id == aCertId)
        {
            TInt newFlags = flags & 0x6;
            switch (aCertState)
            {
            case STATE_DISABLED:
                newFlags |= 0x1;
            case STATE_ENABLED:
                // replace the flags with newFlags
                writer.WriteUint32L(id);
                writer.WriteUint32L(newFlags);
                writer.WriteUint32L(length);
                break;
            case STATE_DELETED:
                // don't write the entry
                break;
            }
        }
        else
        {
            writer.WriteUint32L(id);
            writer.WriteUint32L(flags);
            writer.WriteUint32L(length);
        }
        bufferPos += 12;
    }
    delete iBuffer;
    iBuffer = newBuffer;
    CleanupStack::Pop(newBuffer);
}

void CertificatesManager::LazyInit()
{
    if (iInitialized)
    {
        return;
    }
    // kick off the state machine (RunL method)
    SetActive();
    CompleteRequest();
    // start the nested active scheduler (in this way we wait for the RunL method to complete)
    CActiveScheduler::Start();
    // mark that the initialization is complete
    iInitialized = true;
}
