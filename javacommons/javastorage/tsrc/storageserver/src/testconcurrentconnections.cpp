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

TEST_GROUP(TestConcurrentConnections)
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
 * 1. Send four open messages and check the session IDs, send one duplicate open message for already open session
 * 2. Close one session and check that session ID has been removed from server
 * 3. Use closed sessions ID to send a read statement, will fail
 * 4. Use third session to write in to DB and close session
 * 5. Check that third session isn't still found from server by sending a read statement
 * 6. Read the inserted data (see 4. part) from DB using the first session
 * 7. Remove inserted data (see 4. part) from DB using the fourth session
 * 8. Close the remaining sessions
 */
TEST(TestConcurrentConnections, TestConcurrentConnections)
{
    CHECK(ctu->connect());

    // 1. Send four open messages and check the session IDs, send one duplicate open message for already open session
    CommsMessage sendMessage;
    CommsMessage receivedMessage;

    string sessionID1 = "";
    string sessionID2 = "";
    string sessionID3 = "";
    string sessionID4 = "";
    string sessionID5 = "";

    sendMessage.setMessageId(IPC_JS_CONN_REG);

    string msgID = JavaCommonUtils::intToString(JavaStorageMessage::EOpen);
    string msgIDSize = JavaCommonUtils::intToString(msgID.size());
    string headers;
    string jsDBName = "JavaStorage";

    headers.insert(0, msgIDSize);
    headers.insert(1, msgID);
    sendMessage<<headers;
    // Pass also the JavaStorage DB name so DB gets initialized
    sendMessage<<jsDBName;

    // Open first
    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    int responseCode = -1;
    receivedMessage>>responseCode;
    receivedMessage>>sessionID1;

    // Open second
    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    responseCode = -1;
    receivedMessage>>responseCode;
    receivedMessage>>sessionID2;

    // Open third
    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    responseCode = -1;
    receivedMessage>>responseCode;
    receivedMessage>>sessionID3;

    // Open fourth
    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    responseCode = -1;
    receivedMessage>>responseCode;
    receivedMessage>>sessionID4;

    sendMessage.reset();
    receivedMessage.reset();

    // Open first again by passing the session ID and check that server returns the same session ID
    string headersFirstSession = sessionID1;
    headersFirstSession.insert(0, msgIDSize);
    headersFirstSession.insert(1, msgID);
    sendMessage<<headersFirstSession;
    sendMessage<<jsDBName;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    responseCode = -1;
    receivedMessage>>responseCode;
    receivedMessage>>sessionID5;

    CHECK(sessionID1 == sessionID5);

    sendMessage.reset();
    receivedMessage.reset();


    // 2. Close one session and check that session ID has been removed from server

    string msgIDClose = JavaCommonUtils::intToString(JavaStorageMessage::EClose);
    string msgIDCloseSize = JavaCommonUtils::intToString(msgIDClose.size());
    string headers2 = "";
    headers2 = sessionID2;
    headers2.insert(0, msgIDSize);
    headers2.insert(1, msgIDClose);
    sendMessage<<headers2;
    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    sendMessage.reset();
    receivedMessage.reset();

    // 3. Use closed sessions ID to send a read statement, will fail
    string msgIDRead = JavaCommonUtils::intToString(JavaStorageMessage::ERead);
    string msgIDReadSize = JavaCommonUtils::intToString(msgIDRead.size());
    string headersRead = "";
    headersRead = sessionID2;
    headersRead.insert(0, msgIDReadSize);
    headersRead.insert(1, msgIDRead);

    string selectStmt = "SELECT ID FROM APPLICATION_PACKAGE;";

    sendMessage<<headersRead;
    sendMessage<<selectStmt;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    int resultID = 0;
    string resultMessage = "";
    receivedMessage>>resultID;
    receivedMessage>>resultMessage;

    CHECK(resultID == EInvalidConnection);
    CHECK(resultMessage == "Invalid sessionID");

    sessionID2 = "";

    sendMessage.reset();
    receivedMessage.reset();

    // 4. Use third session to write in to DB and close session

    string msgIDWrite = JavaCommonUtils::intToString(JavaStorageMessage::EWrite);
    string msgIDWriteSize = JavaCommonUtils::intToString(msgIDWrite.size());
    string headersWrite = "";
    headersWrite = sessionID3;
    headersWrite.insert(0, msgIDWriteSize);
    headersWrite.insert(1, msgIDWrite);

    string writeStmt = "INSERT INTO APPLICATION_PACKAGE (ID) VALUES ('SuperGame');";

    sendMessage<<headersWrite;
    sendMessage<<writeStmt;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    sendMessage.reset();
    receivedMessage.reset();

    string headers3 = sessionID3;
    headers3.insert(0, msgIDCloseSize);
    headers3.insert(1, msgIDClose);
    sendMessage<<headers3;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    sendMessage.reset();
    receivedMessage.reset();

    // 5. Check that third session isn't still found from server by sending a read statement

    headersRead = "";
    headersRead = sessionID3;
    headersRead.insert(0, msgIDReadSize);
    headersRead.insert(1, msgIDRead);

    sendMessage<<headersRead;
    sendMessage<<selectStmt;  // selectStmt declared in 3. part of the test

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    resultID = 0;
    resultMessage = "";
    receivedMessage>>resultID;
    receivedMessage>>resultMessage;

    CHECK(resultID == EInvalidConnection);
    CHECK(resultMessage == "Invalid sessionID");

    sessionID3 = "";

    sendMessage.reset();
    receivedMessage.reset();

    // 6. Read the inserted data (see 4. part) from DB using the first session

    headersRead = "";
    headersRead = sessionID1;
    headersRead.insert(0, msgIDReadSize);
    headersRead.insert(1, msgIDRead);

    sendMessage<<headersRead;
    sendMessage<<selectStmt;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    resultID = 0;
    resultMessage = "";
    receivedMessage>>resultID;
    receivedMessage>>resultMessage;

    CHECK(resultID == 0);
    CHECK(resultMessage == ";#\n;ID=SuperGame;\n;");

    sendMessage.reset();
    receivedMessage.reset();

    // 7. Remove inserted data (see 4. part) from DB using the fourth session

    string msgIDRemove = JavaCommonUtils::intToString(JavaStorageMessage::ERemove);
    string msgIDRemoveSize = JavaCommonUtils::intToString(msgIDRemove.size());

    string headersRemove = "";
    headersRemove = sessionID4;
    headersRemove.insert(0, msgIDRemoveSize);
    headersRemove.insert(1, msgIDRemove);

    string removeStmt = "DELETE FROM APPLICATION_PACKAGE WHERE ID='SuperGame';";

    sendMessage<<headersRemove;
    sendMessage<<removeStmt;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    resultID = 0;
    resultMessage = "";
    receivedMessage>>resultID;
    receivedMessage>>resultMessage;

    sendMessage.reset();
    receivedMessage.reset();

    // 8. Close the remaining sessions

    string headers1 = sessionID1;
    headers1.insert(0, msgIDCloseSize);
    headers1.insert(1, msgIDClose);
    sendMessage<<headers1;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    sendMessage.reset();
    receivedMessage.reset();

    string headers4 = sessionID4;
    headers4.insert(0, msgIDCloseSize);
    headers4.insert(1, msgIDClose);
    sendMessage<<headers4;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    sendMessage.reset();
    receivedMessage.reset();


    CHECK(ctu->disconnect());
}
