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


package com.nokia.mj.impl.rt.midp;

import java.util.Hashtable;

/**
 * A Utility class for parsing and storing the arguments provided to the
 * MIDP runtime. It is assumed that the arguments are provided so that
 * there is always a key starting with '-' available and after that the
 * value. Keys without value are not supported.
 */
public final class MainArgs
{
    /**
     * For storing the arguments.
     */
    private Hashtable mArgs;

    /**
     * For storing the application arguments.
     */
    private String[] mAppArgs;

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Default construtor not allowed.
     */
    private MainArgs()
    {
    }


    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Constructor of the MainArgs. Will parse the given key-value pair
     * arguments into a hash table.
     * @param args Array of arguments.
     */
    public MainArgs(String[] args)
    {
        if (Log.mOn) Log.logI("Parsing args.");
        if (args == null)
        {
            throw new RuntimeException("No arguments provided to main()");
        }
        mArgs = new Hashtable();
        int i = 0;
        for (; i < args.length; ++i)
        {
            if (args[i].equals("-interalArgEnd"))
            {
                ++i;
                break;
            }
            if (i + 1 == args.length)
            {
                throw new RuntimeException("Not equal amount of arguments "+
                                           "provided to main()");
            }
            String argKey = args[i];
            String argValue = args[++i];
            if (!argKey.startsWith("-"))
            {
                throw new RuntimeException("Illegal key " + argKey + "in the" +
                                           "arguments provided to main()");
            }
            if (Log.mOn) Log.logI("  key: " + argKey + ", value:" + argValue);
            mArgs.put(argKey, argValue);
        }
        if (i < args.length)
        {
            int appArgCount = args.length - i;
            mAppArgs = new String[appArgCount];
            System.arraycopy(args, i, mAppArgs, 0, appArgCount);
            for (int j = 0; j < mAppArgs.length; ++j)
            {
                if (Log.mOn) Log.logI("AppArg: " + mAppArgs[j]);
            }
        }
    }

    /**
     * Searches the attribute using the provided key.
     * @param key The search key.
     * @return the value of the attribute if found, otherwise null.
     */
    public String findArgument(String key)
    {
        return (String)mArgs.get(key);
    }

    public String[] getAppArgs()
    {
        return mAppArgs;
    }

}
