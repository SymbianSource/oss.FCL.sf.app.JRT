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


package com.nokia.mj.impl.installer.jadjarmatcher;

import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;

import java.io.IOException;
import java.util.Vector;

/**
 * JadJarMatcher offers services for finding Jar when Jad filename is known,
 * and finding Jad when Jar filename is known.  JadJarMatcher only searches
 * files from local folders.
 */
public final class JadJarMatcher extends JadJarMatcherBase
{
    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Searches for Jad file matching to given Jar file.
     *
     * @param aJar Jar file name.
     * @return JadJarFile containing matching file names.
     * @throws InstallerException if error occurs.
     */
    public static JadJarFile findJad(String aJar)
    {
        JadJarFile result = null;
        if (FileUtils.isInboxFile(aJar))
        {
            // Search from Inbox.
            Log.log("JadJarMatcher: finding Jad from Inbox for " + aJar);
            result = findFilesFromInbox(aJar, null, false);
            if (result.iJadFilename == null)
            {
                Log.log("JadJarMatcher: Matching Jad not found from Inbox");
            }
        }
        else
        {
            // Search from the same directory where the Jar is.
            result = JadJarMatcherBase.findJad(aJar);
        }
        return result;
    }

    /**
     * Searches for Jar file matching to given Jad file.
     *
     * @param aJad Jad file name.
     * @param aJadCharset Character encoding used in Jad file.
     * @return JadJarFile containing matching file names and their attributes.
     * @throws InstallerException if error occurs.
     */
    public static JadJarFile findJar(String aJad, String aJadCharset)
    {
        JadJarFile result = null;
        if (FileUtils.isInboxFile(aJad))
        {
            // Search from Inbox.
            Log.log("JadJarMatcher: finding Jar from Inbox for " + aJad);
            result = findFilesFromInbox(aJad, aJadCharset, true);
            if (result.iJarFilename == null)
            {
                Log.log("JadJarMatcher: Matching Jar not found from Inbox");
            }
        }
        else
        {
            // Search from the same directory where the Jad is.
            result = JadJarMatcherBase.findJar(aJad, aJadCharset);
        }
        return result;
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Get list of file names from Inbox.
     * @param aStartFilename file name where the installation was started from.
     * @param aJadCharset Character encoding used in Jad file.
     * @param aFindJars true if finding jar files, false if finding jad files.
     * @return array of found file names from Inbox.
     * @throws InstallerException if an error occurs.
     */
    private static JadJarFile findFilesFromInbox(
        String aStartFilename, String aJadCharset, boolean aFindJars)
    {
        // Check that we have IntegrityService instance.
        if (getIntegrityService() == null)
        {
            InstallerException.internalError(
                "Finding files from Inbox failed, IntegrityService not available");
        }

        // Find files from Inbox.
        Vector files = new Vector();
        Vector handles = new Vector();
        Vector timestamps = new Vector();
        int ret = 0;
        if (aFindJars)
        {
            ret = _findJarFilesFromInbox(
                      aStartFilename, files, handles, timestamps);
        }
        else
        {
            ret = _findJadFilesFromInbox(
                      aStartFilename, files, handles, timestamps);
        }
        if (ret < 0)
        {
            InstallerException.internalError(
                "Finding files from Inbox failed with code " + ret);
        }
        if (files.size() < 1
                || files.size() != handles.size()
                || files.size() != timestamps.size())
        {
            // There must be as many files as there are handles.
            InstallerException.internalError(
                "Finding files from Inbox failed, files.size=" +
                files.size() + ", handles.size=" + handles.size() +
                ", timestamps.size=" + timestamps.size());
        }

        JadJarFile result = null;
        String[] resultFilenames = new String[files.size()];
        int[] resultHandles = new int[handles.size()];
        long[] resultTimestamps = new long[timestamps.size()];
        for (int i = 0; i < files.size(); i++)
        {
            resultFilenames[i] = (String)files.elementAt(i);
            resultHandles[i] = ((Integer)handles.elementAt(i)).intValue();
            resultTimestamps[i] = ((Long)timestamps.elementAt(i)).longValue();
        }
        try
        {
            // Construct a FileList object and sort it by time distance from
            // the file where listing was started from.
            FileList fileList = new FileList(
                resultFilenames, resultHandles, resultTimestamps);
            fileList.initTimeDistances(resultTimestamps[0]);
            fileList.sortByTimeDistance();
            fileList.sortByBasename(null);
            // Because JavaInstaller does not have AllFiles capability in
            // S60, it must read the files from Inbox using RFile handle
            // the Messaging Framework provides.
            // Get attributes for the file where installation was started from.
            String tmpFilename = createTempFile(resultFilenames, resultHandles, 0);
            if (aFindJars)
            {
                result = getJadAttributes(tmpFilename, aJadCharset);
                // Sort resultFilenames and resultHandles so that the
                // file specified in MIdlet-Jar-URL attribute will
                // be matched first.
                String jarUrl = getMidletJarUrl(result);
                if (jarUrl != null)
                {
                    fileList.sortByName(jarUrl, 1);
                }
            }
            else
            {
                result = getJarAttributes(tmpFilename);
                if (result.iJadFilename != null)
                {
                    // Jad file was found from inside midlet message package,
                    // no need to find it.
                    Log.log("JadJarMatcher: Found jad from midlet message");
                    return result;
                }
            }
            Log.log("JadJarMatcher: Found " + (resultFilenames.length-1) +
                    " files from Inbox:\n" + fileList.toString());
            // Go through the list of Inbox files to match.
            MatchData matchData = null;
            if (aFindJars)
            {
                matchData = MatchData.getMatchData(result.iJadAttributes);
                for (int i = 1; i < resultFilenames.length; i++)
                {
                    tmpFilename = createTempFile(resultFilenames, resultHandles, i);
                    result.iJarAttributes = matchJar(matchData, tmpFilename);
                    if (result.iJarAttributes != null)
                    {
                        result.iJarFilename = tmpFilename;
                        break;
                    }
                }
            }
            else
            {
                matchData = MatchData.getMatchData(result.iJarAttributes);
                for (int i = 1; i < resultFilenames.length; i++)
                {
                    tmpFilename = createTempFile(resultFilenames, resultHandles, i);
                    result.iJadAttributes = matchJad(matchData, tmpFilename);
                    if (result.iJadAttributes != null)
                    {
                        result.iJadFilename = tmpFilename;
                        break;
                    }
                }
            }
        }
        finally
        {
            // Close the Inbox file handles.
            ret = _closeFileHandles(resultHandles);
            if (ret < 0)
            {
                InstallerException.internalError(
                    "Closing Inbox file handles failed with code " + ret);
            }
        }

        return result;
    }

    /**
     * Creates a new temp file from Inbox file identified in the
     * aFilenames and aHandles arrays with aIndex.
     */
    private static String createTempFile(
        String[] aFilenames, int[] aHandles, int aIndex)
    {
        String tmpFilename = null;
        if (aFilenames.length > aIndex)
        {
            int drive = FileUtils.getDrive(aFilenames[aIndex]);
            String fileExtension = FileUtils.getExtension(aFilenames[aIndex]);
            tmpFilename = getIntegrityService().createTempFile(
                              drive, fileExtension);
            int ret =_copyFile(aHandles[aIndex], tmpFilename);
            if (ret < 0)
            {
                InstallerException.internalError(
                    "Copying Inbox file failed with code " + ret);
            }
            Log.log("JadJarMatcher: " + aFilenames[aIndex] +
                    " ==> " + tmpFilename);
        }
        return tmpFilename;
    }

    /**
     * Get list of Jad file names from Inbox.
     * @param aJar Jar file name.
     * @param aJadFiles Vector where the found file names are inserted.
     * The aJar file name will be returned as the first item of this list.
     * @param aJadHandles Vector where the handles for the found files
     * are inserted.
     * @param aTimestamps Vector where the timestamps for the found
     * files are inserted.
     * @return Symbian error code (negative number).
     */
    private static native int _findJadFilesFromInbox(String aJar, Vector aJadFiles, Vector aJadHandles, Vector aTimestamps);

    /**
     * Get list of Jar file names from Inbox.
     * @param aJad Jad file name.
     * @param aJarFiles Vector where the found file names are inserted.
     * The aJad file name will be returned as the first item of this list.
     * @param aJarHandles Vector where the handles for the found files
     * are inserted.
     * @param aTimestamps Vector where the timestamps for the found
     * files are inserted.
     * @return Symbian error code (negative number).
     */
    private static native int _findJarFilesFromInbox(String aJad, Vector aJarFiles, Vector aJarHandles, Vector aTimestamps);

    /**
     * Copy a file from RFile handle.
     * @param aHandle RFile handle to a source file.
     * @param aFilename Target file name.
     * @return Symbian error code (negative number).
     */
    private static native int _copyFile(int aHandle, String aFilename);

    /**
     * Closes given RFile handles.
     * @param aHandles RFile handles to be closed.
     * @return Symbian error code (negative number).
     */
    private static native int _closeFileHandles(int[] aHandles);

    /*** ----------------------------- NATIVE ----------------------------- */
}
