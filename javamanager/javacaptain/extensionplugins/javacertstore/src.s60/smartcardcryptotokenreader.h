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


#ifndef SMARTCARDCRYPTOTOKENREADER_H
#define SMARTCARDCRYPTOTOKENREADER_H

#include "javaosheaders.h"

#include <e32base.h>
#include <f32file.h>
#include <ct.h>
#include <mctcertstore.h>
#include <mctwritablecertstore.h>

namespace java
{
namespace security
{

OS_NONSHARABLE_CLASS(SmartCardCryptoTokenReader) : public CActive
{

    enum State
    {
        EStart,
        EListTokenTypes,
        EOpenTokenType,
        EOpenToken,
        EGetTokenInterface,
        EListCertificates,
        EFilterCertificates,
        ERetrieveCertificates,
        EReleaseToken,
        EFinish
    };

public:
    static SmartCardCryptoTokenReader* NewL();
    virtual ~SmartCardCryptoTokenReader();
    void RetrieveCerts(RArray<HBufC8*>& aSmartCardCerts);
    // CActive
    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);
private:
    SmartCardCryptoTokenReader();
    void ConstructL();
    bool Initialize();
    bool ListTokenTypesL();
    bool OpenTokenType();
    bool OpenToken();
    bool GetTokenInterface();
    bool ListCertificates();
    bool FilterCertificates();
    bool RetrieveCertificates();
    bool ReleaseToken();
    void CancelOpenTokenType();
    void CancelOpenToken();
    void CancelGetTokenInterface();
    void CompleteRequest(TInt aCompletionCode);
    void NextState(bool aCurrentOperationSucceeded);
    void DoReleaseToken();

private:
    // file handler
    RFs iFs;
    // the state machine's state (see State for values)
    State iState;
    // smart card token attributes
    RArray<TUid> iSmartCardTokensInterfaces;
    RArray <TCTTokenTypeAttribute> iSmartCardTokensAttributes;
    // smart card token types
    RCPointerArray <CCTTokenTypeInfo> iSmartCardTokenTypes;
    // smart card tokens from the list are handled one by one
    // -> this is the index of the token type being currently handled
    int iCurrentTokenType;
    // smart card token type & info
    MCTToken* iSmartCardToken;
    CCTTokenType* iSmartCardTokenType;
    RCPointerArray<HBufC> iSmartCardTokenInfo;
    // smart card token interface
    MCTTokenInterface* iSmartCardTokenInterface;
    MCTCertStore* iSmartCardStore;
    // smart card certificate filters
    CCertAttributeFilter* iSmartCardCertsFilter;
    // smart card certificate infos
    RMPointerArray<CCTCertInfo> iSmartCardCertInfos;
    // smart card certificates contents are retrieved one by one by the state machine
    // -> this is the index of the smart card cert content being currently retrieved
    int iCurrentRetrievedSmartCardCert;
    // smart card certs
    RArray<HBufC8*>* iSmartCardCerts;
    // flag identifying if the currently retrieved cert is trusted
    TBool iCurrentSmartCardCertIsTrusted;
    // temporary buffer (and pointer to it) via which a single certificate
    // content is retrieved
    HBufC8* iTmpBuf;
    TPtr8 iTmpBufPtr;
    CActiveScheduler* iActiveScheduler;
};

} //end namespace security
} //end namespace java

#endif // SMARTCARDCRYPTOTOKENREADER_H
