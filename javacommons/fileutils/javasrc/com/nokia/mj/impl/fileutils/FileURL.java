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

package com.nokia.mj.impl.fileutils;

/**
 * FileURL manages URL/Path handling. It is responsible to manage paths which
 * have path separators specific to the platform. It will also manage
 * differences between URL and Absolute path and make it transparent to use
 * from.
 */
public final class FileURL
{
    private static String FILE_PREFIX = "file://";

    private static String FILE_VALID_START_1 = "file:///";

    private static String FILE_VALID_START_2 = "file://localhost/";

    public static final String S60_FILE_SEPARATOR = "\\";

    public static final String LINUX_FILE_SEPARATOR = "/";

    private boolean iIsS60Path;

    // Contains the unescaped path.
    private String iUnescapedPath;

    // Contains the absolute path to the current target.
    private String iTargetPath;

    // Contains the name of the target.
    private String iTargetName;

    private String iSeparatorUsed;

    /**
     * Constructor for FileURL.
     *
     * @param aName
     *            Can contain either an Absolute path name or a File URL.
     *            <p>
     *            Path separator in Absolute path can be either "/" or platform
     *            specific path separator. This is specified in file.separator
     *            system property.
     *            <p>
     *            In case of file URL being passed as parameter, it must follow
     *            the format of a fully-qualified, absolute path file name as
     *            described by the file URL format in IETF RFCs 1738 & 2396.
     */
    public FileURL(String aName)
    {
        iIsS60Path = false;
        String initialPath  = aName;
        String path = "";

        if (aName.trim().length() <= 0)
        {
            throw new IllegalArgumentException();
        }

        if (aName.startsWith(FILE_PREFIX))
        {
            validateUrlStart(aName);

            // aName is a url of the form file://<host>/<path>
            // We need just the absolute path. So we extract it.
            path = initialPath.substring(initialPath.indexOf('/', FILE_PREFIX
                                         .length()) + 1);

            // A file URL should have only "/" as file separator.
            if (initialPath.indexOf(S60_FILE_SEPARATOR) != -1)
            {
                throw new IllegalArgumentException();
            }
        }
        else
        {
            path = aName;
        }

        int endIndex = 0;
        String tempName = path;
        while (tempName.indexOf("//")!=-1)
        {
            endIndex = path.indexOf("//");
            tempName = path.substring(0, endIndex);
            if (endIndex < path.length())
            {
                tempName += path.substring(endIndex+1);
            }
        };
        path = tempName;

        // Check the sanity of path specified
        checkPathValidity(path);
        iUnescapedPath = path;

        iIsS60Path = (iUnescapedPath.indexOf("\\") != -1);
        int index = 0;

        if (iIsS60Path)
        {
            iSeparatorUsed = "\\";
            index = iUnescapedPath.lastIndexOf('\\',
                                               iUnescapedPath.length() - 2);
        }
        else
        {
            iSeparatorUsed = "/";
            index = iUnescapedPath
                    .lastIndexOf('/', iUnescapedPath.length() - 2);
        }

        iTargetName = iUnescapedPath.substring(index + 1);
        iTargetPath = iUnescapedPath.substring(0, index + 1);
    }

    /**
     * checks to see if URL begins with valid urls starts.<br/> It can either
     * be file:/// or file://localhost/.
     *
     * @param url
     *            contains url passed to the constructor
     */
    private void validateUrlStart(String url)
    {
        if (url.startsWith(FILE_VALID_START_1)
                || url.startsWith(FILE_VALID_START_2))
        {
            return;
        }
        throw new IllegalArgumentException();
    }

    /**
     * Getter method to retrieve path to the parent directory of the current
     * target.
     *
     * @return string containing absloute path to the target's parent.
     */
    public String getPath()
    {
        return iTargetPath;
    }

    /**
     * Getter method to retrieve name of the target.
     *
     * @return string containing the name of the target currently pointed to.
     *         This is derived from the url.
     */
    public String getName()
    {
        return iTargetName;
    }

    /**
     * Getter method to retrieve the full path of the target. It will contain
     * path to the target and the target too as a complete url.
     *
     * @return the full path of the target
     */
    public String getFullPath()
    {
        return iTargetPath + iTargetName;
    }

    protected String getSeparatorUsed()
    {
        return iSeparatorUsed;
    }

    /**
     * Getter method to retrieve the file URL that can be passed as a parameter
     * to FileConnection url. It will conform to File URL format in IETF RFCs
     * 1738 & 2396.
     *
     * @return fully qualified File URL
     */
    public String getUrl()
    {
        // Get URL must return escaped url.
        String aUrl = FILE_VALID_START_1 + iTargetPath + iTargetName;
        String value = "";

        for (int i = 0; i < aUrl.length(); i++)
        {
            char ch = aUrl.charAt(i);

            // Also replace "\\" with "/"
            if (ch == '\\')
            {
                value += "/";
                continue;
            }

            boolean isLetter = (ch >= 'a' && ch <= 'z')
                               || (ch >= 'A' && ch <= 'Z');
            boolean isDigit = (ch >= '0' && ch <= '9');
            if (isLetter || isDigit)
            {
                value += ch;
            }
            else
            {
                value += stringToHex("" + ch);
            }

        }

        return value;
    }

    /**
     * Checks to see if the path passed is valid. Simple sanity checks for the
     * path to point to a valid target.
     *
     * @param name
     *            contains an absolute path to a target.
     */
    private static final void checkPathValidity(String name)
    {
        // Only in S60 file system, we have the concept of drives.
        // So, ":" should only appear in File systems that have
        // file.separator as '\\'

        String separator = System.getProperty("file.separator");

        // If platform is S60 like.
        if (separator.equals(S60_FILE_SEPARATOR))
        {
            makeS60SpecificChecks(name);
        }
        else
        {
            makeLinuxSpecificChecks(name);
        }

        if ((name.indexOf('*') != -1) || (name.indexOf('?') != -1)
                || (name.indexOf('<') != -1) || (name.indexOf('>') != -1)
                || (name.indexOf("//") != -1) || name.endsWith("/."))
        {
            throw new IllegalArgumentException(name);
        }
    }

    /**
     * Makes checks that are specific to S60 like File System. This should be
     * called only in case file.separator is "\\"
     *
     * @param name
     *            contians the path that needs to be checked for sanity.
     */
    private static final void makeS60SpecificChecks(String name)
    {
        // First index of colon
        int firstIndexOfColon = name.indexOf(':');
        int indexOfColon = name.indexOf(':', firstIndexOfColon + 1);

        if (indexOfColon != -1)
        {
            throw new IllegalArgumentException(name);
        }

        // In S60 like file system, it is possible to have "/" as the file
        // separator as file url accepts it. But then, once there is "\\" as
        // file separator, the entire url should follow the same convention.

        if (name.indexOf(S60_FILE_SEPARATOR) != -1)
        {
            if (name.indexOf(LINUX_FILE_SEPARATOR) != -1)
            {
                // We must be having only one kind of separator
                throw new IllegalArgumentException(name);
            }

            if (firstIndexOfColon > name.indexOf(S60_FILE_SEPARATOR))
            {
                throw new IllegalArgumentException(name);
            }
        }
        else
        {
            if ((firstIndexOfColon > name.indexOf(LINUX_FILE_SEPARATOR)))
            {
                throw new IllegalArgumentException(name);
            }
        }
    }

    /**
     * Makes checks that are specific to Linux like File System. This should be
     * called only in case file.separator is "/"
     *
     * @param name
     *            contians the path that needs to be checked for sanity.
     */
    private static final void makeLinuxSpecificChecks(String name)
    {
        if (name.indexOf(':') != -1)
        {
            throw new IllegalArgumentException(name);
        }

        if (name.indexOf(S60_FILE_SEPARATOR) != -1)
        {
            throw new IllegalArgumentException(name);
        }
    }

    /**
     * encodes a unicode string to a string of 4-digit hex numbers <br />
     * Note: this method is slower than stringToHex(String,StringBuffer)
     *
     * @param java
     *            normal java string
     * @return string of 4-digit hex numbers
     */
    public static final String stringToHex(String str)
    {
        if (str == null)
            return "";
        int length = str.length();
        StringBuffer result = new StringBuffer(length * 4);
        stringToHex(str, result);
        return result.toString();
    }

    /**
     * encodes a unicode string to a string of 4-digit hex numbers.
     *
     * @param str
     *            normal java string
     * @param out
     *            string of 4-digit hex numbers
     */
    public static final void stringToHex(String str, StringBuffer out)
    {
        // To improve performance, implement a table-lookup instead of if/then cases.

        if (str == null)
            return;
        int length = str.length();
        for (int pos = 0; pos < length; pos++)
        {
            char cur_char = str.charAt(pos);
            int this_char = (int) cur_char;
            if (this_char < 127 && cur_char != ' ')
            {
                out.append((char) this_char);
                continue;
            }
            for (int digit = 0; digit < 4; digit++)
            {
                int this_digit = this_char & 0xf000;
                this_char = this_char << 4;
                this_digit = (this_digit >> 12);

                // We get 4 digits, so on 2nd we insert a % sign and then value
                if (digit == 2)
                    out.append('%');
                if (digit < 2)
                    continue;
                if (this_digit >= 10)
                    out.append((char)(this_digit + 87));
                else
                    out.append((char)(this_digit + 48));
            }
        }
    }
}
