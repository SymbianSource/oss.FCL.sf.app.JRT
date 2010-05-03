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


#ifndef CSTSCREDENTIALMANAGER_H
#define CSTSCREDENTIALMANAGER_H

//  INCLUDES
#include "functionserver.h"
#include <e32base.h>
#include <ct/rmpointerarray.h>
#include <cctcertinfo.h>
#include <ct/tcttokenobjecthandle.h>

using namespace java::util;
// CONSTANTS

// FORWARD DECLARATIONS
class CCTKeyInfo;
class MPKIDialog;
class CUnifiedCertStore;
class CUnifiedKeyStore;
class CX500DistinguishedName;
class CX520AttributeTypeAndValue;
class CPKCS10Request;
class CCMSIssuerAndSerialNumber;


// Key usage constants
const TInt KSTSKeyUsageAuthentication = 0;
const TInt KSTSKeyUsageNonRepudiation = 1;



// Algorithm ID constants
_LIT(KSTSAlgorithmDsa, "1.2.840.10040.4.1");
_LIT(KSTSAlgorithmRsa, "1.2.840.113549.1.1");
const TUint KSTSDot = '.';
const TUint KSTSLine = '-';


// CLASS DECLARATION

/**
*  Provides functionality for user credential management.
*  This class can be used for creating certificate signing requests,
*  adding user credentials and removing user credentials.
*
*/
NONSHARABLE_CLASS(CSTSCredentialManager): public CActive, public FunctionServer
{
public: // type definitions
    enum TState
    {
        EReady,
        EInitialising,
        EKeyStoreInit,
        EAddListing,
        EAddRetrieve,
        EAddDialog,
        EAdding,
        ERemoveListing,
        ERemoveRetrieve,
        ERemoveDialog,
        EDeleting,
        ESearchingKey,
        ECSRDialog,
        ECreatingCSR,
        EError
    };

    struct TGenerateCSRParams
    {
public:
        const TDesC* iNameInfo;
        const TDesC* iAlgorithm;
        TInt iKeyLen;
        TInt iKeyUsage;
        const TDesC* iSecurityElementID;
        const TDesC* iSecurityElementPrompt;
    };

public:  // Constructors and destructor

    /**
        * Two-phased constructor.
        */
    static CSTSCredentialManager* NewLC();

    /**
        * Destructor.
        */
    virtual ~CSTSCredentialManager();

public: // New functions

    /**
        * Adds a certificate or certificate URI to certificate store
        *
        * @param aCertDisplayName The user friendly name associated
        *        with the certificate.
        * @param aPkiPath DER encoded PKIPath containing user
        *        certificate and certificate authority certificates
        * @param aUri URI that resolvers to a X.509v3 certificate.
        *
        * @return true if the operation was successful
        * @par Leaving:
        * @li KErrNotReady - The manager is not in ready state
        * @li KErrNotSupported - No writable certificate stores found
        * @li KErrCancel - Operation was cancelled by the user
        * @li KSTSErrAlreadyRegistered - aPkiPath was already
        *     registered
        * @li KErrArgument - Invalid Argument
        * @li KErrBadName - aCertDisplayName already exists
        */

    TBool AddCredentialL(TInt aCertDisplayName,
                         TInt aPkiPath);

    static void StaticAddCredentialL(TInt aManager,
                                     TInt aCertDisplayName,
                                     TInt aPkiPath,
                                     TInt aRetVal);


    /**
        * Creates a DER encoded PKCS#10 certificate enrollment
        * request.
        *
        * @param aNameInfo The distinguished name to be included in
        *        The CSR.
        * @param aAlgorithm OID for the algorithm to use
        * @param aKeyLen the key length
        * @param aKeyUsage The functionality for which the key is
        *        marked inside the security element. Either
        *        KSTSKeyUsageAuthentication or KSTSKeyUsageNonRepudiation.
        * @param aSecurityElementID identifies the security element
        *        on which the key resides or will be generated.
        * @param aSecurityElementPrompt guides the user to insert the
        *        security element
        * @param aForceKeyGen if true, a new key MUST be generated.
        *
        * @return DER encoded PKCS#10 CER, ownership is not transferred.
        *
        * @par Leaving:
        * @li KErrCancel - User cancelled the operation
        * @li KSTSErrSeNoKeys - SE contains no keys that could be used
        * @li KSTSErrSeNotFound - SE could not be found
        * @li KErrKeyUsage - Invalid key usage
        * @li KErrArgument - Invalid argument
        * @li KErrLocked - PIN is blocked.
        */
    TDesC8& GenerateCSRL(const TDesC& aNameInfo, const TDesC& aAlgorithm,
                         TInt aKeyLen, TInt aKeyUsage,
                         const TDesC* aSecurityElementID,
                         const TDesC* aSecurityElementPrompt);


    void StaticGenerateCSRL(TInt aParams,
                            TInt aRetVal);


    /**
        * Removes a certificate or certificate URI from a certificate
        * store.
        *
        * @param aCertDisplayName User friendly name associated with
        *        the certificate.
        * @param aIssuerAndSerialNumber DER encoded ASN.1 structure
        *        that contains the certificate issuer and serial number.
        * @param aSecurityElementID identifies the security element
        *        where the key resides.
        * @param aSecurityElementPrompt guides the user to insert the
        *        security element.
        *
        * @return true if operation was successful
        * @li KErrCancel - Operation was cancelled by user
        * @li KErrNotFound - Credential was not found
        * @li KErrArgument - Invalid argument
        * @li KSTSErrSeNotFound - SE was not found
        */
    TBool RemoveCredentialL(const TDesC& aCertDisplayName,
                            const TDesC8& aIssuerAndSerialNumber,
                            const TDesC* aSecurityElementID,
                            const TDesC* aSecurityElementPrompt);

    void StaticRemoveCredentialL(TInt aCertDisplayName,
                                 TInt aIssuerAndSerialNumber,
                                 TInt aSecurityElementID,
                                 TInt aSecurityElementPrompt,
                                 TInt aRetVal);



public: // CActive

    /**
        * Handles an active object's request completion event.
        */
    void RunL();

    /**
        * Handles a leave occurring in the request completion event handler RunL().
        */
    TInt RunError(TInt aError);

    /**
        * Implements cancellation of an outstanding request.
        */
    void DoCancel();

private:

    // From FunctionServer
    virtual void vmAttached();
    virtual void doServerSideInit();
    void AddToScheduler();

    /**
        * C++ default constructor.
        */
    CSTSCredentialManager();

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
        * Retrieves the certificate to be removed, in order to check
        * the Issuer and SerialNumber of the certificate
        */
    void RemoveRetrieveL();

    /**
        * Retrieves possibly conflicting certificate for adding, in order
        * to check SerialNumber (issuer and subject are certainly same)
        */
    void AddRetrieveL();

    /**
        * verifies that the retrieved certificate has correct Issuer
        * and SerialNumber.
        */
    void RemoveVerifyL();

    /**
        * verifies that the retrieved certificate does not have correct
        * SerialNumber.
        */
    void AddVerifyL();

    /**
        * Displays a dialog asking user permission to remove a certificate
        */
    void RemoveDialog();

    /**
        * Removes the found certificate.
        */
    void Remove();

    /**
        * Adds a certificate
        */
    void AddL();

    /**
        * Displays a CSR creation dialog
        */
    void CSRDialog();

    /**
        * Creates a CSR from the member data, stored in iRequestEncoded.
        */
    void CreateCSRL();

    /**
        * Checks the results of a key search, and if a key has been found,
        * creates a CSR.
        */
    void CheckSearchResultsL();

    /**
        * Checks if given SecurityElementID is available as a
        * (readable) key store, and prompts user
        * if it isn't. Leaves with KSTSErrSeNotFound if SE could not be
        * found even after prompt, or if aSecurityElementPrompt is null
        * @param aSecurityElementID ID of the SE to search for
        * @param aSecurityElementPrompt displayed to user if needed
        * @return index of the key/cert store to use
        */
    TInt CheckKeyStoreSEIDL(const TDesC* aSecurityElementID,
                            const TDesC* aSecurityElementPrompt);

    /**
        * Checks if given SecurityElementID is available as a
        * writable cert store, and prompts user
        * if it isn't. Leaves with KSTSErrSeNotFound if SE could not be
        * found even after prompt, or if aSecurityElementPrompt is null
        * @param aSecurityElementID ID of the SE to search for
        * @param aSecurityElementPrompt displayed to user if needed
        * @return index of the key/cert store to use
        */
    TInt CheckWriteCertStoreSEIDL(const TDesC* aSecurityElementID,
                                  const TDesC* aSecurityElementPrompt);


    /**
        * Checks if given string is a RFC1778 formatted OID. If it is not,
        * leaves with KErrArgument.
        * @param aOID OID to check.
        */
    void CheckOIDL(const TDesC& aOID);

private:    // Data

    // internal state
    TState iState;

    // internal error variable
    TInt iError;

    // PKI Dialog, owned
    MPKIDialog* iPKIDialog;

    // The actual store, owned
    CUnifiedCertStore* iStore;

    // Key store, used for creating keys when needed, owned
    CUnifiedKeyStore* iKeyStore;

    // index to current writable cert store, which is used in current
    TInt iWritableCertStoreIndex;

    // Waits for the operation to complete. Owned.
    CActiveSchedulerWait* iWait;

    // Used by CUnifiedCertStore
    RFs iFileServer;

    // Used for storing the results of search operations, owned
    RMPointerArray< CCTCertInfo >* iCertInfoArray;

    // Used for storing the results of search operations, owned
    RMPointerArray< CCTKeyInfo >* iKeyInfoArray;

    // Key info object, not owned but must call Release()
    CCTKeyInfo* iKeyInfo;

    // Length of the key we are searching for
    TInt iKeyLen;

    // Distinguished name, used for creating CSR, owned
    CX500DistinguishedName* iDistinguishedName;

    // Display name, used for displaying the CSR info, owned
    HBufC* iDisplayName;

    // Display name, used when adding a certificate to the database, not owned
    const TDesC* iCertDisplayName;

    // Certificate to be added
    TPtrC8 iSubject;

    // Key Identifier of the certificate to be added
    TKeyIdentifier iSubjectKeyId;

    // Key Identifier of the issuer of the certificate to be added
    TKeyIdentifier iIssuerKeyId;

    // Used for creating CSR, owned
    CPKCS10Request* iRequest;

    // Stores the created request, owned
    HBufC8* iRequestEncoded;

    // contains handle to an object which is used in a PKI dialog
    TCTTokenObjectHandle iHandle;

    // contains the IssuerAndSerialNumber of the certificate to be
    // added or removed, owned
    CCMSIssuerAndSerialNumber* iIssuerAndSerialNumber;

    // certificate to be removed, owned
    HBufC8* iEncodedCert;
    TPtr8 iEncodedCertDes;

};


#endif // CSTSCREDENTIALMANAGER_H

// End of File
