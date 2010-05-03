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

package com.nokia.mj.impl.comms;

/**
* CommsPermission defines the set of supported permissions.
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
* class MyListener implements CommsListener {
*   public void processMessage(CommsMessage aMessage) {
*   {
*       if( aMessage.hasPermission(CommsPermission.MANAGE_CERTIFICATES) )
*       {
*         // manage certificates
*       }
*   }
* }
* @endcode
* @see CommsMessage
*/
public final class CommsPermission
{
    public static final int MANAGE_CERTIFICATES = 1;
    public static final int INSTALL_APPLICATION = 2;
    public static final int LAUNCH_APPLICATION = 4;
    public static final int STOP_APPLICATION = 8;
}
