/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CHTTPSCERTINFO_H
#define CHTTPSCERTINFO_H

#include <ssl.h>
#include <signed.h>
#include "mnativesecureconnectioninformation.h"

class CHttpsCertInfo : public CBase, public MNativeSecureConnectionInformation
{
public:
    IMPORT_C static CHttpsCertInfo* NewL(const CX509Certificate& aCertInfo, HBufC* aCipherSuite);
    IMPORT_C ~CHttpsCertInfo();
public:// from MNativeSecureConnectionInformation
    void GetSecurityInfoL(CDesCArray& aStringResult, TTime& aStartTime, TTime& aEndTime);
private:
    HBufC* SerializeDistinguishedNameLC(const CX500DistinguishedName& aDN);
    CHttpsCertInfo(const CX509Certificate& aCertInfo, HBufC* aCipherSuite);
    void ConstructL();
private:
    const CX509Certificate& iCertInfo;
    HBufC* iSubject;
    HBufC* iIssuer;
    HBufC& iCipherSuite;
};

#endif // CHTTPSCERTINFO_H
