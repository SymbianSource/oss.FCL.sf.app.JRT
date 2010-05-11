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

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.IllegalArgumentException;
import java.lang.SecurityException;
import java.util.Enumeration;
import java.util.Random;
import java.util.Vector;

import javax.microedition.io.Connector;

import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.utils.Tokenizer;

/**
 * FileUtility provides File handling APIs for internal use in OMJ.
 *
 * <p>
 * <strong>Creating FileUtility</strong>
 * <p>
 * The pathname used to create the FileUtility object must always be abolute. An
 * absolute pathname is complete in that no other information is required in
 * order to locate the file that it denotes. A relative pathname, in contrast,
 * must be interpreted in terms of information taken from some other pathname.
 * Relative paths cannot be passed to the constructor of FileUtility.
 * FileUtility.getCanonicalPath() can be used to resolve relative paths to
 * absolute path.
 * <p>
 * File URL can also be used to create FileUtility object. The format of the
 * input string used to access a FileUtility must follow the format of a
 * fully-qualified, absolute path file name as described by the file URL format
 * in IETF RFCs 1738 & 2396.
 *
 * <p>
 * <strong>FileUtility Behavior</strong>
 * <p>
 * FileUtility can be created immeterial of whether the intended iTarget exists
 * or not. This behavior allows the creation of new files and directories on a
 * file system. For example, the following code snippet can be used to create a
 * file on the file system.
 *
 * <pre>
 * try
 * {
 *     FileUtility fileTarget = new FileUtility(&quot;/home/user1/newFile.txt&quot;);
 *     // If no exception is thrown, URI is valid, but file may or may not exist
 *
 *     if (!fileTarget.exists())
 *     {
 *         fileTarget.createNewFile();
 *     }
 *
 *     fileTarget = null;
 * }
 * catch (IOException ex)
 * {
 * }
 * </pre>
 *
 * Developers should always check for the file's or directory's existence after
 * a construction to determine if the file or directory actually exists.
 * Similarly, files or directories can be deleted using the delete() method.
 *
 * <p>
 * <strong>Streams in FileUtility</strong>
 * <p>
 * Input and output streams may be opened and closed multiple times on a
 * FileUtility instance.<br/>
 *
 * All FileUtility instances have one underlying InputStream and one
 * OutputStream. Opening a DataInputStream counts as opening an InputStream, and
 * opening a DataOutputStream counts as opening an OutputStream. A FileUtility
 * instance can have only one InputStream and one OutputStream open at any one
 * time. Trying to open more than one InputStream or more than one OutputStream
 * from a StreamConnection causes an IOException. Further detials can be found
 * in Common Streams.
 * <p>
 * Instances of the FileUtility class are immutable; that is, once created, the
 * pathname represented by a FileUtility object will never change.
 *
 */
public final class FileUtility
{
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javafileutils");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaFile, "Unable to load javafileutils dll");
        }
    }

    // Contains the iTarget to which the instance of FileUtility is pointing to.
    private FileURL iTarget;

    private Finalizer iFinalizer;

    private FileStreamHandler iStreamHandler;

    private int iDrmIntent;

    private boolean iExecuteIntent;

    /**
     * File name passed here should be absolute path.
     *
     * @param aFilePath
     *            file/directory that FileUtility must work on.
     */
    public FileUtility(String aFilePath)
    {
        this(aFilePath, FileDRMContentHandler.Execute, false);
    }

    /**
     * Constructs a FileUtility with given parameters. This is to be used when
     * trying to access DRM protected content.
     *
     * @param aFilePath
     *            absolute path of the file/directory.
     * @param aDrmIntent
     *            intent with which the user wants to open the file. The DRM
     *            intent specified must be one of the values present in
     *            FileDRMContentHandler
     * @param aExecuteIntent
     *            specifies whether to execute the intent when an InputStream is
     *            opened on the file.
     * @see FileDRMContentHandler
     */
    public FileUtility(String aFilePath, int aDrmIntent, boolean aExecuteIntent)
    {
        // Create a new FileURL that needs to be used internally.
        iTarget = new FileURL(aFilePath);
        iFinalizer = registerFinalize();
        iStreamHandler = null;
        iDrmIntent = aDrmIntent;
        iExecuteIntent = aExecuteIntent;
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
     * Creates a new empty file in the specified directory, using the given
     * prefix and suffix strings to generate its name. If this method returns
     * successfully then it is guaranteed that:
     * <ol>
     * <li>The file denoted by the returned abstract pathname did not exist
     * before this method was invoked, and</li>
     * <li>Neither this method nor any of its variants will return the same
     * abstract pathname again in the current invocation of the virtual machine.
     * </li>
     * </ol>
     * <p>
     * The prefix argument must be at least three characters long. It is
     * recommended that the prefix be a short, meaningful string such as "hjb"
     * or "mail".<br/>
     * The suffix argument may be null, in which case the suffix ".tmp" will be
     * used. <br/>
     * The directory argument should not be null and must provide a directory to
     * which the file has to be created.
     * <p>
     * New file will be generated by concatenating the prefix, five or more
     * internally-generated characters, and the suffix.
     *
     * @param aPrefix
     *            The prefix string to be used in generating the file's name
     *            must be at least three characters long
     * @param aSuffix
     *            The suffix string to be used in generating the file's name may
     *            be null, in which case the suffix ".tmp" will be used
     * @param aDirectory
     *            The directory in which the file is to be created
     * @return A new FileUtility instance denoting the newly created file.
     * @throws IOException
     *             if the file could not be created or the directory does not
     *             exist
     * @throws IllegalArgumentException
     *             if the prefix is fewer than three characters or directory
     *             parameter is null
     * @throws SecurityException
     *             if access was not allowed to create a file in the directory
     *             specified
     */
    public static FileUtility createTempFile(String aPrefix, String aSuffix,
            FileUtility aDirectory) throws IOException
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                   "--> FileUtility.createTempFile() Prefix: " + aPrefix
                   + " Suffix: " + aSuffix);

        if ((aPrefix.length() < 3) || (null == aDirectory))
        {
            throw new IllegalArgumentException("Directory passed is null or "
                                               + " file prefix is invalid");
        }

        if (!aDirectory.exists())
        {
            Logger.WLOG(Logger.EJavaFile, "    FileUtility.createTempFile(): "
                        + "Directory specified does not exist.");
            throw new IOException(FileUtilityErrorStrings.TEMP_DIR_NOT_PRESENT
                                  + ": " + aDirectory);
        }

        if (!aDirectory.isDirectory())
        {
            Logger.WLOG(Logger.EJavaFile, "    FileUtility.createTempFile(): "
                        + "Argument passed is a file.");
            throw new IOException(FileUtilityErrorStrings.TARGET_POINTS_TO_FILE
                                  + ": " + aDirectory);
        }

        if (null == aSuffix)
        {
            aSuffix = ".tmp";
        }

        Random generator = new Random();
        int generated = 0;
        String midString, file;

        String parent = aDirectory.getPath() + aDirectory.getName();
        char separator = '/';

        if (parent.indexOf('\\') != -1)
        {
            separator = '\\';
        }

        if (!parent.endsWith("" + separator))
        {
            parent += separator;
        }

        do
        {
            // The file name generated should be unique in the folder.
            generated = generator.nextInt();
            midString = "" + generated;
            midString = midString.substring(midString.length() - 5);
            file = aPrefix + midString + aSuffix;
        }
        while (_exists(parent + file));

        Logger.LOG(Logger.EJavaFile, Logger.EJavaFile,
                   "    FileUtility.createTempFile(): Attempting to create "
                   + parent + file);

        // Create the temporary file
        FileUtility newTempFile = new FileUtility(parent + file);

        if (!newTempFile.createNewFile())
        {
            Logger.WLOG(Logger.EJavaFile,
                        "    FileUtility.createTempFile(): Create failed.");
            // File could not be created.
            throw new IOException(FileUtilityErrorStrings.UNABLE_TO_CREATE_FILE
                                  + ": " + newTempFile);
        }

        Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                   "<-- FileUtility.createTempFile()");
        return newTempFile;
    }

    /**
     * Resolves incomplete path by prefixing the path with one of the roots of
     * the file system.
     * <p>
     *
     * <pre>
     * Example of S60:
     *     String resolved = FileUtility.resolve(&quot;\\data\\images\\existingFile&quot;);
     *     // resolved will be C:\data\images\existingFile
     * </pre>
     *
     * @param aPath
     *            incomplete path that does not contain drive information.
     * @return resolved path or null if not found
     */
    public static String resolveDrive(String aPath)
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                   "--> FileUtility.resolveDrive() Path: " + aPath);

        String retVal = null;
        String path = aPath;
        try
        {
            retVal = FilePlatformSpecifics.resolveDrive(path);
        }
        catch (Exception e)
        {
            retVal = null;
        }

        Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                   "<-- FileUtility.resolveDrive() returns: " + retVal);
        return retVal;
    }

    /**
     * A canonical pathname is both absolute and unique. The precise definition
     * of canonical form is system-dependent. This method first converts this
     * pathname to absolute form if necessary, and then maps it to its unique
     * form in a system-dependent way. This typically involves removing
     * redundant names such as "." and ".." from the pathname, resolving
     * symbolic links (on UNIX platforms), and converting drive letters to a
     * standard case (on Microsoft Windows platforms).
     * <p>
     * Every pathname that denotes an existing file or directory has a unique
     * canonical form. Every pathname that denotes a nonexistent file or
     * directory also has a unique canonical form.
     *
     * @param aPath
     * @return The canonical pathname string denoting the same file or
     *         directory.
     * @throws IOException
     *             If an I/O error occurs, which is possible because the
     *             construction of the canonical pathname may require filesystem
     *             queries
     *
     */
    public static String getCanonicalPath(String aPath) throws IOException
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                   "--> FileUtility.getCanonicalPath() Path: " + aPath);

        char separator;
        String tmpPath = aPath;
        if (tmpPath.endsWith("/") || tmpPath.endsWith("\\"))
        {
            tmpPath = aPath.substring(0, aPath.length() - 1);
        }

        // Find separator and check to see if the same separator is used
        // everywhere.
        if (tmpPath.indexOf("/") != -1)
        {
            // Store the separator used.
            separator = '/';

            // In case any other kind of separator is used in the path
            if (tmpPath.indexOf("\\") != -1)
            {
                throw new IllegalArgumentException("Invalid path separator: "
                                                   + aPath);
            }
        }
        else if (tmpPath.indexOf("\\") != -1)
        {
            separator = '\\';
            if (tmpPath.indexOf("/") != -1)
            {
                throw new IllegalArgumentException("Invalid path separator: "
                                                   + aPath);
            }
        }
        else
        {
            // no separtor present in the input, return as it is
            return tmpPath;

        }

        // Split the path based on the separator used.
        String[] pathTokens = Tokenizer.split(tmpPath, "" + separator);

        String initialPath = pathTokens[0];
        if (pathTokens == null)
        {
            initialPath = "";
        }
        initialPath += separator;

        // Resolves in case path starts with "." or ".." or "/" or "\\"
        String path = _getRealPath(initialPath);

        int count = 1;
        String sub = null;
        while (count < pathTokens.length)
        {
            sub = pathTokens[count];
            count++;
            if (sub != null)
            {
                // null implies that path was sumething like //a, which must be
                // considered as /a. Hence we ignore and move on to the nex
                // token.

                if (sub.equals("."))
                {
                    // We need to stay in the current working directory.
                    continue;
                }
                else if (sub.equals(".."))
                {
                    // Substring the main one
                    if (path.indexOf(separator) == path.lastIndexOf(separator))
                    {
                        // We are already at the root, we cannot go further back
                        continue;
                    }

                    path = path.substring(0, path.lastIndexOf(separator, path
                                          .length() - 2));
                }
                else
                {
                    if (!path.endsWith("" + separator))
                    {
                        path += separator;
                    }

                    // Just append the thing. It is some string.
                    path += sub;
                }

                // Add the separator in the end.
                if (count != pathTokens.length)
                {
                    path += separator;
                }

                // In case there are any symbolic links, resolve them
                path = _getRealPath(path);
            }
        }

        // In case of symbolic paths, resolve it.
        String toReturn = _getRealPath(path);

        Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                   "<-- FileUtility.getCanonicalPath(): " + toReturn);

        return toReturn;
    }

    /**
     * Get file Content ID. Only DRM protected files have content Id.
     *
     * @param full
     *            path to file.
     * @return Content ID. Null if cannot be read or does not exists.
     */
    public static String getContentId(String aPath)
    {
        return FilePlatformSpecifics.getContentId(aPath);
    }

    /**
     * Checks to see if the Stream Handler instance is already present in the
     * object. If it is not present, a new instance is created.
     *
     */
    private void createStreamHandler()
    {
        if (null == iStreamHandler)
        {
            String name = iTarget.getFullPath();
            if (FileDRMContentHandler.isDRMProtectedFile(name))
            {
                Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                           "FileUtility.checkStreamHandler(): "
                           + "Choosing DRM content handler");
                iStreamHandler = new FileDRMContentHandler(name, iDrmIntent,
                        iExecuteIntent);
            }
            else
            {
                Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                           "FileUtility.checkStreamHandler(): "
                           + "Choosing normal stream handler");
                iStreamHandler = new FileStreamHandler(name);
            }
        }
    }

    /**
     * Open and return an input stream for a connection. The connection's
     * iTarget must already exist and be accessible for the input stream to be
     * created.
     *
     * @return An open input stream
     * @throws IOException
     *             if an I/O error occurs, if the method is invoked on a
     *             directory, if the connection's iTarget does not yet exist, or
     *             the connection's iTarget is not accessible.
     */
    public InputStream openInputStream() throws IOException
    {
        if (!exists())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + iTarget.getFullPath());
        }

        if (isDirectory())
        {
            throw new IOException(
                FileUtilityErrorStrings.TARGET_POINTS_TO_DIRECTORY + ": "
                + iTarget.getFullPath());
        }
        createStreamHandler();

        // Throws IOException if opening a file in the specified mode failed.
        iStreamHandler.openFileForReading();
        return iStreamHandler.openInputStream();
    }

    /**
     * Open and return a data input stream for a connection. The connection's
     * iTarget must already exist and be accessible for the input stream to be
     * created.
     *
     * @return An open input stream
     * @throws IOException
     *             If an I/O error occurs, if the method is invoked on a
     *             directory, if the connection's iTarget does not yet exist, or
     *             the connection's iTarget is not accessible.
     */
    public DataInputStream openDataInputStream() throws IOException
    {
        if (!exists())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + iTarget.getFullPath());
        }

        if (isDirectory())
        {
            throw new IOException(
                FileUtilityErrorStrings.TARGET_POINTS_TO_DIRECTORY + ": "
                + iTarget.getFullPath());
        }
        createStreamHandler();

        // Throws IOException if opening a file in the specified mode failed.
        iStreamHandler.openFileForReading();
        return iStreamHandler.openDataInputStream();
    }

    /**
     * Open and return an output stream for a connection. The output stream is
     * positioned at the start of the file. Writing data to the output stream
     * overwrites the contents of the files (i.e. does not insert data). Writing
     * data to output streams beyond the current end of file automatically
     * extends the file size. The connection's iTarget must already exist and be
     * accessible for the output stream to be created. openOutputStream(long)
     * should be used to position an output stream to a different position in
     * the file.
     * <p>
     * Changes made to a file through an output stream may not be immediately
     * made to the actual file residing on the file system because platform and
     * implementation specific use of caching and buffering of the data. Stream
     * contents and file length extensions are not necessarily visible outside
     * of the application immediately unless flush() is called on the stream.
     * The returned output stream is automatically and synchronously flushed
     * when it is closed.
     *
     * @return An open output stream
     * @throws IOException
     *             If an I/O error occurs, if the method is invoked on a
     *             directory, the file does not yet exist, or the connection's
     *             iTarget is not accessible.
     */
    public OutputStream openOutputStream() throws IOException
    {
        if (!exists())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + iTarget.getFullPath());
        }

        if (isDirectory())
        {
            throw new IOException(
                FileUtilityErrorStrings.TARGET_POINTS_TO_DIRECTORY + ": "
                + iTarget.getFullPath());
        }

        createStreamHandler();
        iStreamHandler.openFileForWriting(0);
        return iStreamHandler.openOutputStream();
    }

    /**
     * Open and return a data output stream for a connection. The output stream
     * is positioned at the start of the file. Writing data to the output stream
     * overwrites the contents of the files (i.e. does not insert data). Writing
     * data to output streams beyond the current end of file automatically
     * extends the file size. The connection's iTarget must already exist and be
     * accessible for the output stream to be created. openOutputStream(long)
     * should be used to position an output stream to a different position in
     * the file.
     * <p>
     * Changes made to a file through an output stream may not be immediately
     * made to the actual file residing on the file system because platform and
     * implementation specific use of caching and buffering of the data. Stream
     * contents and file length extensions are not necessarily visible outside
     * of the application immediately unless flush() is called on the stream.
     * The returned output stream is automatically and synchronously flushed
     * when it is closed.
     *
     * @return An open output stream
     * @throws IOException
     *             If an I/O error occurs, if the method is invoked on a
     *             directory, the file does not yet exist, or the connection's
     *             iTarget is not accessible.
     */
    public DataOutputStream openDataOutputStream() throws IOException
    {
        if (!exists())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + iTarget.getFullPath());
        }

        if (isDirectory())
        {
            throw new IOException(
                FileUtilityErrorStrings.TARGET_POINTS_TO_DIRECTORY + ": "
                + iTarget.getFullPath());
        }

        createStreamHandler();
        iStreamHandler.openFileForWriting(0);
        return iStreamHandler.openDataOutputStream();
    }

    /**
     * This method opens an output stream and positions it at the indicated byte
     * offset in the file. Data written to the returned output stream at that
     * position overwrites any existing data until EOF is reached, and then
     * additional data is appended. The connection's iTarget must already exist
     * and be accessible for the output stream to be created.
     * <p>
     * Changes made to a file through an output stream may not be immediately
     * made to the actual file residing on the file system because platform and
     * implementation specific use of caching and buffering of the data. Stream
     * contents and file length extensions are not necessarily visible outside
     * of the application immediately unless flush() is called on the stream.
     * The returned output stream is automatically and synchronously flushed
     * when it is closed.
     *
     * @param aByteOffset
     *            number of bytes to skip over from the beginning of the file
     *            when positioning the start of the OutputStream. If the
     *            provided offset is larger than or equal to the current file
     *            size, the OutputStream is positioned at the current end of the
     *            file for appending.
     * @return an open OutputStream positioned at the byte offset in the file,
     *         or the end of the file if the offset is greater than the size of
     *         the file.
     * @throws IOException
     *             If an I/O error occurs, if the method is invoked on a
     *             directory, the file does not yet exist, or the connection's
     *             iTarget is not accessible.
     */
    public OutputStream openOutputStream(long aByteOffset) throws IOException
    {
        if (aByteOffset < 0)
        {
            throw new IllegalArgumentException();
        }

        if (!exists())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + iTarget.getFullPath());
        }

        if (isDirectory())
        {
            throw new IOException(
                FileUtilityErrorStrings.TARGET_POINTS_TO_DIRECTORY + ": "
                + iTarget.getFullPath());
        }

        createStreamHandler();
        iStreamHandler.openFileForWriting(aByteOffset);
        return iStreamHandler.openOutputStream();
    }

    /**
     * Checks to see if current FileUtility is pointing to a File.
     *
     * @return true if the current iTarget is a file. false in case the iTarget
     *         is not a file or is not accessible.
     */
    public boolean isFile()
    {
        return _isFile(iTarget.getFullPath());
    }

    /**
     * Checks to see if the current FileUtility is pointing to a Directory.
     *
     * @return true if the current iTarget is a directory. false in case the
     *         iTarget is not a directory or is not accessbile.
     */
    public boolean isDirectory()
    {
        return _isDirectory(iTarget.getFullPath());
    }

    /**
     * Checks to see if the current FileUtility iTarget exists.
     *
     * @return true if the current iTarget exists. false in case the iTarget did
     *         not exist or there was a fault while reading or accessing the
     *         file.
     */
    public boolean exists()
    {
        return _exists(iTarget.getFullPath());
    }

    /**
     * Checks to see if the current FileUtility iTarget can be read from the
     * current application.
     *
     * @return true if read access is allowed to the file. false if read access
     *         is not allowed or if there was error reading the file.
     */
    public boolean canRead()
    {
        if (!exists())
        {
            return false;
        }

        return _canRead(iTarget.getFullPath());
    }

    /**
     * Checks to see if the current FileUtility iTarget can be written to from
     * the current application.
     *
     * @return true if write access is allowed to the file. false if write
     *         access is not allowed or if there was error reading the file.
     */
    public boolean canWrite()
    {
        if (!exists())
        {
            return false;
        }

        return _canWrite(iTarget.getFullPath());
    }

    /**
     * Tests whether the file named by pathname is a hidden file.
     * <p>
     * The exact definition of hidden is system-dependent. On UNIX systems, a
     * file is considered to be hidden if its name begins with a period
     * character ('.'). On Microsoft Windows systems, a file is considered to be
     * hidden if it has been marked as such in the filesystem.
     * <p>
     * NOTE: There is no method provided to hide a file/directory.
     *
     * @return true if the file exists, is accessible, and is hidden, otherwise
     *         false.
     *
     */
    public boolean isHidden(String aPath)
    {
        return _isHidden(aPath);
    }

    /**
     * Tests whether the file named by pathname is a hidden file.
     * <p>
     * The exact definition of hidden is system-dependent. On UNIX systems, a
     * file is considered to be hidden if its name begins with a period
     * character ('.'). On Microsoft Windows systems, a file is considered to be
     * hidden if it has been marked as such in the filesystem.
     * <p>
     * NOTE: There is no method provided to hide a file/directory.
     *
     * @return true if the file exists, is accessible, and is hidden, otherwise
     *         false.
     */
    public boolean isHidden()
    {
        if (!exists())
        {
            return false;
        }

        return _isHidden(iTarget.getFullPath());
    }

    /**
     * Sets the file or directory readable attribute to the indicated value. The
     * readable attribute for the file on the actual file system is set
     * immediately upon invocation of this method.
     *
     * @param aReadable
     *            The new state of the readable flag of the selected file.
     * @throws IOException
     *             if the connection's iTarget does not exist or is not
     *             accessible.
     */
    public void setReadable(boolean aReadable) throws IOException
    {
        if (!exists())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + iTarget.getFullPath());
        }

        if (canRead() == aReadable)
        {
            return;
        }

        _setReadable(iTarget.getFullPath(), aReadable);
    }

    /**
     * Sets the selected file or directory writable attribute to the indicated
     * value. The writable attribute for the file on the actual file system is
     * set immediately upon invocation of the method.
     *
     * @param aWritable
     *            The new state of the writable flag of the selected file.
     * @throws IOException
     *             if the connection's iTarget does not exist or is not
     *             accessible.
     */
    public void setWritable(boolean aWritable) throws IOException
    {
        if (!exists())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + iTarget.getFullPath());
        }

        if (canWrite() == aWritable)
        {
            return;
        }

        if (iStreamHandler != null)
        {
            iStreamHandler.closeInputStreams();
            // close temporarily
            iStreamHandler.closeFileStream();
        }

        _setWritable(iTarget.getFullPath(), aWritable);
    }

    /**
     * Sets the hidden attribute of the selected file to the value provided. The
     * attribute is applied to the file on the actual file system immediately
     * upon invocation of this method if the file system and platform support
     * it.
     * <p>
     * If the file system doesn't support a hidden attribute, this method is
     * ignored and isHidden() always returns false. Since the exact definition
     * of hidden is system-dependent, this method only works on file systems
     * that support a settable file attribute.
     * <p>
     * For example, on Win32 and FAT file systems, a file may be considered
     * hidden if it has been marked as such in the file's attributes; therefore
     * this method is applicable.<br/>
     * However on UNIX systems a file may be considered to be hidden if its name
     * begins with a period character ('.'). In the UNIX case, this method may
     * be ignored and the method to make a file hidden may be the rename()
     * method.
     *
     * @param aHidden
     *            The new state of the hidden flag of the selected file.
     * @throws IOException
     *             if the connection's iTarget does not exist or is not
     *             accessible.
     */
    public void setHidden(boolean aHidden) throws IOException
    {
        if (!exists())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + iTarget.getFullPath());
        }

        if (isHidden() == aHidden)
        {
            return;
        }

        // We need to close the streams in order to change the hidden status
        // else it will fail. In Linux, it might be so that setHidden might lead
        // to rename. So, we close the Files so that it can be renamed.
        // The offsets remain the same.
        if (iStreamHandler != null)
        {
            iStreamHandler.closeInputStreams();
            iStreamHandler.closeFileStream();
        }

        // SetHidden is extremely platform specific behaviour.
        FilePlatformSpecifics.setHidden(this, aHidden);
    }

    /**
     * Retrieves the size of the file.
     *
     * @return size of file in bytes or -1 if the file did not exist
     * @throws IOException
     *             in case there was any error retrieving the file size.
     */
    public long fileSize() throws IOException
    {
        if (!_exists(iTarget.getFullPath()))
        {
            return -1;
        }

        if (!isFile())
        {
            throw new IOException(
                FileUtilityErrorStrings.TARGET_POINTS_TO_DIRECTORY + ": "
                + iTarget.getFullPath());
        }

        return _fileSize(iTarget.getFullPath());
    }

    /**
     * Determines the size in bytes on a file system of all of the files that
     * are contained in a directory.
     *
     * @param aIncludeSubDirs
     *            specifies if dir size has to be taken recursively
     * @return The size in bytes occupied by the files included in the
     *         directory, \ or -1 if the directory does not exist or is not
     *         accessible.
     *
     * @throws IOException
     *             if the method is invoked on a file.
     *
     * @throws SecurityException
     *             if the security of the application does not have read access
     *             to the file.
     */
    public long directorySize(boolean aIncludeSubDirs) throws IOException
    {
        if (!_exists(iTarget.getFullPath()))
        {
            return -1;
        }

        if (!isDirectory())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_POINTS_TO_FILE
                                  + ": " + iTarget.getFullPath());
        }

        return _dirSize(iTarget.getFullPath(), aIncludeSubDirs);
    }

    /**
     * Determines the total size of the file system the iTarget resides on.
     *
     * @return The total size of the file system in bytes, or -1 if the file
     *         system is not accessible.
     */
    public long totalSize()
    {
        return _totalSize(iTarget.getFullPath());
    }

    /**
     * Determines the free memory that is available on the file system the file
     * or directory resides on. This may only be an estimate and may vary based
     * on platform-specific file system blocking and metadata information.
     *
     * @return The available size in bytes on a file system, or -1 if the file
     *         system is not accessible.
     */
    public long availableSize()
    {
        return _availableSize(iTarget.getFullPath());
    }

    /**
     * Determines the used memory of a file system the connection's iTarget
     * resides on. This may only be an estimate and may vary based on
     * platform-specific file system blocking and metadata information.
     *
     * @return The used size of bytes on a file system, or -1 if the file system
     *         is not accessible.
     */
    public long usedSize()
    {
        return _usedSize(iTarget.getFullPath());
    }

    /**
     * Returns the time that the FileUtility currently points to was last
     * modified.
     *
     * @return A long value representing the time the file was last modified,
     *         measured in milliseconds, 0L if an I/O error occurs. If
     *         modification date is not supported by the underlying platform
     *         and/or file system, then 0L is also returned. If the connection's
     *         iTarget does not exist or is not accessible, 0L is returned.
     */
    public long lastModified()
    {
        long value = _lastModified(iTarget.getFullPath());
        // Time got is not got in milliseconds, so multiplying with 1000.
        return value * 1000;
    }

    /**
     * Fetches the name of the iTarget currently pointed to by FileUtility. If
     * the iTarget is a file, the result is of the form <file.extention> else if
     * it is a directory it has a trailing "/" of the form <directory/>
     *
     * @return name of the iTarget currently pointed to.
     */
    public String getName()
    {
        String name = iTarget.getName();
        return name;
    }

    /**
     * Fetches the path to the iTarget currently pointed to by FileUtility.
     * Return value is of the form /<root>/<directory>/
     *
     * @return path to the iTarget currently pointed to.
     */
    public String getPath()
    {
        String name = iTarget.getPath();
        return name;
    }

    /**
     * Returns FileUtility object that points to the parent of the current
     * iTarget.
     *
     * @return The parent directory named by this FileUtility, or null if this
     *         pathname does not name a parent
     */
    public FileUtility getParentFile()
    {
        String name = iTarget.getPath();
        if (null == name)
            return null;

        return new FileUtility(name);
    }

    /**
     * Returns the absolute path traced from the root of the file system.
     *
     * @return the absolute path of the file
     */
    public String getAbsolutePath()
    {
        return iTarget.getFullPath();
    }

    /**
     * Returns the full file URL including the scheme, host, and path from where
     * the file or directory specified in the Connector.open() method is opened.
     * The string returned is in an escaped ASCII format as defined by RFC 2396.
     * The resulting String looks as follows:
     *
     * <pre>
     * file://&lt;host&gt;/&lt;root&gt;/&lt;directory&gt;/&lt;filename&gt;
     * </pre>
     *
     * @return The URL of a file or directory in the format specified above.
     */
    public String getURL()
    {
        return iTarget.getUrl();
    }

    /**
     * Creates a file corresponding to the file string provided constructor of
     * this FileUtility. The file is created immediately on the actual file
     * system upon invocation of this method. Files are created with zero length
     * and data can be put into the file through output streams opened on the
     * file. This method does not create any directories specified in the file's
     * path.
     *
     * @return true if the file did not exist and was successfully created;
     *         false if the file already existed.
     * @throws IOException
     *             in case an error occured; or if the URL/Path given during
     *             construction had a trailing "/"(or "\\") to indicate that it
     *             is a directory.
     * @throws SecurityException
     *             in case the access was denied to create the file
     */
    public boolean createNewFile() throws IOException, SecurityException
    {
        String toCreate = iTarget.getFullPath();

        // This is used to create files. If the iTarget specified is that of a
        // directory, it is not created.
        if (toCreate.endsWith(FileURL.S60_FILE_SEPARATOR)
                || toCreate.endsWith(FileURL.LINUX_FILE_SEPARATOR))
        {
            throw new IOException(
                FileUtilityErrorStrings.TARGET_POINTS_TO_DIRECTORY + ": "
                + iTarget.getFullPath());
        }

        // If any exception occured, the native call must throw IOException
        // In case it already existed, it will return false; true if successful.
        return _createNewFile(toCreate);
    }

    /**
     * Creates the directory named by this pathname. Directories in the
     * specified path are not recursively created and must be explicitly created
     * before sub directories can be created.
     *
     * @return true if and only if the directory was created; false if the
     *         directory already existed.
     * @throws IOException
     *             in case an error occurred; or if the URL/Path given during
     *             construction did not have trailing "/"(or "\\") to indicating
     *             that it is not supposed to be a directory.
     * @throws SecurityException
     *             in case the access was denied to create the directory
     */
    public boolean mkdir() throws IOException, SecurityException
    {
        String toCreate = iTarget.getFullPath();
        // OpenC does not recognise already existing directories
        // So, we do it ourselves.
        if (exists())
        {
            return false;
        }

        // This is used to create directories. If the iTarget specified is that
        // of a file, it is not created.
        if ((toCreate.endsWith(FileURL.S60_FILE_SEPARATOR) || toCreate
                .endsWith(FileURL.LINUX_FILE_SEPARATOR)))
        {
            // Must ignore the separator
            // Remove the trailing slash and pass it to native side.
            toCreate = toCreate.substring(0, toCreate.length() - 1);
        }

        return _createNewDirectory(toCreate);
    }

    /**
     * Creates the directory named by this pathname, including any necessary but
     * nonexistent parent directories. Note that if this operation fails it may
     * have succeeded in creating some of the necessary parent directories.
     *
     * @return true if and only if the directory was created, along with all
     *         necessary parent directories; false otherwise
     * @throws IOException
     *             in case an error occurred
     * @throws SecurityException
     */
    public boolean mkdirs() throws IOException, SecurityException
    {

        String fullPath = iTarget.getFullPath();

        String separator = "/";
        if (fullPath.indexOf("\\") != -1)
        {
            separator = "\\";
        }

        String[] pathTokens = Tokenizer.split(fullPath, separator);
        String progressivePath = "";

        for (int i = 1; i < pathTokens.length - 1; i++)
        {
            if (pathTokens[i].equals(""))
            {
                throw new IllegalArgumentException();

            }
            // Check rename parameters checks for validity of the folders
            // within the path
            checkRenameParameters(pathTokens[i]);
        }

        for (int dirs = 0; dirs < pathTokens.length; dirs++)
        {
            if (pathTokens[dirs].equals(""))
            {
                progressivePath += separator;
                continue;
            }
            progressivePath += pathTokens[dirs];

            if (!_exists(progressivePath))
            {
                // Create the directory.
                if (!_createNewDirectory(progressivePath))
                {
                    // Could not create a directory in the path
                    return false;
                }
            }
            progressivePath += separator;
        }
        return true;
    }

    /**
     * Deletes the file or directory denoted by this pathname. If this pathname
     * denotes a directory, then the directory must be empty in order to be
     * deleted.
     * <p>
     * All open input and output streams are automatically flushed and closed.
     * Attempts to further use those streams result in an IOException. The
     * FileUtility instance object remains available for use.
     *
     * @return true if and only if the file or directory is successfully
     *         deleted; false otherwise
     * @throws SecurityException
     *             if access was denied to the file/directory
     */
    public boolean delete() throws SecurityException
    {
        // Close input and output Streams if created.
        // Found in StreamConnectionBase
        if (!canWrite())
        {
            return false;
        }

        if (iStreamHandler != null)
        {
            iStreamHandler.closeInputStreams();
            iStreamHandler.closeOutputStreams();
        }

        return _delete(iTarget.getFullPath());
    }
    
    /**
     * Deletes the file or directory denoted by this pathname. If this pathname
     * denotes a directory, then the directory must be empty in order to be
     * deleted. Deletes the file or directory even if it is marked as read-only.
     * <p>
     * All open input and output streams are automatically flushed and closed.
     * Attempts to further use those streams result in an IOException. The
     * FileUtility instance object remains available for use.
     *
     * @return true if and only if the file or directory is successfully
     *         deleted; false otherwise
     * @throws SecurityException
     *             if access was denied to the file/directory
     */    
    public boolean forceDelete() throws SecurityException
    {
      
        // Close input and output Streams if created.
        // Found in StreamConnectionBase

        if (iStreamHandler != null)
        {
            iStreamHandler.closeInputStreams();
            iStreamHandler.closeOutputStreams();
        }

        return _delete(iTarget.getFullPath());
    }

    /**
     * Renames the selected file or directory to a new name in the same
     * directory. The file or directory is renamed immediately on the actual
     * file system upon invocation of this method. No file or directory by the
     * original name exists after this method call.
     * <p>
     * All previously open input and output streams are automatically flushed
     * and closed. Attempts to further use those streams result in an
     * IOException. The FileUtility instance object remains open and available
     * for use, referring now to the file or directory by its new name.
     *
     * @param aNewName
     *            new name to which the current iTarget has to be renamed
     * @return true if rename succeeded, false otherwise
     * @throws IOException
     *             if the connection's iTarget does not exist, the connection's
     *             iTarget is not accessible, a file or directory already exists
     *             by the newName, or newName is an invalid filename for the
     *             platform (e.g. contains characters invalid in a filename on
     *             the platform)
     * @throws SecurityException
     *             if access/permission was denied to rename file
     * @throws IllegalArgumentException
     *             if newName contains any path specification.
     */
    public boolean rename(String aNewName) throws IOException,
                SecurityException
    {

        String toRename = aNewName.trim();
        toRename = FileUTF8Handler.decode(toRename);

        if (toRename.endsWith("\n"))
        {
            toRename = toRename.substring(0, toRename.length() - 1);
        }
        checkRenameParameters(toRename);

        // Check if the cuurent iTarget exists
        if (!exists())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + iTarget.getFullPath());
        }

        // Checks if the toRename is intended for Directory or File.
        if ((toRename.endsWith(FileURL.S60_FILE_SEPARATOR) || toRename
                .endsWith(FileURL.LINUX_FILE_SEPARATOR)))
        {
            // Rename to a directory
            // If the existing iTarget is a file, we cant do it.
            if (isFile())
            {
                throw new IllegalArgumentException(
                    FileUtilityErrorStrings.TARGET_POINTS_TO_FILE + ": "
                    + iTarget.getFullPath());
            }
        }

        String renameTo = iTarget.getPath() + toRename;
        FileURL newUrl = new FileURL(renameTo);

        // OpenC renames even if there is another file/directory with
        // the same name as the toRename. So, we need to check it ourselves.
        if (_exists(renameTo))
        {
            throw new IOException();
        }

        if (iStreamHandler != null)
        {
            // Close all input and output streams if created now
            iStreamHandler.closeInputStreams();
            iStreamHandler.closeOutputStreams();
        }

        if (_renameFile(iTarget.getFullPath(), newUrl.getFullPath()))
        {
            // If rename succeeded, we will be pointing to a new iTarget.
            // Assign new URL to member.
            iTarget = newUrl;

            // Since rename succeeded, we dispose the native peer and create
            // create a new peer pointing to the new file.
            if (iStreamHandler != null)
            {
                iStreamHandler = new FileStreamHandler(iTarget.getFullPath());
            }
            return true;
        }
        return false;
    }

    /**
     * Renames the iTarget to new file. Similar to rename, but allows us to
     * rename files to different paths in the file system. Allows for faster
     * move within a file system.
     *
     * @param aNewName
     *            new path to which it has to be moved. Note that the newName
     *            should be absolute path.
     * @return true in case rename was successful, false otherwise
     * @throws IOException
     *             in case some unexpected error occured.
     */
    public boolean renameTo(String aNewName) throws IOException
    {
        String renameTo = aNewName.trim();
        if (renameTo.endsWith("\n"))
        {
            renameTo = renameTo.substring(0, renameTo.length() - 1);
        }
        // Check if the current iTarget exists
        if (!exists())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + iTarget.getFullPath());
        }

        FileURL newUrl = new FileURL(renameTo);
        // // OpenC renames even if there is another file/directory with
        // // the same name as the toRename. So, we need to check it ourselves.
        // if (_exists(renameTo))
        // {
        // throw new IOException();
        // }

        String separator = iTarget.getSeparatorUsed();
        FileUtility temp = new FileUtility(aNewName);
        if (renameTo.indexOf(separator) == -1)
        {
            throw new IllegalArgumentException("Invalid parameter");
        }

        // Make sure paths are created
        FileUtility parentPath = temp.getParentFile();
        if (!parentPath.exists())
        {
            parentPath.mkdirs();
        }
        parentPath = null;

        if (iStreamHandler != null)
        {
            // Close all input and output streams if created now
            iStreamHandler.closeInputStreams();
            iStreamHandler.closeOutputStreams();
        }

        if (_renameFile(iTarget.getFullPath(), newUrl.getFullPath()))
        {
            // If rename succeeded, we will be pointing to a new iTarget.
            // Assign new URL to member.
            iTarget = newUrl;

            // Since rename succeeded, we dispose the native peer and create
            // create a new peer pointing to the new file.

            if (iStreamHandler != null)
            {
                iStreamHandler = new FileStreamHandler(iTarget.getFullPath());
            }
            return true;
        }
        return false;
    }

    public String[] listFileArray(boolean aIncludeHidden) throws IOException
    {
        if (!_exists(iTarget.getFullPath()))
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + iTarget.getFullPath());
        }

        if (isFile())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_POINTS_TO_FILE
                                  + ": " + iTarget.getFullPath());
        }

        // Simply lists all the contents of the file.
        String fileListString = "";

        // Gives full path with "/" in the end
        String path = iTarget.getFullPath();

        if (path.endsWith("/") || path.endsWith("\\"))
        {
            path = path.substring(0, path.length() - 1);
        }

        path = path.replace('\\', '/');
        fileListString = _listDirContents(path, aIncludeHidden);

        // Split the string into separate files
        String[] splitStrings = Tokenizer.split(fileListString, "*");
        String[] trimmed = new String[splitStrings.length - 1];

        for (int index = 0; index < splitStrings.length - 1; index++)
        {
            trimmed[index] = splitStrings[index];
        }
        return trimmed;
    }

    /**
     * Gets a filtered list of files and directories contained in a directory
     *
     * @param filter
     *            String against which all files and directories are matched for
     *            retrieval. includeHidden: boolean indicating whether files
     *            marked as hidden should be included or not in the list of
     *            files and directories returned.
     *
     * @return Vector of strings, denoting the files and directories in the
     *         directory
     *
     * @throws IOException
     *             if invoked on a file, the directory does not exist, the
     *             directory is not accessible, or an I/O error occurs.
     *
     * @throws SecurityException
     *             if the security of the application does not have read access
     *             for the connection's iTarget.
     *
     * @throws IllegalModeException
     *             If application does have read access to the file but has
     *             opened the connection in Connector.WRITE mode.
     * @throws ConnectionClosedException
     *             If Connection is closed.
     */
    public Vector listFiles(String aFilter, boolean aIncludeHidden)
    throws IOException
    {
        String filter = trimFilter(aFilter);
        checkFilter(filter);

        if (!_exists(iTarget.getFullPath()))
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + iTarget.getFullPath());
        }

        if (isFile())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_POINTS_TO_FILE
                                  + ": " + iTarget.getFullPath());
        }

        // need to make checks still
        Vector fileList = new Vector();
        String[] splitStrings = listFileArray(aIncludeHidden);

        boolean matchFlag = false;
        // result from native call will contain all files. Need to filter it now
        for (int i = 0; i < splitStrings.length; i++)
        {
            splitStrings[i] = splitStrings[i].trim();
            if (splitStrings[i].equals(""))
                continue;
            matchFlag = false;

            // Check if name matches the string
            matchFlag = matchString(filter.toLowerCase(), splitStrings[i]
                                    .toLowerCase());

            if (splitStrings[i].equals("./") || splitStrings[i].equals("../"))
            {
                matchFlag = false;
            }

            if (iTarget.getSeparatorUsed() != "/")
            {
                splitStrings[i] = splitStrings[i].replace('/', '\\');
            }

            if (matchFlag)
            {
                fileList.addElement(splitStrings[i]);
            }
        }

        return fileList;
    }

    /**
     * Gets a filtered list of files and directories contained in a directory
     *
     * @param aFilter
     *            String against which all files and directories are matched for
     *            retrieval. includeHidden: boolean indicating whether files
     *            marked as hidden should be included or not in the list of
     *            files and directories returned.
     *
     * @return Enumeration of strings, denoting the files and directories in the
     *         directory
     *
     * @throws IOException
     *             if invoked on a file, the directory does not exist, the
     *             directory is not accessible, or an I/O error occurs.
     *             SecurityException: if the security of the application does
     *             not have read access for the connection's iTarget.
     *             IllegalModeException: If application does have read access to
     *             the file but has opened the connection in Connector.WRITE
     *             mode. ConnectionClosedException: If Connection is closed.
     */
    public Enumeration list(String aFilter, boolean aIncludeHidden)
    throws IOException
    {
        Vector fileList = listFiles(aFilter, aIncludeHidden);
        return fileList.elements();
    }

    /**
     * Gets a list of all visible files and directories contained in a
     * directory. The directory is the connection's iTarget as specified in
     * constructor.
     * <p>
     * Any hidden files and directories in the directory are not included in the
     * returned list. Any current directory indication (".") and any parent
     * directory indication ("..") is not included in the list of files and
     * directories returned.
     *
     * @return An Enumeration of strings, denoting the files and directories in
     *         the directory. The string returned contain only the file or
     *         directory name and does not contain any path prefix (to get a
     *         complete path for each file or directory, prepend getPath()).
     *         Directories are denoted with a trailing slash "/" in their
     *         returned name. The Enumeration has zero length if the directory
     *         is empty.
     * @throws IOException
     */
    public Enumeration list() throws IOException
    {
        return list("", false);
    }

    /**
     * Gets a list of all visible files and directories contained in a
     * directory. The directory is the connection's iTarget as specified in
     * constructor.
     * <p>
     * Any hidden files and directories in the directory are not included in the
     * returned list. Any current directory indication (".") and any parent
     * directory indication ("..") is not included in the list of files and
     * directories returned.
     *
     * @return An list of FileUtility objects, denoting the files and
     *         directories in the directory.
     * @throws IOException
     */
    public FileUtility[] listFiles() throws IOException
    {
        Vector fileList = listFiles("", false);
        FileUtility[] files = new FileUtility[fileList.size()];

        for (int i = 0; i < fileList.size(); i++)
        {
            String element = iTarget.getFullPath();
            if (!element.endsWith(iTarget.getSeparatorUsed()))
            {
                element += iTarget.getSeparatorUsed();
            }
            files[i] = new FileUtility(element + fileList.elementAt(i));
        }
        return files;
    }

    /**
     * Truncates the file, discarding all data from the given byte offset to the
     * current end of the file
     *
     * @param aByteOffset
     *            Offset into the file from which truncation occurs.
     *
     * @throws java.io.IOException
     *             if invoked on a directory or the file does not exist or is
     *             not accessible.
     * @throws SecurityException
     *             if the security of the application does not have write access
     *             for the connection's iTarget.
     * @throws IllegalArgumentException
     *             if byteOffset is less than zero.
     */
    public void truncate(long aByteOffset) throws java.io.IOException
    {
        if (aByteOffset < 0)
        {
            throw new IllegalArgumentException(
                "Argument value cannot be negative");
        }

        if (!exists())
        {
            throw new IOException("File does not exist "
                                  + iTarget.getFullPath());
        }

        if (isDirectory())
        {
            throw new IOException("Connection is a directory "
                                  + iTarget.getFullPath());
        }

        if (aByteOffset > fileSize())
        {
            return;
        }

        _truncate(iTarget.getFullPath(), aByteOffset);
    }

    /**
     * Disposes native resource.
     *
     */
    private synchronized void doFinalize()
    {
        if (iFinalizer == null)
        {
            return;
        }
        iFinalizer = null;
        iStreamHandler = null;
    }

    public String toString()
    {
        return getAbsolutePath();
    }

    /** * ----------------------------- HELPERS ----------------------------- */

    /**
     * Checks for sanity of the name passed to rename method. It must not
     * contain characters that are not allowed in file names. Also, it must not
     * have any path separators in it. Will throw IllegalArgumentException if
     * the name was found to be improper.
     *
     * @param aName
     *            name of file/directory to which the iTarget needs to be
     *            renamed
     * @throws IOException
     *             in case it contains any of invalid characters.
     */
    private static final void checkRenameParameters(String aName)
    throws IOException
    {
        boolean result = true;
        if ((aName.indexOf('*') != -1) || (aName.indexOf('\\') != -1)
                || (aName.indexOf('/') != -1 && !aName.endsWith("/"))
                || aName.endsWith("//"))

        {
            throw new IllegalArgumentException();
        }

        if ((aName.indexOf('?') != -1) || (aName.indexOf('<') != -1)
                || (aName.indexOf('<') != -1) || (aName.indexOf(':') != -1)
                || (aName.indexOf('|') != -1))
        {
            throw new IOException();
        }
    }

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
     * Matches a given string with filter. Filter string can contain * as a wild
     * card. Used in case of list. Where application might give a filter string
     * to filter out contents of a directory.
     * <p>
     * Logic followed is this:
     * <ol>
     * <li>1. Filter can be of the form *a*b*c* (in its most tricky form.</li>
     * <li>2. a, b, c can themselves be strings.</li>
     * <li>3. First split the sting into tokens taking "*" as delimiter.</li>
     * <li>4. Now check if the name parameter has a, b and c in the same
     * sequence.</li>
     * <li>5. Now, if 'a' occurs after 'b', and 'c' occurs after 'b', we can be
     * sure that the name parameter already matches the filter.</li>
     *
     * @param aFilter
     *            filter string that will be used to check validity of name
     *            parameter
     * @param aName
     *            name of the file/directory that will be compared against the
     *            filter
     * @return true if the name matches the filter, false otherwise
     */
    public static boolean matchString(String aFilter, String aName)
    {
        Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                   "--> FileUtility.matchString(): " + " Filter: " + aFilter
                   + " Name: " + aName);

        String filter = aFilter;
        filter = trimFilter(filter);

        if (filter.equals("*"))
        {
            Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                       "<-- FileUtility.matchString(): returns true");
            return true;
        }

        // Since only "*" is used as wild card, we split the strings taking
        // "*" as delimiter.
        String[] tokens = Tokenizer.split(filter, "*");
        int prevTokenEndIndex = 0;
        boolean matchFlag = true;

        prevTokenEndIndex = 0;
        matchFlag = true;

        int j = 0;
        // Check if the name contains all the tokens in the filter string.
        for (j = 0; j < tokens.length; j++)
        {
            if (tokens[j].equals(""))
            {
                // happens if filter starts and ends with "*"
                continue;
            }

            // Check if all the tokens are present

            int index = aName.indexOf(tokens[j], prevTokenEndIndex);
            if (-1 != index)
            {
                if (index >= prevTokenEndIndex)
                {
                    prevTokenEndIndex = aName.indexOf(tokens[j])
                                        + tokens[j].length();

                    matchFlag = true;
                }
                else
                {
                    matchFlag = false;
                    break;
                }
            }
            else
            {
                matchFlag = false;
                break;
            }
        }

        // We now need to check if the start and end of the name match that of
        // the filter given. In case the filter does not start and end with "*",
        // we must ensure that the name starts and ends with the same tokens as
        // specified in the filter.
        if (matchFlag) // We proceed only if all the tokens are present in name
        {
            if (!filter.startsWith("*"))
            {
                // IF the filter does not start with "*" then name should start
                // with first token
                if (!aName.startsWith(tokens[0]))
                {
                    matchFlag = false;
                }
            }

            if (!filter.endsWith("*"))
            {
                // IF the filter does not end with "*" then name should end
                // with last token
                if (!aName.endsWith(tokens[tokens.length - 1]))
                {
                    matchFlag = false;
                }
            }
        }

        Logger.LOG(Logger.EJavaFile, Logger.EInfo,
                   "<-- FileUtility.matchString(): returns: " + matchFlag);

        return matchFlag;
    }

    /** * ----------------------------- NATIVE ----------------------------- */

    private static native boolean _exists(String path);

    private static native String _getRealPath(String path) throws IOException;

    private native long _availableSize(String path);

    private native boolean _canRead(String name);

    private native boolean _canWrite(String name);

    private native boolean _createNewDirectory(String fileName)
    throws IOException, SecurityException;

    private native boolean _createNewFile(String fileName) throws IOException,
                SecurityException;

    private native boolean _delete(String fileName) throws SecurityException;

    private native long _dirSize(String filePath, boolean includeSubDirs);

    private native long _fileSize(String fileName) throws IOException;

    private native boolean _isDirectory(String dirName);

    private native boolean _isFile(String fileName);

    private native boolean _isHidden(String name);

    private native long _lastModified(String name);

    private native String _listDirContents(String fullPath,
                                           boolean includeHidden);

    private native boolean _renameFile(String oldPath, String newPath)
    throws IOException, SecurityException;

    private native void _setReadable(String name, boolean readable)
    throws IOException;

    private native void _setWritable(String name, boolean writable)
    throws IOException;

    private native long _totalSize(String path);

    private native void _truncate(String name, long offset) throws IOException;

    private native long _usedSize(String path);
}
