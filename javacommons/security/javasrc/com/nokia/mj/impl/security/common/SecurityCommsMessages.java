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
* Description:
*
*/

package com.nokia.mj.impl.security.common;

/**
 * This file defines the COMMS messages (both requests and responses) to be
 * used communicating with JavaCertStore (the JavaCaptain plugin, which hosts
 * the certificates used for java signing).
 *
 * The syntax of the (request) COMMS messages to be used when interacting with
 * JavaCertStore is the following:
 *
 *       PLUGIN_ID MESSAGE_ID OPERATION_ID [FILTER_ID FILTER_VALUE] [QUERY_ID]
 *
 * where PLUGIN_ID    = PLUGIN_ID_JAVA_CERT_STORE_EXTENSION
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
 * CommsEndpoint comms = null;
 * try
 * {
 *     comms = new CommsEndpoint();
 *     comms.connect(CommsEndpoint.JAVA_CAPTAIN);
 *     CommsMessage sMessage = new CommsMessage();
 *     sMessage.setMessageId(SecurityCommsMessages.JAVA_CERT_STORE_MSG_ID_REQUEST);
 *     sMessage.setModuleId(SecurityCommsMessages.PLUGIN_ID_JAVA_CERT_STORE_EXTENSION);
 *     sMessage.write(SecurityCommsMessages.JAVA_CERT_STORE_OPERATION_QUERY_CERTS);
 *     // add filter
 *     sMessage.write(SecurityCommsMessages.JAVA_CERT_STORE_FILTER_ID_STATE);
 *     sMessage.write(SecurityCommsMessages.JAVA_CERT_STORE_STATE_ENABLED);
 *     // add the query ID
 *     sMessage.write(SecurityCommsMessages.JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS);
 *     CommsMessage rMessage = comms.sendReceive(sMessage, 10);
 *     // read the reply
 *     int certContentLen = rMessage.readInt();
 *     while (certContentLen != 0)
 *     {
 *         String certContent = rMessage.readString();
 *         String certHash = rMessage.readString();
 *         int certState = rMessage.readInt();
 *         certContentLen = rMessage.readInt();
 *     }
 * }catch(CommsException e)
 * {
 *     System.out.println("Troubles!");
 * }finally
 * {
 *     if (comms != null)
 *     {
 *         comms.destroy();
 *      }
 * }
 * @endcode
 *
 * Usage: Disabling a certificate identified by hash value:
 *
 * @code
 * CommsEndpoint comms = null;
 * try
 * {
 *     comms = new CommsEndpoint();
 *     comms.connect(CommsEndpoint.JAVA_CAPTAIN);
 *     CommsMessage sMessage = new CommsMessage();
 *     sMessage.setMessageId(SecurityCommsMessages.JAVA_CERT_STORE_MSG_ID_REQUEST);
 *     sMessage.setModuleId(SecurityCommsMessages.PLUGIN_ID_JAVA_CERT_STORE_EXTENSION);
 *     sMessage.write(SecurityCommsMessages.JAVA_CERT_STORE_OPERATION_DISABLE_CERT);
 *     // add filter
 *     sMessage.write(SecurityCommsMessages.JAVA_CERT_STORE_FILTER_ID_HASH);
 *     sMessage.write("certHash");
 *     comms.send(sMessage);
 * }catch(CommsException e)
 * {
 *     System.out.println("Troubles!");
 * }finally
 * {
 *     if (comms != null)
 *     {
 *         comms.destroy();
 *      }
 * }
 * @endcode
 */
public final class SecurityCommsMessages
{
    // PLUGIN ID
    public static final int PLUGIN_ID_JAVA_CERT_STORE_EXTENSION   = 7;
    // MESSAGE_IDs
    public static final int JAVA_CERT_STORE_MSG_ID_REQUEST    = 101;
    public static final int JAVA_CERT_STORE_MSG_ID_RESPONSE   = JAVA_CERT_STORE_MSG_ID_REQUEST + 1;
    // OPERATION_IDs
    public static final int JAVA_CERT_STORE_OPERATION_QUERY_CERTS    = 1;
    public static final int JAVA_CERT_STORE_OPERATION_DELETE_CERT    = 2;
    public static final int JAVA_CERT_STORE_OPERATION_DISABLE_CERT   = 3;
    public static final int JAVA_CERT_STORE_OPERATION_ENABLE_CERT    = 4;
    public static final int JAVA_CERT_STORE_OPERATION_REFRESH_CERTS  = 5;
    // FILTER_IDs
    public static final int JAVA_CERT_STORE_FILTER_ID_HASH = 1;
    public static final int JAVA_CERT_STORE_FILTER_ID_STATE = 2;
    // QUERY_IDs
    public static final int JAVA_CERT_STORE_QUERY_ID_PROTECTION_DOMAIN = 3;
    public static final int JAVA_CERT_STORE_QUERY_ID_FULL_DETAILS = 4;
    public static final int JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT = 5;
    public static final int JAVA_CERT_STORE_QUERY_ID_STATE = 6;
    public static final int JAVA_CERT_STORE_QUERY_ID_CERT_CONTENT_PEM = 7;
    // FILTER_VALUEs
    public static final int JAVA_CERT_STORE_STATE_ENABLED = 1;
    public static final int JAVA_CERT_STORE_STATE_DISABLED = 2;
    public static final int JAVA_CERT_STORE_STATE_DELETED = 4;
    public static final int JAVA_CERT_STORE_STATE_UNKNOWN = 8;
    public static final int JAVA_CERT_STORE_STATE_NOT_PRESENT = 16;
}


