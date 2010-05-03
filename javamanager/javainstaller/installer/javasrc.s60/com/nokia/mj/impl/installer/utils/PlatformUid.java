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
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;

import java.util.Random;

/**
 * Unique identifier for S60 platform.
 */
public class PlatformUid extends Uid
{
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
        int i = parseUid(aValue);
        if (i == -1)
        {
            return null;
        }
        return createUid(i);
    }

    /**
     * Creates a new Uid object from given int.
     */
    public static Uid createUid(int aIntValue)
    {
        String uidStr = toHexString(aIntValue);
        PlatformUid uid = new PlatformUid(uidStr);
        uid.iIntValue = aIntValue;
        return uid;
    }

    /**
     * Checks that given Uid is valid.
     * @throws InvalidAttributeException if given Uid is not valid
     */
    public static void checkValidity(
        String aAttrName, Uid aUid, boolean aTrusted,
        boolean aExistsInJad, boolean aExistsInJar)
    {
        // Choose correct OTA status code, depending on if the Uid
        // is present in jad or in jar.
        int otaStatusCode = 0;
        if (aTrusted)
        {
            otaStatusCode = OtaStatusCode.INVALID_DESCRIPTOR;
            if (aExistsInJar)
            {
                otaStatusCode = OtaStatusCode.INVALID_JAR;
            }
        }
        else
        {
            otaStatusCode = OtaStatusCode.INVALID_JAR;
            if (aExistsInJad)
            {
                otaStatusCode = OtaStatusCode.INVALID_DESCRIPTOR;
            }
        }
        int uid = getIntValue(aUid);
        // Check that Uid is from allowed range
        if ((uid >= 0x20000000 && uid < 0x30000000) ||
                (uid >= 0xA0000000 && uid < 0xB0000000) ||
                (uid >= 0xE0000000 && uid < 0xF0000000))
        {
            // Uid is valid
        }
        else
        {
            // Uid is invalid.
            Log.logError("Uid value is out of allowed range: " +
                         aAttrName + ": " + aUid.toString());
            throw new InvalidAttributeException
            (InstallerErrorMessage.INST_CORRUPT_PKG, null,
             InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
             new String[] { aAttrName }, otaStatusCode);
        }

        if (uid >= 0x20000000 && uid < 0x30000000)
        {
            // Protected Uids can only be used in trusted applications.
            if (!aTrusted)
            {
                Log.logError("Protected Uids can only be used in trusted apps: " +
                             aAttrName + ": " + aUid.toString());
                throw new InvalidAttributeException
                (InstallerErrorMessage.INST_CORRUPT_PKG, null,
                 InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                 new String[] { aAttrName }, otaStatusCode);
            }
        }

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
        // Set S60 uid limits.
        final int javaUidLowerLimit = 269480878; // 0x100ff3ae
        final int javaUidUpperLimit = 270480878; // 0x101f35ee

        int i = -1;
        if (aPreviousUid != null && aPreviousUid instanceof PlatformUid)
        {
            i = ((PlatformUid)aPreviousUid).getIntValue() + 1;
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
        Uid uid = createUid(i);
        return uid;
    }

    /**
     * Returns int value for this Uid.
     */
    public int getIntValue()
    {
        return iIntValue;
    }

    /**
     * Returns int value for an S60 uid.
     */
    public static int getIntValue(Uid aUid)
    {
        if (aUid == null)
        {
            // This should never happen, throw descriptive
            // InstallerException to aid in debugging if
            // this happens.
            InstallerException.internalError(
                "PlatformUid.getIntValue: uid is null");
        }
        PlatformUid uid = (PlatformUid)PlatformUid.createUid(
                              aUid.getStringValue());
        if (uid == null)
        {
            // This should never happen, throw descriptive
            // InstallerException to aid in debugging if
            // this happens.
            InstallerException.internalError(
                "PlatformUid.getIntValue: invalid uid " +
                aUid.getStringValue());
        }
        return uid.getIntValue();
    }

    /**
     * Parses int format Uid string.
     * Recognizes the following Uid formats:
     * 12345678 (decimal), 0x1234abcd (hexadecimal),
     * [1234abcd] (S60 hexadecimal).
     * Returns -1 if Uid cannot be parsed.
     */
    private static int parseUid(String aStr)
    {
        int result = -1;
        if (aStr == null || aStr.length() == 0)
        {
            return result;
        }

        String numStr = aStr;

        // Check if value is negative.
        boolean negative = false;
        if (numStr.startsWith("-"))
        {
            negative = true;
            numStr = numStr.substring(1);
        }

        // Check for optional radix prefix.
        int radix = 10;
        if (numStr.startsWith("0x"))
        {
            radix = 16;
            numStr = numStr.substring(2);
        }

        // Check if numStr is in Symbian TUid form [12345678].
        if (numStr.length() <= 10 && numStr.startsWith("[") && numStr.endsWith("]"))
        {
            radix = 16;
            numStr = numStr.substring(1, numStr.length()-1);
        }

        int i = 0;
        while (i < numStr.length())
        {
            char ch = numStr.charAt(i);
            if (Character.isDigit(ch) || (radix == 16 && isHexDigit(ch)))
            {
                // Is valid digit, continue.
            }
            else
            {
                // No more valid digits, break the loop.
                break;
            }
            i++;
        }
        if (i == 0)
        {
            return result;
        }
        numStr = numStr.substring(0, i);

        long value = Long.parseLong(numStr, radix);
        if (negative)
        {
            result = (int)-value;
        }
        else
        {
            result = (int)value;
        }
        return result;
    }

    /**
     * Returns true if given char is a hexadecimal digit.
     * This is a temporary helper method and will be removed.
     */
    private static boolean isHexDigit(char aChar)
    {
        switch (Character.toUpperCase(aChar))
        {
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            return true;
        }
        if (Character.isDigit(aChar))
        {
            return true;
        }
        return false;
    }

    /**
     * Returns decimal string format for given int Uid.
     */
    private static String toDecString(int aUid)
    {
        return Integer.toString(aUid);
    }

    /**
     * Returns S60 hexadecimal string format for given int Uid.
     */
    private static String toHexString(int aUid)
    {
        StringBuffer result =
            new StringBuffer(Integer.toHexString(aUid).toLowerCase());
        for (int i = result.length(); i < 8; i++)
        {
            result.insert(0, '0');
        }
        // Add brackets that surround S60 Uid.
        result.insert(0, "[");
        result.append("]");
        return result.toString();
    }
}
