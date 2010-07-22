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

import com.nokia.mj.impl.utils.Id;
import com.nokia.mj.impl.utils.LocalizedTextProvider;
import com.nokia.mj.impl.utils.ResourceLoader;

public class SecurityPermissionTexts extends LocalizedTextProvider
{
    /** Singleton instance of SecurityPermissionTexts. */
    private static SecurityPermissionTexts self = null;
    /** ResourceLoader for fetching localised texts. */
    private static ResourceLoader iRes = null;

    // identifiers for the security permission texts
    public static final Id CALL_CONTROL = new Id("call_control", "N/A");
    public static final Id NET_ACCESS = new Id("net_access", "N/A");
    public static final Id LOW_LEVEL_NET_ACCESS = new Id("low_level_net_access", "N/A");
    public static final Id MESSAGING = new Id("messaging", "qt_kalle_test_d");
    public static final Id RESTRICTED_MESSAGING = new Id("restricted_messaging", "N/A");
    public static final Id APP_AUTO_INVOCAT = new Id("app_auto_invocat", "N/A");
    public static final Id LOCAL_CONN = new Id("local_conn", "N/A");
    public static final Id MM_RECORD = new Id("mm_record", "N/A");
    public static final Id READ_DATA = new Id("read_data", "N/A");
    public static final Id WRITE_DATA = new Id("write_data", "N/A");
    public static final Id LOCATION = new Id("location", "N/A");
    public static final Id LANDMARKS = new Id("landmarks", "N/A");
    public static final Id AUTH = new Id("auth", "N/A");
    public static final Id BROADCAST = new Id("broadcast", "N/A");

    private static final String AVKON_LOC_FILE = "javainstallation";
    private static final String AVKON_PREFIX = "qtn_java_inst_perm_";
    private static final String QT_LOC_FILE = "N/A";
    private static final String QT_PREFIX = "N/A";

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
    public String getText(Id aTextId, Object[] aTextParams)
    {
        if (iRes == null)
        {
            iRes = ResourceLoader.getInstance(AVKON_LOC_FILE, AVKON_PREFIX, QT_LOC_FILE, QT_PREFIX);
        }

        return iRes.format(aTextId, aTextParams);
    }
}
