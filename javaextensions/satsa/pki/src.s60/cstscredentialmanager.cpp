/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "cstscredentialmanager.h"
#include "tstsdistinguishednameconverter.h"
#include "CCMSIssuerAndSerialNumber.h"
#include "cstsseprompt.h"
#include "stspkiconstants.h"
#include "fs_methodcall.h"
#include <asn1dec.h>
#include <x509cert.h>
#include <unifiedcertstore.h>
#include <unifiedkeystore.h>
#include <mctwritablecertstore.h>
#include <charconv.h>
#include <pkcs10.h>
#include <secdlg.h>
#include <PKIDlg.h>
#include <securityerr.h>



// CONSTANTS
_LIT(KSTSSymbianKeyStoreLabel, "Software key store");
_LIT(KSTSSymbianCertificateStoreLabel, "Software certificate store");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSCredentialManager::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSCredentialManager* CSTSCredentialManager::NewLC()
{
    CSTSCredentialManager* self = new(ELeave) CSTSCredentialManager;
    CallMethod(self,&CSTSCredentialManager::AddToScheduler,self);
    CleanupStack::PushL(self);
    CallMethodL(self,&CSTSCredentialManager::ConstructL,self);
    return self;
}

void CSTSCredentialManager::AddToScheduler()
{
    CActiveScheduler::Add(this);
}

// Destructor
CSTSCredentialManager::~CSTSCredentialManager()
{
    delete iEncodedCert;
    delete iIssuerAndSerialNumber;
    if (iPKIDialog)
    {
        iPKIDialog->Release();
    }
    delete iDistinguishedName;
    delete iDisplayName;
    delete iRequestEncoded;
    delete iRequest;
    if (iCertInfoArray)
    {
        iCertInfoArray->Close();
        delete iCertInfoArray;
    }
    if (iKeyInfoArray)
    {
        iKeyInfoArray->Close();
        delete iKeyInfoArray;
    }
    if (iKeyInfo)
    {
        iKeyInfo->Release();
    }
    delete iKeyStore;
    delete iStore;
    delete iWait;
    iFileServer.Close();
}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::AddCredentialL
// Adds a certificate or certificate URI to certificate store
// -----------------------------------------------------------------------------
//
TBool CSTSCredentialManager::AddCredentialL(
    TInt/*const TDesC&*/ aCertDisplayName,
    TInt/*const TDesC&*/ aPkiPath)
{

    if (iState != EReady)
    {
        User::Leave(KErrNotReady);
    }

    iWritableCertStoreIndex = CheckWriteCertStoreSEIDL(NULL, NULL);

    const TDesC8* path = reinterpret_cast< const TDesC8* >(aPkiPath);

    TASN1DecGeneric genericDecoder(*path);

    genericDecoder.InitL();

    TPtrC8 certificates = genericDecoder.GetContentDER();

    TInt certificatesLength = certificates.Length();
    TInt pos = 0;

    iSubject.Set(certificates.Right(certificatesLength));

    TPtrC8 issuer(iSubject);

    // go through the certificate chain, leaving the last certificate as subject
    // and second last as issuer
    while (pos < certificatesLength)
    {
        issuer.Set(iSubject);
        TASN1DecGeneric
        certDecoder(certificates.Right(certificatesLength - pos));
        certDecoder.InitL();
        TInt certLength = certDecoder.LengthDER();

        TPtrC8 singleCert(certificates.Mid(pos, certLength));
        iSubject.Set(singleCert);
        pos+=certLength;
    }

    CX509Certificate* cert =
        CX509Certificate::NewLC(iSubject);

    CX509Certificate* issuerCert =
        CX509Certificate::NewLC(issuer);

    iSubjectKeyId = cert->KeyIdentifierL();
    iIssuerKeyId = issuerCert->KeyIdentifierL();

    iCertDisplayName = reinterpret_cast<TDesC*>(aCertDisplayName);

    // now we have to check that this certificate does not exist in the
    // database yet
    CCertAttributeFilter* filter = CCertAttributeFilter::NewLC();
    filter->SetFormat(EX509Certificate);
    filter->SetSubjectKeyId(iSubjectKeyId);
    filter->SetIssuerKeyId(iIssuerKeyId);

    RMPointerArray< CCTCertInfo >* certInfoArray =
        new(ELeave) RMPointerArray< CCTCertInfo >();
    if (iCertInfoArray)
    {
        iCertInfoArray->Close();
        delete iCertInfoArray;
    }
    iCertInfoArray = certInfoArray;

    iState = EAddListing;
    iStore->WritableCertStore(iWritableCertStoreIndex)
    .List(*iCertInfoArray, *filter,
          iStatus);
    WaitForCompletionL();

    CleanupStack::PopAndDestroy(3); // filter, cert, issuerCert

    iCertInfoArray->Close();
    delete iCertInfoArray;
    iCertInfoArray = NULL;


    return ETrue;
}

void CSTSCredentialManager::StaticAddCredentialL(
    TInt /*aManager*/,
    TInt /*aCertDisplayName*/,
    TInt /*aPkiPath*/,
    TInt /*aRetVal*/)
{

}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::GenerateCSRL
// Creates a DER encoded PKCS#10 certificate enrollment request.
// -----------------------------------------------------------------------------
//
TDesC8& CSTSCredentialManager::GenerateCSRL(
    const TDesC& aNameInfo,
    const TDesC& aAlgorithm,
    TInt aKeyLen,
    TInt aKeyUsage,
    const TDesC* aSecurityElementID,
    const TDesC* aSecurityElementPrompt)
{
    TInt storeIndex = CheckKeyStoreSEIDL(aSecurityElementID,
                                         aSecurityElementPrompt);

    CKeyInfoBase::EKeyAlgorithm algorithm(CKeyInfoBase::EInvalidAlgorithm);
    if (aAlgorithm.Compare(KSTSAlgorithmRsa()) == 0)
    {
        algorithm = CKeyInfoBase::ERSA;
    }
    else
    {
        CheckOIDL(aAlgorithm);
        User::Leave(KSTSErrSeNoKeys);
    }

    TSTSDistinguishedNameConverter dnConverter;

    CX500DistinguishedName* dName = dnConverter.CreateDNL(aNameInfo);
    delete iDistinguishedName;
    iDistinguishedName = dName;

    HBufC* dispName = dName->DisplayNameL();
    delete iDisplayName;
    iDisplayName = dispName;

    TKeyUsagePKCS15 keyUsage(EPKCS15UsageNone);
    switch (aKeyUsage)
    {
    case KSTSKeyUsageAuthentication:
    {
        keyUsage = EPKCS15UsageSign;
        break;
    }
    case KSTSKeyUsageNonRepudiation:
    {
        keyUsage = EPKCS15UsageNonRepudiation;
        break;
    }
    default:
    {
        User::Leave(KErrKeyUsage);
    }
    }

    // have to search for the key
    TCTKeyAttributeFilter filter;
    filter.iKeyAlgorithm = algorithm;
    filter.iUsage = keyUsage;
    RMPointerArray< CCTKeyInfo >* keyInfoArray =
        new(ELeave) RMPointerArray< CCTKeyInfo >();
    if (iKeyInfoArray)
    {
        iKeyInfoArray->Close();
        delete iKeyInfoArray;
    }
    iKeyInfoArray = keyInfoArray;

    iKeyLen = aKeyLen;

    iState = ESearchingKey;
    iKeyStore->KeyStore(storeIndex).List(
        *iKeyInfoArray, filter, iStatus);

    WaitForCompletionL();

    return *iRequestEncoded;
}

void CSTSCredentialManager::StaticGenerateCSRL(
    TInt aParams,
    TInt aRetVal)
{
    TGenerateCSRParams* params = reinterpret_cast<TGenerateCSRParams*>(aParams);
    TDesC8** ret = reinterpret_cast<TDesC8**>(aRetVal);

    TDesC8& retVal = GenerateCSRL(
                         *(params->iNameInfo), *(params->iAlgorithm),
                         params->iKeyLen, params->iKeyUsage,
                         params->iSecurityElementID,
                         params->iSecurityElementPrompt);
    *ret = &retVal;
}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::RemoveCredentialL
// Removes a certificate or certificate URI from a certificate store.
// -----------------------------------------------------------------------------
//
TBool CSTSCredentialManager::RemoveCredentialL(
    const TDesC& aCertDisplayName,
    const TDesC8& aIssuerAndSerialNumber,
    const TDesC* aSecurityElementID,
    const TDesC* aSecurityElementPrompt)

{
    TInt storeIndex = CheckWriteCertStoreSEIDL(aSecurityElementID,
                      aSecurityElementPrompt);

    // currently the certificates are stored in the symbian store, always
    storeIndex = CheckWriteCertStoreSEIDL(NULL, NULL);

    if (iState != EReady)
    {
        User::Leave(KErrNotReady);
    }
    // Create filter
    CCertAttributeFilter* filter = CCertAttributeFilter::NewLC();
    TCertLabel label(aCertDisplayName);
    filter->SetLabel(label);

    RMPointerArray< CCTCertInfo >* certInfoArray =
        new(ELeave) RMPointerArray< CCTCertInfo >();
    if (iCertInfoArray)
    {
        iCertInfoArray->Close();
        delete iCertInfoArray;
    }
    iCertInfoArray = certInfoArray;

    // Decode Issuer and SerialNumber
    CCMSIssuerAndSerialNumber* issuerAndSerialNumber =
        CCMSIssuerAndSerialNumber::NewL();
    delete iIssuerAndSerialNumber;
    iIssuerAndSerialNumber = issuerAndSerialNumber;

    iIssuerAndSerialNumber->DecodeL(aIssuerAndSerialNumber);

    iState = ERemoveListing;
    iStore->WritableCertStore(storeIndex).
    List(*iCertInfoArray, *filter, iStatus);
    WaitForCompletionL();

    CleanupStack::PopAndDestroy();  // filter

    iCertInfoArray->Close();
    delete iCertInfoArray;
    iCertInfoArray = NULL;

    delete iIssuerAndSerialNumber;
    iIssuerAndSerialNumber = NULL;

    delete iEncodedCert;
    iEncodedCert = NULL;

    return ETrue;
}

void CSTSCredentialManager::StaticRemoveCredentialL(
    TInt aCertDisplayName,
    TInt aIssuerAndSerialNumber,
    TInt aSecurityElementID,
    TInt aSecurityElementPrompt,
    TInt aRetVal)
{
    *(reinterpret_cast<TBool*>(aRetVal)) = RemoveCredentialL(*(reinterpret_cast<TDesC*>(aCertDisplayName)),
                                           *(reinterpret_cast<TDesC8*>(aIssuerAndSerialNumber)),
                                           reinterpret_cast<TDesC*>(aSecurityElementID),
                                           reinterpret_cast<TDesC*>(aSecurityElementPrompt));

}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::RunL
// CActive callback
// -----------------------------------------------------------------------------
//
void CSTSCredentialManager::RunL()
{
    if (iStatus != KErrNone)
    {
        // Error has occured; inform java
        Complete(iStatus.Int());
    }
    else
    {
        switch (iState)
        {
        case EInitialising:
        {
            iState = EKeyStoreInit;
            iKeyStore->Initialize(iStatus);
            SetActive();
            break;
        }
        case ERemoveListing:
        {
            RemoveRetrieveL();
            break;
        }
        case ERemoveRetrieve:
        {
            RemoveVerifyL();
            break;
        }
        case ERemoveDialog:
        {
            Remove();
            break;
        }
        case EAddListing:
        {
            AddRetrieveL();
            break;
        }
        case EAddRetrieve:
        {
            AddVerifyL();
            break;
        }
        case EAddDialog:
        {
            AddL();
            break;
        }
        case EAdding:
        case EDeleting:
        case ECreatingCSR:
        case EKeyStoreInit:
        {
            Complete(KErrNone);
            break;
        }
        case ECSRDialog:
        {
            CreateCSRL();
            break;
        }
        case ESearchingKey:
        {
            CheckSearchResultsL();
            break;
        }
        default:
        {
            Complete(KErrGeneral);
        }
        }
    }
}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::RunError
// CActive error callback
// -----------------------------------------------------------------------------
//
TInt CSTSCredentialManager::RunError(TInt aError)
{
    Complete(aError);
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::DoCancel
// CActive cancellation callback - cancel whatever doing
// -----------------------------------------------------------------------------
//
void CSTSCredentialManager::DoCancel()
{
    switch (iState)
    {
    case EInitialising:
    {
        iStore->CancelInitialize();
        break;
    }
    case EKeyStoreInit:
    {
        iKeyStore->CancelInitialize();
        break;
    }
    case EAdding:
    {
        MCTWritableCertStore& writableStore =
            iStore->WritableCertStore(iWritableCertStoreIndex);
        writableStore.CancelAdd();
        break;
    }
    case EAddListing:
    case ERemoveListing:
    {
        iStore->CancelList();
        break;
    }
    case EAddRetrieve:
    case ERemoveRetrieve:
    {
        iStore->CancelRetrieve();
        break;
    }
    case EDeleting:
    {
        iStore->CancelRemove();
        break;
    }
    case EAddDialog: // AddDialog has nothing to cancel
    case ERemoveDialog: // RemoveDialog has nothing to cancel
    default:
    {
        // nothing to cancel
    }
    }
    iState = EError;
}



// -----------------------------------------------------------------------------
// CSTSCredentialManager::CSTSCredentialManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSCredentialManager::CSTSCredentialManager()
        : CActive(EPriorityStandard),java::util::FunctionServer("MyPKIServer"),
        iState(EInitialising),
        iEncodedCertDes(NULL, 0)
{
    createServerToNewThread();
}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSCredentialManager::ConstructL()
{
    iPKIDialog = PKIDialogFactory::CreateNoteL();

    User::LeaveIfError(iFileServer.Connect());
    iWait = new(ELeave) CActiveSchedulerWait;
    iStore = CUnifiedCertStore::NewL(iFileServer, ETrue);
    iKeyStore = CUnifiedKeyStore::NewL(iFileServer);
    iStore->Initialize(iStatus);
    WaitForCompletionL();

}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::Complete
// Completes the operation
// -----------------------------------------------------------------------------
//
void CSTSCredentialManager::Complete(TInt aError)
{
    if (KErrNone == aError)
    {
        iState = EReady;
    }
    else
    {
        iState = EError;
        iError = aError;
    }
    iWait->AsyncStop();

}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::WaitForCompletionL()
// Wait for completion, leave on error
// -----------------------------------------------------------------------------
//
void CSTSCredentialManager::WaitForCompletionL()
{
    SetActive();
    iWait->Start();
    if (iState != EReady)
    {
        // we need to be ready for the next request
        iState = EReady;
        User::Leave(iError);
    }
}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::RemoveRetrieveL
// Retrieves the certificate to be removed
// -----------------------------------------------------------------------------
//
void CSTSCredentialManager::RemoveRetrieveL()
{
    if (iCertInfoArray->Count() == 0)
    {
        Complete(KErrNotFound);
        return;
    }

    CCTCertInfo* certInfo = (*iCertInfoArray)[ 0 ];

    HBufC8* encodedCert = HBufC8::NewMaxL(certInfo->Size());
    delete iEncodedCert;
    iEncodedCert = encodedCert;

    iEncodedCertDes.Set(iEncodedCert->Des());

    iState = ERemoveRetrieve;
    iStore->Retrieve(*certInfo, iEncodedCertDes, iStatus);
    SetActive();
}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::AddRetrieveL
// Retrieves possible conflicting certificate
// -----------------------------------------------------------------------------
//
void CSTSCredentialManager::AddRetrieveL()
{

    if (iCertInfoArray->Count() == 0)
    {
        // not found, ok to add
        iState = EAddDialog;
        iPKIDialog->SaveCertificate(EX509Certificate, EUserCertificate,
                                    iSubject, iStatus);
        SetActive();
        return;
    }


    CCTCertInfo* certInfo = (*iCertInfoArray)[ 0 ];

    HBufC8* encodedCert = HBufC8::NewMaxL(certInfo->Size());
    delete iEncodedCert;
    iEncodedCert = encodedCert;

    iEncodedCertDes.Set(iEncodedCert->Des());

    iState = EAddRetrieve;
    iStore->Retrieve(*certInfo, iEncodedCertDes, iStatus);
    SetActive();

}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::RemoveVerify
// Verifies that the certificate to be removed has correct issuer and serial
// -----------------------------------------------------------------------------
//
void CSTSCredentialManager::RemoveVerifyL()
{

    TPtr8 certPtr(iEncodedCert->Des());
    CX509Certificate* cert = CX509Certificate::NewLC(certPtr);

    // Check issuer
    const CX500DistinguishedName& certIssuer = cert->IssuerName();
    if (!certIssuer.ExactMatchL(iIssuerAndSerialNumber->IssuerName()))
    {
        // remove current certificate from array, fetch next
        (*iCertInfoArray)[ 0 ]->Release();
        iCertInfoArray->Remove(0);
        RemoveRetrieveL();
    }
    // check serialNumber
    else if (cert->SerialNumber() != iIssuerAndSerialNumber->SerialNumber())
    {
        // remove current certificate from array, fetch next
        (*iCertInfoArray)[ 0 ]->Release();
        iCertInfoArray->Remove(0);
        RemoveRetrieveL();
    }
    else
    {
        // We have a match, show dialog
        RemoveDialog();
    }
    CleanupStack::PopAndDestroy(cert);

}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::AddVerify
// Verifies that the certificate does not have correct serial
// -----------------------------------------------------------------------------
//
void CSTSCredentialManager::AddVerifyL()
{

    TPtr8 certPtr(iEncodedCert->Des());
    CX509Certificate* cert = CX509Certificate::NewLC(certPtr);

    CX509Certificate* certToAdd = CX509Certificate::NewLC(iSubject);

    // check serialNumber
    if (cert->SerialNumber() != certToAdd->SerialNumber())
    {
        // remove current certificate from array, fetch next
        (*iCertInfoArray)[ 0 ]->Release();
        iCertInfoArray->Remove(0);
        AddRetrieveL();
    }
    else
    {
        // We have a match, bail out
        Complete(KSTSErrAlreadyRegistered);
    }
    CleanupStack::PopAndDestroy(2);   // certToAdd, cert

}


// -----------------------------------------------------------------------------
// CSTSCredentialManager::RemoveDialog
// Displays a dialog asing user permission to remove a certificate
// -----------------------------------------------------------------------------
//
void CSTSCredentialManager::RemoveDialog()
{


    CCTCertInfo* certInfo = (*iCertInfoArray)[ 0 ];
    iState = ERemoveDialog;
    iHandle = certInfo->Handle();
    iPKIDialog->DeleteCertificate(iHandle, iStatus);
    SetActive();
}



// -----------------------------------------------------------------------------
// CSTSCredentialManager::Remove
// Remove the found certificate
// -----------------------------------------------------------------------------
//
void CSTSCredentialManager::Remove()
{
    if (iCertInfoArray->Count() == 0)
    {
        Complete(KErrNotFound);
        return;
    }

    CCTCertInfo* certInfo = (*iCertInfoArray)[ 0 ];
    iState = EDeleting;
    iStore->Remove(*certInfo, iStatus);
    SetActive();
}



// -----------------------------------------------------------------------------
// CSTSCredentialManager::AddL
// Adds a certificate
// -----------------------------------------------------------------------------
//
void CSTSCredentialManager::AddL()
{
    MCTWritableCertStore& writableStore =
        iStore->WritableCertStore(iWritableCertStoreIndex);

    iState = EAdding;
    writableStore.Add(
        *iCertDisplayName,
        EX509Certificate,
        EUserCertificate,
        &iSubjectKeyId,
        &iIssuerKeyId,
        iSubject,
        iStatus);
    SetActive();
}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::CSRDialog
// Displays a CSR creation dialog
// -----------------------------------------------------------------------------
void CSTSCredentialManager::CSRDialog()
{
    if (!iDisplayName || !iKeyInfo)
    {
        Complete(KErrGeneral);
        return;
    }
    iState = ECSRDialog;
    iHandle = iKeyInfo->Handle();

    iPKIDialog->CreateCSR(*iDisplayName, iHandle, iStatus);
    SetActive();

}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::CreateCSRL
// Creates a CSR
// -----------------------------------------------------------------------------
void CSTSCredentialManager::CreateCSRL()
{

    if ((!iKeyInfo) || (!iDistinguishedName))
    {
        Complete(KErrGeneral);
        return;
    }

    iRequest = CPKCS10Request::NewL(*iDistinguishedName, *iKeyInfo);

    delete iRequestEncoded;
    iRequestEncoded = NULL;

    iState = ECreatingCSR;
    iRequest->CreateEncoding(iRequestEncoded, iStatus);
    SetActive();
}


// -----------------------------------------------------------------------------
// CSTSCredentialManager::CheckSearchResultsL
// Check key search results, generate CSR
// -----------------------------------------------------------------------------
void CSTSCredentialManager::CheckSearchResultsL()
{
    // we take the first key with valid dates
    TInt keyCount = iKeyInfoArray->Count();
    CCTKeyInfo* keyInfo = NULL;
    TTime timeNow;
    timeNow.UniversalTime();

    TBool found = EFalse;
    TInt keyIndex = 0;

    for (TInt i = 0; (i < keyCount) && (!found); i++)
    {
        keyInfo = iKeyInfoArray->operator[](i);
        TTime startDate = keyInfo->StartDate();
        if ((startDate.Int64() == 0) || (timeNow >= startDate))
        {
            // startDate was not set or is before current time
            // exactly same time is valid as well
            TTime endDate = keyInfo->EndDate();
            if ((endDate.Int64() == 0) || (timeNow < endDate))
            {
                // endDate was not set or is after current time
                // exactly same time is not valid
                // (the key expires immediatelly)
                if (keyInfo->Size() == iKeyLen)
                {
                    // key must have same size
                    found = ETrue;
                    keyIndex = i;
                }
            }
        }
    }
    if (!found)
    {
        Complete(KSTSErrSeNoKeys);
        return;
    }

    if (iKeyInfo)
    {
        iKeyInfo->Release();
    }
    iKeyInfo = keyInfo;
    // iKeyInfo is no longer owned by iKeyInfoArray
    iKeyInfoArray->Remove(keyIndex);

    CSRDialog();
}


// -----------------------------------------------------------------------------
// CSTSCredentialManager::CheckKeyStoreSEIDL
// Check if given SecurityElementID is available, prompts user if needed
// -----------------------------------------------------------------------------
TInt CSTSCredentialManager::CheckKeyStoreSEIDL(
    const TDesC* aSecurityElementID,
    const TDesC* aSecurityElementPrompt)
{

    // Symbian store can't be used
    if (aSecurityElementID &&
            (KSTSSymbianKeyStoreLabel() == *aSecurityElementID))
    {
        User::Leave(KSTSErrSeNotFound);
    }


    TInt retVal = 0;
    TInt keyStoreCount = iKeyStore->KeyStoreCount();
    TBool found = EFalse;
    TInt i = 0;
    for (i = 0; (i < keyStoreCount) && (!found); i++)
    {
        // if aSecurityElementID is NULL, use the first one, otherwise first
        // with the same label
        const TDesC& storeLabel = iKeyStore->KeyStore(i).Token().Label();
        if (!aSecurityElementID ||
                (*aSecurityElementID == KNullDesC()) ||
                storeLabel == *aSecurityElementID)
        {
            if (storeLabel != KSTSSymbianKeyStoreLabel())
            {
                found = ETrue;
                retVal = i;
            }
        }
    }

    // if not found, display prompt( if not NULL ) and search again
    if (!found && aSecurityElementPrompt)
    {
        CSTSSEPrompt* prompt = CSTSSEPrompt::NewLC();
        prompt->DisplayPromptL(*aSecurityElementPrompt);
        keyStoreCount = iKeyStore->KeyStoreCount();
        for (i = 0; (i < keyStoreCount) && (!found); i++)
        {
            if (iKeyStore->KeyStore(i).Token().Label() ==
                    *aSecurityElementID)
            {
                found = ETrue;
                retVal = i;
            }
        }
    }

    if (!found)
    {
        User::Leave(KSTSErrSeNotFound);
    }

    return retVal;
}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::CheckWriteCertStoreSEIDL
// Check if given SecurityElementID is available, prompts user if needed
// -----------------------------------------------------------------------------
TInt CSTSCredentialManager::CheckWriteCertStoreSEIDL(
    const TDesC* aSecurityElementID,
    const TDesC* aSecurityElementPrompt)
{
    // If aSecurityElementID is NULL, use the default Symbian cert store.
    TPtrC securityElementID(KSTSSymbianCertificateStoreLabel());
    if (aSecurityElementID &&
            *aSecurityElementID != KNullDesC())
    {
        securityElementID.Set(*aSecurityElementID);
    }

    TInt retVal = 0;
    TInt certStoreCount = iStore->WritableCertStoreCount();
    TBool found = EFalse;
    TInt i = 0;
    for (i = 0; (i < certStoreCount) && (!found); i++)
    {
        // Select the first store with the specified label.
        const TDesC& storeLabel =
            iStore->WritableCertStore(i).Token().Label();

        if (storeLabel == securityElementID)
        {
            found = ETrue;
            retVal = i;
        }
    }

    // if not found, display prompt( if not NULL ) and search again
    if (!found && aSecurityElementPrompt)
    {
        CSTSSEPrompt* prompt = CSTSSEPrompt::NewLC();
        prompt->DisplayPromptL(*aSecurityElementPrompt);
        CleanupStack::PopAndDestroy(prompt);
        certStoreCount = iStore->CertStoreCount();
        for (i = 0; (i < certStoreCount) && (!found); i++)
        {
            if (iStore->CertStore(i).Token().Label() ==
                    securityElementID)
            {
                found = ETrue;
                retVal = i;
            }
        }
    }

    if (!found)
    {
        User::Leave(KSTSErrSeNotFound);
    }

    return retVal;
}

// -----------------------------------------------------------------------------
// CSTSCredentialManager::CheckOIDL
// Checks if given OID is formatted correctly
// -----------------------------------------------------------------------------
void CSTSCredentialManager::CheckOIDL(const TDesC& aOID)
{
    TInt oidLength = aOID.Length();
    if (oidLength == 0)
    {
        // empty oid is illegal
        User::Leave(KErrArgument);
    }
    for (TInt i = 0; i < oidLength; i++)
    {
        TChar oidChar = aOID[ i ];
        if (!oidChar.IsAlphaDigit())
        {
            if ((oidChar != KSTSDot) && (oidChar != KSTSLine))
            {
                User::Leave(KErrArgument);
            }
        }
    }
}

//  End of File
void CSTSCredentialManager::vmAttached()
{

}

void CSTSCredentialManager::doServerSideInit()
{

    FunctionServer::doServerSideInit();

}


