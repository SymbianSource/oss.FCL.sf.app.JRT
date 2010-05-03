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



#include "wimdummies.h"

EXPORT_C void CWimCertConverter::CancelRetrieve()
{

}

EXPORT_C void CWimCertConverter::AddCertificate(const TDesC& aLabel,
        const TCertificateFormat aFormat,
        const TCertificateOwnerType aCertificateOwnerType,
        const TKeyIdentifier& aSubjectKeyId,
        const TKeyIdentifier& aIssuerKeyId,
        const TDesC8& aCert,
        TRequestStatus& aStatus)
{

}

EXPORT_C void CWimCertConverter::CancelAddCertificate()
{

}

EXPORT_C void CWimCertConverter::CancelRemove()
{

}

EXPORT_C CWimCertConverter::~CWimCertConverter()
{

}

EXPORT_C void CWimCertConverter::CancelRestore()
{

}

EXPORT_C CWimKeyDetails::~CWimKeyDetails()
{

}

EXPORT_C void CWimKeyDetails::GetKeyList(RPointerArray<CCTKeyInfo>& aKeys,
        CArrayFixFlat<TUint8>& aKeyNumbers,
        TRequestStatus& aStatus)
{

}

EXPORT_C void CWimKeyDetails::CancelList()
{

}

EXPORT_C void CWimKeyDetails::CancelSign()
{

}

EXPORT_C void CWimKeyDetails::ExportPublicKeyL(TDesC8& aKeyId,
        HBufC8*& aPublicKey,
        TRequestStatus& aStatus)
{

}

EXPORT_C void CWimKeyDetails::CancelExport()
{

}


EXPORT_C CWimOMAProv::~CWimOMAProv()
{

}

EXPORT_C void CWimOMAProv::Initialize(TRequestStatus& aStatus)
{

}

EXPORT_C void CWimOMAProv::CancelInitialize()
{

}

EXPORT_C void CWimOMAProv::GetSize(TInt& aSize,
                                   const TOMAType& aOMAType,
                                   TRequestStatus& aStatus)
{

}

EXPORT_C void CWimOMAProv::CancelGetSize()
{

}

EXPORT_C void CWimOMAProv::CancelRetrieve()
{

}

EXPORT_C CWimPin::~CWimPin()
{

}

EXPORT_C void CWimPin::EnablePinQuery(TRequestStatus& aStatus)
{

}

EXPORT_C void CWimPin::CancelEnablePinQuery()
{

}

EXPORT_C void CWimPin::DisablePinQuery(TRequestStatus& aStatus)
{

}

EXPORT_C void CWimPin::CancelDisablePinQuery()
{

}

EXPORT_C void CWimPin::ChangePin(TRequestStatus& aStatus)
{

}

EXPORT_C void CWimPin::CancelChangePin()
{

}

EXPORT_C void CWimPin::CancelUnblockPin()
{

}

EXPORT_C void CWimPin::CancelVerifyPin()
{

}

EXPORT_C TPtrC CWimPin::Label()
{

}

EXPORT_C CWimSecModule::~CWimSecModule()
{

}

EXPORT_C TBool CWimSecModule::IsOpen()
{

}

EXPORT_C TInt CWimSecModule::CloseAfter()
{

}

EXPORT_C TPtrC CWimSecModule::Label()
{

}

EXPORT_C const TDesC& CWimSecModule::Manufacturer()
{

}

EXPORT_C TInt CWimSecModule::Close()
{

}

EXPORT_C void CWimSecModule::CancelNotifyOnRemoval()
{

}


EXPORT_C CWimSecModuleMgr::~CWimSecModuleMgr()
{

}

EXPORT_C void CWimSecModuleMgr::GetEntries(RCPointerArray<HBufC>& aTokens,
        TRequestStatus& aStatus)
{

}

EXPORT_C void CWimSecModuleMgr::CancelList()
{

}

EXPORT_C CWimSecModule* CWimSecModuleMgr::GetWimSecModuleByIndexL(
    const TInt aIndex)
{

}

EXPORT_C CWimTrustSettingsAPI::~CWimTrustSettingsAPI()
{

}

EXPORT_C void CWimTrustSettingsAPI::GetTrustSettings(const CWimCertInfo& aCert,
        TBool& aTrusted,
        RArray<TUid>& aApplications,
        TRequestStatus& aStatus)
{

}

EXPORT_C void CWimTrustSettingsAPI::Close()
{

}

EXPORT_C void CWimTrustSettingsAPI::CancelDoing()
{

}
