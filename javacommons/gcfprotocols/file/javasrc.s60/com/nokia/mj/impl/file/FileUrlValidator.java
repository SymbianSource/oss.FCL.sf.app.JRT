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

import java.io.IOException;
import java.util.Vector;

final class FileUrlValidator
{
    private static String FILE_LOCALHOST = "file://localhost/";

    private static String FILE_SIMPLE = "file:///";

    public static String validateFileUrl(String aFileUrl) throws IOException
    {
        String path = aFileUrl;

        if (aFileUrl.startsWith(FILE_LOCALHOST))
        {
            path = aFileUrl.substring(FILE_LOCALHOST.length());
        }
        else if (aFileUrl.startsWith(FILE_SIMPLE))
        {
            path = aFileUrl.substring(FILE_SIMPLE.length());
        }
        else
        {
            // Changed to illegal argument exception from IOException
            throw new IOException("Improper URL Start");
        }

        if (path.length() < 3)
        {
            throw new IllegalArgumentException("Invalid Uri: "+aFileUrl);
        }

        checkPathStart(path);
        if (aFileUrl.length() > 256)
        {
            throw new IllegalArgumentException("Invalid Uri: "+aFileUrl);
        }
        return path;
    }

    /**
     * Checks to see if the URL begins properly. Case 1: It must either start
     * wth file:/// or file://localhost/ Case 2: The string following the start
     * must be one of the roots.
     */
    private static void checkPathStart(String aFilePath) throws IOException
    {
        FileLogger.Log("FileUrlValidator.checkPathStart: " + aFilePath);
        Vector roots = FileSystemUtils.listRoots();
        boolean validPath = false;

        for (int i = 0; i < roots.size(); i++)
        {
            if (aFilePath.startsWith((String) roots.elementAt(i))
                    || aFilePath.toLowerCase().startsWith(
                        ((String) roots.elementAt(i)).toLowerCase()))
            {
                validPath = true;
                break;
            }
        }

        if (!validPath)
        {
            throw new IOException("Invalid Uri");
        }
    }
}
