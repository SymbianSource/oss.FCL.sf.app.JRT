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

#include "s60commonutils.h"

using namespace std;
using namespace java::util;

_LIT(KUnknownResource, "blahblah");
_LIT(KKnownResource, "pimevent.rsc");
_LIT(KKnownLanguageResource, "pimevent.r01");
_LIT(ZResourceDir, "z:\\resource\\");

TEST_GROUP(TestLanguageResource)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

/**
 * Test JavaCommonUtils::ResourceLanguageFileNameL().
 *
 * 1. Null
 * 2. Unknown resource
 * 3. Known resource
 */
TEST(TestLanguageResource, ResourceLanguageFileNameL)
{
    // 1. Null
    TFileName resource = S60CommonUtils::ResourceLanguageFileNameL(KNullDesC());
    resource.LowerCase();
    CHECK((resource.Length() == ZResourceDir().Length())
          && (resource.Find(ZResourceDir) == 0));
    // 2. Unknown resource
    resource = S60CommonUtils::ResourceLanguageFileNameL(KUnknownResource);
    resource.LowerCase();
    CHECK((resource.Length() == ZResourceDir().Length() + KUnknownResource().Length())
          && (resource.Find(ZResourceDir) == 0)
          && (resource.Find(KUnknownResource) == (resource.Length() - KUnknownResource().Length())));
    // 3. Known resource
    resource = S60CommonUtils::ResourceLanguageFileNameL(KKnownResource);
    resource.LowerCase();
    CHECK((resource.Length() == ZResourceDir().Length() + KKnownLanguageResource().Length())
          && (resource.Find(ZResourceDir) == 0)
          && (resource.Find(KKnownLanguageResource) == (resource.Length() - KKnownLanguageResource().Length())));
}

