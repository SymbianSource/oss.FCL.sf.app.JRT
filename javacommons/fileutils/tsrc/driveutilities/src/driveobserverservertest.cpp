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
* Description:  DriveObserverServerTest
*
*/

#include "TestHarness.h"

#include "comms.h"
#include "commsserverendpoint.h"

#include "driveutilities.h"

using namespace java::fileutils;

class testListener : public DriveListenerInterface
{
public:
    testListener()
    {   // dummy calls in order to disable false memory leaks in real test cases
//        DriveUtilities::registerListener(this);
//        DriveUtilities::unregisterListener(this);
    }

    virtual void driveChanged(const int& aOperation, const driveInfo& /*aDriveInfo*/)
    {
        CHECK(aOperation == REMOVABLE_MEDIA_INSERTED_C ||
              aOperation == REMOVABLE_MEDIA_REMOVED_C);
    }
} listener2;


TEST_GROUP(DriveObserverServer)
{
    java::comms::CommsServerEndpoint comms;

    TEST_SETUP()
    {
        comms.start(java::comms::IPC_ADDRESS_JAVA_CAPTAIN_C+1);
    }

    TEST_TEARDOWN()
    {
        comms.stop();
    }
};

TEST(DriveObserverServer, 01_AllInOne)
{
    DriveObserverServerInterface* server = NULL;
    server = DriveUtilities::getDriveObserverServer();
    CHECK(server != NULL);
    server->startServer(&comms);

    DriveUtilities::registerListener(&listener2,
                                     java::comms::IPC_ADDRESS_JAVA_CAPTAIN_C+1);

    DriveUtilities::unregisterListener(&listener2,
                                       java::comms::IPC_ADDRESS_JAVA_CAPTAIN_C+1);

    server->stopServer();

    delete server;
}

