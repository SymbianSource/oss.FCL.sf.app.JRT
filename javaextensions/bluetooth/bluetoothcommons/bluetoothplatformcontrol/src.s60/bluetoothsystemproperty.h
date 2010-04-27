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



#ifndef BLUETOOTHSYSTEMPROPERTY_H
#define BLUETOOTHSYSTEMPROPERTY_H

/**
 * System property names. Idea is to keep the platform specific stuff here.
 * LocalDevice.getProperty and System.getProperty, both load this dll and
 * query for the platform specific values.
 *
 */

#define BT_L2CAP_RECEIVEMTU_MAX     1
#define BT_CONNECTED_DEVICES_MAX    2

//Following return True/False
#define BT_CONNECTED_INQUIRY        3
#define BT_CONNECTED_PAGE           4
#define BT_CONNECTED_INQUIRY_SCAN   5
#define BT_CONNECTED_PAGE_SCAN      6
#define BT_MASTER_SWITCH            7

//Value as a string in Base10 Digits
#define BT_SD_TRANS_MAX             8
#define BT_SD_ATTR_RETRIEVE_MAX     9

#define BT_L2CAP_RECEIVEMTU_MAX_V   L"672"
#define BT_CONNECTED_DEVICES_MAX_V  L"7"

//Following return True/False
#define BT_CONNECTED_INQUIRY_V      L"false"
#define BT_CONNECTED_PAGE_V         L"false"
#define BT_CONNECTED_INQUIRY_SCAN_V L"false"
#define BT_CONNECTED_PAGE_SCAN_V    L"false"

#ifdef BLUETOOTH_MASTER_SUPPORT
#define BT_MASTER_SWITCH_V      L"true"
#else
#define BT_MASTER_SWITCH_V      L"false"
#endif

//Value as a string in Base10 Digits
#define BT_SD_TRANS_MAX_V           L"1"
#define BT_SD_ATTR_RETRIEVE_MAX_V   L"65535"

#endif // BLUETOOTHSYSTEMPROPERTY_H
