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


#ifndef OCSPCLIENT_H
#define OCSPCLIENT_H

#include "ocspchecker.h"
#include <vector>
#include <ocsp.h>
#include <ocsptransport.h>
#include <unifiedcertstore.h>
#include <x509cert.h>
#include <f32file.h>
#include <securitydefs.h>
#include "javacommonutils.h"
#include "monitor.h"

namespace java
{
namespace security
{

class OcspResponse: public CBase
{

public:

    static OcspResponse* NewL();
    void Clear();
    virtual ~OcspResponse();
    OcspResponse(const OcspResponse&);
    OcspResponse &operator=(const OcspResponse&);
    OcspResponse();

private:
    void ConstructL();

public:

    int iSummary;
    std::vector<int> iIndividualResponses;
};

class OcspClient : public CActive
{

    enum State
    {
        ESendOcspRequest,
        EProcessOcspResponse
    };

public:

    static OcspClient* createInstance(long iap, const char* defaultUrl);
    void startOcspCheck(const char **, int);
    OcspResponse getOcspCheckResponse();
    void cancelOcspCheck(bool doCleanup);

    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);
    virtual ~OcspClient();

private:
    OcspClient(long iap);
    void ConstructL(const char* defaultUrl);
    void Start();
    void SendOcspRequestL();
    void ProcessOcspResponse();
    void CompleteRequest();
    void InitOcspClientL();
    TUint32 iIap;
    HBufC8* iDefaultUrl;
    State iState;
    COCSPClient* iOcspClient;
    CUnifiedCertStore* iCertStore;
    RFs iRfs;
    TBool iInitialized;
    OcspResponse* iOcspResponse;
    const char ** iCertChain;
    int iCertChainLen;
    CArrayPtrFlat<CX509Certificate>*  iCertArray;
    java::util::Monitor* iMonitor;
};

} //end namespace security
} //end namespace java

#endif // OCSPCLIENT_H
