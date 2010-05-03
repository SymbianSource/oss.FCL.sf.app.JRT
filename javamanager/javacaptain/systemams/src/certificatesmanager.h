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
#include <e32base.h>
#include <mctwritablecertstore.h>
#include <ct.h>
#include <ecom/ecom.h>
#include <s32mem.h>

namespace java
{
namespace security
{
namespace legacysupport
{

class CertificatesManager: public CActive
{

    enum State
    {
        EStart,
        EOpenToken,
        EGetTokenInterface,
        EListCertificates,
        EInitCertsRetrieval,
        ERetrieveCertificates,
        ERetrieveCertState,
        ERetrieveCertLength,
        ECollectCertInfo,
        EFinish
    };

public:

    static CertificatesManager* NewL();

    TUint32 TrustRootCount(const TDesC& aProtectionDomain);
    TPtr8 TrustRootInfo(const TDesC& aProtectionDomain);
    TPtr8 TrustRootInfoCertificateL(const TInt id);
    void DeleteTrustRootL(const TInt id);
    void DisableTrustRootL(const TInt id);
    void EnableTrustRootL(const TInt id);

    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);

public:

    virtual ~CertificatesManager();

private:

    CertificatesManager();
    void ConstructL();
    CCTCertInfo* getTrustRootL(const TInt aId);
    void CompleteRequest();
    void InitL();
    void OpenToken();
    void GetTokenInterface();
    void ListCertificatesL();
    void InitCertsRetrievalL();
    void RetrieveCertificates();
    void RetrieveCertState();
    void RetrieveCertLengthL();
    void CollectCertInfoL();
    void UpdateCacheL(TInt aCertId, TInt aCertState);
    void LazyInit();

private:
    State iState;
    MCTToken* iCTToken;
    CCTTokenType* iCTTokenType;
    RCPointerArray<HBufC> iCTTokenInfo;
    MCTTokenInterface* iCTTokenInterface;
    MCTWritableCertStore* iCertStore;
    CCertAttributeFilter* iCertsFilter;
    RMPointerArray<CCTCertInfo> iCertInfos;
    TUid iJavaCertStoreEcomPluginId;
    CBufFlat* iBuffer;
    int iBufferPos;
    TBool iCanDisable;
    RArray<TUid> iApplications;
    HBufC8* iEncodedCert;
    int iCurrentRetrievedCert;
    HBufC8* iCurrentCert;
    CCTCertInfo* iCurrentCertInfo;
    HBufC* iProtDomain;
    int iCertCount;
    TBool iInitialized;
};

} //end namespace legacysupport
} //end namespace security
} //end namespace java

