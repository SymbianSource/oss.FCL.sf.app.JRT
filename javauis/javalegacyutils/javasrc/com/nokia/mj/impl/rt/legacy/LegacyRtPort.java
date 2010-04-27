/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.rt.legacy;

import javax.microedition.midlet.MIDlet;

import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * A class for porting legacy JSRs to new runtime.
 */
public class LegacyRtPort
{

    /**
     * A singleton instance.
     */
    private static LegacyRtPort mInstance = new LegacyRtPort();

    /**
     * The running MIDlet instance.
     */
    private MIDlet    mMidlet;

    /**
     * The registered UI toolkit.
     */
    private String    mRegisteredToolkit;

    /**
     * Contructor, allowed only for this class.
     */
    private LegacyRtPort()
    {
    }

    /**
     * Sets the running MIDlet.
     */
    static void setMidlet(MIDlet midlet)
    {
        mInstance.mMidlet = midlet;
    }

    /**
     * Gets the running MIDlet instance.
     */
    public static MIDlet getMidlet()
    {
        return mInstance.mMidlet;
    }

    /**
     * Gets the UID of the running MIDlet in int format.
     */
    public static int getMidletUid()
    {
        int midletUid = -1;
        try
        {
            String uidS = ApplicationInfo.getInstance().getUid().getStringValue();
            // In Symbian the UID is in format '[<uid>]' where <uid> is in hex
            // format. So wee need to take the brackets away.
            // Long is needed in conversion because UIDs greater than 0x80000000
            // would fail if Integer would be used. However typecast from long
            // to int is safe since UID in Symbian is 32 bit.
            long uidL = Long.parseLong(uidS.substring(1,uidS.length()-1), 16);
            return (int)uidL;
        }
        catch (Throwable t)
        {
        }
        return midletUid;
    }


    /**
     * Registers active UI toolkit. There can be only one active UI toolkit at the
     * same time.
     */
    public static void registerToolkit(String toolkitId)
    {
        if (mInstance.mRegisteredToolkit != null)
        {
            throw new RuntimeException("Could not register toolkit: " +
                                       toolkitId);
        }
        mInstance.mRegisteredToolkit = toolkitId;
    }

    /**
     * Unregisters the given UI toolkit.
     */
    public static void unRegisterToolkit(String toolkitId)
    {
        if (mInstance.mRegisteredToolkit != null &&
                mInstance.mRegisteredToolkit.equals(toolkitId))
        {
            mInstance.mRegisteredToolkit = null;
        }
    }

    /**
     * Gets the active UI toolkit.
     */
    public static String getRegisteredToolkit()
    {
        return mInstance.mRegisteredToolkit;
    }

    /**
     * Gets the ApplicationId.
     */
    public static Object getApplicationId()
    {
        return mInstance;
    }

}
