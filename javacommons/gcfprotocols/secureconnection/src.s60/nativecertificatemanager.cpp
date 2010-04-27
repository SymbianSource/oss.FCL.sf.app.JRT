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
* Description:  Provides functionality for verifying the server certificate
 *
*/


#include "nativecertificatemanager.h"

#include <x509keys.h>
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
#include <pkixcertchain.h>
#include "logger.h"

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ssl.h>
#else
#include <ssl_internal.h>
#endif

using namespace java::util;
//using namespace java::certificatemanagement;

// CONSTANTS
//_LIT( KSTSSymbianKeyStoreLabel, "Software key store" );
//_LIT(KSTSSymbianCertificateStoreLabel, "Software certificate store");
//_LIT(KSTSSymbianTrustCertStoreLabel, "trust server certstore");

NativeCertificateManager* NativeCertificateManager::NewL(X509* aCert)
{
    JELOG2(ESOCKET);
    NativeCertificateManager* self = new(ELeave) NativeCertificateManager;
    CallMethod(self, &NativeCertificateManager::AddToScheduler, self);
    CallMethodL(self, &NativeCertificateManager::ConstructL, aCert, self);
    return self;
}

void NativeCertificateManager::AddToScheduler()
{
    CActiveScheduler::Add(this);
}

void NativeCertificateManager::doClose()
{
    iFileServer.Close();
}

// Destructor
NativeCertificateManager::~NativeCertificateManager()
{
    CallMethod(this, &NativeCertificateManager::doClose, this);
    //stopServer();

}

int NativeCertificateManager::doValidationL()
{
    JELOG2(ESOCKET);
    unsigned char *der, *data;

    int len = i2d_X509(iCert, NULL);
    der = (unsigned char *) OPENSSL_malloc(len);
    if (der == NULL)
        User::Leave(-1);
    data = der;
    int ret = i2d_X509(iCert, &data);
    HBufC8* certData = HBufC8::NewL(len);
    TPtr8 tmpPtr(certData->Des()); // pointer to hbuf
    tmpPtr.Copy(der, ret);         // copy the der format data to hbuf

    TTime ValidationTime;
    ValidationTime.UniversalTime();
    CPKIXValidationResult *certVerificationResult =
        CPKIXValidationResult::NewL();
    CPKIXCertChain *serverCertsChain = CPKIXCertChain::NewL(iFileServer,
                                       certData->Des(), TUid::Uid(KUidUnicodeSSLProtocolModule));
    serverCertsChain->ValidateL(*certVerificationResult, ValidationTime,
                                iStatus);

    OPENSSL_free(der);
    delete certData;
    return 0;
}

// NativeCertificateManager::RunL
// CActive callback
// -----------------------------------------------------------------------------
//
void NativeCertificateManager::RunL()
{
    ILOG2(ESOCKET, "++NativeCertificateManager::RunL iState %d, iStatus = %d", iState, iStatus.Int());
    if (iStatus != KErrNone)
    {
        // Error has occured; inform java
        Complete(iStatus.Int());
    }
    else
    {
        switch (iState)
        {
        case EValidating:
            Complete(KErrNone);
            break;
        default:
        {
            Complete(KErrGeneral);
        }
        }
    }
}

// -----------------------------------------------------------------------------
// NativeCertificateManager::RunError
// CActive error callback
// -----------------------------------------------------------------------------
//
TInt NativeCertificateManager::RunError(TInt /*aError*/)
{
    // Complete( aError );
    return KErrNone;
}

// -----------------------------------------------------------------------------
// NativeCertificateManager::NativeCertificateManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
NativeCertificateManager::NativeCertificateManager() :
        CActive(EPriorityStandard), java::util::FunctionServer("Myserver"), iState(
            EValidating), iCertVerErrCode(0)
{
    createServerToNewThread();
}

void NativeCertificateManager::DoCancel()
{

}

int NativeCertificateManager::doValidateX509Certificate()
{
    TRAPD(err, doValidationL());
    if (err < KErrNone)
    {
        iCertVerErrCode = err;
        ELOG1(ESOCKET, "Error in X509 certificate validation: %d", err);
    }
    else
    {
        WaitForCompletion();

    }
    return iCertVerErrCode;
}

int NativeCertificateManager::validation()
{
    int ret = 0;
    CallMethod(ret, this, &NativeCertificateManager::doValidateX509Certificate,
               this);
    return ret;

}

int NativeCertificateManager::validateX509Certificate(X509 *aCert)
{
    JELOG2(ESOCKET);
    int ret = 0;
    NativeCertificateManager *obj = NULL;
    TRAPD(err, obj = NativeCertificateManager::NewL(aCert));
    if (err < 0)
    {
        return err;
    }
    ret = obj->validation();
    // obj->Dispose();
    delete obj;
    return ret;

}

// -----------------------------------------------------------------------------
// NativeCertificateManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void NativeCertificateManager::ConstructL(X509* aCert)
{
    iCert = aCert;
    User::LeaveIfError(iFileServer.Connect());
    iWait = new(ELeave) CActiveSchedulerWait;
    iState = EValidating;
}

// -----------------------------------------------------------------------------
// NativeCertificateManager::Complete
// Completes the operation
// -----------------------------------------------------------------------------
//
void NativeCertificateManager::Complete(TInt aError)
{
    JELOG2(ESOCKET);
    if (KErrNone == aError)
    {
        iCertVerErrCode = KErrNone;
    }
    else
    {
        iCertVerErrCode = aError;
    }
    iWait->AsyncStop();
}

// -----------------------------------------------------------------------------
// NativeCertificateManager::WaitForCompletion()
// Wait for completion, leave on error
// -----------------------------------------------------------------------------
//
void NativeCertificateManager::WaitForCompletion()
{
    SetActive();
    iWait->Start(); // will return at the end of complete
}

void NativeCertificateManager::vmAttached()
{

}

void NativeCertificateManager::doServerSideInit()
{

    FunctionServer::doServerSideInit();

}
