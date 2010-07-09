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

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.Vector;

import javax.microedition.io.Connector;
import javax.microedition.io.file.ConnectionClosedException;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.file.IllegalModeException;

import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.fileutils.FileUTF8Handler;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.Jvm;

/**
 * FileConnectionImpl implements all the methods of
 * javax.microedition.io.file.FileConnection;
 */
public final class FileConnectionImpl implements FileConnection
{
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javafile");
        }
        catch (Exception e)
        {
            FileLogger.ELog("Unable to load javafile native library");
        }
    }

    private FileUtility iFileUtility;

    private Finalizer iFinalizer;

    private boolean iIsOpen;

    private int iMode;

    private String iInitialUrl;

    private String FILE_LOCALHOST = "//localhost/";

    private String FILE_SIMPLE = "///";

    private FileConnectionURL iFileConnectionUrl;

    public FileConnectionImpl(String aName, int aMode, boolean aTimeouts)
    throws IOException
    {
        iInitialUrl = "file:" + aName;

        String decodedUrl = FileUTF8Handler.decode(iInitialUrl);
        decodedUrl = decodedUrl.replace('|', ':');

        // Validate File URL first.
        FileConnectionURL.checkFileUrlValidity(decodedUrl);
        String actual = FileUrlValidator.validateFileUrl(decodedUrl);

        if (iInitialUrl.startsWith("file:" + FILE_SIMPLE))
        {
            // Construct a FileConnectionURL type if the validation passes.
            iFileConnectionUrl = new FileConnectionURL(iInitialUrl, "file:///"
                    + actual, false);
        }
        else
        {
            // Construct a FileConnectionURL type if the validation passes.
            iFileConnectionUrl = new FileConnectionURL(iInitialUrl,
                    "file://localhost/" + actual, false);
        }

        // Check access.
        checkAccess(actual);

        // Go ahead to prompt the user
        checkSecurityPermission(actual, aMode);

        FileLogger.Log("FileConnectionImpl: Creating FileUtility With: "
                       + actual);

        // FileUtility can handle multiple "//" in the path.
        // Need to check it ourselves.
        if (actual.indexOf("//") != -1)
        {
            throw new IllegalArgumentException("Invalid Uri :" + aName);
        }
        iFileUtility = new FileUtility(actual);
        iFinalizer = registerFinalize();
        iIsOpen = true;
        iMode = aMode;

        FileLogger.Log("FileConnectionImpl: Connector.open succeeded");
    }

    /**
     * Registers with Finalizer to call a method when the object gets collected
     * by GC
     *
     * @return Finalizer object that will be notified when GC happens
     */
    private Finalizer registerFinalize()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    /**
     * Checks for already granted permissions and prompts the user if access is
     * allowed to the particular path. Prompting is delegated to
     * ApplicationUtils
     *
     * @param aAbsolutePath
     *            target to which the connection is currently open
     * @param aMode
     *            can either be FileAccessHelper.INTENT_READ or
     *            FileAccessHelper.INTENT_WRITE
     */
    private static void checkSecurityPermission(String aAbsolutePath, int aMode)
    {
        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        String accessIntent = (aMode == Connector.READ) ? FileAccessHelper.INTENT_READ
                              : FileAccessHelper.INTENT_WRITE;

        FileLogger.Log("FileConnectionImpl: Check Permission to "
                       + aAbsolutePath + " in " + accessIntent + " mode");

        // Go ahead to prompt the user
        if (Connector.READ_WRITE == aMode)
        {
            FileInternalPermission permission = new FileInternalPermission(
                aAbsolutePath, FileAccessHelper.INTENT_READ);
            appUtils.checkPermission(permission);

            FileInternalPermission permission1 = new FileInternalPermission(
                aAbsolutePath, FileAccessHelper.INTENT_WRITE);
            appUtils.checkPermission(permission1);
        }
        else
        {
            FileInternalPermission permission = new FileInternalPermission(
                aAbsolutePath, accessIntent);
            appUtils.checkPermission(permission);
        }
    }


    /**
     * Checks access to a specified path given its absolute path and the mode in
     * which the connection has been opened.
     *
     * @param aAbsolutePath
     *            path of the connection
     * @throws SecurityException
     *             in case access was allowed for the application for the given
     *             intent and target.
     */
    private static void checkAccess(String aAbsolutePath)
    {
        FileLogger.Log("FileConnectionImpl: Check Access to " + aAbsolutePath );
                       
        if (!FileAccessHelper.accessAllowed(aAbsolutePath,FileSystemUtils.getProtectionDomain()))
        {
            // Access to the specified path not allowed.
            // Throw Security Exception
            throw new SecurityException("Permission denied: " + aAbsolutePath);
        }
    }

    private synchronized void doFinalize()
    {
        iFileUtility = null;
    }

    public void close()
    {
        iIsOpen = false;
        iFileUtility = null;
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public final void create() throws IOException
    {
        checkConnection();
        checkConnectionMode(Connector.READ);

        String domain = FileSystemUtils.getProtectionDomain();

        if (!iFileUtility.createNewFile())
        {
            FileLogger.WLog("FileConnectionImpl: File Creation failed. "
                            + "Throwing IOException");
            throw new IOException("Unable to create file: "
                                  + iFileUtility.getAbsolutePath());
        }
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public final void rename(String aName) throws IOException
    {
        FileLogger.Log("FileConnectionImpl: rename(): " + aName);

        checkConnection();
        checkConnectionMode(Connector.READ);
        checkAccess(iFileUtility.getAbsolutePath());

        if (!iFileUtility.rename(aName))
        {
            FileLogger
            .ELog("FileConnectionImpl: File rename failed. Throwing IOException");
            throw new IOException("Unable to rename file or directory: "
                                  + iFileUtility.getAbsolutePath());
        }

        String curUrl = iFileConnectionUrl.getUrl();
        if (curUrl.endsWith("/"))
        {
            // Remove trailing "/"
            curUrl = curUrl.substring(0, curUrl.length() - 1);
        }
        String newUrl = curUrl.substring(0, curUrl.lastIndexOf('/') + 1);
        newUrl += FileUTF8Handler.encode(iFileUtility.getName());

        iFileConnectionUrl = new FileConnectionURL(newUrl);
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public final void mkdir() throws IOException
    {
        checkConnection();
        checkConnectionMode(Connector.READ);
        checkAccess(iFileUtility.getAbsolutePath());

        FileLogger.Log("FileConnectionImpl: mkdir(): "
                       + iFileUtility.getAbsolutePath());
        if (!iFileUtility.mkdir())
        {
            FileLogger.WLog("  FileConnectionImpl: "
                            + "FileUtility was unable to create directory.");
            throw new IOException("Unable to create directory: "
                                  + iFileUtility.getAbsolutePath());
        }
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public final void delete() throws IOException
    {
        checkConnection();
        checkConnectionMode(Connector.READ);
        checkAccess(iFileUtility.getAbsolutePath());

        FileLogger.Log("FileConnectionImpl: delete(): "
                       + iFileUtility.getAbsolutePath());
        try
        {
            if (!iFileUtility.delete())
            {
                FileLogger.WLog("FileConnectionImpl: Unable to delete file. "
                                + "Throwing IOException.");
                throw new IOException("Unable to delete file or directory: "
                                      + iFileUtility.getAbsolutePath());
            }
        }
        catch (SecurityException ex)
        {
            throw new IOException("Unable to delete file or directory: "
                                  + iFileUtility.getAbsolutePath());
        }
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public boolean exists()
    {
        checkConnection();
        checkConnectionMode(Connector.WRITE);
        return iFileUtility.exists();
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public String getName()
    {
        String name = iFileConnectionUrl.getName();
        if (name.equals(""))
        {
            return "";
        }

        if (iFileUtility.isDirectory())
        {
            name += "/";
        }
        else if (!iFileUtility.exists())
        {
            // If it does not exist, and of the initial url ended with "/"
            // we must return with a trailing "/"
            if (iInitialUrl.endsWith("/"))
            {
                name += "/";
            }
        }

        return name;
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public String getPath()
    {
        String path = iFileConnectionUrl.getPath();
        return path;
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public String getURL()
    {
        String ret = iFileConnectionUrl.getUrl();
        if (iFileUtility.isDirectory())
        {
            if (!ret.endsWith("/"))
            {
                ret += "/";
            }
        }
        else
        {
            if (iFileUtility.isFile())
            {
                if (ret.endsWith("/"))
                {
                    ret = ret.substring(0, ret.length() - 1);
                }
            }
        }
        return ret;
    }

    /**
     * Please refer JSR 75 for more details.
     *
     */
    public boolean isOpen()
    {
        return iIsOpen;
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public boolean canRead()
    {
        // Access Check
        checkConnection();
        checkConnectionMode(Connector.WRITE);
        return iFileUtility.canRead();
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public boolean canWrite()
    {
        // Access Check
        checkConnection();
        checkConnectionMode(Connector.WRITE);

        if (Connector.READ == iMode)
        {
            return false;
        }
        return iFileUtility.canWrite();
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public long lastModified()
    {
        checkConnection();
        checkConnectionMode(Connector.WRITE);
        return iFileUtility.lastModified();
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public boolean isDirectory()
    {
        checkConnection();
        checkConnectionMode(Connector.WRITE);
        return iFileUtility.isDirectory();
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public void setReadable(boolean aReadable) throws IOException
    {
        checkConnection();
        checkAccess(iFileUtility.getAbsolutePath());
        checkConnectionMode(Connector.READ);
        iFileUtility.setReadable(aReadable);
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public void setWritable(boolean aWritable) throws IOException
    {
        checkConnection();
        checkAccess(iFileUtility.getAbsolutePath());
        checkConnectionMode(Connector.READ);
        iFileUtility.setWritable(aWritable);
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public void setHidden(boolean aHidden) throws IOException
    {
        checkConnection();
        checkAccess(iFileUtility.getAbsolutePath());
        checkConnectionMode(Connector.READ);
        iFileUtility.setHidden(aHidden);
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public Enumeration list() throws IOException
    {
        return list("*", false);
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public Enumeration list(String aFilter, boolean aIncludeHidden)
    throws IOException
    {
        FileLogger.Log("+FileConnectionImpl: list()");
        checkConnection();
        checkConnectionMode(Connector.WRITE);

        String filter = aFilter;
        filter = trimFilter(filter);
        checkFilter(filter);

        // Ask FileUtility to give a list of files depending on its hidden
        // status.
        String[] fileList = iFileUtility.listFileArray(aIncludeHidden);
        Vector accessibleFileList = new Vector();
        String parent = iFileUtility.getAbsolutePath();

        if (!parent.endsWith("/"))
        {
            parent += "/";
        }

        String domain = FileSystemUtils.getProtectionDomain();

        // Check if read access is allowed to all contents in the list.
        boolean accessAllowed = true;

        for (int index = 0; index < fileList.length; index++)
        {
            String fullPath = parent + fileList[index];

            if (FileUtility.matchString(filter.toLowerCase(), fileList[index]))
            {
                // If it passes the filter, check if midlet has access.
                accessAllowed = FileAccessHelper.accessAllowed(fullPath,
                                     domain);

                if (accessAllowed)
                {
                    accessibleFileList.addElement(fileList[index]);
                }
            }
        }
        return accessibleFileList.elements();
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public void truncate(long aByteOffset) throws IOException
    {
        checkConnection();
        checkConnectionMode(Connector.READ);
        checkAccess(iFileUtility.getAbsolutePath());
        iFileUtility.truncate(aByteOffset);
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public final void setFileConnection(String aName) throws IOException
    {
        int result = 0;
        checkConnection();

        if (aName == null)
        {
            FileLogger.ELog("FileConnectionImpl: setFileConnection(null)");
            throw new NullPointerException("Argument passed was null");
        }

        aName = aName.trim();
        checkSetFileConnParam(aName);

        if (aName.equals("."))
        {
            FileLogger.Log("FileConnectionImpl: Set FileConnection to current");
            // already in current directory
            return;
        }

        if (!iFileUtility.isDirectory())
        {
            if (iFileUtility.exists())
            {
                throw new IOException("Connection is not a directory: "
                                      + iFileUtility.getAbsolutePath());
            }
        }

        // Case where we move up a directory
        if (aName.equals("..") || aName.equals("../"))
        {
            FileLogger.Log("FileConnectionImpl: Set FileConnection to parent");
            FileLogger.Log("     Current target: "
                           + iFileUtility.getAbsolutePath());
            FileLogger.Log("     Next target   : " + iFileUtility.getPath());

            String parent = iFileUtility.getPath();

            FileUtility tempTarget;
            try
            {
                // If no exception is thrown, its safe to set connection
                tempTarget = new FileUtility(parent);
            }
            catch (IllegalArgumentException ex)
            {
                FileLogger.WLog("FileConnectionImpl: setFileConnection: "
                                + "Unable to create new FileUtility object. "
                                + "Cannot set fileconnection to new target.");
                throw new IOException(
                    "File name is invalid or not accessible: " + aName);
            }
            catch (Exception e)
            {
                throw new IOException(
                    "File name is invalid or not accessible: " + aName);
            }

            // See if access is allowed. True because setting to new target is
            // as good as opening.
            checkAccess(tempTarget.getAbsolutePath());
            checkSecurityPermission(tempTarget.getAbsolutePath(), iMode);

            // if we reach here, it is fine :-)
            iFileUtility = tempTarget;

            String prevUrl = iFileConnectionUrl.getUrl();
            if (prevUrl.endsWith("/"))
            {
                prevUrl = prevUrl.substring(0, prevUrl.length() - 1);
            }

            String newUrl = prevUrl.substring(0, prevUrl.lastIndexOf('/'));
            FileLogger.Log("FileConnectionImpl: setFileConnection: "
                           + "Creating new FileConnectionURL With: " + newUrl);
            iFileConnectionUrl = new FileConnectionURL(newUrl, false);
            return;
        }

        if (!iFileUtility.exists())
        {
            throw new IOException("Current directory does not exist: "
                                  + iFileUtility.getAbsolutePath());
        }

        FileLogger.Log("FileConnectionImpl: setFileConnection(): "
                       + " Setting FileConnectionUrl to: " + aName);

        String newTarget = iFileUtility.getAbsolutePath() + aName;

        FileLogger.Log("FileConnectionImpl: setFileConnection(): "
                       + " Creating FileUtility with " + newTarget);

        FileUtility tempTarget = new FileUtility(newTarget);

        if (!tempTarget.exists())
        {
            FileLogger.WLog("FileConnectionImpl: setFileConnection(): "
                            + " New Target does not exist: "
                            + tempTarget.getAbsolutePath());

            throw new IllegalArgumentException(
                "File name is invalid or not accessible: " + aName);
        }

        // See if access is allowed. True because setting to new target is
        // as good as opening.
        checkAccess(tempTarget.getAbsolutePath());
        checkSecurityPermission(tempTarget.getAbsolutePath(), iMode);

        // If no exception is thrown, its safe to set connection
        iFileUtility = tempTarget;

        String prevUrl = iFileConnectionUrl.getUrl();
        if (!prevUrl.endsWith("/"))
        {
            prevUrl += "/";
        }
        String newUrl = prevUrl + iFileUtility.getName();

        FileLogger.Log("FileConnectionImpl: setFileConnection: "
                       + "Creating new FileConnectionURL With: " + newUrl);

        iFileConnectionUrl = new FileConnectionURL(newUrl, false);
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public boolean isHidden()
    {
        checkConnection();
        checkConnectionMode(Connector.WRITE);
        return iFileUtility.isHidden();
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public long availableSize()
    {
        checkConnection();
        checkConnectionMode(Connector.WRITE);
        return iFileUtility.availableSize();
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public long usedSize()
    {
        checkConnection();
        checkConnectionMode(Connector.WRITE);
        return iFileUtility.usedSize();
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public long totalSize()
    {
        checkConnection();
        checkConnectionMode(Connector.WRITE);
        return iFileUtility.totalSize();
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public long fileSize() throws IOException
    {
        checkConnection();
        checkConnectionMode(Connector.WRITE);
        return iFileUtility.fileSize();
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public long directorySize(boolean aIncludeSubDirs) throws IOException
    {
        checkConnection();
        checkConnectionMode(Connector.WRITE);
        return iFileUtility.directorySize(aIncludeSubDirs);
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public InputStream openInputStream() throws IOException
    {
        checkConnectionMode(Connector.WRITE);
        if (!isOpen())
        {
            throw new IOException();
        }

        checkSecurityPermission(iFileUtility.getAbsolutePath(), Connector.READ);
        try
        {
            return iFileUtility.openInputStream();
        }
        catch (SecurityException ex)
        {
            FileLogger.ELog("FileConnectionImpl: " + "openInputStream(): "
                            + " SecurityException Caught from FileUtility. "
                            + "Throwing IOException.");

            throw new IOException("Unable to open InputStream: "
                                  + iFileUtility.getAbsolutePath());
        }
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public DataInputStream openDataInputStream() throws IOException
    {
        checkConnectionMode(Connector.WRITE);

        if (!isOpen())
        {
            throw new IOException();
        }

        checkSecurityPermission(iFileUtility.getAbsolutePath(), Connector.READ);
        try
        {
            return iFileUtility.openDataInputStream();
        }
        catch (SecurityException ex)
        {
            FileLogger.ELog("FileConnectionImpl: " + "openDataInputStream(): "
                            + " SecurityException Caught from FileUtility. "
                            + "Throwing IOException.");

            throw new IOException("Unable to open DataInputStream: "
                                  + iFileUtility.getAbsolutePath());
        }
    }

    /**
     * Opens an Output Stream. If there an already open output stream,
     * IOException is thrown. Open succeeds only in case the application has
     * access to open the file in write mode.
     */
    public OutputStream openOutputStream() throws IOException
    {
        checkConnectionMode(Connector.READ);
        if (!isOpen())
        {
            throw new IOException();
        }

        checkSecurityPermission(iFileUtility.getAbsolutePath(), Connector.WRITE);
        try
        {
            return iFileUtility.openOutputStream();
        }
        catch (SecurityException ex)
        {
            FileLogger.ELog("FileConnectionImpl: " + "openOutputStream(): "
                            + " SecurityException Caught from FileUtility. "
                            + "Throwing IOException.");

            throw new IOException("Unable to open OutputStream: "
                                  + iFileUtility.getAbsolutePath());
        }
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public DataOutputStream openDataOutputStream() throws IOException
    {
        checkConnectionMode(Connector.READ);

        if (!isOpen())
            throw new IOException();

        checkSecurityPermission(iFileUtility.getAbsolutePath(), Connector.WRITE);
        try
        {
            return iFileUtility.openDataOutputStream();
        }
        catch (SecurityException ex)
        {
            FileLogger.ELog("FileConnectionImpl: " + "openDataOutputStream(): "
                            + " SecurityException Caught from FileUtility."
                            + " Throwing IOException.");

            throw new IOException("Unable to open DataOutputStream: "
                                  + iFileUtility.getAbsolutePath());
        }
    }

    /**
     * Please refer JSR 75 for more details.
     */
    public OutputStream openOutputStream(long aOffset) throws IOException
    {
        checkConnectionMode(Connector.READ);
        if (!isOpen())
            throw new IOException();

        if (aOffset < 0)
        {
            throw new IllegalArgumentException();
        }

        checkSecurityPermission(iFileUtility.getAbsolutePath(), Connector.WRITE);
        try
        {
            checkSecurityPermission(iFileUtility.getAbsolutePath(), iMode);
            return iFileUtility.openOutputStream(aOffset);
        }
        catch (SecurityException ex)
        {
            FileLogger.ELog("FileConnectionImpl: "
                            + "openDataOutputStream(offset): "
                            + " SecurityException Caught from FileUtility."
                            + " Throwing IOException.");

            throw new IOException("Unable to open OutputStream: "
                                  + iFileUtility.getAbsolutePath());
        }
    }

    /** * ----------------------------- HELPERS ----------------------------- */

    /**
     * If the filter contains escaped characters, it unescapes them.
     *
     * @param aFilter
     *            filter string passed to list method
     * @return new string with unescaped characters
     */
    private static String trimFilter(String aFilter)
    {
        String filter;
        // If the name contains escaped sequences, remove them: %20
        filter = FileUTF8Handler.decode(aFilter);
        filter = filter.trim();
        return filter;
    }

    /**
     * Checks to see if the filter given by the user is valid. Checks against
     * special charaters that must not be in the filter string.
     *
     * @param aFilter
     *            parameter passed to list method.
     * @throws IllegalArgumentException
     *             in case it contains a character that is not allowed to be in
     *             a filer.
     */
    private static void checkFilter(String aFilter)
    {
        if ((aFilter.indexOf('?') != -1) || (aFilter.indexOf(":") != -1)
                || (aFilter.indexOf("/") != -1)
                || (aFilter.indexOf("\\") != -1)
                || (aFilter.indexOf("//") != -1)
                || (aFilter.indexOf("|") != -1) || (aFilter.indexOf("<") != -1)
                || (aFilter.indexOf(">") != -1))
        {
            throw new IllegalArgumentException();
        }
    }

    /**
     * Checks to see if the connection is open.
     *
     */
    private void checkConnection()
    {
        if (!iIsOpen)
            throw new ConnectionClosedException("Connection has been closed");
    }

    /**
     * Checks to see if the mode given aMode matches with the mode of the
     * Connection.
     *
     * @param aMode
     *            mode in which the connection must not be opened.
     * @throws IllegalModeException
     *             in case the mode is invalid
     */
    private final void checkConnectionMode(int aMode)
    throws IllegalModeException
    {
        if (iMode == aMode)
        {
            throw new IllegalModeException(
                "Illegal mode for this operation call");
        }
    }

    /**
     * Checks for sanity of the name passed to setFileConnection method. It must
     * not contain characters that are not allowed in file names. Also, it must
     * not have any path separators in it. Will throw IllegalArgumentException
     * if the name was found to be improper.
     *
     * @param aName
     *            name of file/directory to which the target needs to be renamed
     * @throws IOException
     *             in case it contains any of invalid characters.
     */
    private void checkSetFileConnParam(String aName) throws IOException
    {
        String toCheck = aName;
        if (toCheck.endsWith("/"))
        {
            toCheck = toCheck.substring(0, toCheck.length() - 1);
        }

        if (toCheck.indexOf('?') != -1 || (toCheck.indexOf('<') != -1)
                || (toCheck.indexOf('<') != -1) || (toCheck.indexOf(':') != -1)
                || (toCheck.indexOf('|') != -1))
        {
            throw new IOException("Invalid file or directory name");
        }

        if ((toCheck.indexOf('*') != -1) || (toCheck.indexOf("/") != -1)
                || (toCheck.indexOf("\\") != -1))
        {
            throw new IllegalArgumentException(
                "Argument passed has path specification");
        }
    }
}
