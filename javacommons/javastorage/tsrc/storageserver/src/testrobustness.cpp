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
#include <vector>

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

TEST_GROUP(TestRobustness)
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
 * 1. Open one session
 * 2. Use session to write in to DB
 * 3. Open many sessions and store the session IDs
 * 4. Read inserted data with each session
 * 5. Open one session to remove the test data inserted in test part 2
 */
TEST(TestRobustness, TestOpenReadClose)
{
    CHECK(ctu->connect());

    // 1. Open one session
    CommsMessage sendMessage;
    CommsMessage receivedMessage;

    string iCurrentSessionID;
    vector<string> iDbSessionList;
    vector<string>::iterator iDbSessionListIter;

    string sessionID = "";

    sendMessage.setMessageId(IPC_JS_CONN_REG);

    string msgID = JavaCommonUtils::intToString(JavaStorageMessage::EOpen);
    string msgIDSize = JavaCommonUtils::intToString(msgID.size());
    string headers = "";
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
    receivedMessage>>sessionID;

    iDbSessionList.push_back(sessionID);

    // 2. Use session to write in to DB

    string msgIDWrite = JavaCommonUtils::intToString(JavaStorageMessage::EWrite);
    string msgIDWriteSize = JavaCommonUtils::intToString(msgIDWrite.size());
    string headersWrite = "";
    headersWrite = sessionID;
    headersWrite.insert(0, msgIDWriteSize);
    headersWrite.insert(1, msgIDWrite);

    string writeStmt = "INSERT INTO APPLICATION_PACKAGE (ID) VALUES ('SuperGame');";

    sendMessage<<headersWrite;
    sendMessage<<writeStmt;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    sendMessage.reset();
    receivedMessage.reset();

    // 3. Open many sessions and store the session IDs

    for (int i = 0; i < 1000; i++)
    {
        headers.insert(0, msgIDSize);
        headers.insert(1, msgID);
        sendMessage<<headers;
        sendMessage<<jsDBName;

        CHECK(ctu->sendReceive(sendMessage, receivedMessage));
        int responseCode = -1;
        receivedMessage>>responseCode;
        receivedMessage>>sessionID;

        iDbSessionList.push_back(sessionID);
    }

    // 4. Read inserted data with each session and close the sessions

    string readStmt = "SELECT ID FROM APPLICATION_PACKAGE WHERE ID='SuperGame';";

    for (iDbSessionListIter = iDbSessionList.begin(); iDbSessionListIter != iDbSessionList.end(); iDbSessionListIter++)
    {
        iCurrentSessionID = *iDbSessionListIter;

        string msgIDRead = JavaCommonUtils::intToString(JavaStorageMessage::ERead);
        string msgIDReadSize = JavaCommonUtils::intToString(msgIDRead.size());
        string headersRead = "";

        headersRead = iCurrentSessionID;
        headersRead.insert(0, msgIDReadSize);
        headersRead.insert(1, msgIDRead);

        sendMessage<<headersRead;
        sendMessage<<readStmt;

        CHECK(ctu->sendReceive(sendMessage, receivedMessage));

        sendMessage.reset();
        receivedMessage.reset();

        string msgIDClose = JavaCommonUtils::intToString(JavaStorageMessage::EClose);
        string msgIDCloseSize = JavaCommonUtils::intToString(msgIDClose.size());
        string headersClose = "";

        headersClose = iCurrentSessionID;
        headersClose.insert(0, msgIDReadSize);
        headersClose.insert(1, msgIDRead);

        sendMessage<<headersClose;
        CHECK(ctu->sendReceive(sendMessage, receivedMessage));

        sendMessage.reset();
        receivedMessage.reset();
    }

    // 5. Open one session to remove the test data inserted in test part 2 and close session

    headers = "";

    headers.insert(0, msgIDSize);
    headers.insert(1, msgID);
    sendMessage<<headers;
    sendMessage<<jsDBName;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    responseCode = -1;
    receivedMessage>>responseCode;
    receivedMessage>>sessionID;

    sendMessage.reset();
    receivedMessage.reset();

    string msgIDRemove = JavaCommonUtils::intToString(JavaStorageMessage::ERemove);
    string msgIDRemoveSize = JavaCommonUtils::intToString(msgIDRemove.size());
    string headersRemove = "";
    headersRemove = sessionID;
    headersRemove.insert(0, msgIDRemoveSize);
    headersRemove.insert(1, msgIDRemove);

    string removeStmt = "DELETE FROM APPLICATION_PACKAGE WHERE ID='SuperGame';";

    sendMessage<<headersRemove;
    sendMessage<<removeStmt;

    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    sendMessage.reset();
    receivedMessage.reset();

    string msgIDClose = JavaCommonUtils::intToString(JavaStorageMessage::EClose);
    string msgIDCloseSize = JavaCommonUtils::intToString(msgIDClose.size());
    string headersClose = "";

    headersClose = sessionID;
    headersClose.insert(0, msgIDCloseSize);
    headersClose.insert(1, msgIDClose);

    sendMessage<<headersClose;
    CHECK(ctu->sendReceive(sendMessage, receivedMessage));

    sendMessage.reset();
    receivedMessage.reset();

    CHECK(ctu->disconnect());
}
