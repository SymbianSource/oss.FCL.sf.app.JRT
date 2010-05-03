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


#ifndef PUSHCONSTANT_H
#define PUSHCONSTANT_H

namespace java
{
namespace push
{
//Two weeks long period in milliseconds.
const long long TWO_WEEKS_IN_MILLI_SECS = 1209601000LL;

//Maximum time when is no sense to initialize timer. This is used in the following check:
//newAlarmTime > (currentTime + NO_NEED_TO_INITIALZE_TIMER_TIME_IN_MILLI_SECS).
const long long NO_NEED_TO_INITIALZE_TIMER_TIME_IN_MILLI_SECS = 300LL;

//Timeout value for sendReceive() comms method.
const int COMMS_TIMEOUT = 15; //15 sec.

const char PREFIX_OF_SRV_CONN_PLUGIN[] = "java";
const char SUFFIX_OF_SRV_CONN_PLUGIN[] = "scplugin";

enum EPushInfoQueryParams
{
    FILTER_OF_PUSH_CONN = 0x1,
    CLASSNAME_OF_PUSH_CONN = 0x2
};

const int UNDEFINED_MEDIA_ID = -2;
const int MEDIA_ID_IN_LINUX  = -1;

//Timeout value for "push connection unregistered" response
//in the case wherein push connection is unregistered from other MIDlet.
const unsigned int TIMEOUT_VALUE_FOR_UNREG_RESPONSE = 5; //5 sec

}//end namespace push
}//end namespace java

#endif // PUSHCONSTANT_H
