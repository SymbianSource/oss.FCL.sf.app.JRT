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

#ifndef PUSHERRORCODES_H
#define PUSHERRORCODES_H

namespace java
{
namespace push
{

//Can be set to the error status in the successful case.
const int NO_ERROR = 0;

//Unknown error. This should never throw to the user.
const int UNKNOWN_ERROR = -11000;

//This is used if reading/writing to the comms message fails.
const int INCORRECT_COMMS_MSG = -11001;

//This is used in the following situations:
//- initialization of comms failed.
//- sending a comms msg fails.
//- unexpected comms msg arrived.
const int GENERAL_COMMS_ERROR = -11002;

//ServerConnection object by URI is not found.
const int SRV_CONN_NOT_FOUND = -11010;

//Push URI does not belong to this MIDlet.
const int URI_BELONGS_TO_OTHER_MIDLET = -11011;

//URI is static push URI.
const int NOT_DYNAMIC_PUSH_URI = -11012;

//Invalid push URI.
const int INVALID_URI = -11020;

//Invalid filter argument.
const int INVALID_FILTER = -11021;

//Invalid alarm time.
const int INVALID_ALARM_TIME = -11022;

//Initialization of the alarm timer failed.
const int INIT_OF_ALARM_TIMER_FAILED = -11023;

//Connection already exists.
const int PUSH_CONNECTION_ALREADY_EXISTS = -11030;

//Inserting/updating/deleting data from the db failed.
const int DB_ERROR = -11040;

//Inserting/updating/deleting data from the db failed.
const int CONVERTING_DB_VALUE_FAILED = -11041;

//This error code is used for protocol specific errors occurred in the Server Connection Plugin.
const int COMMON_SRV_CONN_PLUGIN_ERROR = -11050;

//This error is used if initialization of all push connections failed in the listen mode.
const int INIT_OF_ALL_PUSH_CONNS_FAILED = -11051;

//Identifier data does not match to any application.
const int INCORRECT_APP_DATA = -11052;

}//end namespace push
}//end namespace java

#endif // PUSHERRORCODES_H
