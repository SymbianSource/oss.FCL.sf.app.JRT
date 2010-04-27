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

import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;

import java.util.Random;

/**
 * Unique identifier for Linux platform.
 */
public class PlatformUid extends Uid
{
    private static final String UID_PREFIX = "java";

    private static Random iRandom = null;

    private int iIntValue = -1;

    /**
     * Default constructor.
     */
    protected PlatformUid()
    {
        super();
    }

    /**
     * Constructor.
     */
    protected PlatformUid(String aValue)
    {
        super(aValue);
    }

    /**
     * Creates a new Uid object from given string.
     * If given string is not a valid Uid, returns null.
     */
    public static Uid createUid(String aValue)
    {
        if (aValue == null || aValue.length() == 0)
        {
            return null;
        }
        String value = aValue;
        if (value.startsWith(UID_PREFIX))
        {
            value = value.substring(UID_PREFIX.length());
        }
        Uid uid = null;
        try
        {
            uid = createUid(Integer.parseInt(value));
        }
        catch (NumberFormatException nfe)
        {
            uid = new PlatformUid(aValue);
        }
        return uid;
    }

    /**
     * Creates a new Uid object from given int.
     */
    public static Uid createUid(int aIntValue)
    {
        PlatformUid uid = new PlatformUid(
            UID_PREFIX + Integer.toString(aIntValue));
        uid.iIntValue = aIntValue;
        return uid;
    }

    /**
     * Checks that given Uid is valid.
     */
    public static void checkValidity(
        String aAttrName, Uid aUid, boolean aTrusted,
        boolean aExistsInJad, boolean aExistsInJar)
    {
        // For trusted application Uid must be specified in Jar.
        if (aTrusted && !aExistsInJar)
        {
            Log.logError("For trusted application Uid must be specified in Jar: " +
                         aAttrName + ": " + aUid.toString());
            throw new InvalidAttributeException
            (InstallerErrorMessage.INST_CORRUPT_PKG, null,
             InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
             new String[] { aAttrName }, OtaStatusCode.INVALID_JAR);
        }
    }

    /**
     * Generates a Uid from allowed Uid range.
     * If given "previous Uid" is not null, returns the next Uid
     * following the "previous Uid".
     */
    public static Uid generateUid(Uid aPreviousUid, String aSeed)
    {
        // Set uid limits.
        final int javaUidLowerLimit = 0;
        final int javaUidUpperLimit = 1000000;

        int i = -1;
        if (aPreviousUid != null && aPreviousUid instanceof PlatformUid)
        {
            i = ((PlatformUid)aPreviousUid).iIntValue + 1;
            if (i >= javaUidUpperLimit)
            {
                i = javaUidLowerLimit;
            }
        }
        else
        {
            if (aSeed != null)
            {
                iRandom = new Random(aSeed.hashCode());
            }
            if (iRandom == null)
            {
                iRandom = new Random();
            }
            int r = iRandom.nextInt();
            // Ensure that generated random number is positive.
            if (r < 0)
            {
                r = -r;
            }
            i = javaUidLowerLimit + (r % (javaUidUpperLimit - javaUidLowerLimit));
        }
        return createUid(i);
    }

    /**
     * Returns int value for a uid.
     */
    public static int getIntValue(Uid aUid)
    {
        if (aUid == null)
        {
            throw new NullPointerException(
                "PlatformUid.getIntValue: uid is null");
        }
        int result = -1;
        if (aUid instanceof PlatformUid)
        {
            result = ((PlatformUid)aUid).iIntValue;
        }
        return result;
    }
}
