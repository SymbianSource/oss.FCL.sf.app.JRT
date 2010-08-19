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


#include "telutils.h"

using namespace java::security;

TelUtils* TelUtils::createInstance()
{
    TelUtils* self = NULL;
    TRAPD(err,
          self = new(ELeave) TelUtils();
          CleanupStack::PushL(self);
          self->ConstructL();
          CleanupStack::Pop(self);
         );
    if (err == KErrNone)
    {
        return self;
    }
    else
    {
        return NULL;
    }
}

void TelUtils::getNetworkCodes(std::string& mcc, std::string& mnc)
{
    //JELOG2(EUtils);
    mnc += "000";
    mcc += "000";
    if (iMnc)
    {
        descToStr(iMnc, mnc);
    }
    if (iMcc)
    {
        descToStr(iMcc, mcc);
    }
}

void TelUtils::getImei(std::string& imei)
{
    //JELOG2(EUtils);
    if (iImei)
    {
        descToStr(iImei, imei);
    }
}

signed int TelUtils::getSecureTime()
{
    TTime t;
    if (t.UniversalTimeSecure())
    {
        t.UniversalTime();
    }
    TTimeIntervalSeconds sec;
    TTime baseTime(MAKE_TINT64(0x00dcddb3 ,0x0f2f8000)); // 1st Jan 1970
    t.SecondsFrom(baseTime, sec);
    return sec.Int();
}

/**
 * Return 0 if secure time has not been set
 */
int TelUtils::isSecureTimeSet()
{
    TTime t;
    if (KErrNoSecureTime == t.UniversalTimeSecure())
    {
        return 0;
    }
    return 1;
}

void TelUtils::ConstructL()
{
    RMobilePhone::TMobilePhoneIdentityV1        phoneId;
    RTelServer::TPhoneInfo                      phoneInfo;
    User::LeaveIfError(iServer.Connect());
    User::LeaveIfError(iServer.LoadPhoneModule(KMmTsyModuleName));
    User::LeaveIfError(iServer.GetPhoneInfo(0, phoneInfo));
    User::LeaveIfError(iPhone.Open(iServer, phoneInfo.iName));

    TUint32 idCaps;
    TInt result = iPhone.GetIdentityCaps(idCaps);
    if (KErrNone == result && idCaps&RMobilePhone::KCapsGetSerialNumber)
    {
        TRequestStatus  status;
        iPhone.GetPhoneId(status, phoneId);
        User::WaitForRequest(status);
        User::LeaveIfError(status.Int());
        iImei = phoneId.iSerialNumber.Alloc();
    }

    TUint32 networkCaps;
    result = iPhone.GetNetworkCaps(networkCaps);
    if (KErrNone == result && networkCaps & RMobilePhone::KCapsGetCurrentNetwork)
    {
        TRequestStatus  status;
        RMobilePhone::TMobilePhoneOPlmnV3   networkInfo;
        RMobilePhone::TMobilePhoneOPlmnV3Pckg pckgNetworkInfo(networkInfo);
        RMobilePhone::TMobilePhoneNetworkNameV3 networkName;
        RMobilePhone::TMobilePhoneNetworkNameV3Pckg pckgNetworkName(networkName);
        iPhone.GetCurrentNetworkName(status, pckgNetworkName, pckgNetworkInfo);
        User::WaitForRequest(status);
        User::LeaveIfError(status.Int());
        iMnc = (networkInfo.iNetworkId).Alloc();
        iMcc = (networkInfo.iCountryCode).Alloc();
    }

    if (iServer.Handle())
    {
        iPhone.Close();
        iServer.UnloadPhoneModule(KMmTsyModuleName);
        iServer.Close();
    }
}

TelUtils::TelUtils()
        : iImei(NULL), iMnc(NULL), iMcc(NULL)
{
}

TelUtils::~TelUtils()
{
    if (iServer.Handle())
    {
        iPhone.Close();
        iServer.UnloadPhoneModule(KMmTsyModuleName);
        iServer.Close();
    }
    if (iImei)
    {
        delete iImei;
        iImei = NULL;
    }
    if (iMnc)
    {
        delete iMnc;
        iMnc = NULL;
    }
    if (iMcc)
    {
        delete iMcc;
        iMcc = NULL;
    }
}

void TelUtils::descToStr(HBufC * desc, std::string& str)
{
    if (desc->Length() > 0)
    {
        str.clear();
        const char * tmp = (const char *)desc->Ptr();
        for (int i=0; i < desc->Length(); i++)
        {
            str += tmp;
            tmp += 2;
        }
    }
}

