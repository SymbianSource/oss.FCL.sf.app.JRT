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


package com.nokia.mj.test.file.utils;

import java.io.IOException;
import javax.microedition.io.file.*;
import javax.microedition.io.*;

public class FileTestUtils
{
    public static void IllegalArgumentExceptionCheck(String aUri,
            boolean aIsSuccessCase) throws IOException
    {
        FileConnection fileConnection = null;
        try
        {
            fileConnection = (FileConnection) Connector.open(aUri);
            if (aIsSuccessCase)
            {
                fileConnection.close();
                return;
            }
            throw new TestFailedException();
        }
        catch (IllegalArgumentException ex)
        {
            if (aIsSuccessCase)
                throw new TestFailedException(ex.toString());
        }
        catch (Exception ex)
        {
            if (fileConnection != null)
                fileConnection.close();
            // append(ex.toString());
            ex.printStackTrace();
            throw new TestFailedException(ex.toString());
        }
    }

    public static void createObject(String aUri, boolean aIsFile)
    {
        FileConnection fileConnection = null;
        try
        {
            fileConnection = (FileConnection) Connector.open(aUri);
            if (aIsFile)
                fileConnection.create();
            else
                fileConnection.mkdir();
        }
        catch (Exception ex)
        {
            // append(ex.toString());
            ex.printStackTrace();
            throw new TestFailedException(ex.toString());
        }
        finally
        {
            if (fileConnection != null)
            {
                try
                {
                    fileConnection.close();
                }
                catch (Exception e)
                {
                    throw new TestFailedException(e.toString());
                }
            }
        }// end finally
    }

    public static void endpointExists(String aUri)
    {
        FileConnection fileConnection = null;
        try
        {
            fileConnection = (FileConnection) Connector.open(aUri);
            boolean retValue = fileConnection.exists();
            // fileConnection.close();
            if (false == retValue)
                throw new TestFailedException();
        }
        catch (Exception ex)
        {
            // append(ex.toString());
            ex.printStackTrace();
            throw new TestFailedException(ex.toString());
        }
        finally
        {
            if (fileConnection != null)
            {
                try
                {
                    fileConnection.close();
                }
                catch (Exception e)
                {
                    throw new TestFailedException();
                }
            }
        }// end finally
    }

    public static void IoExceptionCheck(String aUri, int aMode,
                                        boolean aNoException)
    {
        FileConnection fileConnection = null;
        try
        {
            fileConnection = (FileConnection) Connector.open(aUri, aMode);
            if (aNoException == false)
                throw new TestFailedException();
        }
        catch (IOException ex)
        {
            ex.printStackTrace();
            if (aNoException)
                throw new TestFailedException("#1 " + ex.toString());
        }
        catch (Exception ex)
        {
            // append(ex.toString());
            ex.printStackTrace();
            throw new TestFailedException("#2 " + ex.toString());
        }
        finally
        {
            if (fileConnection != null)
            {
                try
                {
                    fileConnection.close();
                }
                catch (Exception e)
                {
                    throw new TestFailedException("#3 " + e.toString());
                }
            }
        }// end finally
    }

    public static FileConnection openExistsURI(String aUri, int aMode)
    {
        FileConnection fileConnection = null;
        try
        {
            fileConnection = (FileConnection) Connector.open(aUri, aMode);
            if (fileConnection.exists())
            {
                return fileConnection;
            }
            else
            {
                fileConnection.close();
                return null;
            }
        }
        catch (Exception ex)
        {
            // append(ex.toString());
            ex.printStackTrace();
            return null;
        }
    }

    public static void testGetConnectionMethods(String aEndpoint, String aName,
            String aPath, String aUri) throws IOException
    {
        FileConnection conn1 = null;
        conn1 = (FileConnection) Connector.open(aEndpoint, Connector.READ);
        String name = conn1.getName();
        if (false == name.equals(aName))
            throw new TestFailedException("Name: " + name + " Expected: "
                                          + aName);
        String path = conn1.getPath();
        if (false == path.equals(aPath))
            throw new TestFailedException("path: " + path + " Expected: "
                                          + aPath);
        String uri = conn1.getURL();
        if (false == uri.equals(aUri))
            throw new TestFailedException("uri: " + uri + " Expected: " + aUri);
        conn1.close();
    }

    public static void createFile(String aUri) throws IOException
    {
        FileConnection conn1 = (FileConnection) Connector.open(aUri,
                               Connector.READ_WRITE);
        if (conn1.exists())
            throw new TestFailedException();
        conn1.create();
        conn1.close();
        // Re-open a file to confirm that file was created on the filesystem.
        conn1 = openExistsURI(aUri, Connector.READ);
        if (conn1 == null)
            throw new TestFailedException();
        if (true == conn1.isDirectory())
            throw new TestFailedException();
        conn1.close();
    }

    public static void createDir(String aUri) throws IOException
    {
        FileConnection conn1 = (FileConnection) Connector.open(aUri,
                               Connector.READ_WRITE);
        if (conn1.exists())
            throw new TestFailedException();
        conn1.mkdir();
        conn1.close();
        // Re-open a file to confirm that file was created on the filesystem.
        conn1 = openExistsURI(aUri, Connector.READ);
        if (conn1 == null)
            throw new TestFailedException();
        if (false == conn1.isDirectory())
            throw new TestFailedException();
        conn1.close();
    }

    public static void illegalArgumentFromRenaming(String aNewName, String aUri)
    throws IOException
    {
        FileConnection conn1 = null;
        boolean exceptionOccured = false;
        conn1 = (FileConnection) Connector.open(aUri, Connector.READ_WRITE);
        if (!conn1.exists())
            throw new TestFailedException();
        exceptionOccured = false;
        try
        {
            conn1.rename(aNewName);
        }
        catch (IllegalArgumentException ex)
        {
            exceptionOccured = true;
        }
        if (exceptionOccured == false)
            throw new TestFailedException();
        conn1.close();
    }

    public static void IOExceptionFromRenaming(String aNewName, String aUri)
    throws IOException
    {
        FileConnection conn1 = null;
        boolean exceptionOccured = false;
        conn1 = (FileConnection) Connector.open(aUri, Connector.READ_WRITE);
        if (!conn1.exists())
            throw new TestFailedException();
        exceptionOccured = false;
        try
        {
            conn1.rename(aNewName);
        }
        catch (IOException ex)
        {
            exceptionOccured = true;
        }
        if (exceptionOccured == false)
            throw new TestFailedException();
        conn1.close();
    }
}