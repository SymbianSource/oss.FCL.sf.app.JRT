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
* Description:  Locator which maps standard store names and URIs
 *
*/


// PACKAGE
package com.nokia.mj.impl.location;

// CLASS DESCRIPTION
/**
 * Locator which maps standard store names and URIs
 *
 * It is possible to map standard strings to URIs and vice versa
 */
public class Locator
{
    // Landmark database file extension
    public static final String DB_FILE_EXTENSION = ".ldb";

    // Protocol separator
    public static final String PROTOCOL_SEPARATOR = "://";

    // Default landmark database protocol
    public static final String DB_DEFAULT_PROTOCOL = "file";

    // Locator string including protocol, drive letter and file extension
    private String iLocatorString;

    // Default drive to where databases are created and expected to be in
    private static final String DEFAULT_DRIVE = "c:";

    // Minimum size of specially formatted store names
    private static final int MIN_SPECIAL_NAME_LENGTH = 5;

    /**
     * Creates a store locator from a name. Note that location API
     * cannot open other than stores which are located in the file
     * system of the device
     *
     * The name can contain a special syntax which can be used to open
     * stores from non-default drives. The syntax is as follows:
     *
     *      - <storename> (<drive letter>:)
     *
     * @param aName The name of the store
     */
    public static Locator createLocator(String aName)
    {
        StringBuffer buffer = new StringBuffer(DB_DEFAULT_PROTOCOL);
        buffer.append(PROTOCOL_SEPARATOR);
        // Create URI from the name. Note that this expects that the name
        // is not in URI format and parses the string according to that
        int length = aName.length();
        int driveStart = aName.lastIndexOf('(');
        int driveEnd = aName.indexOf(":)");
        // Parse the drive letter from the URI
        if (length >= MIN_SPECIAL_NAME_LENGTH && aName.endsWith(":)")
                && driveEnd == driveStart + DEFAULT_DRIVE.length())
        {
            // Add the drive letter and colon to the uri
            buffer.append(aName.substring(driveStart + 1, driveEnd + 1));
            // Parse the name of the store and add the default file extension
            // Remove white space from the end of the name.
            buffer.append(aName.substring(0, driveStart - 1));
        }
        // No drive letter. Add just name to the string buffer
        else
        {
            buffer.append(DEFAULT_DRIVE);
            buffer.append(aName);
        }
        // Add the default file extensions to the buffer and convert it to a string
        buffer.append(DB_FILE_EXTENSION);
        return new Locator(buffer.toString());
    }

    /**
     * Package private constructor. Creates a new Locator directly from
     * the passed string. The string must be a valid locator. Otherwise
     * the creating of the object will throw IllegalArgumentException
     *
     * @param aLocatorString The locator. Must be a valid locator
     */
    Locator(String aLocatorString)
    {
        if (aLocatorString.indexOf(PROTOCOL_SEPARATOR) == -1)
        {
            throw new IllegalArgumentException();
        }
        // Remove white spaces from the end and from the start
        iLocatorString = aLocatorString;
    }

    /**
     * Returns the store URI. null indicates the default store
     */
    public String getStoreURI()
    {
        return iLocatorString;
    }

    /**
     * Returns the name of the store. If the store is located in a non-default
     * drive, the drive letter will be added to the end of the name as a
     * special syntax.
     *
     * The default store locator (null) returns a null name
     */
    public String getName()
    {
        // Create a string buffer from the URI without the protocl and extension
        int s = getProtocol().length() + PROTOCOL_SEPARATOR.length();
        int e = getExtension().length();
        String drive = getDrive().toLowerCase();

        StringBuffer buffer = new StringBuffer(iLocatorString.substring(s
                                               + drive.length(), iLocatorString.length() - e));

        // Add the drive letter to the end of the name if the URI has it
        // and it is a non default drive letter
        if (drive.length() > 0 && !drive.equals(DEFAULT_DRIVE))
        {
            // The syntax for other than default drives is "name (<drive>:)"
            buffer.append(" (");
            buffer.append(drive);
            buffer.append(")");
        }

        return buffer.toString();
    }

    /**
     * Returns the drive of this store locator. An empty string is returned
     * if there is no drive in the the locator
     */
    private String getDrive()
    {
        // Get the last index of the colon mark. This is because the procol
        // separator has also a colon mark
        int colon = iLocatorString.lastIndexOf(':');
        return colon != -1 ? iLocatorString.substring(colon - 1, colon + 1)
               : "";
    }

    /**
     * Returns the file extensions of this store locator. An empty string is
     * returned if the locator does not have a file extension
     */
    private String getExtension()
    {
        // Find the index of the file extension
        int sep = iLocatorString.indexOf(DB_FILE_EXTENSION);
        if (sep == -1)
        {
            // Find "." separator if the database extensions is not in the URI
            // (i.e this is an external landmark store)
            sep = iLocatorString.lastIndexOf('.');
        }
        // Return an empty string if the uri does not have a protocol
        return sep != -1 ? iLocatorString.substring(sep, iLocatorString
                .length()) : "";
    }

    /**
     * Returns the protocol of this locator. An emptry string is returned
     * if the locator does not have a protocol
     */
    private String getProtocol()
    {
        // Find the index of the protocol separator
        int sep = iLocatorString.indexOf(PROTOCOL_SEPARATOR);
        // Return an empty string if the uri does not have a protocol
        return sep != -1 ? iLocatorString.substring(0, sep) : "";
    }
}

// End of file
