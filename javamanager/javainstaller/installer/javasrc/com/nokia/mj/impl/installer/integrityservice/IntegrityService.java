/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.integrityservice;

import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Hashtable;
import java.util.Vector;

/**
 * IntegrityService offers services for making file operations on disk
 * in atomic way. Client first creates an IntegrityService session,
 * performs variety of file operations, and in the end either commits
 * or rolls back the changes. IntegrityService guaranteees that either
 * all or none of the operations will be made in the disk, depending
 * on if the session was committed or rolled back. Client must ensure
 * that previous session gets committed or rolled back before next
 * session to the same root is created.
 */
public class IntegrityService
{
    private static Hashtable iInstances = new Hashtable();
    private static final String JOURNAL_FILENAME = "isjournal.dat";
    private static final String TEMP_FILE_SUFFIX = ".tmp";

    // True if operation details are logged, enabled with "-log=is"
    // command line argument.
    private static boolean iOperationsLogged = false;

    // Member variables.
    private String iJournalRoot = null;
    private String iJournalFilename = null;
    private OutputStream iOut = null;
    private Vector iUsedDrives = null; // drives to which operations are made during this session

    private IntegrityService(String aJournalRoot)
    {
        iJournalRoot = aJournalRoot;
        iJournalFilename = iJournalRoot + JOURNAL_FILENAME;
        boolean result = FileOps.mkdirs(iJournalRoot);
        if (result)
        {
            try
            {
                addUsedDrive(iJournalRoot);
                iOut = FileUtils.getOutputStream(iJournalFilename, true);
            }
            catch (IOException ioe)
            {
                Log.logError("IntegrityService journal file open failed", ioe);
                result = false;
            }
        }
        if (!result)
        {
            InstallerException.internalError("IntegrityService constructor failed!");
        }
    }

    /**
     * Gets instance of IntegrityService. Creates required journal
     * file and temp directory.
     * @param aJournalRoot Root directory for journal and temp files.
     */
    synchronized public static IntegrityService getInstance(String aJournalRoot)
    {
        IntegrityService instance = (IntegrityService)iInstances.get(aJournalRoot);
        if (instance == null)
        {
            instance = new IntegrityService(aJournalRoot);
            iInstances.put(aJournalRoot, instance);
        }
        return instance;
    }

    /**
     * Succesfully completes integrity service session.
     * Closes journal file and removes it as well as temp files
     * and folders. After committing, this IntegrityService instance
     * can no longer be used.
     * @return True if operation succeeded, false otherwise.
     */
    synchronized public boolean commit()
    {
        boolean result = closeJournal();
        if (result)
        {
            result = deleteJournalRoots();
        }
        if (!result && operationsLogged())
        {
            Log.logError("IntegrityService commit failed!!!");
        }
        iInstances.remove(iJournalRoot);
        return result;
    }

    /**
     * Rolls back operations currently listed in integrity service
     * journal file. After rollback, this IntegrityService instance
     * can no longer be used.
     * @return True if operation succeeded, false otherwise.
     */
    synchronized public boolean rollback()
    {
        boolean result = closeJournal();
        // Check if journal file exists.
        if (result)
        {
            if (!FileUtils.exists(iJournalFilename))
            {
                // No journal file, nothing to rollback.
                Log.log("IntegrityService journal file does not exist.");
                return result;
            }
        }

        // Perform rollback.
        if (result)
        {
            InputStream in = null;
            try
            {
                // Read operations from journal file.
                in = FileUtils.getInputStream(iJournalFilename);
                Vector ops = new Vector();
                while (in.available() > 0)
                {
                    Operation op = new Operation((byte)0);
                    op.read(in);
                    ops.addElement(op);
                }
                in.close();
                in = null;

                if (operationsLogged())
                {
                    Log.log("IntegrityService to rollback " +
                            ops.size() + " operations from " +
                            iJournalFilename);
                }

                // Cancel operations in reverse order.
                for (int i = ops.size() - 1; i >= 0 && result; i--)
                {
                    Operation op = (Operation)ops.elementAt(i);
                    if (op.getStatus() == op.STATUS_OK)
                    {
                        // Call cancel only for successful operations.
                        result = op.cancel();
                    }
                    if (result)
                    {
                        // Remove canceled operation from list.
                        ops.removeElementAt(i);
                    }
                    else
                    {
                        Log.logWarning(
                            "IntegrityService rollback failed for " +
                            op.toString());
                    }
                }
                if (result)
                {
                    result = deleteJournalRoots();
                }
                else
                {
                    // Write remaining uncanceled operations back
                    // to journal file.
                    writeOperations(ops);
                }
            }
            catch (IOException ioe)
            {
                Log.logError("IOException during IntegrityService rollback", ioe);
                result = false;
            }
            finally
            {
                if (in != null)
                {
                    try
                    {
                        in.close();
                    }
                    catch (IOException ioe)
                    {
                    }
                }
            }
        }
        if (!result && operationsLogged())
        {
            Log.logError("IntegrityService rollback failed!!!");
        }
        iInstances.remove(iJournalRoot);
        return result;
    }

    /**
     * Rolls back operations currently listed in integrity service
     * journal file. This variant of rollback can be called when there
     * is no IntegrityService session open in given journal root.
     * @param aJournalRoot Root directory for journal and temp files.
     * @return True if operation succeeded, false otherwise.
     */
    synchronized public static boolean rollback(String aJournalRoot)
    {
        if (operationsLogged())
        {
            Log.log("IntegrityService static rollback " + aJournalRoot);
        }
        IntegrityService instance = getInstance(aJournalRoot);
        return instance.rollback();
    }

    /**
     * Create file. Updates file creation to journal file, does
     * not create file to the disk.
     * @return True if operation succeeded, false otherwise.
     */
    synchronized public boolean create(String aFile)
    {
        boolean result = true;
        try
        {
            addUsedDrive(aFile);
            result = writeAndExecute(new Operation(Operation.CREATE, aFile));
        }
        catch (IOException ioe)
        {
            Log.logError("IntegrityService create failed: " + aFile, ioe);
            result = false;
        }
        return result;
    }

    /**
     * Create directory. Updates directory creation to journal file and
     * creates directory to the disk.
     * @return True if operation succeeded, false otherwise.
     */
    synchronized public boolean createDir(String aDir)
    {
        boolean result = create(aDir);
        if (result)
        {
            result = FileOps.mkdirs(aDir);
        }
        return result;
    }

    /**
     * Create temporary file. Generates temporary file name,
     * updates file creation to journal file, but does
     * not create file to the disk.
     * @param aDrive drive for temp file, or -1 if drive not specified
     * @return Temporary file name, or null if operation failed.
     */
    synchronized public String createTempFile(int aDrive)
    {
        return createTempFile(aDrive, TEMP_FILE_SUFFIX);
    }

    /**
     * Create temporary file. Generates temporary file name,
     * updates file creation to journal file, but does
     * not create file to the disk.
     * @param aDrive drive for temp file, or -1 if drive not specified
     * @param aSuffix suffix string used when generating the file name
     * @return Temporary file name, or null if operation failed.
     */
    synchronized public String createTempFile(int aDrive, String aSuffix)
    {
        String tempFile = null;
        boolean result = true;
        try
        {
            tempFile = getTempFile(aDrive, aSuffix);
        }
        catch (IOException ioe)
        {
            result = false;
        }
        if (result)
        {
            result = create(tempFile);
        }
        if (!result)
        {
            tempFile = null;
        }
        return tempFile;
    }

    /**
     * Delete file or directory. Updates file/directory deletion to
     * journal file and deletes file/directory from the disk.
     * @return True if operation succeeded, false otherwise.
     */
    synchronized public boolean delete(String aFile)
    {
        boolean result = true;
        try
        {
            addUsedDrive(aFile);
            String tempFile = getTempFile(FileOps.getDrive(aFile));
            result = writeAndExecute(
                         new Operation(Operation.DELETE, aFile, tempFile));
        }
        catch (IOException ioe)
        {
            Log.logError("IntegrityService delete failed: " + aFile, ioe);
            result = false;
        }
        return result;
    }

    /**
     * Copy file or directory. Updates file/directory copy to
     * journal file and copies file/directory in the disk.
     * Directory copy replaces the whole target directory,
     * i.e. the files in the old directory will be removed.
     * @return True if operation succeeded, false otherwise.
     */
    synchronized public boolean copy(String aFrom, String aTo)
    {
        boolean result = true;
        try
        {
            addUsedDrive(aFrom);
            addUsedDrive(aTo);
            String tempFile = getTempFile(FileOps.getDrive(aTo));
            result = writeAndExecute(
                         new Operation(Operation.COPY, aFrom, aTo, tempFile));
        }
        catch (IOException ioe)
        {
            Log.logError("IntegrityService copy failed: " + aFrom + " -> " + aTo, ioe);
            result = false;
        }
        return result;
    }

    /**
     * Move file or directory. Updates file/directory move to
     * journal file and moves file/directory in the disk.
     * @return True if operation succeeded, false otherwise.
     */
    synchronized public boolean move(String aFrom, String aTo)
    {
        boolean result = true;
        try
        {
            addUsedDrive(aFrom);
            addUsedDrive(aTo);
            String tempFile = getTempFile(FileOps.getDrive(aTo));
            result = writeAndExecute(
                         new Operation(Operation.MOVE, aFrom, aTo, tempFile));
        }
        catch (IOException ioe)
        {
            Log.logError("IntegrityService move failed: " + aFrom + " -> " + aTo, ioe);
            result = false;
        }
        return result;
    }

    /**
     * Update file. Marks the file as updated to journal file.
     * @return True if operation succeeded, false otherwise.
     */
    synchronized public boolean update(String aFile)
    {
        boolean result = true;
        try
        {
            addUsedDrive(aFile);
            String tempFile = getTempFile(FileOps.getDrive(aFile));
            result = writeAndExecute(
                         new Operation(Operation.UPDATE, aFile, tempFile));
        }
        catch (IOException ioe)
        {
            Log.logError("IntegrityService update failed: " + aFile, ioe);
            result = false;
        }
        return result;
    }

    /**
     * Enable/disable operation details logging.
     */
    public static void setOperationsLogged(boolean aOperationsLogged)
    {
        iOperationsLogged = aOperationsLogged;
    }

    /**
     * Returns true if operation details are logged.
     */
    public static boolean operationsLogged()
    {
        return iOperationsLogged;
    }

    private String getTempFile(int aDrive) throws IOException
    {
        return getTempFile(aDrive, TEMP_FILE_SUFFIX);
    }

    private String getTempFile(int aDrive, String aSuffix) throws IOException
    {
        String journalRoot = FileOps.getPathFromDrive(aDrive, iJournalRoot);
        boolean result = FileOps.mkdirs(journalRoot);
        if (!result)
        {
            throw new IOException("Creating dir " + journalRoot + " failed");
        }
        FileUtility journalRootFile = new FileUtility(journalRoot);
        FileUtility f = FileUtility.createTempFile(
                            "jis", aSuffix, journalRootFile);
        f.delete();
        return f.getAbsolutePath();
    }

    private boolean writeAndExecute(Operation aOp) throws IOException
    {
        aOp.write(iOut);
        boolean result = aOp.execute();
        if (!result)
        {
            aOp.setStatus(aOp.STATUS_FAIL);
        }
        aOp.writeStatus(iOut);
        return result;
    }

    private boolean writeOperations(Vector aOps)
    {
        boolean result = true;
        OutputStream out = null;
        try
        {
            out = FileUtils.getOutputStream(iJournalFilename);
            for (int i = 0; i < aOps.size(); i++)
            {
                Operation op = (Operation)aOps.elementAt(i);
                op.write(out);
                op.writeStatus(out);
            }
        }
        catch (IOException ioe)
        {
            result = false;
        }
        finally
        {
            try
            {
                if (out != null)
                {
                    out.close();
                }
            }
            catch (IOException ioe)
            {
            }
        }
        return result;
    }

    /**
     * Adds drive from given filename to the list of used drives
     * if it does not exist there yet.
     */
    private void addUsedDrive(String aFilename)
    {
        if (iUsedDrives == null)
        {
            iUsedDrives = new Vector();
        }
        Integer drive = new Integer(FileUtils.getDrive(aFilename));
        if (!iUsedDrives.contains(drive))
        {
            iUsedDrives.addElement(drive);
        }
    }

    /**
     * Closes jouranl file. Returns true if file was open and it could be
     * successfully closed, false otherwise.
     */
    private boolean closeJournal()
    {
        boolean result = true;
        if (iOut != null)
        {
            try
            {
                iOut.close();
                iOut = null;
            }
            catch (IOException ioe)
            {
                result = false;
                Log.logWarning("IntegrityService journal closing failed", ioe);
            }
        }
        else
        {
            // This instance has already been either committed or rolled back.
            result = false;
            Log.logWarning("IntegrityService journal has already been closed");
        }
        return result;
    }

    /**
     * Removes IntegrityService root directories from all
     * drives where operations are made during this session.
     */
    private boolean deleteJournalRoots()
    {
        if (iUsedDrives == null)
        {
            return true;
        }
        boolean result = true;
        for (int i = 0; i < iUsedDrives.size(); i++)
        {
            String journalRoot = FileUtils.setDrive(
                                     iJournalRoot, ((Integer)iUsedDrives.elementAt(i)).intValue());
            if (operationsLogged())
            {
                Log.log("IntegrityService removing " + journalRoot);
            }
            boolean opResult = FileOps.deleteAll(journalRoot);
            if (!opResult)
            {
                Log.logError("IntegrityService failed to remove " + journalRoot);
                result = opResult;
            }
        }
        return result;
    }
}
