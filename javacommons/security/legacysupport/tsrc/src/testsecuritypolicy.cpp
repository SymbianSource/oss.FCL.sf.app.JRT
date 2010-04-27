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

#include <stdio.h>
#include <string>

#include "TestHarness.h"

#include <msecuritypolicyv2.h>

using namespace MIDP;

IMPORT_C MSecurityPolicyV2* GetSecurityPolicyL(TUint32 aFlags, const TDesC& aPolicyName);

TEST_GROUP(TestSecurityPolicy)
{

    MSecurityPolicyV2* iSecurityPolicy;

    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
        if (iSecurityPolicy)
        {
            delete iSecurityPolicy;
            iSecurityPolicy = NULL;
        }
    }
};

/**
 */
TEST(TestSecurityPolicy, TestGetSecurityPolicyL)
{
    _LIT(KDummy, "dummy");
    iSecurityPolicy = GetSecurityPolicyL(0 /* security policy flags */, KNullDesC());
    CHECK(iSecurityPolicy != NULL);
    iSecurityPolicy = GetSecurityPolicyL(0 /* security policy flags */, KDummy);
    CHECK(iSecurityPolicy != NULL);
    int error;
    TRAP(error, iSecurityPolicy->InterfaceVersion());
    CHECK(error == KErrNotSupported);
    TRAP(error, iSecurityPolicy->UntrustedPermissionsL());
    CHECK(error == KErrNotSupported);
    TRAP(error, iSecurityPolicy->PermissionsL(KNullDesC()));
    CHECK(error == KErrNotSupported);
    TRAP(error, iSecurityPolicy->ProtectionDomainL(KNullDesC()));
    CHECK(error == KErrNotSupported);
    TRAP(error, iSecurityPolicy->UntrustedProtectionDomainL());
    CHECK(error == KErrNotSupported);
}
