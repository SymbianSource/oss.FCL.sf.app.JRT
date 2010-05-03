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


#include "com_nokia_mj_impl_bluetooth_BluetoothStackConsts.h"

#ifndef BLUETOOTHCONSTS_H
#define BLUETOOTHCONSTS_H

namespace java
{
namespace bluetooth
{

#define UUID_STRING_MAX_LENGTH               32

#define SHORT_UUID_BASE_STRING               L"00001000800000805F9B34FB"

#define PROTOCOL_L2CAP                       0x0100

#define PROTOCOL_RFCOMM                      0x0003

#define PROTOCOL_GOEP                        0x0008

#define HCI_TIMEOUT                          -6004

#define BC_TIMEOUT                           0x0005

#define BC_FAILED_NOINFO                     0x0004

#define NOT_DISCOVERABLE                     com_nokia_mj_impl_bluetooth_BluetoothStackConsts_NOT_DISCOVERABLE

#define GIAC                                 com_nokia_mj_impl_bluetooth_BluetoothStackConsts_GIAC

#define LIAC                                 com_nokia_mj_impl_bluetooth_BluetoothStackConsts_LIAC

#define INQUIRY_COMPLETED                    com_nokia_mj_impl_bluetooth_BluetoothStackConsts_INQUIRY_COMPLETED

#define INQUIRY_TERMINATED                   com_nokia_mj_impl_bluetooth_BluetoothStackConsts_INQUIRY_TERMINATED

#define INQUIRY_ERROR                        com_nokia_mj_impl_bluetooth_BluetoothStackConsts_INQUIRY_ERROR

#define SERVICE_SEARCH_COMPLETED             com_nokia_mj_impl_bluetooth_BluetoothStackConsts_SERVICE_SEARCH_COMPLETED

#define SERVICE_SEARCH_TERMINATED            com_nokia_mj_impl_bluetooth_BluetoothStackConsts_SERVICE_SEARCH_TERMINATED

#define SERVICE_SEARCH_ERROR                 com_nokia_mj_impl_bluetooth_BluetoothStackConsts_SERVICE_SEARCH_ERROR

#define SERVICE_SEARCH_NO_RECORDS            com_nokia_mj_impl_bluetooth_BluetoothStackConsts_SERVICE_SEARCH_NO_RECORDS

#define SERVICE_SEARCH_DEVICE_NOT_REACHABLE  com_nokia_mj_impl_bluetooth_BluetoothStackConsts_SERVICE_SEARCH_DEVICE_NOT_REACHABLE

#define NOAUTHENTICATE_NOENCRYPT             com_nokia_mj_impl_bluetooth_BluetoothStackConsts_NOAUTHENTICATE_NOENCRYPT

#define AUTHENTICATE_NOENCRYPT               com_nokia_mj_impl_bluetooth_BluetoothStackConsts_AUTHENTICATE_NOENCRYPT

#define AUTHENTICATE_ENCRYPT                 com_nokia_mj_impl_bluetooth_BluetoothStackConsts_AUTHENTICATE_ENCRYPT

#define DataElement_NULL                     com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_NULL

#define DataElement_U_INT_1                  com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_U_INT_1

#define DataElement_U_INT_2                  com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_U_INT_2

#define DataElement_U_INT_4                  com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_U_INT_4

#define DataElement_U_INT_8                  com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_U_INT_8

#define DataElement_U_INT_16                 com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_U_INT_16

#define DataElement_INT_1                    com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_INT_1

#define DataElement_INT_2                    com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_INT_2

#define DataElement_INT_4                    com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_INT_4

#define DataElement_INT_8                    com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_INT_8

#define DataElement_INT_16                   com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_INT_16

#define DataElement_URL                      com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_URL

#define DataElement_UUID                     com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_UUID

#define DataElement_BOOL                     com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_BOOL

#define DataElement_STRING                   com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_STRING

#define DataElement_DATSEQ                   com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_DATSEQ

#define DataElement_DATALT                   com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_DATALT

#define CONNECTION_ERROR_UNKNOWN_PSM         com_nokia_mj_impl_bluetooth_BluetoothStackConsts_CONNECTION_ERROR_UNKNOWN_PSM

#define CONNECTION_ERROR_SECURITY_BLOCK      com_nokia_mj_impl_bluetooth_BluetoothStackConsts_CONNECTION_ERROR_SECURITY_BLOCK

#define CONNECTION_ERROR_NO_RESOURCES        com_nokia_mj_impl_bluetooth_BluetoothStackConsts_CONNECTION_ERROR_NO_RESOURCES

#define CONNECTION_ERROR_FAILED_NOINFO       com_nokia_mj_impl_bluetooth_BluetoothStackConsts_CONNECTION_ERROR_FAILED_NOINFO

#define CONNECTION_ERROR_TIMEOUT             com_nokia_mj_impl_bluetooth_BluetoothStackConsts_CONNECTION_ERROR_TIMEOUT

#define CONNECTION_ERROR_UNACCEPTABLE_PARAMS com_nokia_mj_impl_bluetooth_BluetoothStackConsts_CONNECTION_ERROR_UNACCEPTABLE_PARAMS

#define DataElement_EndList                  com_nokia_mj_impl_bluetooth_BluetoothStackConsts_DataElement_EndList
} //end namespace bluetooth
} //end namespace java

#endif // BLUETOOTHCONSTS_H
