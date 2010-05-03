/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file acts as reservation table for comms addresses,
*                module and message ids.
*
*/

#ifndef COMMS_H
#define COMMS_H

namespace java
{
namespace comms
{

/**
* Defines the 'well-known' names for server endpoints.
*/
const int IPC_ADDRESS_JAVA_CAPTAIN_C               = 10000;
const int IPC_ADDRESS_JAVA_REGISTRY_C              = 10100;
const int IPC_ADDRESS_JAVA_STORAGE_C               = 10200;
const int IPC_ADDRESS_JAVA_TCK_RUNNER_C            = 10300;
const int IPC_ADDRESS_JAVA_PREINSTALLER_C          = 10400;
const int IPC_ADDRESS_JAVA_APPCONVERTER_C          = 10500;
const int IPC_ADDRESS_JAVA_STORAGE_PLUGIN_C        = 10600;
const int IPC_ADDRESS_JAVA_DEBUGAPI_C              = 10700;
const int IPC_ADDRESS_JAVA_STARTSCREEN_SERVER_C    = 10900;
const int IPC_ADDRESS_JAVA_SIF_PLUGIN_C            = 11000;
const int IPC_ADDRESS_JAVA_INSTALLER_SERVER_C      = 11100;

/**
* Defines available module ids.
*
* Module id identifies to which component message belongs.
* Module id is used when registering listeners.
*/
const int PLUGIN_ID_SAMPLE_C                      = 0;
const int PLUGIN_ID_RTC_C                         = 1;
const int PLUGIN_ID_RMS_C                         = 2;
const int PLUGIN_ID_PUSH_REGISTRY_C               = 3;
const int PLUGIN_ID_PUSH_CONTROLLER_C             = 4;
const int PLUGIN_ID_JAVASTORAGE_CLIENT_C          = 5;
const int PLUGIN_ID_AMC_C                         = 6;
const int PLUGIN_ID_JAVA_CERT_STORE_EXTENSION_C   = 7;
const int PLUGIN_ID_JAVA_CERT_STORE_ECOM_C        = 8;
const int PLUGIN_ID_PMC_C                         = 9;
const int PLUGIN_ID_SIGNAL_C                      = 10;
const int PLUGIN_ID_MMC_EXTENSION_C               = 11;
const int PLUGIN_ID_RTC_PUSH_C                    = 12;
const int PLUGIN_ID_JAVACAPTAIN_CORE_C            = 13;
const int PLUGIN_ID_DRIVE_OBSERVER_NATIVE_C       = 50;
const int PLUGIN_ID_DRIVE_OBSERVER_JAVA_C         = 51;
const int PLUGIN_ID_JAVACAPTAIN_TESTER_C          = 100;
const int PLUGIN_ID_JAVACAPTAIN_COMMS_TESTER_C    = 101;
const int PLUGIN_ID_CERT_STORE_PLUGIN_TESTER_C    = 102;
const int PLUGIN_ID_PUSH_TESTER_C                 = 103;
const int PLUGIN_ID_BT_DEVICE_CLASS_MANAGER_C     = 200;

//const int PLUGIN_ID_XYZ_C                       = 8;


/**
* Message id ranges
*/
const int IPCLIB_MESSAGE_ID_RANGE_START_C           = 0;
const int IPCLIB_MESSAGE_ID_RANGE_END_C             = 9;

const int COMMS_MESSAGE_ID_RANGE_START_C            = 10;
const int COMMS_MESSAGE_ID_RANGE_END_C              = 19;

const int JAVACOMMS_MESSAGE_ID_RANGE_START_C        = 20;
const int JAVACOMMS_MESSAGE_ID_RANGE_END_C          = 29;

const int JSR_82_MESSAGE_ID_RANGE_START_C           = 30;
const int JSR_82_MESSAGE_ID_RANGE_END_C             = 31;

const int JAVACAPTAIN_MESSAGE_ID_RANGE_START_C      = 100;
const int JAVACAPTAIN_MESSAGE_ID_RANGE_END_C        = 299;

const int JAVACAPTAIN_PUSH_REGISTRY_RANGE_START_C   = 300;
const int JAVACAPTAIN_PUSH_REGISTRY_RANGE_END_C     = 399;

const int JAVASTORAGE_MESSAGE_ID_RANGE_START_C      = 400;
const int JAVASTORAGE_MESSAGE_ID_RANGE_END_C        = 499;

// note that preinstaller and appconverter use same messages as TCK Runner
const int TCK_RUNNER_MESSAGE_ID_RANGE_START_C       = 500;
const int TCK_RUNNER_MESSAGE_ID_RANGE_END_C         = 599;

const int JAVAINSTALLER_MESSAGE_ID_RANGE_START_C    = 600;
const int JAVAINSTALLER_MESSAGE_ID_RANGE_END_C      = 699;

//const int JSR_XYZ_MESSAGE_ID_RANGE_START_C        = 700;
//const int JSR_XYZ_MESSAGE_ID_RANGE_END_C          = 799;

} // namespace comms
} // namespace java

#endif // COMMS_H
