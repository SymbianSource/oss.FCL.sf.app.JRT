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


#ifndef CSTSSIGNATURESERVICE_H
#define CSTSSIGNATURESERVICE_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <mctkeystore.h>
#include "functionserver.h"

// CONSTANTS



// FORWARD DECLARATIONS
class MSecurityDialog;
class MPKIDialog;
class CUnifiedCertStore;
class CUnifiedKeyStore;
class CCTCertInfo;
class CCertAttributeFilter;
class CCTKeyInfo;
class CCertificate;
class CCMSSignerInfo;
class CCMSX509Certificate;
using namespace java::util;

// CLASS DECLARATION
/**
*  Provides signature services for cryptographic messages.
*  This class can be used for creating signatures for authentication
*  and non-repudiation purposes.
*
*/
NONSHARABLE_CLASS(CSTSSignatureService): public CActive, public FunctionServer
{

public: // type definitions
    enum TState
    {
        EReady,
        EInitialising,
        EKeyStoreInit,
        EListing,
        EDialog,
        EGetCert,
        ERetrieveCert,
        EFindKey,
        EOpenSigner,
        ESign,
        EFinalNote,
        EERROR
    };

    enum TAuthDialogType
    {
        EAuthWithoutText,
        EAuthWithText,
        ESignWithText
    };

public:  // Constructors and destructor

    /**
        * Two-phased constructor.
        */
    static CSTSSignatureService* NewL();

    /**
        * Destructor.
        */
    virtual ~CSTSSignatureService();

public: // New functions

    /**
        * Authenticates given data.
        * @param aBytesToAuthenticate bytes to be authenticated
        * @param aOptions bitmask of options
        * @param aCaNames array of distinguished names of certificate
        * authorities which are trusted to issue certificates which
        * may be used for authenticating a user.
        * @param aSecurityElementPrompt shown if the user needs to
        * insert a security element.
        * @param aShowData if true, the data is shown to the user
        * before authentication.
        * @return DER encoded signature
        */
    HBufC8* AuthenticateL(const TDesC8& abytesToAuthenticate,
                          TInt aOptions,
                          const CDesCArray& acaNames,
                          const TDesC& asecurityElementPrompt,
                          TBool aShowData);

    static TInt StaticAuthenticateL(
        JNIEnv* aJniEnv,
        CSTSSignatureService* service,
        jbyteArray aByteArrayToAuthenticate,
        jint aOptions,
        jobjectArray aCaNames,
        jstring aSecurityElementPrompt,
        jboolean aShowData,
        HBufC8** aRetVal);


    /**
    * Signs given data.
    * @param aBytesToSign bytes to be authenticated
    * @param aOptions bitmask of options
    * @param aCaNames array of distinguished names of certificate
    * authorities which are trusted to issue certificates which
    * may be used for authenticating a user.
    * @param aSecurityElementPrompt shown if the user needs to
    * insert a security element.
    * @return DER encoded signature
    */
    HBufC8* SignL(const TDesC8& aBytesToSign,
                  TInt aOptions,
                  const CDesCArray& aCaNames,
                  const TDesC& aSecurityElementPrompt);


    // Wrapper function to use TRAP and Calmethod() of FunctionServer
    static TInt StaticSignL(JNIEnv* aJniEnv,
                            CSTSSignatureService* service,
                            jbyteArray aBytesToAuthenticate,
                            jint aOptions,
                            jobjectArray aCaNames,
                            jstring aSecurityElementPrompt,
                            HBufC8** aRetVal);


public: // CActive

    /**
        * Handles an active object's request completion event.
        */
    void RunL();

    /**
        * Handles a leave occurring in the request completion event
        * handler RunL().
        */
    TInt RunError(TInt aError);

    /**
        * Implements cancellation of an outstanding request.
        */
    void DoCancel();

private:

    /**
        * C++ default constructor.
        */
    CSTSSignatureService();

    /**
        * By default Symbian 2nd phase constructor is private.
        */
    void ConstructL();

    /**
        * Completes current asynchronic operation.
        * If aError is not KErrNone, sets iError before calling
        * iWait->AsyncStop().
        * @param aError return value from the completed operation.
        */
    void Complete(TInt aError);

    /**
        * Waits for the current asynchronic operation to complete.
        * Leaves with the error code if the asynchronic operation fails.
        */
    void WaitForCompletionL();

    /**
        * Creates a signature.
        * @param aCaNames distinguished names for certificate
        * authorities which are trusted to sign certificates.
        * @param aUsage Usage of the certificates (either
        * EX509UsageDigitalSignature or EX509UsageNonRepudiation )
        * @param aSecurityElementPrompt displayed to user if key is
        * not found
        */
    HBufC8* CreateSignatureL(const CDesCArray& aCaNames,
                             const TKeyUsageX509 aUsage,
                             const TDesC& aSecurityElementPrompt);

    /**
        * Fills iCertificateArray with applicable certificate handles.
        * Leaves with KErrNotFound if no certificates could be found.
        * @param aCaNames distinguished names for certificate
        * authorities which are trusted to sign certificates.
        * @param aUsage Usage of the certificates (either
        * EX509UsageDigitalSignature or EX509UsageNonRepudiation )
        */
    void FindCertificatesL(const CDesCArray& aCaNames,
                           const TKeyUsageX509 aUsage);

    /**
        * Fills iCertificateArray from iCertInfos.
        */
    void HandlesFromCertInfosL();

    /**
        * Finds the key associated with iCertificate
        */
    void FindKeyL();

    /**
        * Opens a signer for the key in iKeys
        */
    void OpenSignerL();

    /**
        * Signs the message
        */
    void SignL();

    /**
        * Creates signed data
        */
    void CreateSignedDataL();

    /**
        * Converts data from UTF8 to Unicode
        * @param aUTF8String UTF8String to convert
        * @return Unicode string
        */
    HBufC* ConvertUTF8ToUnicodeL(const TDesC8& aUTF8String);

    // From FunctionServer
    virtual void vmAttached();
    virtual void doServerSideInit();
    void AddToScheduler();

private:    // Data

    // internal state
    TState iState;

    // internal error variable
    TInt iError;

    // type of current authentication operation
    TAuthDialogType iAuthType;

    // security dialog, owned
    MSecurityDialog* iDialog;

    // PKI Dialog, owned
    MPKIDialog* iPKIDialog;

    // certificate store, owned
    CUnifiedCertStore* iCertStore;

    // key store, owned
    CUnifiedKeyStore* iKeyStore;

    // Waits for the operation to complete. Owned.
    CActiveSchedulerWait* iWait;

    // Used by CUnifiedCertStore
    RFs iFileServer;

    // holds the selection of certificates which are displayed to the user
    RArray< TCTTokenObjectHandle > iCertificateArray;

    // certificate store places found certificates here
    RMPointerArray< CCTCertInfo > iCertInfos;

    // filter is used for searching appropriate certificates, owned
    CCertAttributeFilter* iFilter;

    // contains certificate authority distinguished names in DER
    // encoding. The descriptors are owned by iDERNamesHBufC, as
    // const pointers can't be deleted.
    RPointerArray< const TDesC8 > iDERNames;

    // contains certificate authority distinguished names in DER
    // encoding. Owns the descriptors.
    RPointerArray< HBufC8 > iDERNamesHBufC;

    // handle to the specific certificate which is used in
    // creation of the signature
    TCTTokenObjectHandle iCertificateHandle;

    // The certificate used in creation of the signature, owned
    CCTCertInfo* iCertificate;

    // The certificate in X.509 format, owned
    CCertificate* iX509Certificate;

    // Data to be signed; this is shown to the user. owned.
    HBufC* iTextToDisplay;

    // filter used to find the correct key from key store
    TCTKeyAttributeFilter iKeyFilter;

    // contains the key used for signing
    RMPointerArray< CCTKeyInfo > iKeys;

    // RSA Signer, owned
    MRSASigner* iRSASigner;

    // RSA Signature, owned
    CRSASignature* iRSASignature;

    // DSA Signer, owned
    MDSASigner* iDSASigner;

    // DSA Signature, owned
    CDSASignature* iDSASignature;

    // message to be signed
    TPtrC8 iMessage;

    // certificate to be included in the message, owned
    CCMSX509Certificate* iCMSCertificate;

    // SignerInfo to be included in the message, owned
    CCMSSignerInfo* iSignerInfo;

    // encoded signed attributes hash, owned
    HBufC8* iEncodedSignedAttributesHash;

    // pointer to the above
    TPtrC8 iEncodedSignedAttributesHashPointer;

    // signature, owned
    HBufC8* iSignature;

    // security element prompt, owned
    HBufC* iSecurityElementPrompt;

    // options
    TInt iOptions;

    // ETrue if signing done/cancelled notes are to be shown
    TBool iShowNotes;
};


#endif // CSTSSIGNATURESERVICE_H

// End of File
