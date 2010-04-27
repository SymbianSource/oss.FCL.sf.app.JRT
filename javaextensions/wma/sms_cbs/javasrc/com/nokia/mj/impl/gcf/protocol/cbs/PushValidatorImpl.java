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


package com.nokia.mj.impl.gcf.protocol.cbs;

import java.lang.String;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.gcf.PushValidator;
import com.nokia.mj.impl.cbs.CBSPermissionImpl;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
/**
 * This is implementation for validation of URI and 'filter' push arguments for
 * CBS.
 */
public class PushValidatorImpl extends PushValidator
{
    private final String CBS_PREFIX = "cbs://:";

    private final String INVALID_URI = "Invalid URI";

    public PushValidatorImpl()
    {
    }

    public void validate(String aUri, String aFilter, Uid aSuiteUid,
                         String aMidletName, boolean aIsStaticRegistration)
    {
        // URI Validation Part
        boolean flag = aUri.startsWith(CBS_PREFIX);

        if (false == flag)
        {
            throw new IllegalArgumentException(INVALID_URI);
        }
        String port = aUri.substring(CBS_PREFIX.length());

        // Port Validation
        int cbsPort = 0;
        try
        {
            cbsPort = Integer.parseInt(port);
        }
        catch (NumberFormatException exp)
        {
            throw new IllegalArgumentException(INVALID_URI);
        }
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        CBSPermissionImpl permission = new CBSPermissionImpl("cbs://*", "open");
        appUtils.checkPermission(permission);
    }
}