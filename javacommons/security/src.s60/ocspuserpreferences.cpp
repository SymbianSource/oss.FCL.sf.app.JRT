/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <centralrepository.h>
#include <SWInstallerInternalCRKeys.h>
#include "s60commonutils.h"
#include "com_nokia_mj_impl_security_midp_authentication_OcspUserPreferences.h"

const int OCSP_MODE_ON = 1;
const int OCSP_MODE_MUST = 2;
const int OCSP_MODE_OFF = 3;

using namespace java::util;

jint JNICALL Java_com_nokia_mj_impl_security_midp_authentication_OcspUserPreferences__1getOcspMode
(JNIEnv *, jobject)
{
    int ocspMode = OCSP_MODE_OFF;
    CRepository* repository = NULL;
    TRAP_IGNORE(
        repository = CRepository::NewL(KCRUidSWInstallerSettings);
        CleanupStack::PushL(repository);
        TInt value(0);
        repository->Get(KSWInstallerOcspProcedure, value);
        TSWInstallerOcspProcedure ocspProc = TSWInstallerOcspProcedure(value);
        switch (ocspProc)
{
case ESWInstallerOcspProcedureOn:
    ocspMode = OCSP_MODE_ON;
    break;
case ESWInstallerOcspProcedureMust:
    ocspMode = OCSP_MODE_MUST;
    break;
case ESWInstallerOcspProcedureOff:
default:
    ocspMode = OCSP_MODE_OFF;
}
CleanupStack::PopAndDestroy(repository);
);
    return ocspMode;
}

jstring JNICALL Java_com_nokia_mj_impl_security_midp_authentication_OcspUserPreferences__1getOcspUrl
(JNIEnv * aEnv, jobject)
{
    jstring ret = NULL;
    CRepository* repository = NULL;
    HBufC* ocspUrl = NULL;
    TRAP_IGNORE(
        repository = CRepository::NewL(KCRUidSWInstallerSettings);
        CleanupStack::PushL(repository);
        ocspUrl = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
        TPtr ptr(ocspUrl->Des());
        repository->Get(KSWInstallerOcspDefaultURL, ptr);
        ret = S60CommonUtils::NativeToJavaString(*aEnv, ptr);
        CleanupStack::PopAndDestroy(ocspUrl);
        CleanupStack::PopAndDestroy(repository);
    );
    return ret;
}
