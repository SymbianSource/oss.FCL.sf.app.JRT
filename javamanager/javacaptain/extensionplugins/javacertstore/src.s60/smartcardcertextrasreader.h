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


#ifndef SMARTCARDCERTEXTRASREADER_H
#define SMARTCARDCERTEXTRASREADER_H

#include "javaosheaders.h"

#include <e32base.h>
#include <WimCert.h>
#include <string>

namespace java
{
namespace security
{

OS_NONSHARABLE_CLASS(SmartCardCertExtrasReader) : public CActive
{

    enum State
    {
        EStart,
        EGetExtras,
        EFinish
    };

public:
    static SmartCardCertExtrasReader* NewL();
    virtual ~SmartCardCertExtrasReader();
    void GetCertTrustedUsages(TDesC8& aSmartCardCertId, RCPointerArray<HBufC>& aSmartCardCertTrustedUsages);
    // CActive
    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);
private:
    SmartCardCertExtrasReader();
    void ConstructL();
    bool Initialize();
    bool GetExtras();
    bool Finish();
    void CancelInitialize();
    void CancelGetExtras();
    void CompleteRequest(TInt aCompletionCode);
    void NextState(bool aCurrentOperationSucceeded);

private:
    // the state machine's state (see State for values)
    State iState;
    RCPointerArray<HBufC>* iSmartCardCertTrustedUsages;
    CWimCert* iSmartCardCertExtras;
    HBufC8* iSmartCardCertId;
    TCertLocation iSmartCardCertLocation;
    CActiveScheduler* iActiveScheduler;
};

} //end namespace security
} //end namespace java

#endif // SMARTCARDCERTEXTRASREADER_H
