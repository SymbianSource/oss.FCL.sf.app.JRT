/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface which handles certificate related operations
*
*/



#ifndef WIMDUMMIES_H
#define WIMDUMMIES_H


//INCLUDES
#include <e32base.h>
#include <ct.h>
#include <MCTkeyStore.h>

class CWimCertInfo;

enum TOMAType
{
    EBootStrap,
    EConfig1,
    EConfig2
};

class CWimCertConverter : public CBase
{

public:
    IMPORT_C void CancelRetrieve();
    IMPORT_C void AddCertificate(const TDesC& aLabel,
                                 const TCertificateFormat aFormat,
                                 const TCertificateOwnerType aCertificateOwnerType,
                                 const TKeyIdentifier& aSubjectKeyId,
                                 const TKeyIdentifier& aIssuerKeyId,
                                 const TDesC8& aCert,
                                 TRequestStatus& aStatus);

    IMPORT_C void CancelAddCertificate();
    IMPORT_C void CancelRemove();
    IMPORT_C virtual ~CWimCertConverter();
    IMPORT_C void CancelRestore();
};

class CWimKeyDetails : public CBase
{
public:
    IMPORT_C virtual ~CWimKeyDetails();
    IMPORT_C void GetKeyList(RPointerArray<CCTKeyInfo>& aKeys,
                             CArrayFixFlat<TUint8>& aKeyNumbers,
                             TRequestStatus& aStatus);

    IMPORT_C void CancelList();
    IMPORT_C void CancelSign();
    IMPORT_C void ExportPublicKeyL(TDesC8& aKeyId,
                                   HBufC8*& aPublicKey,
                                   TRequestStatus& aStatus);
    IMPORT_C void CancelExport();
};


class CWimOMAProv : public CBase
{
public:
    IMPORT_C virtual ~CWimOMAProv();
    IMPORT_C void Initialize(TRequestStatus& aStatus);
    IMPORT_C void CancelInitialize();
    IMPORT_C void GetSize(TInt& aSize,
                          const TOMAType& aOMAType,
                          TRequestStatus& aStatus);
    IMPORT_C void CancelGetSize();
    IMPORT_C void CancelRetrieve();
};


class CWimPin : public CBase
{
public:
    IMPORT_C virtual ~CWimPin();
    IMPORT_C void EnablePinQuery(TRequestStatus& aStatus);
    IMPORT_C void CancelEnablePinQuery();
    IMPORT_C void DisablePinQuery(TRequestStatus& aStatus);
    IMPORT_C void CancelDisablePinQuery();
    IMPORT_C void ChangePin(TRequestStatus& aStatus);
    IMPORT_C void CancelChangePin();
    IMPORT_C void CancelUnblockPin();
    IMPORT_C void CancelVerifyPin();
    IMPORT_C TPtrC Label();
};


class CWimSecModule : public CBase
{
public:
    IMPORT_C virtual ~CWimSecModule();
    IMPORT_C TBool IsOpen();
    IMPORT_C TInt CloseAfter();
    IMPORT_C TPtrC Label();
    IMPORT_C const TDesC& Manufacturer();
    IMPORT_C TInt Close();
    IMPORT_C void CancelNotifyOnRemoval();
};


class CWimSecModuleMgr : public CBase
{
public:
    IMPORT_C virtual ~CWimSecModuleMgr();
    IMPORT_C void GetEntries(RCPointerArray<HBufC>& aTokens,
                             TRequestStatus& aStatus);
    IMPORT_C void CancelList();
    IMPORT_C CWimSecModule* GetWimSecModuleByIndexL(
        const TInt aIndex);
};


class CWimTrustSettingsAPI : public CBase
{
public:
    IMPORT_C virtual ~CWimTrustSettingsAPI();
    IMPORT_C void GetTrustSettings(const CWimCertInfo& aCert,
                                   TBool& aTrusted,
                                   RArray<TUid>& aApplications,
                                   TRequestStatus& aStatus);
    IMPORT_C void Close();
    IMPORT_C void CancelDoing();
};

#endif // WIMDUMMIES_H
