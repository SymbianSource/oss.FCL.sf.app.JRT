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


package com.nokia.mj.impl.gcf.protocol.datagram;

import java.lang.String;
import java.util.Vector;
import java.lang.NumberFormatException;
import java.lang.Integer;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.gcf.PushValidator;
import com.nokia.mj.impl.gcf.utils.UrlParser;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.ApplicationUtils;

/**
 * This is simple default implementation for validation of URI and 'filter' push
 * arguments.
 */
public class PushValidatorImpl extends PushValidator
{

    private static final String DATAGRAM_PREFIX = "datagram://:";

    public PushValidatorImpl()
    {
    }

    public void validate(String aUri, String aFilter, Uid aSuiteUid,
                         String aMidletName, boolean aIsStaticRegistration)
    {
        // Validating URI. Note: this validates only server datagram. It does not
        // contain host part.
        // Note: String.startsWith() does not accept "datagram://:" URI.
        boolean flag = aUri.startsWith(DATAGRAM_PREFIX);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,"+datagram - push validator");
        if (false == flag)
        {
            throw new IllegalArgumentException("Invalid URL: "+aUri);
        }

        String portNumStr = null;
        int startIndex = DATAGRAM_PREFIX.length();
        int endIndex = aUri.length();
        if (endIndex > startIndex) // dont validate port for "datagram://:" uri
        {
            portNumStr = aUri.substring(startIndex,endIndex);
            //Logger.LOG(Logger.ESOCKET, Logger.EInfo,"datagram - push validator portNumStr = "+portNumStr+"DATAGRAM_PREFIX.length() = "+DATAGRAM_PREFIX.length()+"aUri.length() = "+aUri.length());
            try
            {
                Integer.parseInt(portNumStr);
            }
            catch (java.lang.NumberFormatException ex)
            {
                throw new IllegalArgumentException(
                    "Invalid URL: " + aUri);
            }
        }

        // validate the filter
        if (aFilter == null)
            throw new IllegalArgumentException("Invalid filter: "+aFilter);
        if (aFilter == "")
            throw new IllegalArgumentException("Invalid filter: "+aFilter);
        if (aFilter.length() == 1)
        {
            char ch = aFilter.charAt(0);
            if ((ch == '*') || (ch == '?'))
            {
                // OK
            }

            else
            {
                throw new IllegalArgumentException("Invalid filter: "+aFilter);
            }
        }
        else
        {
            // validate the IPV6 or IPV4 format
            char ch;
            for (int i =0; i< aFilter.length(); i++)
            {
                ch = aFilter.charAt(i);
                int val = ch;
                //Logger.LOG(Logger.ESOCKET,Logger.EInfo, "ch = "+ch+" val = "+val);
                if ((ch == '?') ||
                        (ch == '*') ||
                        (ch == '.') ||
                        (ch == ':') ||
                        (ch == '[') ||
                        (ch == ']') ||
                        ((val >= 48) && (val <= 57)) || // between '0' and '9'
                        ((val >= 97) && (val <= 102)) || //between 'a' and 'f'
                        ((val >= 65) && (val <= 70)))  // between 'A' and 'F'
                {
                    // Ok
                }
                else
                {
                    Logger.LOG(Logger.ESOCKET,Logger.EInfo, "Invalid filter argument");
                    throw new IllegalArgumentException("Invalid filter: "+aFilter);
                }
            }// end of for
        } // end of else
        String url1 = "datagram://" + aFilter;
        Logger.LOG(Logger.ESOCKET,Logger.EInfo,"Socket PushValidator - after appending url = "+url1);
        UrlParser url2 = new UrlParser(url1);

        /* security check */
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,"datagram - push validator : before security check");
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        DatagramPermissionImpl per = new DatagramPermissionImpl(DatagramPermissionImpl.SERVER_TARGET);
        appUtils.checkPermission(per);
        /* security check */
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,"datagram - push validator : after security check");

    }// end validate()

}// end class PushValidatorImpl
