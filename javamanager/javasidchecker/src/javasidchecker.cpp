/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Java Sid Checker is an ECOM plugin for Symbian AppArc
 *                (database that contains info on all applications installed
 *                 to the device)
 *                It tells AppArc whether Java application installed
 *                to a removable storage media is present and can be executed.
 *
*/


#include <e32std.h>
#include <implementationproxy.h>
#include <appversion.h>

#include "javauids.h"
#include "javasidchecker.h"
#include "logger.h"


// DLL Entry point
TBool E32Dll()
{
    return ETrue;
}

// ECOM Implementation Table
const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(KSidCheckerEcomImplUid, CJavaSidChecker::NewL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}


CJavaSidChecker* CJavaSidChecker::NewL()
{
    // Minimal construction (we don't want this to fail)
    return new(ELeave) CJavaSidChecker;
}

CJavaSidChecker::CJavaSidChecker()
{
    iJavaRegistry = NULL;
    LOG(EUtils, EInfo, "CJavaSidChecker: Constructor called");
}

CJavaSidChecker::~CJavaSidChecker()
{
    if (NULL != iJavaRegistry)
    {
        delete iJavaRegistry;
        iJavaRegistry = NULL;
    }
    LOG(EUtils, EInfo, "CJavaSidChecker: Destructor called");
}

TBool CJavaSidChecker::AppRegisteredAt(const TUid& aSid, TDriveUnit aDrive)
{
    TBool installed = EFalse;

    LOG1(EUtils, EInfo, "CJavaSidChecker: AppRegisteredAt called, %x", aSid.iUid);

    // No Java application can have Uid 0
    if (aSid.iUid == 0)
    {
        return installed;
    }

    TRAPD(err, installed = AppRegisteredAtL(aSid, aDrive));
    if (KErrNone != err)
    {
        ELOG1(EUtils, "CJavaSidChecker: AppRegisteredAt error %d", err);
    }

    return installed;
}

TBool CJavaSidChecker::AppRegisteredAtL(const TUid& aSid, TDriveUnit /*aDrive*/)
{
    // Create the Java Registry if it has not been created already
    if (NULL == iJavaRegistry)
    {
        iJavaRegistry = Java::CJavaRegistry::NewL();
    }

    // JavaRegistry does not return entries that are not present.
    return iJavaRegistry->RegistryEntryExistsL(aSid);
}

void CJavaSidChecker::SetRescanCallBackL(const TCallBack &/*aCallBack*/)
{
    // Do nothing
    return;
}
