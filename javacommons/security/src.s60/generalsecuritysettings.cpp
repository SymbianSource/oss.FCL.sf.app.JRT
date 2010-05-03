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
#include "com_nokia_mj_impl_security_midp_common_GeneralSecuritySettings.h"

using namespace java::util;

// Java Security Policy Central Repository UID
static const TUid KCRUidJavaSecurityPolicy = { 0x2001B289 };
// IDs for security policy and security warnings mode
static const TUint32 KPolicyFileName = 0x01;
static const TUint32 KPolicy = 0x03;
static const TUint32 KWarningsMode = 0x04;

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_security_midp_common_GeneralSecuritySettings__1getSecurityPolicy
(JNIEnv * aEnv, jclass)
{
    jstring ret = NULL;
    CRepository* repository = NULL;
    HBufC* buf = NULL;
    TRAP_IGNORE(
        repository = CRepository::NewLC(KCRUidJavaSecurityPolicy);
        buf = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
        TPtr policy(buf->Des());
        TInt err = repository->Get(KPolicy, policy);
        if (err != KErrNone)
{
    // one more try // remove this once the new key (0x03) is in place
    User::LeaveIfError(repository->Get(KPolicyFileName, policy));
        TInt sep = policy.Find(_L("_"));
        if (sep > 0)
        {
            policy.Set(policy.LeftTPtr(sep));
            policy.SetLength(sep);
        }
    }
    ret = S60CommonUtils::NativeToJavaString(*aEnv, policy);
    CleanupStack::PopAndDestroy(buf);
    CleanupStack::PopAndDestroy(repository);
    );
    return ret;
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_security_midp_common_GeneralSecuritySettings__1getDefaultSecurityWarningsMode
(JNIEnv *, jclass)
{
    TInt ret = -1;
    CRepository* repository = NULL;
    TRAP_IGNORE(
        repository = CRepository::NewLC(KCRUidJavaSecurityPolicy);
        User::LeaveIfError(repository->Get(KWarningsMode, ret));
        CleanupStack::PopAndDestroy(repository);
    );
    return ret;
}

