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


package com.nokia.mj.impl.installer.customisationproperties;

import java.util.Hashtable;
import java.util.Vector;


/**
 * Fetches product program/operator specific customizable settings.
 */
public class CustomisationPropertiesBase
{
    // The names of the properties

    /**
     * The search key of the default installation drive property.
     * In S60 this property has both String and Integer value
     * (both getProperty and getIntegerProperty return value)
     */
    public static final String DefaultInstallationDrive = "A";

    /**
     * The search key of the possible installation drives property.
     * In S60 this property has both String and Integer value
     * (getProperty returns vector that contains several Strings
     *  and getIntegerProperty returns vector that contains the corresponding
     *  drive integers)
     */
    public static final String PossibleInstallationDrives = "B";

    /**
     * The search key of the default installation folder property.
     * This property has only single String value.
     */
    public static final String DefaultInstallationFolder = "C";

    /**
     * The search key of the possible installation folders property.
     * getProperty returns vector that contains several Strings
     */
    public static final String PossibleInstallationFolders = "D";

    /**
     * The search key of the possible installation folders property.
     * getProperty returns vector that contains several Strings.
     * One of them is always Nokia-MIDlet-Category
     */
    public static final String ShellFolderAttributeNames = "E";

    /**
     * The search key of the default preinstallation drive property.
     * In S60 this property has both String and Integer value
     * (both getProperty and getIntegerProperty return value)
     */
    public static final String PreinstalledDefaultDrive = "F";

    /**
     * The search key of the types of the possible installation drives property.
     * The types have been defined in DriveInfo class (PHONE_MEMORY,
     * MEMORY_CARD, INTERNAL_MASS_STORAGE and UNKNOWN_DRIVE)
     * getIntegerProperty returns vector that contains the types of the
     * possible installation drives in the same order as
     * the drives itself are in the vectors returned for
     * 'PossibleInstallationDrives'
     * @see com.nokia.mj.impl.installer.utils.DriveInfo
     */
    public static final String TypesOfPossibleInstallationDrives = "G";

    /**
     * The name of Nokia specific JAD/JAR attribute that tells the default
     * installation folder name
     */
    public static final String NokiaShellFolderAttributeName = "Nokia-MIDlet-Category";


    /**
     * Contains the customisation properties. Initialised in constructor.
     * The key is always String, the value is vector that contains either
     * Strings or Integers.
     */
    Hashtable iProperties;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Creates member iProperties
     */
    public CustomisationPropertiesBase()
    {
        iProperties = new Hashtable(7);
    }

    /**
     * Returns the value of the property or null if the property does not
     * exist or do not have value.
     *
     * @param aProperty One of the search keys defined in this class.
     * @return String vector containing the values of the property or null
     */
    public Vector getProperty(String aProperty)
    {
        return (Vector)iProperties.get(aProperty);
    }

    /**
     * Returns the value of the property or null if the property does not
     * exist or do not have value.
     *
     * @param aProperty One of the search keys defined in this class.
     * @return Integer vector containing the values of the property or null
     */
    public Vector getIntegerProperty(String aProperty)
    {
        return (Vector)iProperties.get(IntegerPropertyPrefix + aProperty);
    }

    /*** ----------------------------- PACKAGE  --------------------------- */

    /**
     * Split comma (',') separated string into substrings that
     * are returned in vector return value.
     * For example "Games,Applications,Utilities" will be
     * split into Vector("Games", "Applications", "Utilities")
     *
     * @param aValueList comma separated string
     * @return vector that contains the substrings
     */
    Vector splitCommaSeparatedString(String valueList)
    {
        Vector value = new Vector(5);
        String substring;

        if (valueList == null)
        {
            return value;
        }

        valueList = valueList.trim();
        if (valueList.equals(""))
        {
            return value;
        }

        int ind;
        do
        {
            ind = valueList.indexOf(',');
            if (-1 == ind)
            {
                // no ',' chars in string, add whole string
                value.addElement(valueList);
                break;
            }
            else
            {
                if (ind > 0)
                {
                    // substring is longer than ",", add it
                    // if there is something left after trim
                    substring = valueList.substring(0, ind).trim();
                    if (substring.length() > 0)
                    {
                        value.addElement(substring);
                    }
                }
            }

            // beginning of string handled, make new string that
            // contains the rest of the string
            if ((ind + 1) < valueList.length())
            {
                valueList = valueList.substring(ind + 1);
                valueList = valueList.trim();
            }
            else
            {
                break;
            }
        }
        while (true);

        return value;
    }

    /*** ----------------------------- PROTECTED  --------------------------- */

    /**
     * Used when storing both String value and Integer value for the same
     * property into iProperties
     */
    protected static final String IntegerPropertyPrefix = "I";

}
