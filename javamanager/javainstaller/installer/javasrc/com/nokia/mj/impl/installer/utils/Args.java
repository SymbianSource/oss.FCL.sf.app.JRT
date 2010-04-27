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


package com.nokia.mj.impl.installer.utils;

import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.Tokenizer;

import java.util.Hashtable;

/**
 * Class for handling command line arguments.
 */
public class Args
{
    private Hashtable iArgs = null;

    public Args(String[] aArgs)
    {
        iArgs = new Hashtable();
        for (int i = 0; i < aArgs.length; i++)
        {
            if (aArgs[i].startsWith("-"))
            {
                String arg = aArgs[i].substring(1);
                String value = null;
                int separator = arg.indexOf("=");
                if (separator > 0)
                {
                    value = arg.substring(separator+1);
                    arg = arg.substring(0, separator);
                }
                else
                {
                    value = ""; // Arg is present without value.
                }
                //if (i+1 < aArgs.length) {
                //    value = aArgs[i+1];
                //} else {
                //    value = ""; // Arg is present without value.
                //}
                iArgs.put(arg, value);
                //Log.log("Args: " + arg + "=" + value);
            }
        }
    }

    /**
     * Returns command line argument with the specified name.
     * If argument is not found, returns null.
     */
    public String get(String aName)
    {
        String arg = (String)iArgs.get(aName);
        if (arg != null)
        {
            if (arg.length() == 0)
            {
                Log.log("Args: " + aName);
            }
            else
            {
                Log.log("Args: " + aName + "=" + arg);
            }
        }
        return arg;
    }

    /**
     * Returns command line argument with the specified name.
     * If argument is not found, returns default value.
     */
    public String get(String aName, String aDefault)
    {
        String arg = get(aName);
        if (arg == null)
        {
            arg = aDefault;
            if (arg != null)
            {
                if (arg.length() == 0)
                {
                    Log.log("Args: " + aName + " (default)");
                }
                else
                {
                    Log.log("Args: " + aName + "=" + arg + " (default)");
                }
            }
        }
        return arg;
    }

    /**
     * Returns boolean type command line argument with the specified name.
     * If argument is not found, returns null.
     */
    public Boolean getBoolean(String aName)
    {
        Boolean arg = null;
        if (valueContains(aName, "no"))
        {
            Log.log("Args: " + aName + " disabled");
            arg = new Boolean(false);
        }
        else if (valueContains(aName, "yes"))
        {
            Log.log("Args: " + aName + " enabled");
            arg = new Boolean(true);
        }
        return arg;
    }

    /**
     * Returns boolean type command line argument with the specified name.
     * If argument is not found, returns default value.
     */
    public Boolean getBoolean(String aName, Boolean aDefault)
    {
        Boolean arg = getBoolean(aName);
        if (arg == null)
        {
            arg = aDefault;
        }
        return arg;
    }

    /**
     * Returns boolean type command line argument with the specified name.
     * If argument is not found, returns default value.
     */
    public boolean getBoolean(String aName, boolean aDefault)
    {
        Boolean arg = getBoolean(aName);
        if (arg == null)
        {
            return aDefault;
        }
        return arg.booleanValue();
    }

    /**
     * Returns true if command line argument with specified name
     * exists, and if its value contains the specified value part.
     * Value parts are separated with commas, for example argument
     * "-arg=foo,bar" has two value parts.
     */
    public boolean valueContains(String aName, String aValuePart)
    {
        String value = get(aName);
        if (value == null || value.length() == 0)
        {
            return false;
        }
        String[] tokens = Tokenizer.split(value, ",");
        for (int i = 0; i < tokens.length; i++)
        {
            if (tokens[i].equals(aValuePart))
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Returns drive id from given string.
     */
    public static int parseDrive(String aDrive) throws InstallerException
    {
        int drive = -1;
        try
        {
            drive = Integer.parseInt(aDrive);
        }
        catch (NumberFormatException nfe)
        {
            // Drive argument is not a number, assume it is a string
            // of length one indicating drive letter.
        }
        if (drive == -1)
        {
            if (aDrive.length() != 1)
            {
                InstallerException.internalError(
                    "Invalid drive argument: " + aDrive);
            }
            drive = aDrive.toLowerCase().charAt(0) - 'a';
        }
        Log.log("Parsed drive " + aDrive + " --> " + drive);
        return drive;
    }
}
