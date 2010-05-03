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
* Description:  Defines constants for the COMMS messages of security
*
*/


#ifndef SECURITYCOMMSMESSAGEDEFS_H
#define SECURITYCOMMSMESSAGEDEFS_H

#include "comms.h"

/**
* This header file defines the COMMS messages (both requests and responses)
* to be used communicating with JavaCertStore (the JavaCaptain plugin, which
* hosts the certificates used for java signing).
*
* The syntax of the (request) COMMS messages to be used when interacting with
* JavaCertStore is the following:
*
*       PLUGIN_ID MESSAGE_ID OPERATION_ID [FILTER_ID FILTER_VALUE] [QUERY_ID]
*
* where PLUGIN_ID    = PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C
*       MESSAGE_ID   = JAVA_CERT_STORE_MSG_ID_REQUEST
*       OPERATION_ID = JAVA_CERT_STORE_OPERATION_QUERY_CERTS |
*                      JAVA_CERT_STORE_OPERATION_DELETE_CERT |
*                      JAVA_CERT_STORE_OPERATION_DISABLE_CERT |
*                      JAVA_CERT_STORE_OPERATION_ENABLE_CERT |
*                      JAVA_CERT_STORE_OPERATION_REFRESH_CERTS
*       FILTER_ID    = JAVA_CERT_STORE_FILTER_ID_HASH |
*                      JAVA_CERT_STORE_FILTER_ID_STATE
*       QUERY_ID     = JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN |
*                      JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS |
*                      JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT
* Notes:
* 1) FILTER_VALUE has one of the following values,
*    when used in conjunction with JAVA_CERT_STORE_FILTER_ID_STATE:
*       FILTER_VALUE = JAVA_CERT_STORE_STATE_ENABLED |
*                      JAVA_CERT_STORE_STATE_DISABLED |
*                      JAVA_CERT_STORE_STATE_DELETED
*    Otherwise the FILTER_VALUE is a String value.
* 2) QUERY_ID is to be used only with JAVA_CERT_STORE_OPERATION_QUERY_CERTS
*    operation. If used otherwise, it is ignored.
* 3) JAVA_CERT_STORE_OPERATION_QUERY_CERTS operation is the only operation at the
*    moment which replies (sends back a response):
*                  QUERY_ID                                            Response
*    =====================================================================================================
*    =    JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN    =    String (protection domain)                  =
*    =                                                  =    String (protection domain category)         =
*    =                                                  =                                                =
*    =    JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS         =    Long long (length of the cert;0 is returned =
*    =                                                  =             if no matching found)              =
*    =                                                  =    String (the content of the certificate)     =
*    =                                                  =    String (hash of the certificate)            =
*    =                                                  =    Integer (certificate state)                 =
*    =                                                  =             where certificate state is encoded =
*    =                                                  =             on 3 bits, with MSB set to 1 if    =
*    =                                                  =             the certificate can be deleted     =
*    =                                                  =             (or 0 otherwise), the second bit   =
*    =                                                  =             set to 1 if the certificate can be =
*    =                                                  =             disabled (or 0 otherwise) and the  =
*    =                                                  =             LSB set to 1 the certificate is    =
*    =                                                  =             disabled (or 0 othewaise)          =
*    =                                                  =                                                =
*    =    JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT         =    Long long (length of the cert;0 is returned =
*    =                                                  =             if no matching found)              =
*    =                                                  =    String (the content of the certificate)     =
*    =                                                  =                                                =
*    =    If nothing specifically was queried           =    String (full path to the certificate)       =
*    =====================================================================================================
*
*
* Usage: Querying the full details of the enabled certificates:
*
* @code
* CommsClientEndpoint cli_conn;
* if (cli_conn.connect(IPC_ADDRESS_JAVA_CAPTAIN_C) != 0) {
*     printf("Troubles!\n");
*     return;
* }
* CommsMessage s_msg;
* CommsMessage r_msg;
* s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
* s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
* s_msg << JAVA_CERT_STORE_OPERATION_QUERY_CERTS;
* s_msg << JAVA_CERT_STORE_FILTER_ID_STATE << JAVA_CERT_STORE_STATE_ENABLED;
* s_msg << JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS;
* if (cli_conn.sendReceive(s_msg,r_msg,10) != 0)
* {
*     printf("Troubles!\n");
*     return;
* }
* long long cert_content_len;
* r_msg >> cert_content_len;
* while (cert_content_len != 0)
* {
*     std::string cert_content;
*     std::string hash:
*     int state;
*     r_msg >> cert_content;
*     r_msg >> hash;
*     r_msg >> state;
*     r_msg >> cert_content_len;
* }
* @endcode
*
* Usage: Disabling a certificate identified by hash value:
*
* @code
* CommsClientEndpoint cli_conn;
* if (cli_conn.connect(IPC_ADDRESS_JAVA_CAPTAIN_C) != 0) {
*     printf("Troubles!\n");
*     return;
* }
* CommsMessage s_msg;
* s_msg.setModuleId(PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C);
* s_msg.setMessageId(JAVA_CERT_STORE_MSG_ID_REQUEST);
* s_msg << JAVA_CERT_STORE_OPERATION_DISABLE_CERT;
* s_msg << JAVA_CERT_STORE_FILTER_ID_HASH << "certHash";
* if (cli_conn.sendReceive(s_msg,r_msg,10) != 0)
* {
*     printf("Troubles!\n");
*     return;
* }
* @endcode
*
* @see JavaCertStore for details on the server-side handling on the COMMS
*      messages
*/
// MESSAGE_IDs
const int JAVA_CERT_STORE_MSG_ID_REQUEST    = java::comms::JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 1;
const int JAVA_CERT_STORE_MSG_ID_RESPONSE   = JAVA_CERT_STORE_MSG_ID_REQUEST + 1;
// OPERATION_IDs
const int JAVA_CERT_STORE_OPERATION_QUERY_CERTS    = 1;
const int JAVA_CERT_STORE_OPERATION_DELETE_CERT    = 2;
const int JAVA_CERT_STORE_OPERATION_DISABLE_CERT   = 3;
const int JAVA_CERT_STORE_OPERATION_ENABLE_CERT    = 4;
const int JAVA_CERT_STORE_OPERATION_REFRESH_CERTS  = 5;
// FILTER_IDs
const int JAVA_CERT_STORE_FILTER_ID_HASH = 1;
const int JAVA_CERT_STORE_FILTER_ID_STATE = 2;
// QUERY_IDs
const int JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN = 3;
const int JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS = 4;
const int JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT = 5;
const int JAVA_CERT_STORE_QUERY_ID_STATE = 6;
const int JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT_PEM = 7;
// FILTER_VALUEs
const int JAVA_CERT_STORE_STATE_ENABLED = 1;
const int JAVA_CERT_STORE_STATE_DISABLED = 2;
const int JAVA_CERT_STORE_STATE_DELETED = 4;
const int JAVA_CERT_STORE_STATE_UNKNOWN = 8;
const int JAVA_CERT_STORE_STATE_NOT_PRESENT = 16;

#endif // SECURITYCOMMSMESSAGEDEFS_H


