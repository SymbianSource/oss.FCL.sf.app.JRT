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

import javax.microedition.io.file.*;
import javax.microedition.io.*;

import com.nokia.mj.test.file.utils.TestFailedException;

public class FileSecurityUtils
{
    public static void checkSecurityExceptionOnOpen(String aFileUrl)
    throws TestFailedException
    {
        try
        {
            FileConnection conn = (FileConnection) Connector.open(aFileUrl);
            throw new TestFailedException("Security Exception not thrown: "
                                          + aFileUrl);
        }
        catch (SecurityException ex)
        {
            // SecurityException thrown
        }
        catch (Exception ex)
        {
            throw new TestFailedException("Security Exception not thrown: "
                                          + aFileUrl);
        }
    }

    public static void checkSecurityExceptionOnSetWritableOperation(
        String aFileUrl) throws TestFailedException
    {
        int count = 0;
        FileConnection conn = null;
        try
        {
            conn = (FileConnection) Connector.open(aFileUrl);
            count++;
            conn.setWritable(true);
            throw new TestFailedException(
                "Security Exception not thrown: Count: " + count);
        }
        catch (SecurityException ex)
        {
            // Cool
            try
            {
                count++;
                conn.setWritable(false);
                throw new TestFailedException(
                    "Security Exception not thrown: Count: " + count);
            }
            catch (SecurityException ex1)
            {
                // Cool
            }
            catch (Exception ex1)
            {
                throw new TestFailedException(
                    "Security Exception not thrown: Count: " + count);
            }
            // SecurityException thrown
        }
        catch (Exception ex)
        {
            throw new TestFailedException(
                "Security Exception not thrown: Count: " + count);
        }
    }

    public static void checkSecurityExceptionOnSetReadableOperation(
        String aFileUrl) throws TestFailedException
    {
        int count = 0;
        FileConnection conn = null;
        try
        {
            conn = (FileConnection) Connector.open(aFileUrl);
            count++;
            conn.setReadable(true);
            throw new TestFailedException(
                "Security Exception not thrown: Count: " + count);
        }
        catch (SecurityException ex)
        {
            // Cool
            try
            {
                count++;
                conn.setReadable(false);
                throw new TestFailedException(
                    "Security Exception not thrown: Count: " + count);
            }
            catch (SecurityException ex1)
            {
                // Cool
            }
            catch (Exception ex1)
            {
                throw new TestFailedException(
                    "Security Exception not thrown: Count: " + count);
            }
            // SecurityException thrown
        }
        catch (Exception ex)
        {
            throw new TestFailedException(
                "Security Exception not thrown: Count: " + count);
        }
    }

    public static void checkSecurityExceptionOnSetHiddenOperation(
        String aFileUrl) throws TestFailedException
    {
        int count = 0;
        FileConnection conn = null;
        try
        {
            conn = (FileConnection) Connector.open(aFileUrl);
            count++;
            conn.setHidden(true);
            throw new TestFailedException(
                "Security Exception not thrown: Count: " + count);
        }
        catch (SecurityException ex)
        {
            // Cool
            try
            {
                count++;
                conn.setHidden(false);
                throw new TestFailedException(
                    "Security Exception not thrown: Count: " + count);
            }
            catch (SecurityException ex1)
            {
                // Cool
            }
            catch (Exception ex1)
            {
                throw new TestFailedException(
                    "Security Exception not thrown: Count: " + count);
            }
            // SecurityException thrown
        }
        catch (Exception ex)
        {
            throw new TestFailedException(
                "Security Exception not thrown: Count: " + count);
        }
    }

    public static void checkSecurityExceptionOnRenameOperation(String aFileUrl)
    throws TestFailedException
    {
        int count = 0;
        FileConnection conn = null;
        try
        {
            conn = (FileConnection) Connector.open(aFileUrl);
            count++;
            conn.rename("oops");
            throw new TestFailedException(
                "Security Exception not thrown: Count: " + count);
        }
        catch (SecurityException ex)
        {
            // Cool
        }
        catch (Exception ex)
        {
            throw new TestFailedException(
                "Security Exception not thrown: Count: " + count);
        }
    }

    public static void checkSecurityExceptionOnDeleteOperation(String aFileUrl)
    throws TestFailedException
    {
        int count = 0;
        FileConnection conn = null;
        try
        {
            conn = (FileConnection) Connector.open(aFileUrl);
            count++;
            conn.delete(); // Oops
            throw new TestFailedException(
                "Security Exception not thrown: Count: " + count);
        }
        catch (SecurityException ex)
        {
            // Cool
        }
        catch (Exception ex)
        {
            throw new TestFailedException(
                "Security Exception not thrown: Count: " + count);
        }
    }
}
