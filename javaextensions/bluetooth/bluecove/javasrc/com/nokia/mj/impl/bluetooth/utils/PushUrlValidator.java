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

package com.nokia.mj.impl.bluetooth.utils;

import java.util.Hashtable;
import com.intel.bluetooth.UtilsStringTokenizer;

/**
 * PushURLValidator is common utility used to validate url and filters passed by
 * Push framework during push registration. Protocol specific push validators
 * use this to validate their urls.
 *
 */
public class PushUrlValidator
{
    private static int ADDRESS_LENGTH = 12;

    private static int UUID_LENGTH = 32;

    private static int MAX_BLACKLIST_COUNT = 1024;

    // Common Server parameters
    private static final String AUTHENTICATE = "authenticate";

    private static final String AUTHORIZE = "authorize";

    private static final String ENCRYPT = "encrypt";

    /**
     * Returns string matching the ones present in srvParams and filterParams if
     * paramName is one of the keys of the hashtable. If paramName is not one
     * among the valid parameters, then null is returned.
     *
     * @param map
     *            hashtable containing valid param names in either url or the
     *            filter.
     * @param paramName
     *            string that needs to be matched with valid parameters.
     * @return valid param value in filter or url. Null in case paramName is not
     *         valid.
     */
    public static String validParamName(Hashtable map, String paramName)
    {
        String validName = (String) map.get(paramName.toLowerCase());
        if (validName != null)
        {
            return validName;
        }
        return null;
    }

    /**
     * Validates if the tokens have properties that are specific to url.
     * srvParams are hashtables which have parameters that are present in
     * connection url string .
     * <p>
     * Each token should begin with one of the values in the hashtable. If not
     * then IllegalArgumentException is thrown.
     *
     * @param aProperty
     *            srvParams
     * @param tok
     *            tokens that need to be checked against aProperty
     */
    public static void validateParams(Hashtable aProperty,
                                      UtilsStringTokenizer tok)
    {
        DebugLog.debug(" +  PushUrlValidator: validateParams()");
        Hashtable values = new Hashtable();
        // Holds the values already present in the url passed
        // it is later used to check for duplicate values

        while (tok.hasMoreTokens())
        {
            String t = tok.nextToken();
            int equals = t.indexOf('=');

            if (equals > -1)
            {
                String param = t.substring(0, equals);
                String value = t.substring(equals + 1);
                String validName = validParamName(aProperty, param);

                if (validName != null)
                {
                    String hasValue = (String) values.get(validName);
                    if ((hasValue != null) && (!hasValue.equals(value)))
                    {
                        DebugLog.error(" -  PushUrlValidator: Validate(): "
                                       + "duplicate param [" + param + "] value ["
                                       + value + "]");
                        throw new IllegalArgumentException("duplicate param ["
                                                           + param + "] value [" + value + "]");
                    }
                    values.put(validName, value);
                }
                else
                {
                    DebugLog.error(" -  PushUrlValidator: Validate(): "
                                   + "invalid param [" + param + "] value [" + value
                                   + "]");
                    throw new IllegalArgumentException("invalid param ["
                                                       + param + "] value [" + value + "]");
                }
            }
            else
            {
                DebugLog.error(" -  PushUrlValidator: Validate(): "
                               + "invalid param [" + t + "]");
                throw new IllegalArgumentException("invalid param [" + t + "]");
            }
        }

        Object authenticate = values.get(AUTHENTICATE);
        Object encrypt = values.get(ENCRYPT);
        Object authorize = values.get(AUTHORIZE);

        if ((authenticate != null && (authenticate.toString())
                .equalsIgnoreCase("false"))
                && (encrypt != null && (encrypt.toString())
                    .equalsIgnoreCase("true")))
        {
            DebugLog.error(" -  PushUrlValidator: Validate(): "
                           + "invalid params authenticate[" + authenticate
                           + "] encrypt [" + encrypt + "]");
            throw new IllegalArgumentException("invalid params ");
        }
        if ((authenticate != null && (authenticate.toString())
                .equalsIgnoreCase("false"))
                && (authorize != null && (authorize.toString())
                    .equalsIgnoreCase("true")))
        {
            DebugLog.error(" -  PushUrlValidator: Validate(): "
                           + "invalid params authenticate[" + authenticate
                           + "] authorize [" + authorize + "]");
            throw new IllegalArgumentException("invalid params ");
        }

        DebugLog.debug(" -  PushUrlValidator: validateParams()");
    }

    /**
     * Validates if the tokens have properties that are specific to url.
     * filterParams are hashtables which have parameters that are present in
     * filter string .
     * <p>
     * Each token should begin with one of the values in the hashtable. If not
     * then IllegalArgumentException is thrown.
     *
     * @param aProperty
     *            filterParams
     * @param tok
     *            tokens that need to be checked against aProperty
     */

    public static void validateFilterParams(Hashtable aProperty,
                                            UtilsStringTokenizer tok)
    {
        DebugLog.debug("+  PushUrlValidator: validateFilterParams");
        int state = 0;
        int blacklistFilterCounter = 0;
        boolean isAllowedSenderCompleted = false;

        while (tok.hasMoreTokens())
        {
            String nextTok = tok.nextToken();
            switch (state)
            {
            case 0: // Expecting AllowedSender address filter string
                state = 1;
                checkAddressLength(nextTok);
                checkHexDig(nextTok);
                break;
            case 1: // Expecting filter key words:
                // "Authenticated/Authorized","Blacklist="
                int equals = nextTok.indexOf('=');
                if (equals < 0)
                {
                    String paramName = validParamName(aProperty, nextTok);
                    if (isAllowedSenderCompleted || null == paramName
                            || paramName.equalsIgnoreCase("BlackList"))
                    {
                        throw new IllegalArgumentException();
                    }
                    isAllowedSenderCompleted = true;
                    break;
                }

                String param = nextTok.substring(0, equals);
                if (!param.equalsIgnoreCase("BlackList"))
                {
                    throw new IllegalArgumentException(
                        "Illegal filter argument");
                }
                // Blacklist
                state = 2;
                nextTok = nextTok.substring(equals + 1);
                // Fall-thru
            case 2: // Expecting address filters of blacklist
                if (blacklistFilterCounter >= MAX_BLACKLIST_COUNT)
                {
                    throw new IllegalArgumentException("Too many "
                                                       + "BlackList entries");
                }

                // Checking for the valid address filters
                checkAddressLength(nextTok);
                checkHexDig(nextTok);

                blacklistFilterCounter++;
                break;
            }
        }
        DebugLog.debug("-  PushUrlValidator: validateFilterParams");
    }

    public static void checkAddressLength(String aAddress)
    {
        if (null == aAddress || aAddress.length() > ADDRESS_LENGTH)
        {
            throw new IllegalArgumentException();
        }
    }

    public static void checkUuidLength(String aUuid)
    {
        if (null == aUuid || aUuid.length() > UUID_LENGTH)
        {
            throw new IllegalArgumentException();
        }
    }

    /**
     * Method checks to see if the string passes into it fits properly into
     * HEXDIG of the AllowedSender ABNF. We also allow * and ? to be present.
     *
     * @param aString
     *            needs to be checked for being a valid HEXDIG.
     */
    public static void checkHexDig(String aString)
    {
        DebugLog.debug(" +  PushUrlValidator: checkHexDig(): String: "
                       + aString);
        if (0 == aString.length())
        {
            throw new IllegalArgumentException();
        }
        for (int i = 0; i < aString.length(); i++)
        {
            char c = aString.charAt(i);
            if (!(c >= 'A' && c <= 'F') && !(c >= 'a' && c <= 'f')
                    && !(c >= '0' && c <= '9') && (c != '?' && c != '*'))
            {
                throw new IllegalArgumentException();
            }
        }
        DebugLog.debug(" -  PushUrlValidator: checkHexDig()");
    }

    /**
     * Checks for power on and discoverability modes. In case they are not
     * correct, prompts the user to change them. If that fails, throws
     * IllegalArgumentException.
     *
     */
    public static void checkPowerAndDiscoverability(String aAppName,
            boolean aIsStaticReg)
    {

        // Check Power on and Discoverability mode.
        try
        {
            // Check for power on now.
            Class stateCheck = Class
                               .forName("com.nokia.mj.impl.bluetooth.BTPlatformControl");
            BluetoothStateCheck stateChecker = (BluetoothStateCheck) stateCheck
                                               .newInstance();

            boolean ready = true;
            if (false == stateChecker
                    .checkDevicePowerOn(aAppName, aIsStaticReg))
            {
                DebugLog.debug(" PushUrlValidator:checkPowerAndDiscoverability"
                               + " Couldn't Switch Power ON");
                ready = false;
            }
            if (false == stateChecker.checkDeviceDiscoverability(aAppName,
                    aIsStaticReg))
            {
                DebugLog.debug(" PushUrlValidator:checkPowerAndDiscoverability"
                               + " Couldn't Turn Discoverability Mode");
                ready = false;
            }

            if (!ready)
            {
                DebugLog.debug(" PushUrlValidator:checkPowerAndDiscoverability"
                               + " Unable to power on bluetooth");
                throw new IllegalArgumentException(
                    "Unable to power on bluetooth");
            }
        }
        catch (Exception ex)
        {
            DebugLog.debug("  PushUrlValidator:checkPowerAndDiscoverability"
                           + " Unable to switch on bluetooth Err:" + ex.getMessage());
            throw new IllegalArgumentException("Unable to switch on Bluetooth");
        }
    }
}
