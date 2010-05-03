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


#ifndef PUSHREGISTRYMESSAGES_H
#define PUSHREGISTRYMESSAGES_H

#include "comms.h"

using namespace java::comms;

namespace java
{
namespace push
{

//This message retrieves all push connections of the MIDlet.
//First argument:
//Uid object => UID of the MIDlet.
const int IPC_PR_GET_ALL_PUSH_REGS = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 1;

//First argument:
//Uid object => UID of the MIDlet.
//Second argument:
//int => status
//0 => success case, less than 0 means the error case
//Third argument:
//string => error text
//This is empty string("") in the success case.
//0...n arguments:
//wstring => push registry URI.
//wstring => push registry filter.
const int IPC_PR_LIST_OF_ALL_PUSH_REGS = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 2;

//First argument:
//Uid object => UID of the MIDlet.
//Second argument:
//wstring => URI
//Third argument:
//wstring => name of the MIDlet
//Fourth argument:
//wstring => filter of the dynamic push connection
//Fifth argument:
//int => this value indicates whether dynamic push conn is registered to the same MIDlet where
//        PushRegistry.registerConnection() operation was called.
//        0 = uid belongs to the other MIDlet in the same MIDlet suite.
//        1 = uid belongs to the same MIDlet where registerConnection() operation was called.
const int IPC_PR_REGISTER_DYNAMIC_PUSH_CONN = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 3;

//First argument:
//Uid object => UID of the MIDlet.
//Second argument:
//int => status
//0 => success case, less than 0 means the error case
//Third argument:
//string => error text
//This is empty string("") in the success case.
const int IPC_PR_REGISTER_DYNAMIC_PUSH_CONN_REPLY = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 4;

//First argument:
//Uid object => UID of the MIDlet.
//Second argument:
//wstring => URI
//Third argument:
//int => this value indicates whether dynamic push conn is unregistered from the same MIDlet where
//        PushRegistry.registerConnection() operation was called.
//        0 = uid belongs to the other MIDlet in the same MIDlet suite.
//        1 = uid belongs to the same MIDlet where registerConnection() operation was called.
const int IPC_PR_UNREGISTER_DYNAMIC_PUSH_CONN = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 5;

//First argument:
//Uid object => UID of the MIDlet.
//Second argument:
//int => status
//0 => success case, less than 0 means the error case
//Third argument:
//string => error text
//This is empty string("") in the success case.
const int IPC_PR_UNREGISTER_DYNAMIC_PUSH_CONN_REPLY = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 6;

//First argument:
//Uid object => UID of the MIDlet suite.
//Second argument:
//long => alarm time in milliseconds.
//Third argument:
//wstring => MIDlet's class name
//Fourth argument:
//int => this value indicates whether alarm is registered to the other MIDlet in the MIDlet suite.
//        0 = uid belongs to the other MIDlet in the same MIDlet suite.
//        1 = uid belongs to 'this' MIDlet.
const int IPC_PR_REGISTER_ALARM = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 7;

//First argument:
//Uid object => UID of the MIDlet suite.
//Second argument:
//int => status
//0 => success case, less than 0 means the error case
//Third argument:
//string => error text
//This is empty string("") in the success case.
//Fourth argument:
//long => time Existing wake up time if alarm is in the pending state.
//            Value of this argument is zero if wake up time is not already pending.
const int IPC_PR_REGISTER_ALARM_REPLY = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 8;

/**
 * This message is sent from PushController to PushRegistryPlugin in the situation wherein
 * all push connections in the listen mode has failed.
 * Note: This message does not have reply message.
 * First argument:
 * Uid object => UID of the MIDlet.
 */
const int IPC_PR_ALL_CONNS_FAILED = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 9;

/**
 * This message is sent from PushController to PushRegistryPlugin in the following situations:
 * 1. Message has arrived to the push connection.
 * 2. Application has started to handle arrived message of the push connection.
 * First argument:
 * Uid object => UID of the MIDlet.
 * Second argument:
 * wstring => URI of the push connection.
 * Third argument:
 * int => this value contains pending status of the push connection.
 *        0 = Application has started to handle arrived msg of the push connection.
 *        1 = Msg has arrived to the push connection and application has not handled it yet.
 */
const int IPC_PR_PENDING_STATUS = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 10;

/**
 * This message retrieves URIs of all MIDlets' push connections in the MIDlet suite.
 * First argument:
 * Uid object => UID of the MIDlet suite.
 * int => this value indicates whether all push connections are returned or
 *        just "pending connections".
 *        0 = all push connections are returned.
 *        1 = only push connections which waiting application's handling.
 */
const int IPC_PR_LIST_CONNECTIONS = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 11;

/**
 * This message is reply message to the IPC_PR_LIST_CONNECTIONS message.
 * First argument:
 * Uid object => UID of the MIDlet suite.
 * Second argument:
 * int => status
 *        0 => success case, less than 0 means the error case
 * Third argument:
 * string => error text
 *           This is empty string("") in the success case.
 * 0...n arguments:
 * wstring => push registry URI.
 */
const int IPC_PR_LIST_CONNECTIONS_REPLY = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 12;


/**
 * This message retrieves push connection information based on query argument.
 * First argument:
 * Uid object => UID of the MIDlet suite.
 * Second argument:
 * wstring    => URI.
 * Third argument:
 * int        => query argument. Can be one of defined in EPushInfoQueryParams enum.
 */
const int IPC_PR_DATA_OF_PUSH_CONN_QUERY = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 13;

/**
 * This message is reply message to the IPC_PR_DATA_OF_PUSH_CONN_QUERY message.
 * First argument:
 * Uid object => UID of the MIDlet suite.
 * Second argument:
 * int => status
 *        0 => success case, less than 0 means the error case.
 *             It is interpreted as an error case if data by query condition is not found.
 * Third argument:
 * string => error text
 *           This is empty string("") in the success case.
 * Fourth argument:
 * wstring => Result of the query.
 */
const int IPC_PR_DATA_OF_PUSH_CONN_QUERY_REPLY = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 14;

/**
 * This message asks from PushRegistryPlugin whether start of any push URI is equal
 * with URI passed as part of this comms msg.
 * First argument:
 * wstring => Base URI of the push connection.
 */
const int IPC_PR_PUSH_URI_STARTS_WITH_QUERY = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 15;

/**
 * This is reply message for IPC_PR_PUSH_URI_STARTS_WITH_QUERY comms msg. This returns
 * information whether base URI matches to start of any push URI.
 * Note: This message returns suite uid of first push URI wherein match occurred.
 * First argument:
 * int => status information.
 *        0 => success case. Base uri matches to start of push URI.
 *        -1 => if match does not occurred.
 * Second argument:
 * Uid object => suite uid of MIDlet which push URI matches to Base URI.
 *               Note: this argument is not set if value of he first argument is -1.
 */
const int IPC_PR_PUSH_URI_STARTS_WITH_QUERY_REPLY = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 16;

/**
 * This message is sent from runtime to push registry plugin in the situation wherein
 * push connection, registered/unregstered by other MIDlet, has been managed.
 * First argument:
 * wstring => URI of the closed push connection.
 */
const int IPC_PR_CONN_COMPLETED = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 17;

/**
 * This message is sent from push registry plugin to runtime after push registry plugin
 * has got IPC_PR_CONN_COMPLETED comms message(= indicates that push connection has been
 * closed in other runtime process). This message is also sent if timeout has expired
 * before arriving a IPC_PR_UNREG_COMPLETED comms message.
 * This message does not have own arguments.
 */
const int IPC_PR_CONN_MANAGED_BY_OTHER_MIDLET = JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C + 18;

}//end namespace push
}//end namespace java

#endif // PUSHREGISTRYMESSAGES_H

