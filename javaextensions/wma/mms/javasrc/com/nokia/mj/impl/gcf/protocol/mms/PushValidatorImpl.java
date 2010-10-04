/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.gcf.protocol.mms;

import java.lang.String;
import java.io.IOException;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.gcf.PushValidator;
import com.nokia.mj.impl.mms.MMSPermissionImpl;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
/**
 * This is implementation for validation of URI and 'filter' push arguments for
 * MMS.
 */
public class PushValidatorImpl extends PushValidator
{
    private static final String MMS_PREFIX = "mms://:";
    private final String INVALID_URI = "Invalid MMS URI";

    public PushValidatorImpl()
    {

    }

    /**
     * Validates both Uri and Filter
     * @param aUri Uri to be validated
     * @param aFilter filter to be validated
     */
    public void validate(String aUri, String aFilter, Uid aSuiteUid,
                         String aMidletName, boolean aIsStaticRegistration)
                         throws IOException
    {
        if (!(aUri.startsWith(MMS_PREFIX)))
        {
            throw new IllegalArgumentException(INVALID_URI);
        }
        String appId = aUri.substring(MMS_PREFIX.length());

        for (int i = 0; i < appId.length(); i++)
        {
            char ch = appId.charAt(i);

            if (!(Character.isDigit(ch) || Character.isLowerCase(ch)||
                    Character.isUpperCase(ch) || (ch == '.') || (ch == '_')))
            {
                throw new IllegalArgumentException(INVALID_URI);
            }
        }

        // Filter Validation Part
        if (!isValidPhoneNumber(aFilter))
        {
            throw new IllegalArgumentException("Invalid Filter");

        }
        Uid suiteUid = null;
        String commsName = null;
        if (aIsStaticRegistration)
        {
            suiteUid = aSuiteUid;
            commsName = PushValidator.INSTALLER_COMMS;
        }
        else
        {
            commsName = PushValidator.RUNTIME_COMMS;
        }
        if (PushValidator.isRegisteredPushUriStartingWith(aUri, suiteUid,
                commsName))
        {
            if (aIsStaticRegistration)
            {
                throw new IllegalArgumentException();
            }
            else
            {
                throw new IOException("connection already exists");
            }
        }
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        MMSPermissionImpl permission = new MMSPermissionImpl("mms://*","open");
        appUtils.checkPermission(permission);
    }

    private boolean isValidPhoneNumber(String aFilter)
    {
        for (int i = 0; i < aFilter.length(); i++)
        {
            char ch = aFilter.charAt(i);
            if (i == 0 && (ch == '+'))
            {
                continue;
            }
            if (!(Character.isDigit(ch) || (ch == '*') || (ch == '?')))
            {
                return false;
            }
        }
        return true;
    }
}