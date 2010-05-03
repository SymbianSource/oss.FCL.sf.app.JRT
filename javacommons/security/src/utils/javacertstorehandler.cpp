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
#include <string.h>

#include "javacertstorehandler.h"

#include "comms.h"
#include "commsclientendpoint.h"
#include "commsmessage.h"
#include "securitycommsmessagedefs.h"

using namespace std;
using namespace java::security;
using namespace java::comms;

void JavaCertStoreHandler::retrieveRootProtDomainInfo(const char rootHash[],
        string& protDomainName,
        string& protDomainCategory)
{
    CommsClientEndpoint cli_conn;
    if (cli_conn.connect(IPC_ADDRESS_JAVA_CAPTAIN_C) != 0)
    {
        return;
    }
    CommsMessage s_msg;
    CommsMessage r_msg;
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    // add filter
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << rootHash;
    // add the query ID
    s_msg << JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN;
    if (cli_conn.sendReceive(s_msg,r_msg,10) != 0)
    {
        return;
    }
    r_msg >> protDomainName;
    if (strcmp(protDomainName.c_str(),""))
    {
        r_msg >> protDomainCategory;
    }
    cli_conn.disconnect();
}

void JavaCertStoreHandler::retrieveRootsContents(std::vector<std::string>& aRootsContents)
{
    CommsClientEndpoint cli_conn;
    if (cli_conn.connect(IPC_ADDRESS_JAVA_CAPTAIN_C) != 0)
    {
        return;
    }
    CommsMessage s_msg;
    CommsMessage r_msg;
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    // operation
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    // add filter
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    // add the query ID
    s_msg << JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT;
    if (cli_conn.sendReceive(s_msg,r_msg,10) != 0)
    {
        return;
    }
    long long tmp;
    r_msg >> tmp;
    while (tmp != 0)
    {
        string rootContent;
        r_msg >> rootContent;
        aRootsContents.push_back(rootContent);
        r_msg >> tmp;
    }
    cli_conn.disconnect();
}

void JavaCertStoreHandler::retrieveRootContent(wstring& rootHash,
        long long* len,
        string& rootContent)
{
    CommsClientEndpoint cli_conn;
    if (cli_conn.connect(IPC_ADDRESS_JAVA_CAPTAIN_C) != 0)
    {
        return;
    }
    CommsMessage s_msg;
    CommsMessage r_msg;
    s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
    // operation
    s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
    s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
    // add filter
    s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
    s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << rootHash;
    // add the query ID
    s_msg << JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT;
    if (cli_conn.sendReceive(s_msg,r_msg,10) != 0)
    {
        return;
    }
    long long tmp;
    r_msg >> tmp;
    *len = tmp;
    if (tmp == 0)
    {
        return;
    }
    r_msg >> rootContent;
    cli_conn.disconnect();
}
