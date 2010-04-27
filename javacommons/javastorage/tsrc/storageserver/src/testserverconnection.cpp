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

#include "servertestutils.h"
#include "commsmessage.h"
#include "javastoragemessage.h"
#include "javacommonutils.h"
#include "javastorageexception.h"

using namespace std;
using namespace java::comms;
using namespace java::storage;
using namespace java::util;

TEST_GROUP(TestServerConnection)
{
    ServerTestUtils* ctu;

    TEST_SETUP()
    {
        ctu = new ServerTestUtils();
    }
    TEST_TEARDOWN()
    {
        if (ctu)
        {
            delete ctu;
            ctu = NULL;
        }
    }
};

/**
 * Test storageserver.cpp.
 *
 * 1. Test empty comms message.
 * 2. Test send one len message INT
 * 3. Test send one len message string
 * 4. Test only string header
 * 5. Test only wstring message
 * 6. Test send invalid message identifier
 * 7. Invalid header part
 * 8. Invalid statement. One len. DB must handle this.
 */
TEST(TestServerConnection, TestServerConnection)
{
    CHECK(ctu->connect());

    // 1. Send empty message
    CommsMessage sendMessage;
    sendMessage.setMessageId(IPC_JS_CONN_REG);

    CommsMessage receivedMessage;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    int responseCode = -1;
    receivedMessage>>responseCode;

    CHECK(responseCode == EInvalidDataStructure)

    sendMessage.reset();
    receivedMessage.reset();

    // 2, Send one len message INT
    sendMessage.setMessageId(IPC_JS_CONN_REG);
    int code = 1;
    sendMessage<<code;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    receivedMessage>>responseCode;
    CHECK(responseCode == EInvalidDataStructure)

    sendMessage.reset();
    receivedMessage.reset();

    // 3. Send one len message string
    sendMessage.setMessageId(IPC_JS_CONN_REG);
    string test = "A";
    sendMessage<<test;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    receivedMessage>>responseCode;
    CHECK(responseCode == EInvalidDataStructure)

    sendMessage.reset();
    receivedMessage.reset();

    // 4. Only valid string header. Check no crashing.
    sendMessage.setMessageId(IPC_JS_CONN_REG);
    string headers = "";

    string msgID = JavaCommonUtils::intToString(JavaStorageMessage::EWrite);
    string msgIDSize = JavaCommonUtils::intToString(msgID.size());
    headers.insert(0, msgIDSize);
    headers.insert(1, msgID);

    sendMessage<<headers;
    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    sendMessage.reset();
    receivedMessage.reset();

    // 5. Only wstring message
    sendMessage.setMessageId(IPC_JS_CONN_REG);
    wstring statement = L"SELECT * FROM APPLICATION_DATA;";
    sendMessage<<statement;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    receivedMessage>>responseCode;
    CHECK(responseCode == EInvalidDataStructure)

    sendMessage.reset();
    receivedMessage.reset();

    // 6. Only invalid message identifier
    sendMessage.setMessageId(IPC_JS_CONN_REG);
    headers = "";

    msgID = JavaCommonUtils::intToString(
                JavaStorageMessage::EWrite + 800);
    msgIDSize = JavaCommonUtils::intToString(msgID.size());
    headers.insert(0, msgIDSize);
    headers.insert(1, msgID);

    sendMessage<<headers;

    // Include also statement that invalid Id gets detected
    statement = L"SELECT * FROM APPLICATION_DATA;";
    sendMessage<<statement;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    receivedMessage>>responseCode;
    CHECK(responseCode == EInvalidConnection)

    sendMessage.reset();
    receivedMessage.reset();

    // 7. Invalid header part
    // msgId size is 9 but id is not included at all.
    sendMessage.setMessageId(IPC_JS_CONN_REG);
    headers = "9";

    sendMessage<<headers;

    // Include also statement that invalid Id gets detected
    statement = L"SELECT * FROM APPLICATION_DATA;";
    sendMessage<<statement;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    receivedMessage>>responseCode;
    CHECK(responseCode == EInvalidDataStructure)

    sendMessage.reset();
    receivedMessage.reset();

    CHECK(ctu->disconnect());
}
