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

package com.nokia.mj.impl.security.midp.common;

/**
 * Encapsulation of a security settings applied to some permissions.
 * The security settings consists of a name, a default interaction mode
 * and a set of allowed interaction modes
 */
public interface UserSecuritySettings
{
    // interaction modes
    public static final int UNDEFINED_INTERACTION_MODE = -1;
    public static final int ONESHOT_INTERACTION_MODE = 1;
    public static final int SESSION_INTERACTION_MODE = 2;
    public static final int BLANKET_INTERACTION_MODE = 3;
    public static final int NO_INTERACTION_MODE = 4;

    // function groups
    public static final String NET_ACCESS_SETTINGS = "Net Access";
    public static final String LOW_LEVEL_NET_ACCESS_SETTINGS = "Low Level Net Access";
    public static final String MESSAGING_SETTINGS = "Messaging";
    public static final String RESTRICTED_MESSAGING_SETTINGS = "Restricted Messaging";
    public static final String CALL_CONTROL_SETTINGS = "Call Control";
    public static final String LOCAL_CONNECTIVITY_SETTINGS = "Local Connectivity";
    public static final String MULTIMEDIA_RECORDING_SETTINGS = "Multimedia Recording";
    public static final String READ_USER_DATA_ACCESS_SETTINGS = "Read User Data Access";
    public static final String WRITE_USER_DATA_ACCESS_SETTINGS = "Write User Data Access";
    public static final String APPLICATION_AUTO_INVOCATION_SETTINGS = "Application Auto Invocation";
    public static final String PHONE_CALL_SETTINGS = "Phone Call";
    public static final String LOCATION_SETTINGS = "Location";
    public static final String LANDMARK_SETTINGS = "Landmark";
    public static final String SMART_CARD_COMMUNICATION_SETTINGS = "Smart Card Communication";
    public static final String AUTHENTICATION_SETTINGS = "Authentication";
    public static final String BROADCAST_SETTINGS = "Broadcast";
    public static final String NFC_WRITE_ACCESS_SETTINGS = "NFC Write Access";
    public static final String URL_START_SETTINGS = "Url start";

    // security warnings modes
    public static final String SECURITY_WARNINGS_USER_DEFINED_MODE = "1";
    public static final String SECURITY_WARNINGS_DEFAULT_MODE = "2";

    public String getName();
    public boolean isActive();
    public int getCurrentInteractionMode();
    public int[] getAllowedInteractionModes();
    public boolean isInteractionModeAllowed(int interactionMode);
    public boolean getBlanketPrompt();
}
