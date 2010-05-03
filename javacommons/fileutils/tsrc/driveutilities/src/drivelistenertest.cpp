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
* Description:  DriveObserverClientTest
*
*/

#include "TestHarness.h"

#include "driveutilities.h"

using namespace java::fileutils;

class testListener : public DriveListenerInterface
{
public:
    testListener()
    {   // dummy calls in order to disable false memory leaks in real test cases
        DriveUtilities::registerListener(this);
        DriveUtilities::unregisterListener(this);
    }

    virtual void driveChanged(const int& aOperation, const driveInfo& /*aDriveInfo*/)
    {
        CHECK(aOperation == REMOVABLE_MEDIA_INSERTED_C ||
              aOperation == REMOVABLE_MEDIA_REMOVED_C);
    }
} listener;


TEST_GROUP(DriveListener)
{

    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

TEST(DriveListener, 03_unegisterListenerNonRegistered)
{
    DriveUtilities::unregisterListener(&listener);
}

TEST(DriveListener, 02_doubleRegisterListener)
{
    DriveUtilities::registerListener(&listener);
    DriveUtilities::registerListener(&listener);
    DriveUtilities::unregisterListener(&listener);
}


TEST(DriveListener, 01_registerListener)
{
    DriveUtilities::registerListener(&listener);
    DriveUtilities::unregisterListener(&listener);
}

