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

#include "cstssignatureservice.h"
#include "tstsdistinguishednameconverter.h"
#include "secdlg.h"
#include <unifiedcertstore.h>
#include <asn1enc.h>
#include <x500dn.h>
#include <charconv.h>
#include <unifiedkeystore.h>
#include <mctwritablecertstore.h>
#include <signed.h>
#include <asymmetric.h>
#include <CCMSX509Certificate.h>
#include <CCMSSignerInfo.h>
#include <CCMSEncapsulatedContentInfo.h>
#include <CCMSSignedData.h>

#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
#include <PKIDlg.h>
#endif

#include "cstsseprompt.h"

#include <hash.h>
#include "fs_methodcall.h"
#include <jutils.h>
#include "satsajnitools.h"
#include "logger.h"
#include "jstringutils.h"


const TInt KDefaultGranularity = 1;

const TInt KOptionIncludeContent = 1;
const TInt KOptionIncludeCertificate = 2;

_LIT(KIdData, "1.2.840.113549.1.7.1");
_LIT(KIdSignedData, "1.2.840.113549.1.7.2");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSSignatureService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSSignatureService* CSTSSignatureService::NewL()
{
    CSTSSignatureService* self = new(ELeave) CSTSSignatureService;
    CallMethodL(self, &CSTSSignatureService::AddToScheduler, self);
    CleanupStack::PushL(self);
    CallMethodL(self, &CSTSSignatureService::ConstructL, self);
    CleanupStack::Pop(self);
    return self;
}

void CSTSSignatureService::AddToScheduler()
{
    CActiveScheduler::Add(this);
}

// Destructor
CSTSSignatureService::~CSTSSignatureService()
{
    if (iDialog)
    {
        iDialog->Release(); // Release deletes the object
    }
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS	
    if (iPKIDialog)
    {
        iPKIDialog->Release();
    }
#endif	
    iCertificateArray.Close();
    iCertInfos.Close();
    delete iFilter;
    iDERNamesHBufC.ResetAndDestroy();
    iDERNames.Close();
    delete iTextToDisplay;
    if (iCertificate)
    {
        iCertificate->Release();
    }
    delete iX509Certificate;
    if (iRSASigner)
    {
        iRSASigner->Release();
    }
    if (iDSASigner)
    {
        iDSASigner->Release();
    }
    delete iCertStore;
    delete iKeyStore;
    iKeys.Close();
    delete iRSASignature;
    delete iDSASignature;
    delete iSignature;
    iFileServer.Close();
    delete iWait;
    delete iSecurityElementPrompt;
    delete iSignerInfo;
    delete iEncodedSignedAttributesHash;
    delete iCMSCertificate;
}

// -----------------------------------------------------------------------------
// CSTSSignatureService::AuthenticateL
// Authenticates given data
// -----------------------------------------------------------------------------
//
HBufC8* CSTSSignatureService::AuthenticateL(const TDesC8& abytesToAuthenticate,
        TInt aOptions, const CDesCArray& aCaNames,
        const TDesC& asecurityElementPrompt, TBool aShowData)
{
    if (iState != EReady)
    {
        ELOG(ESATSA, "CSTSSignatureService::AuthenticateL: Not Ready");
        User::Leave(KErrNotReady);
    }

    // First we need to convert the data to unicode, if we have to display it
    HBufC* textToDisplay = NULL;
    if (aShowData)
    {
        textToDisplay = ConvertUTF8ToUnicodeL(abytesToAuthenticate);
        iAuthType = EAuthWithText;
    }
    else
    {
        textToDisplay = KNullDesC().AllocL();
        iAuthType = EAuthWithoutText;
    }

    delete iTextToDisplay;
    iTextToDisplay = textToDisplay;

    iMessage.Set(abytesToAuthenticate);

    iOptions = aOptions;

    iShowNotes = EFalse;
    // get handles to applicable certificates
    HBufC8* retVal = CreateSignatureL(aCaNames, EX509UsageDigitalSignature,
                                      asecurityElementPrompt);
    return retVal;
}

TInt CSTSSignatureService::StaticAuthenticateL(JNIEnv* aJniEnv,
        CSTSSignatureService* service, jbyteArray aByteArrayToAuthenticate,
        jint aOptions, jobjectArray aCaNames, jstring aSecurityElementPrompt,
        jboolean aShowData, HBufC8** aRetVal)
{
    const TInt byteArrayLength = aJniEnv->GetArrayLength(
                                     aByteArrayToAuthenticate);
    jbyte* bytesToAuthenticate = aJniEnv->GetByteArrayElements(
                                     aByteArrayToAuthenticate, NULL);

    if (!bytesToAuthenticate)
    {
        ELOG(ESATSA, "CSTSSignatureService::StaticAuthenticateL: No memory,return");
        return NULL;
    }

    TPtrC8 desToAuthenticate(reinterpret_cast<TUint8*>(bytesToAuthenticate),
                             byteArrayLength);

    CDesCArrayFlat* nativeCaNames = STSCreateNativeStringArrayL(aJniEnv,
                                    aCaNames);

    const JStringUtils securityElementPrompt(*aJniEnv, aSecurityElementPrompt);

    TBool ShowData = static_cast<TBool>(aShowData);
    const TDesC* sec = static_cast<const TDesC*>(&securityElementPrompt);

    TRAPD(err, CallMethodL(*aRetVal, service,
                           &CSTSSignatureService::AuthenticateL, desToAuthenticate, aOptions,
                           *nativeCaNames, *sec, ShowData, service));

    return err;

}

// -----------------------------------------------------------------------------
// CSTSSignatureService::SignL

// Signs given data
// -----------------------------------------------------------------------------

HBufC8* CSTSSignatureService::SignL(const TDesC8& aBytesToSign, TInt aOptions,
                                    const CDesCArray& aCaNames, const TDesC& aSecurityElementPrompt)
{
    if (iState != EReady)
    {
        ELOG(ESATSA, "CSTSSignatureService::SignL: Not Ready");
        User::Leave(KErrNotReady);
    }

    // convert text from UTF8
    HBufC* textToDisplay = ConvertUTF8ToUnicodeL(aBytesToSign);
    delete iTextToDisplay;
    iTextToDisplay = textToDisplay;

    iAuthType = ESignWithText;

    iMessage.Set(aBytesToSign);

    iOptions = aOptions;

    iShowNotes = ETrue;

    // get handles to applicable certificates
    HBufC8* retVal = CreateSignatureL(aCaNames, EX509UsageNonRepudiation,
                                      aSecurityElementPrompt);

    return retVal;
}

// -----------------------------------------------------------------------------
// CSTSSignatureService::StaticSignL
// Authenticates given data
// -----------------------------------------------------------------------------
//
TInt CSTSSignatureService::StaticSignL(JNIEnv* aJniEnv,
                                       CSTSSignatureService* service, jbyteArray aBytesToSign, jint aOptions,
                                       jobjectArray aCaNames, jstring aSecurityElementPrompt, HBufC8** aRetVal)
{
    const TInt byteArrayLength = aJniEnv->GetArrayLength(aBytesToSign);
    jbyte* bytesToSign = aJniEnv->GetByteArrayElements(aBytesToSign, NULL);

    if (!bytesToSign)
    {
        ELOG(ESATSA, "CSTSSignatureService::StaticSignL: No memory");
        return NULL;
    }

    TPtrC8 desToSign(reinterpret_cast<TUint8*>(bytesToSign), byteArrayLength);

    CDesCArrayFlat* nativeCaNames = STSCreateNativeStringArrayL(aJniEnv,
                                    aCaNames);

    const JStringUtils securityElementPrompt(*aJniEnv, aSecurityElementPrompt);

    
    const TDesC* sec = static_cast<const TDesC*>(&securityElementPrompt);

    TRAPD(err, CallMethodL(*aRetVal, service, &CSTSSignatureService::SignL,
                           desToSign, aOptions, *nativeCaNames, *sec, service));

    return err;
}

// -----------------------------------------------------------------------------
// CSTSSignatureService::RunL
// CActive callback
// -----------------------------------------------------------------------------
//
void CSTSSignatureService::RunL()
{
    if (iStatus == KErrCancel)
    {
        if (iShowNotes)
        {
            iState = EFinalNote;
            iStatus = KErrNone;
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS			
            iPKIDialog->Note(MPKIDialog::ESigningCancelled, iStatus);
#endif			
            SetActive();
         }
        else
        {
             Complete(KErrNone);
        }
    }
    else if (iStatus != KErrNone)
    {
        ELOG(ESATSA, "CSTSSignatureService::RunL: Error occured");
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
        case EKeyStoreInit:
        {
            Complete(KErrNone);
            break;
        }
        case EListing:
        {
            HandlesFromCertInfosL();
            break;
        }
        case EDialog:
        {
            iState = EGetCert;
            if (iCertificate)
            {
                iCertificate->Release();
                iCertificate = NULL;
            }
            // if there is only 1 handle, the dialog won't return it.
            if (iCertificateArray.Count() == 1)
            {
                iCertificateHandle = iCertificateArray[0];
            }

            iCertStore->GetCert(iCertificate, iCertificateHandle, iStatus);
            SetActive();
            break;
        }
        case EGetCert:
        {
            if (iX509Certificate)
            {
                delete iX509Certificate;
                iX509Certificate = NULL;
            }
            iState = ERetrieveCert;
            iCertStore->Retrieve(*iCertificate, iX509Certificate, iStatus);
            SetActive();
            break;
        }
        case ERetrieveCert:
        {
            FindKeyL();
            break;
        }
        case EFindKey:
        {
            OpenSignerL();
            break;
        }
        case EOpenSigner:
        {
            SignL();
            break;
        }
        case ESign:
        {
            CreateSignedDataL();
            break;
        }
        case EFinalNote:
        {
            Complete(KErrNone);
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
// CSTSSignatureService::RunError
// CActive error callback
// -----------------------------------------------------------------------------
//
TInt CSTSSignatureService::RunError(TInt aError)
{
    Complete(aError);
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CSTSSignatureService::DoCancel
// CActive cancellation callback
// -----------------------------------------------------------------------------
//
void CSTSSignatureService::DoCancel()
{
    switch (iState)
    {
    case EInitialising:
    {
        iCertStore->CancelInitialize();
        break;
    }
    case EKeyStoreInit:
    {
        iKeyStore->CancelInitialize();
        break;
    }
    case EListing:
    {
        iCertStore->CancelList();
        break;
    }
    case EDialog:
    {
        iDialog->Cancel();
        break;
    }
    case EGetCert:
    {
        iCertStore->CancelGetCert();
        break;
    }
    case ERetrieveCert:
    {
        iCertStore->CancelRetrieve();
        break;
    }
    case EFindKey:
    {
        iKeyStore->CancelList();
        break;
    }
    case EOpenSigner:
    {
        iKeyStore->CancelOpen();
        break;
    }
    case ESign:
    {
        if (iRSASigner)
        {
            iRSASigner->CancelSign();
        }
        if (iDSASigner)
        {
            iDSASigner->CancelSign();
        }
        break;
    }
    default:
    {
        // nothing to cancel
    }
    }
    iState = EERROR;
    iError = KErrCancel;
}

// -----------------------------------------------------------------------------
// CSTSSignatureService::CSTSSignatureService
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSSignatureService::CSTSSignatureService() :
        CActive(EPriorityStandard), java::util::FunctionServer("MySignatureServer"),
        iState(EInitialising)
{

    createServerToNewThread();

}

// -----------------------------------------------------------------------------
// CSTSSignatureService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSSignatureService::ConstructL()
{
    User::LeaveIfError(iFileServer.Connect());
    iDialog = SecurityDialogFactory::CreateL();
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS	
    iPKIDialog = PKIDialogFactory::CreateNoteL();
#endif
    iFilter = CCertAttributeFilter::NewL();

    iWait = new(ELeave) CActiveSchedulerWait;
    iCertStore = CUnifiedCertStore::NewL(iFileServer, ETrue);
    iKeyStore = CUnifiedKeyStore::NewL(iFileServer);
    iCertStore->Initialize(iStatus);
    WaitForCompletionL();
}

// -----------------------------------------------------------------------------
// CSTSSignatureService::Complete
// Completes the operation
// -----------------------------------------------------------------------------
//
void CSTSSignatureService::Complete(TInt aError)
{
    if (KErrNone == aError)
    {
        iState = EReady;
    }
    else
    {
        iState = EERROR;
        iError = aError;
    }
    iWait->AsyncStop();
    
}

// -----------------------------------------------------------------------------
// CSTSSignatureService::WaitForCompletionL()
// Wait for completion, leave on error
// -----------------------------------------------------------------------------
//
void CSTSSignatureService::WaitForCompletionL()
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
// CSTSSignatureService::CreateSignatureL()
// Creates the signature
// -----------------------------------------------------------------------------
//
HBufC8* CSTSSignatureService::CreateSignatureL(const CDesCArray& aCaNames,
        const TKeyUsageX509 aUsage, const TDesC& aSecurityElementPrompt)
{
    HBufC* sePrompt = NULL;
    if (aSecurityElementPrompt != KNullDesC())
    {
        sePrompt = aSecurityElementPrompt.AllocL();
    }
    delete iSecurityElementPrompt;
    iSecurityElementPrompt = sePrompt;
    FindCertificatesL(aCaNames, aUsage);
    WaitForCompletionL();
    HBufC8* retVal = iSignature;
    iSignature = NULL;
    return retVal;
}
// -----------------------------------------------------------------------------
// CSTSSignatureService::FindCertificatesL()
// Find applicable certificates
// -----------------------------------------------------------------------------
//
void CSTSSignatureService::FindCertificatesL(const CDesCArray& aCaNames,
        const TKeyUsageX509 aUsage)
{

    iFilter->SetKeyUsage(aUsage);
    iFilter->SetOwnerType(EUserCertificate);
    iFilter->SetFormat(EX509Certificate);

    iCertInfos.Close();

    TInt caNameCount = aCaNames.Count();
    if (0 == caNameCount)
    {
        // no caNames, so we may use any.
        iState = EListing;
        iCertStore->List(iCertInfos, *iFilter, iStatus);
    }
    else
    {
        // first we need to convert aCaNames to DER encoding
        // make sure the array is empty
        iDERNames.Reset();
        iDERNamesHBufC.ResetAndDestroy();

        TSTSDistinguishedNameConverter dnConverter;

        for (TInt i = 0; i < caNameCount; i++)
        {
            CX500DistinguishedName* dName = dnConverter.CreateDNL(aCaNames[i]);
            CleanupStack::PushL(dName);

            CASN1EncSequence* dnEncoder = dName->EncodeASN1LC();
            // encode the object in a DER encoding
            HBufC8* der = HBufC8::NewMaxLC(dnEncoder->LengthDER());
            TPtr8 pder(der->Des());
            TUint pos = 0;
            dnEncoder->WriteDERL(pder, pos);
            User::LeaveIfError(iDERNamesHBufC.Append(der));
            CleanupStack::Pop(der);
            User::LeaveIfError(iDERNames.Append(der));
            CleanupStack::PopAndDestroy(2); // dnEncoder, dName
        }

        iState = EListing;
        iCertStore->List(iCertInfos, *iFilter, iDERNames, iStatus);
    }
    
}

// -----------------------------------------------------------------------------
// CSTSSignatureService::HandlesFromCertInfosL()
// Fills iCertificateArray from iCertInfos.
// -----------------------------------------------------------------------------
//
void CSTSSignatureService::HandlesFromCertInfosL()
{
    TInt certificateCount = iCertInfos.Count();
    if (certificateCount == 0)
    {
        ELOG(ESATSA, "CSTSSignatureService::HandlesFromCertInfosL, KErrNotFound");
        User::Leave(KErrNotFound);
    }

    iCertificateArray.Reset();
    for (TInt i = 0; i < certificateCount; i++)
    {
        User::LeaveIfError(iCertificateArray.Append(iCertInfos[i]->Handle()));
    }

    // next is a dialog shown depending on the authentication type
    switch (iAuthType)
    {
    case EAuthWithoutText:
    {
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS        
        iPKIDialog->UserAuthentication(iCertificateArray, iCertificateHandle,
                                       iStatus);
#endif 
        break;
    }
    case EAuthWithText:
    {
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
        iPKIDialog->UserAuthenticationText(*iTextToDisplay, iCertificateArray,
                                           iCertificateHandle, iStatus);
#endif
        break;
    }
    case ESignWithText:
    {
        iDialog->SignText(*iTextToDisplay, iCertificateArray,
                          iCertificateHandle, iStatus);
        break;
    }
    default:
    {

    }
    }
    iState = EDialog;
    SetActive();

}

// -----------------------------------------------------------------------------
// CSTSSignatureService::FindKeyL()
// Finds the key used in iCertificate.
// -----------------------------------------------------------------------------
//
void CSTSSignatureService::FindKeyL()
{
    iKeyFilter.iKeyAlgorithm = CKeyInfoBase::EInvalidAlgorithm;
    iKeyFilter.iKeyId = iCertificate->SubjectKeyId();

    iKeyFilter.iUsage = EPKCS15UsageAll;
    iKeys.Close();
    iState = EFindKey;
    iKeyStore->List(iKeys, iKeyFilter, iStatus);
    SetActive();

}

// -----------------------------------------------------------------------------
// CSTSSignatureService::OpenSignerL()
// Opens a signer for the key in iKeys.
// -----------------------------------------------------------------------------
//
void CSTSSignatureService::OpenSignerL()
{
    // if key is not found, display security element prompt and search again
    if (iKeys.Count() == 0)
    {
        if (iSecurityElementPrompt)
        {
            CSTSSEPrompt* prompt = CSTSSEPrompt::NewLC();
            prompt->DisplayPromptL(*iSecurityElementPrompt);
            CleanupStack::PopAndDestroy(prompt);
            // delete prompt so we only display it once.
            delete iSecurityElementPrompt;
            iSecurityElementPrompt = NULL;
            FindKeyL();
            return;
        }
        else
        {
            User::Leave(KErrNotFound);
        }
    }

    CCTKeyInfo* keyInfo = iKeys[0];

    if (iRSASigner)
    {
        iRSASigner->Release();
        iRSASigner = NULL;
    }
    if (iDSASigner)
    {
        iDSASigner->Release();
        iDSASigner = NULL;
    }

    iState = EOpenSigner;
    switch (keyInfo->Algorithm())
    {
    case CCTKeyInfo::ERSA:
    {
        iKeyStore->Open(keyInfo->Handle(), iRSASigner, iStatus); // codescanner::open
        break;
    }
    case CCTKeyInfo::EDSA:
    {
        iKeyStore->Open(keyInfo->Handle(), iDSASigner, iStatus); // codescanner::open
        break;
    }
    default:
    {
        User::Leave(KErrNotSupported);
    }
    }
    SetActive();

}

// -----------------------------------------------------------------------------
// CSTSSignatureService::SignL()
// Signs the message
// -----------------------------------------------------------------------------
//
void CSTSSignatureService::SignL()
{
    // first we create a digest of the message

    CSHA1* sha = CSHA1::NewL();
    CleanupStack::PushL(sha);
    TPtrC8 hash = sha->Hash(iMessage);

    CCMSX509Certificate* cmsCertificate = CCMSX509Certificate::NewL(
                                              static_cast<CX509Certificate&>(*iX509Certificate));
    delete iCMSCertificate;
    iCMSCertificate = cmsCertificate;
    CCMSSignerInfo* signerInfo = CCMSSignerInfo::NewL(*cmsCertificate, hash);
    delete iSignerInfo;
    iSignerInfo = signerInfo;

    CleanupStack::PopAndDestroy(sha);

    HBufC8* encodedSignedAttributes = iSignerInfo->SignedAttributesEncodedL();
    CleanupStack::PushL(encodedSignedAttributes);

    sha = CSHA1::NewL();
    CleanupStack::PushL(sha);

    TPtrC8 attributeHash = sha->Hash(*encodedSignedAttributes);

    // create digest info
    CASN1EncSequence* digestInfo = CASN1EncSequence::NewLC();
    CASN1EncSequence* digestAlgId = CASN1EncSequence::NewLC();
    CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC(KSHA1);
    digestAlgId->AddAndPopChildL(oid);
    CASN1EncNull* params = CASN1EncNull::NewLC();
    digestAlgId->AddAndPopChildL(params);
    digestInfo->AddAndPopChildL(digestAlgId);
    CASN1EncOctetString* hashEncoder =
        CASN1EncOctetString::NewLC(attributeHash);
    digestInfo->AddAndPopChildL(hashEncoder);

    HBufC8* encodedHash = HBufC8::NewMaxL(digestInfo->LengthDER());
    delete iEncodedSignedAttributesHash;
    iEncodedSignedAttributesHash = encodedHash;
    iEncodedSignedAttributesHashPointer.Set(*encodedHash);
    TPtr8 pder(encodedHash->Des());
    TUint pos = 0;
    digestInfo->WriteDERL(pder, pos);

    CleanupStack::PopAndDestroy(digestInfo);
    CleanupStack::PopAndDestroy(sha);
    CleanupStack::PopAndDestroy(encodedSignedAttributes);

    CCTKeyInfo* keyInfo = iKeys[0];

    delete iRSASignature;
    iRSASignature = NULL;
    delete iDSASignature;
    iDSASignature = NULL;

    iState = ESign;
    switch (keyInfo->Algorithm())
    {
    case CCTKeyInfo::ERSA:
    {
        iRSASigner->Sign(iEncodedSignedAttributesHashPointer, iRSASignature,
                         iStatus);
        break;
    }
    case CCTKeyInfo::EDSA:
    {
        iDSASigner->Sign(iEncodedSignedAttributesHashPointer, iDSASignature,
                         iStatus);
        break;
    }
    default:
    {
        User::Leave(KErrNotSupported);
    }
    }

    SetActive();

}

// -----------------------------------------------------------------------------
// CSTSSignatureService::CreateSignedDataL()
// Creates signed data
// -----------------------------------------------------------------------------
//
void CSTSSignatureService::CreateSignedDataL()
{
    CCTKeyInfo* keyInfo = iKeys[0];

    HBufC8* signature = NULL;
    switch (keyInfo->Algorithm())
    {
    case CCTKeyInfo::ERSA:
    {

        signature = iRSASignature->S().BufferLC();
        break;
    }
    case CCTKeyInfo::EDSA:
    {

        CASN1EncSequence* signatureEncoder = CASN1EncSequence::NewLC();
        CASN1EncBigInt* rEncoder = CASN1EncBigInt::NewLC(iDSASignature->R());
        signatureEncoder->AddAndPopChildL(rEncoder);
        CASN1EncBigInt* sEncoder = CASN1EncBigInt::NewLC(iDSASignature->S());
        signatureEncoder->AddAndPopChildL(sEncoder);
        signature = HBufC8::NewMaxLC(signatureEncoder->LengthDER());
        TPtr8 signatureDes = signature->Des();
        TUint pos = 0;
        signatureEncoder->WriteDERL(signatureDes, pos);
        CleanupStack::Pop(signature);
        CleanupStack::PopAndDestroy(signatureEncoder);
        CleanupStack::PushL(signature);
        break;
    }
    default:
    {
        User::Leave(KErrNotSupported);
    }
    }

    iSignerInfo->SetSignatureValueL(*signature);
    CArrayPtr<CCMSSignerInfo>* signerInfos = new(ELeave) CArrayPtrFlat<
    CCMSSignerInfo> (KDefaultGranularity);
    CleanupStack::PushL(signerInfos);
    signerInfos->AppendL(iSignerInfo);

    CArrayPtr<CCMSX509AlgorithmIdentifier>
    * digestAlgorithmIdentifiers = new(ELeave) CArrayPtrFlat<
    CCMSX509AlgorithmIdentifier> (KDefaultGranularity);
    CleanupStack::PushL(digestAlgorithmIdentifiers);

    CCMSX509AlgorithmIdentifier
    * digestAlgorithm =
        CCMSX509AlgorithmIdentifier::NewL(
            iSignerInfo->DigestAlgorithmIdentifier().AlgorithmIdentifier());
    CleanupStack::PushL(digestAlgorithm);

    digestAlgorithmIdentifiers->AppendL(digestAlgorithm);

    CCMSEncapsulatedContentInfo* contentInfo = NULL;
    if ((iOptions & KOptionIncludeContent) != 0)
    {
        contentInfo = CCMSEncapsulatedContentInfo::NewLC(KIdData(), &iMessage);
    }
    else
    {
        contentInfo = CCMSEncapsulatedContentInfo::NewLC(KIdData(), NULL);
    }

    CCMSSignedData* signedData = CCMSSignedData::NewLC(
                                     *digestAlgorithmIdentifiers, *contentInfo, *signerInfos);

    if ((iOptions & KOptionIncludeCertificate) != 0)
    {
        CCMSCertificateChoices* choices = CCMSCertificateChoices::NewLC(
                                              *iCMSCertificate);
        CArrayPtr<CCMSCertificateChoices>* certificates =
            new(ELeave) CArrayPtrFlat<CCMSCertificateChoices> (1);
        CleanupStack::PushL(certificates);
        certificates->AppendL(choices);
        signedData->SetCertificatesL(certificates);
        CleanupStack::PopAndDestroy(2); // certificates, choices
    }

    HBufC8* plainSignature = NULL;
    signedData->EncodeL(plainSignature);
    CleanupStack::PushL(plainSignature);

    CCMSContentInfo* encapsulatedSignature = CCMSContentInfo::NewLC(
                KIdSignedData(), *plainSignature);

    delete iSignature;
    iSignature = NULL;

    encapsulatedSignature->EncodeL(iSignature);

    CleanupStack::PopAndDestroy(8);
    if (iShowNotes)
    {
        iState = EFinalNote;
#ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS		
        iPKIDialog->Note(MPKIDialog::ESignatureDone, iStatus);
#endif		
        SetActive();
    }
    else
    {
        Complete(KErrNone);
    }

}

// -----------------------------------------------------------------------------
// CSTSSignatureService::ConvertUTF8ToUnicodeLC()
// Converts UTF8 data to Unicode
// -----------------------------------------------------------------------------
//
HBufC* CSTSSignatureService::ConvertUTF8ToUnicodeL(const TDesC8& aUTF8String)
{
    CCnvCharacterSetConverter* converter = CCnvCharacterSetConverter::NewLC();

    CCnvCharacterSetConverter::TAvailability charSetAvailable =
        converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8,
                                             iFileServer);

    if (CCnvCharacterSetConverter::EAvailable != charSetAvailable)
    {
        User::Leave(KErrGeneral);
    }

    TInt state = CCnvCharacterSetConverter::KStateDefault;

    HBufC* textToDisplay = HBufC::NewMaxLC(aUTF8String.Length());

    TPtr16 textToDisplayDes = textToDisplay->Des();

    TInt error = converter->ConvertToUnicode(textToDisplayDes, aUTF8String,
                 state);
    User::LeaveIfError(error);
    CleanupStack::Pop(textToDisplay);
    CleanupStack::PopAndDestroy(converter);
    return textToDisplay;
}

//  End of File

void CSTSSignatureService::vmAttached()
{

}

void CSTSSignatureService::doServerSideInit()
{

    FunctionServer::doServerSideInit();

}


