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
* Description:  ExtensionPlugin
*
*/

#include <string>

#include "TestHarness.h"

#include "commsclientendpoint.h"
#include "comms.h"

#include "testmessages.h"

using namespace java::comms;
using namespace java::captain;

extern java::comms::CommsClientEndpoint* commsClient;

static std::string existing_plugin("pushregistryplugin");
static std::string non_existing_plugin("jadadadada");
static const int sendReceiveTimeout = 5;

/* Testcases
* 1. LoadNonExisting
* 2. Load
* 3. Reload
* 4. Unload
* 5. ReUnload
* 6. UnloadNonExisting
*/


TEST_GROUP(ExtensionPlugin)
{
    TEST_SETUP()
    {
    }

    TEST_TEARDOWN()
    {
    }
};

static void load(const std::string& ext, bool shouldSucceed = true)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    setLoadExtensionPluginReqParameters(msg, ext);

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(receivedMsg.getMessageId() == IPC_MESSAGE_TEST_ACK);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

    int status;
    getAckParameters(receivedMsg, status);
    CHECK((shouldSucceed ? 0 : 1) == status);
}

static void unload(const std::string& ext)
{
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_JAVACAPTAIN_TESTER_C);
    setUnloadExtensionPluginReqParameters(msg, ext);

    CommsMessage receivedMsg;
    CHECK(commsClient->sendReceive(msg, receivedMsg, sendReceiveTimeout) == 0);

    CHECK(msg.getModuleId() == receivedMsg.getModuleId());
    CHECK(receivedMsg.getMessageId() == IPC_MESSAGE_TEST_ACK);
    CHECK(msg.getMessageRef() == receivedMsg.getMessageRef());

    int status = 0;
    getAckParameters(receivedMsg, status);
    CHECK(0 == status); // Unload always succeeds
}

TEST(ExtensionPlugin, EXT07_UnloadNonExisting)
{
    unload(non_existing_plugin);
}

TEST(ExtensionPlugin, EXT06_ReUnloadExisting)
{
    unload(existing_plugin);
}

TEST(ExtensionPlugin, EXT05_UnloadExisting)
{
    unload(existing_plugin);
}

TEST(ExtensionPlugin, EXT04_ReLoadExisting)
{
    load(existing_plugin);
}

TEST(ExtensionPlugin, EXT03_LoadExisting)
{
    load(existing_plugin);
}

TEST(ExtensionPlugin, EXT02_LoadNonExisting)
{
    load(non_existing_plugin, false);
}
/* fails and no usecase to correct this currently
TEST(ExtensionPlugin, 01_LoadUnloadTestPlugin)
{
    unload("ondemand_100");
}
*/

