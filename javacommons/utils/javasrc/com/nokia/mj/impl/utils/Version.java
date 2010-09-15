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


package com.nokia.mj.impl.utils;

/**
 * Class for holding version information.
 */
public class Version
{
    private int iMajor = 0;
    private int iMinor = 0;
    private int iMicro = 0;

    /**
     * Private default constructor.
     */
    private Version()
    {
    }

    /**
     * Constructor for version object.
     *
     * @param aMajor major part of version
     * @param aMinor minor part of version
     * @param aMicro micro part of version
     */
    public Version(int aMajor, int aMinor, int aMicro)
    {
        iMajor = aMajor;
        iMinor = aMinor;
        iMicro = aMicro;
    }

    /**
     * Parses version from given string. If string is not
     * valid version this method returns null.
     */
    public static Version getVersion(String aStr)
    {
        if (aStr == null || aStr.length() == 0)
        {
            return null;
        }
        // Remove all invalid characters from string
        String str = normalizeVersion(aStr);
        if (str.length() == 0)
        {
            return null;
        }

        int major = 0;
        int minor = 0;
        int micro = 0;

        Version version = null;
        try
        {
            if (str != null && str.length() > 0)
            {
                // Parse major part.
                int i = str.indexOf(".");
                if (i == -1)
                {
                    major = Integer.parseInt(str);
                    str = null;
                }
                else
                {
                    major = Integer.parseInt(str.substring(0, i));
                    str = str.substring(i+1);
                }
            }
            if (str != null && str.length() > 0)
            {
                // Parse minor part.
                int i = str.indexOf(".");
                if (i == -1)
                {
                    minor = Integer.parseInt(str);
                    str = null;
                }
                else
                {
                    minor = Integer.parseInt(str.substring(0, i));
                    str = str.substring(i+1);
                }
            }
            if (str != null && str.length() > 0)
            {
                // Parse micro part.
                int i = str.indexOf(".");
                if (i == -1)
                {
                    micro = Integer.parseInt(str);
                    str = null;
                }
                else
                {
                    throw new NumberFormatException(
                        "more than three parts in version number");
                }
            }
            version = new Version(major, minor, micro);
        }
        catch (NumberFormatException nfe)
        {
            //Log.logError("Version parsing failed: " + aStr, nfe);
            throw nfe;
        }
        return version;
    }

    /**
     * Get major part of the version.
     */
    public int getMajor()
    {
        return iMajor;
    }

    /**
     * Get minor part of the version.
     */
    public int getMinor()
    {
        return iMinor;
    }

    /**
     * Get micro part of the version.
     */
    public int getMicro()
    {
        return iMicro;
    }

    /**
     * Compare this version to given one.
     * @return a negative integer, zero, or a positive integer
    * if this version is less than, equal to, or greater than
    * the given version.
     */
    public int compareTo(Version aVersion)
    {
        int result = 0;

        if (getMajor() < aVersion.getMajor())
        {
            result = -1;
        }
        else if (getMajor() > aVersion.getMajor())
        {
            result = 1;
        }

        if (result == 0)
        {
            if (getMinor() < aVersion.getMinor())
            {
                result = -1;
            }
            else if (getMinor() > aVersion.getMinor())
            {
                result = 1;
            }
        }

        if (result == 0)
        {
            if (getMicro() < aVersion.getMicro())
            {
                result = -1;
            }
            else if (getMicro() > aVersion.getMicro())
            {
                result = 1;
            }
        }

        return result;
    }

    /**
     * Returns a hash code value for this object.
     */
    public int hashCode()
    {
        return iMajor + iMinor + iMicro;
    }

    /**
     * Indicates whether given object is equal to this one.
     */
    public boolean equals(Object aObj)
    {
        if (!(aObj instanceof Version))
        {
            return false;
        }
        Version v = (Version)aObj;
        if (this.getMajor() == v.getMajor() &&
                this.getMinor() == v.getMinor() &&
                this.getMicro() == v.getMicro())
        {
            return true;
        }
        return false;
    }

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append(getMajor()).append(".").append(getMinor());
        if (iMicro != 0)
        {
            buf.append(".").append(getMicro());
        }
        return buf.toString();
    }

    /**
     * Remove the illegal characters from version string,
     * for example "1.1 rev.2" becomes "1.1.2"
     */
    private static String normalizeVersion(String aString)
    {
        StringBuffer buf = new StringBuffer();
        int len = aString.length();
        for (int ind = 0; ind < len; ind++)
        {
            char ch = aString.charAt(ind);
            if ((ch == '.') || Character.isDigit(ch))
            {
                buf.append(ch);
            }
        }
        return buf.toString();
    }
}
