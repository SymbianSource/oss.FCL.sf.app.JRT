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

package com.nokia.mj.impl.security.utils;

import com.nokia.mj.impl.utils.LocalizedTextProvider;
import com.nokia.mj.impl.utils.ResourceLoader;

public class SecurityPermissionTexts extends LocalizedTextProvider
{
    /** Singleton instance of SecurityPermissionTexts. */
    private static SecurityPermissionTexts self = null;
    /** ResourceLoader for fetching localised texts. */
    private static ResourceLoader iRes = null;

    // identifiers for the security permission texts
    public static final String CALL_CONTROL = "call_control";
    public static final String NET_ACCESS = "net_access";
    public static final String LOW_LEVEL_NET_ACCESS = "low_level_net_access";
    public static final String MESSAGING = "messaging";
    public static final String RESTRICTED_MESSAGING = "restricted_messaging";
    public static final String APP_AUTO_INVOCAT = "app_auto_invocat";
    public static final String LOCAL_CONN = "local_conn";
    public static final String MM_RECORD = "mm_record";
    public static final String READ_DATA = "read_data";
    public static final String WRITE_DATA = "write_data";
    public static final String LOCATION = "location";
    public static final String LANDMARKS = "landmarks";
    public static final String AUTH = "auth";
    public static final String BROADCAST = "broadcast";

    /**
     * Creates an instance of the SecurityPermissionTexts
     *
     * @return An instance of SecurityPermissionTexts
     */
    public static SecurityPermissionTexts getInstance()
    {
        if (self == null)
        {
            self = new SecurityPermissionTexts();
        }
        return self;
    }

    /**
     * Constructor
     */
    private SecurityPermissionTexts()
    {
    }

    /**
     * Returns the localized texts.
     *
     * @param aTextId     id for the localized text (one of the pre-defined
     *                    constants)
     * @param aTextParams parameters for the localized text
     * @return            the localized text associated with the provided
     *                    identifier and parameters
     */
    public String getText(String aTextId, Object[] aTextParams)
    {
        if (iRes == null)
        {
            iRes = ResourceLoader.getInstance("javainstallation", "qtn_java_inst_perm_");
        }
        return iRes.format(aTextId, aTextParams);
    }
}
