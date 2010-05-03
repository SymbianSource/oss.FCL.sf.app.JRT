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
* Description:  Defines constants for some of the security storage data
*
*/


#ifndef SECURITYSTORAGEDATADEFS_H
#define SECURITYSTORAGEDATADEFS_H

/**
* This header file defines the security storage data. This constants need
* to be in sync with com.nokia.mj.impl.security.midp.common.UserSecuritySettings
*/

// function groups
const wchar_t* const NET_ACCESS_SETTINGS = L"Net Access";
const wchar_t* const LOW_LEVEL_NET_ACCESS_SETTINGS = L"Low Level Net Access";
const wchar_t* const MESSAGING_SETTINGS = L"Messaging";
const wchar_t* const RESTRICTED_MESSAGING_SETTINGS = L"Restricted Messaging";
const wchar_t* const CALL_CONTROL_SETTINGS = L"Call Control";
const wchar_t* const LOCAL_CONNECTIVITY_SETTINGS = L"Local Connectivity";
const wchar_t* const MULTIMEDIA_RECORDING_SETTINGS = L"Multimedia Recording";
const wchar_t* const READ_USER_DATA_ACCESS_SETTINGS = L"Read User Data Access";
const wchar_t* const WRITE_USER_DATA_ACCESS_SETTINGS = L"Write User Data Access";
const wchar_t* const APPLICATION_AUTO_INVOCATION_SETTINGS = L"Application Auto Invocation";
const wchar_t* const PHONE_CALL_SETTINGS = L"Phone Call";
const wchar_t* const LOCATION_SETTINGS = L"Location";
const wchar_t* const LANDMARK_SETTINGS = L"Landmark";
const wchar_t* const SMART_CARD_COMMUNICATION_SETTINGS = L"Smart Card Communication";
const wchar_t* const AUTHENTICATION_SETTINGS = L"Authentication";
const wchar_t* const BROADCAST_SETTINGS = L"Broadcast";
const wchar_t* const NFC_WRITE_ACCESS_SETTINGS = L"NFC Write Access";

// interaction modes values
const wchar_t* const ONESHOT_INTERACTION_MODE = L"1";
const wchar_t* const SESSION_INTERACTION_MODE = L"2";
const wchar_t* const BLANKET_INTERACTION_MODE = L"3";
const wchar_t* const DENIED_INTERACTION_MODE = L"4";

// security warnings modes values
const wchar_t* const SECURITY_WARNINGS_USER_DEFINED_MODE = L"1";
const wchar_t* const SECURITY_WARNINGS_DEFAULT_MODE = L"2";

/**
* Constants defined for different protection domain categories.
*
* IMPORTANT: These constants must be kept in sync with the constants defined
*            on java inside com.nokia.mj.impl.rt.support.ApplicationInfo
*/
const wchar_t* const MANUFACTURER_DOMAIN_CATEGORY             = L"MFD";
const wchar_t* const IDENTIFIED_THIRD_PARTY_DOMAIN_CATEGORY   = L"ITPD";
const wchar_t* const OPERATOR_DOMAIN_CATEGORY                 = L"OPD";
const wchar_t* const UNIDENTIFIED_THIRD_PARTY_DOMAIN_CATEGORY = L"UTPD";

/**
* Constants defined for different protection domain names.
*
* IMPORTANT: These constants must be kept in sync with the constants defined
*            on java inside com.nokia.mj.impl.security.midp.authorization.SecurityPolicyModule
*/
const wchar_t* const MANUFACTURER_DOMAIN_NAME             = L"Manufacturer";
const wchar_t* const IDENTIFIED_THIRD_PARTY_DOMAIN_NAME   = L"IdentifiedThirdParty";
const wchar_t* const OPERATOR_DOMAIN_NAME                 = L"Operator";
const wchar_t* const UNIDENTIFIED_THIRD_PARTY_DOMAIN_NAME = L"UnidentifiedThirdParty";

#endif // SECURITYSTORAGEDATADEFS_H


