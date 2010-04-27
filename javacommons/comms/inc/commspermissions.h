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
* Description:  CommsPermissions
*
*/

#ifndef COMMSPERMISSIONS_H
#define COMMSPERMISSIONS_H

namespace java
{
namespace comms
{

/**
* CommsPermission enumeration defines the set of supported permissions.
* CommsPermission constants are used to hide platform specific permission model.
*
* CommsPermission mapping to Symbian capability model is listed below:
* CommsPermission         TCapability
*   MANAGE_CERTIFICATES     ECapabilityWriteDeviceData
*   INSTALL_APPLICATION     ECapabilityTrustedUI
*   LAUNCH_APPLICATION      ECapabilityNetworkControl
*   STOP_APPLICATION        ECapabilityPowerMgmt
*
* @code
* MyListener::processMessage(CommsMessage& aMessage)
* {
*     if( aMessage.hasPermission(MANAGE_CERTIFICATES) )
*     {
*         // manage certificates
*     }
* }
* @endcode
*
* @see CommsMessage
*/

enum CommsPermission
{
    MANAGE_CERTIFICATES = 1,
    INSTALL_APPLICATION = 2,
    LAUNCH_APPLICATION = 4,
    STOP_APPLICATION = 8,
};


} // namespace comms
} // namespace java

#endif // COMMSPERMISSIONS_H
