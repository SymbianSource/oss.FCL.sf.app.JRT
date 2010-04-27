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


package com.nokia.mj.impl.gcf.protocol.sms;

import java.lang.String;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.gcf.PushValidator;
import com.nokia.mj.impl.sms.SMSPermissionImpl;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
/**
 * This is implementation for validation of URI and 'filter' push arguments for
 * SMS.
 */
public class PushValidatorImpl extends PushValidator
{
    private final int MAX_PORT = 65535;
    private static final String SMS_PREFIX = "sms://:";

    private final String INVALID_URI = "Invalid Uri";

    private final String INVALID_FILTER = "Invalid Filter";


    public PushValidatorImpl()
    {
    }

    public void validate(String aUri, String aFilter, Uid aSuiteUid,
                         String aMidletName, boolean aIsStaticRegistration)
    {
        // URI Validation Part
        boolean flag = aUri.startsWith(SMS_PREFIX);

        if (false == flag)
        {
            throw new IllegalArgumentException(INVALID_URI);
        }
        String port = aUri.substring(SMS_PREFIX.length());

        // Port Validation
        int smsPort = 0;
        try
        {
            smsPort = Integer.parseInt(port);
        }
        catch (NumberFormatException exp)
        {
            throw new IllegalArgumentException(INVALID_URI);
        }
        if (smsPort > MAX_PORT || smsPort < 0)
        {
            throw new IllegalArgumentException("Invalid Port:" + smsPort);
        }

        // Filter Validation
        for (int i = 0; i < aFilter.length(); i++)
        {
            char ch = aFilter.charAt(i);
            if (i == 0 && (ch == '+'))
            {
                continue;
            }
            if (!(Character.isDigit(ch) || (ch == '*') || (ch == '?')))
            {
                throw new IllegalArgumentException(INVALID_FILTER);
            }
        }
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        SMSPermissionImpl permission = new SMSPermissionImpl("sms://*", "open");
        appUtils.checkPermission(permission);
    }
}