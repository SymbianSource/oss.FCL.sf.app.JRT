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

package com.nokia.mj.impl.file;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Vector;

import com.nokia.mj.impl.fileutils.FileUTF8Handler;

/**
 * FileConnectionURL is used to hold and return File URL in the desired format.
 * It also validates the correctness of the URL passed during construction. Some
 * wierd requirements in the spec mean that there is a lot of manipulation to do
 * to get the required data out to application.
 */
final class FileConnectionURL
{
    private static String FILE_SIMPLE = "file:///";

    private static String FILE_LOCALHOST = "file://localhost/";

    // Stores the URL passed by the application, as is.
    private String iInitialUrl;

    private String iRootPath;

    private String iPath;

    private String iName;

    public FileConnectionURL(String aUrl) throws IOException
    {
        this(aUrl, true);
    }

    /**
     * Constructor for FileConnectionURL
     *
     * @param aUrl
     *            URL passed to Connector.open
     */
    public FileConnectionURL(String aUrl, boolean aUnescapeUrl)
    throws IOException
    {
        this(aUrl, aUrl, aUnescapeUrl);
    }

    public FileConnectionURL(String aInitialUrl, String aUrl,
                             boolean aUnescapeUrl) throws IOException
    {
        iInitialUrl = aInitialUrl;

        // Unescape the URL presented.
        String unescapedUrl = aUrl;

        if (aUnescapeUrl)
        {
            unescapedUrl = FileUTF8Handler.decode(aUrl);
        }
        else
        {
            unescapedUrl = aUrl;
        }

        unescapedUrl = unescapedUrl.replace('|', ':');
        checkFileUrlValidity(unescapedUrl);
        String path = null;

        path = unescapedUrl;
        if (iInitialUrl.startsWith(FILE_LOCALHOST))
        {
            path = unescapedUrl.substring(FILE_LOCALHOST.length());
        }
        else if (iInitialUrl.startsWith(FILE_SIMPLE))
        {
            path = unescapedUrl.substring(FILE_SIMPLE.length());
        }

        String actualPath = path;
        if (actualPath.length() < 3)
        {
            if (actualPath.charAt(1) == ':')
            {
                actualPath += "/";
            }
        }
        FileLogger.Log("FileConnectionURL: Resolved  Path: " + actualPath);

        Vector roots = FileSystemUtils.listRoots();
        for (int i = 0; i < roots.size(); i++)
        {
            // Is this valid in all cases in Linux as well?
            if (actualPath.toLowerCase().startsWith(
                        ((String) roots.elementAt(i)).toLowerCase())
                    || actualPath.startsWith((String) roots.elementAt(i)))
            {
                iRootPath = actualPath.substring(0, ((String) roots
                                                     .elementAt(i)).length());
                FileLogger.Log("FileConnectionURL: Root Path: " + iRootPath);
            }
        }

        String filePath = actualPath.substring(iRootPath.length());
        FileLogger.Log("FileConnectionURL: File Path: " + filePath);

        if (filePath.endsWith("/"))
        {
            filePath = filePath.substring(0, filePath.length() - 1);
        }

        iPath = filePath.substring(0, filePath.lastIndexOf('/') + 1);
        iName = filePath.substring(filePath.lastIndexOf('/') + 1);
    }

    public String getPath()
    {
        String retVal;
        if (!iRootPath.startsWith("/"))
        {
            retVal = "/" + iRootPath + iPath;
        }
        else
        {
            retVal = iRootPath + iPath;
        }

        if (retVal == null)
        {
            return "";
        }
        else
        {
            return retVal;
        }
    }

    public String getName()
    {
        if (iName != null)
        {
            return iName;
        }
        else
        {
            return "";
        }
    }

    public String getUrl()
    {
        return FileUTF8Handler.encode(iInitialUrl);
    }

    public static void checkFileUrlValidity(String aFileUrl)
    {
        boolean result = true;
        String name = aFileUrl;
        if ((name.indexOf('*') != -1) || (name.indexOf('\\') != -1))
        {
            throw new IllegalArgumentException();
        }

        if ((name.indexOf('?') != -1) || (name.indexOf('<') != -1)
                || (name.indexOf('<') != -1))
        {
            throw new IllegalArgumentException();
        }

        if ((name.indexOf("/./") != -1) || (name.indexOf("/../") != -1)
                || (name.endsWith("/..")) || (name.endsWith("/.")))
        {
            throw new IllegalArgumentException();
        }
    }
}
