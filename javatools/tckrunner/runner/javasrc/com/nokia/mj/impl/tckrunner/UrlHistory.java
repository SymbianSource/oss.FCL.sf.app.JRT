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

package com.nokia.mj.impl.tckrunner;

import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;

import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.utils.LineReader;
import com.nokia.mj.impl.utils.Logger;

/**
 * UrlHistory saves the used URL to a file, which can be later retrieved
 */
public class UrlHistory
{
    private FileUtility iFile;

    public UrlHistory()
    {
        try
        {
            iFile = new FileUtility(getFilename());
            if (!iFile.exists())
            {
                iFile.createNewFile();
            }
        }
        catch (Exception ex)
        {
            Logger.ELOG(Logger.ETckRunner, "UrlHistory()", ex);
        }
    }

    public String loadUrl()
    {
        InputStream is = null;
        String url = null;

        try
        {
            is = iFile.openInputStream();
            LineReader lr = new LineReader(new InputStreamReader(is));
            url = lr.readLine();
            lr.close();
        }
        catch (Exception ex)
        {
            Logger.ELOG(Logger.ETckRunner, "loadUrl()", ex);
        }
        finally
        {
            try
            {
                if (is != null)
                {
                    is.close();
                }
            }
            catch (Exception ex)
            {
                Logger.ELOG(Logger.ETckRunner, "loadUrl()", ex);
            }
        }

        if (url == null || url.length() == 0)
        {
            url = "http://server:8080/test/getNextApp.jad";
        }
        Logger.ILOG(Logger.ETckRunner, "loadUrl() returns: " + url);
        return url;
    }

    public void saveUrl(String aUrl)
    {
        OutputStream os = null;
        try
        {
            iFile.truncate(0);

            os = iFile.openOutputStream();
            OutputStreamWriter writer = new OutputStreamWriter(os);
            writer.write(aUrl, 0, aUrl.length());
            writer.close();
            Logger.ILOG(Logger.ETckRunner, "saveUrl() ok: " + aUrl);
        }
        catch (Exception ex)
        {
            Logger.ELOG(Logger.ETckRunner, "saveUrl()", ex);
        }
        finally
        {
            try
            {
                if (os != null)
                {
                    os.close();
                }
            }
            catch (Exception ex)
            {
                Logger.ELOG(Logger.ETckRunner, "saveUrl()", ex);
            }
        }
    }

    private String getFilename()
    {
        String path = getRootPath() + "tckrunner.txt";
        Logger.ILOG(Logger.ETckRunner, "getFilename() returns: " + path);
        return path;
    }

    private String getRootPath()
    {
        String root;
        String platform = System.getProperty("os.name");
        if (platform != null && platform.equalsIgnoreCase("linux"))
        {
            root =  System.getProperty("JAVA_BIN_ROOT");
            if (root == null)
            {
                root = "";
            }
        }
        else
        {
            root = "c:\\private\\2002121e\\";
        }
        return root;
    }

}
