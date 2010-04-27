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
#include <e32std.h>

#include "TestHarness.h"
#include "comms.h"
#include "commsmessage.h"
#include "commsclientendpoint.h"
#include "commsendpoint.h"
#include "securitycommsmessagedefs.h"

using namespace java::comms;
using namespace std;

TEST_GROUP(TestCertificatesConfigurator)
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

TEST(TestCertificatesConfigurator, TestCertificatesConfiguratorL)
{
    CommsMessage s_msg;
    CommsMessage r_msg;
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "DB6368D9";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    std::string prot_domain_name;
    std::string prot_domain_category;
    r_msg >> prot_domain_name;
    r_msg >> prot_domain_category;
    CHECK(prot_domain_name == "Manufacturer" && prot_domain_category == "MFD");
    s_msg.reset();
    r_msg.reset();
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "DB6368D9";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    long long cert_content_len;
    r_msg >> cert_content_len;
    CHECK(cert_content_len != 0);
    std::string cert_content;
    std::string hash;
    int state;
    r_msg >> cert_content;
    r_msg >> hash;
    r_msg >> state;
    CHECK(state == 0 /* not disablable and not deletable */);
    s_msg.reset();
    r_msg.reset();


    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "3BACF549";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> prot_domain_name;
    r_msg >> prot_domain_category;
    CHECK(prot_domain_name == "Manufacturer" && prot_domain_category == "MFD");
    s_msg.reset();
    r_msg.reset();
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "3BACF549";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> cert_content_len;
    CHECK(cert_content_len != 0);
    r_msg >> cert_content;
    r_msg >> hash;
    r_msg >> state;
    CHECK(state == 0/* not disablable and not deletable */);
    s_msg.reset();
    r_msg.reset();

    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "C200DD68";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> prot_domain_name;
    r_msg >> prot_domain_category;
    CHECK(prot_domain_name == "IdentifiedThirdParty" && prot_domain_category == "ITPD");
    s_msg.reset();
    r_msg.reset();
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "C200DD68";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> cert_content_len;
    CHECK(cert_content_len != 0);
    r_msg >> cert_content;
    r_msg >> hash;
    r_msg >> state;
    CHECK(state == 0/* not disablable and not deletable */);
    s_msg.reset();
    r_msg.reset();

    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "1A9F76AE";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> prot_domain_name;
    r_msg >> prot_domain_category;
    CHECK(prot_domain_name == "Operator" && prot_domain_category == "OPD");
    s_msg.reset();
    r_msg.reset();
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "1A9F76AE";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> cert_content_len;
    CHECK(cert_content_len != 0);
    r_msg >> cert_content;
    r_msg >> hash;
    r_msg >> state;
    CHECK(state == 0/* not disablable and not deletable */);
    s_msg.reset();
    r_msg.reset();

    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "160ED1E9";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_DISABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> prot_domain_name;
    r_msg >> prot_domain_category;
    CHECK(prot_domain_name == "OperatorExtra" && prot_domain_category == "OPD");
    s_msg.reset();
    r_msg.reset();
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "160ED1E9";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_DISABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> cert_content_len;
    CHECK(cert_content_len != 0);
    r_msg >> cert_content;
    r_msg >> hash;
    r_msg >> state;
    CHECK(state == 1/* not disablable and not deletable but disabled */);
    s_msg.reset();
    r_msg.reset();

    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "50957A0B";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_DELETED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> prot_domain_name;
    r_msg >> prot_domain_category;
    CHECK(prot_domain_name == "Operator" && prot_domain_category == "OPD");
    s_msg.reset();
    r_msg.reset();
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "50957A0B";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_DELETED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> cert_content_len;
    CHECK(cert_content_len != 0);
    r_msg >> cert_content;
    r_msg >> hash;
    r_msg >> state;
    CHECK(state == 0/* not disablable and not deletable */);
    s_msg.reset();
    r_msg.reset();

    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "19812ECB";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> prot_domain_name;
    r_msg >> prot_domain_category;
    CHECK(prot_domain_name == "Operator" && prot_domain_category == "OPD");
    s_msg.reset();
    r_msg.reset();
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "19812ECB";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> cert_content_len;
    CHECK(cert_content_len != 0);
    r_msg >> cert_content;
    r_msg >> hash;
    r_msg >> state;
    CHECK(state == 0/* not disablable and not deletable */);
    s_msg.reset();
    r_msg.reset();

    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "6F70725D";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> prot_domain_name;
    r_msg >> prot_domain_category;
    CHECK(prot_domain_name == "IdentifiedThirdParty" && prot_domain_category == "ITPD");
    s_msg.reset();
    r_msg.reset();
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "6F70725D";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> cert_content_len;
    CHECK(cert_content_len != 0);
    r_msg >> cert_content;
    r_msg >> hash;
    r_msg >> state;
    CHECK(state == 2 /* only disablable */);
    s_msg.reset();
    r_msg.reset();

    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "C9058545";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> prot_domain_name;
    r_msg >> prot_domain_category;
    CHECK(prot_domain_name == "IdentifiedThirdParty" && prot_domain_category == "ITPD");
    s_msg.reset();
    r_msg.reset();
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "C9058545";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> cert_content_len;
    CHECK(cert_content_len != 0);
    r_msg >> cert_content;
    r_msg >> hash;
    r_msg >> state;
    CHECK(state == 0/* not disablable and not deletable */);
    s_msg.reset();
    r_msg.reset();

    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "0EE64645";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> prot_domain_name;
    r_msg >> prot_domain_category;
    CHECK(prot_domain_name == "IdentifiedThirdParty" && prot_domain_category == "ITPD");
    s_msg.reset();
    r_msg.reset();
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "0EE64645";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> cert_content_len;
    CHECK(cert_content_len != 0);
    r_msg >> cert_content;
    r_msg >> hash;
    r_msg >> state;
    CHECK(state == 4 /* only deletable */);
    s_msg.reset();
    r_msg.reset();

    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "1775ABBE";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> prot_domain_name;
    r_msg >> prot_domain_category;
    CHECK(prot_domain_name == "IdentifiedThirdParty" && prot_domain_category == "ITPD");
    s_msg.reset();
    r_msg.reset();
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "1775ABBE";
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
    CHECK(cli_conn.sendReceive(s_msg,r_msg,10) == 0);
    r_msg >> cert_content_len;
    CHECK(cert_content_len != 0);
    r_msg >> cert_content;
    r_msg >> hash;
    r_msg >> state;
    CHECK(state == 6 /* deletable and disablable */);
    s_msg.reset();
    r_msg.reset();
}

