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
* Description:  GetDrivesTest
*
*/

#include "TestHarness.h"

#include "commsclientendpoint.h"
#include "driveutilities.h"
#include "driveobservermessages.h"

using namespace java::fileutils;
using namespace java::comms;

TEST_GROUP(GetDrives)
{
    CommsClientEndpoint comms;

    TEST_SETUP()
    {
        comms.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
    }

    TEST_TEARDOWN()
    {
        comms.disconnect();
    }
};

TEST(GetDrives, 04_GetAccesibleDrives_comms)
{
    CommsMessage msg;
    setGetDrivesParams(msg, DRIVEOBSERER_GET_DRIVES_ACCESIBLE);

    CommsMessage reply;
    CHECK(!comms.sendReceive(msg, reply, 5));

    driveInfos drives;
    getGetDrivesResultParams(reply, drives);
    CHECK(drives.size() > 0);
}

TEST(GetDrives, 03_GetAccesibleDrives_method)
{
    driveInfos drives;
    DriveUtilities::getAccesibleDrives(drives);
    CHECK(drives.size() > 0);
}

TEST(GetDrives, 02_GetAllDrives_comms)
{
    CommsMessage msg;
    setGetDrivesParams(msg, DRIVEOBSERER_GET_DRIVES_ALL);

    CommsMessage reply;
    CHECK(!comms.sendReceive(msg, reply, 5));

    driveInfos drives;
    getGetDrivesResultParams(reply, drives);
    CHECK(drives.size() > 0);
}

TEST(GetDrives, 01_GetAllDrives_method)
{
    driveInfos drives;
    DriveUtilities::getAllDrives(drives);
    CHECK(drives.size() > 0);
}

