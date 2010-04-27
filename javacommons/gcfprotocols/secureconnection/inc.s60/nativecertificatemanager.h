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
* Description:  Provides functionality for adding certificate ino cert store
 *
*/


#ifndef NATIVECERTIFICATEMANAGER_H
#define NATIVECERTIFICATEMANAGER_H

//  INCLUDES
#include "functionserver.h"
#include "fs_methodcall.h"
#include <e32base.h>
#include <f32file.h>
#include <ct/rmpointerarray.h>
#include <cctcertinfo.h>
#include <ct/tcttokenobjecthandle.h>
#include <openssl/x509.h>
#include "s60commonutils.h"
#include <string>

using namespace java::util;
// CONSTANTS

// Key usage constants
const TInt KSTSKeyUsageAuthentication = 0;
const TInt KSTSKeyUsageNonRepudiation = 1;

// FORWARD DECLARATIONS
class CUnifiedCertStore;
class CUnifiedKeyStore;
class CCTKeyInfo;
class CX500DistinguishedName;
class CX520AttributeTypeAndValue;
class CPKCS10Request;
class MPKIDialog;
class CCMSIssuerAndSerialNumber;
using namespace java::util;

// CLASS DECLARATION

/**
 *  Provides functionality for adding certificate into CUnified cert store
 */

NONSHARABLE_CLASS(NativeCertificateManager): public CActive, public FunctionServer
{
private: // type definitions
    enum TCertState
    {
        EValidating
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static NativeCertificateManager* NewL(X509* aCert);

    /**
     * Destructor.
     */
    virtual ~NativeCertificateManager();

public: // New functions

    int AddCertificate();

    int doValidationL();

    int validation();

    void doClose();

    static int validateX509Certificate(X509 *aCert);

    int doValidateX509Certificate();

    void MyAdd();

    void doAdd();

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
    NativeCertificateManager();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(X509* aCert);

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
    void WaitForCompletion();

    /**
     * Retrieves the certificate to be removed, in order to check
     * the Issuer and SerialNumber of the certificate
     */

    /**
     * Adds a certificate
     */
    void AddL();

private: // Data

    // internal state
    TCertState iState;

    // internal error variable
    TInt iError;

    // Waits for the operation to complete. Owned.
    CActiveSchedulerWait* iWait;

    // Used by CUnifiedCertStore
    RFs iFileServer;

    int iCertVerErrCode;

    X509* iCert;

    TInt lab;
};

#endif // NATIVECERTIFICATEMANAGER_H
