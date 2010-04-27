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
* Description:  ?Description
*
*/

#include "TestHarness.h"

#include "comms.h"
#include "commsmessage.h"
#include "commsclientendpoint.h"
#include "commsendpoint.h"
#include "securitycommsmessagedefs.h"

using namespace java::comms;
using namespace std;


TEST_GROUP(SecuritySendReceive)
{
    CommsClientEndpoint cli_conn;

    TEST_SETUP()
    {
        cli_conn.connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
    }

    TEST_TEARDOWN()
    {
        cli_conn.disconnect();
    }
};

/**
 * Test message sending
 * 1. Retrieving info about certificates
 */

TEST(SecuritySendReceive, getCertsPath)
{
    EXPECT_N_LEAKS(2);
    CommsMessage s_msg;
    CommsMessage r_msg;
    std::string path;
    // query enabled certs
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> path;
    CHECK(strcmp(path.c_str(), "") > 0);
    while (strcmp(path.c_str(),""))
    {
        r_msg >> path;
    }
    s_msg.reset();
    r_msg.reset();
    // query disabled certs
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_DISABLED;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> path;
    CHECK(strcmp(path.c_str(), "") == 0);
    s_msg.reset();
    r_msg.reset();
    // query protection domain for certain cert if enabled
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    // add filter
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "F91CE143";
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    std::string protection_domain_name;
    std::string protection_domain_category;
    r_msg >> protection_domain_name;
    r_msg >> protection_domain_category;
    CHECK(strcmp(protection_domain_name.c_str(),"") > 0 &&  strcmp(protection_domain_category.c_str(),"") > 0);
    s_msg.reset();
    r_msg.reset();
    // disable certificate
    std::string ID;
    ID = "98C116D3";
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_DISABLE_CERT;
    s_msg << ID;
    CHECK(cli_conn.send(s_msg) == 0);
    s_msg.reset();
    r_msg.reset();
    // delete certificate
    ID = "6CC300CF";
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_DELETE_CERT;
    s_msg << ID;
    CHECK(cli_conn.send(s_msg) == 0);
    s_msg.reset();
    r_msg.reset();
    // query the full details
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << (JAVA_CERT_STORE_STATE_ENABLED | JAVA_CERT_STORE_STATE_DISABLED);
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    long long cert_content_len;
    r_msg >> cert_content_len;
    CHECK(cert_content_len != 0);
    std::string cert_content;
    int encoded_state;
    while (cert_content_len != 0)
    {
        r_msg >> cert_content;
        CHECK(strcmp(cert_content.c_str(), "") > 0);
        r_msg >> ID;
        CHECK(strcmp(ID.c_str(), "") > 0);
        r_msg >> encoded_state;
        r_msg >> cert_content_len;
    }
    s_msg.reset();
    r_msg.reset();
    // enable certificate
    ID = "98C116D3";
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_ENABLE_CERT;
    s_msg << ID;
    CHECK(cli_conn.send(s_msg) == 0);
    s_msg.reset();
    r_msg.reset();
    // delete certificate
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_DELETE_CERT;
    s_msg << ID;
    CHECK(cli_conn.send(s_msg) == 0);
    s_msg.reset();
    r_msg.reset();
    // enable certificate
    ID = "98C116D3";
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_ENABLE_CERT;
    s_msg << ID;
    CHECK(cli_conn.send(s_msg) == 0);
    s_msg.reset();
    r_msg.reset();
    // enable certificate
    ID = "6CC300CF";
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_ENABLE_CERT;
    s_msg << ID;
    CHECK(cli_conn.send(s_msg) == 0);
    s_msg.reset();
    r_msg.reset();
    // query content
    ID = "6CC300CF";
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << ID;
    s_msg << JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    long long len;
    std::string cert;
    r_msg >> len;
    CHECK(len > 0);
    r_msg >> cert;
    CHECK(strcmp(cert.c_str(), "") > 0);
    s_msg.reset();
    r_msg.reset();
    // Add test case: no metadata (or state) at all
    // Add test case: metadata without domain/category
    // Add test case: metadata with other/garbage data
}
