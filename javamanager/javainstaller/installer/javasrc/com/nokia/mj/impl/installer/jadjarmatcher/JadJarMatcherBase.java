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

import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.installer.integrityservice.IntegrityService;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.JadReader;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.JarManifestReader;
import com.nokia.mj.impl.utils.OtaStatusCode;

import java.io.IOException;
import java.util.Hashtable;
import java.util.Vector;

/**
 * JadJarMatcher offers services for finding Jar when Jad filename is known,
 * and finding Jad when Jar filename is known.  JadJarMatcher only searches
 * files from local folders.
 *
 * @author Nokia Corporation
 * @version $Rev: 9457 $
 */
public class JadJarMatcherBase
{
    /** IntegrityService instance to be used when matching. */
    static IntegrityService iIntegrityService = null;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Set IntegrityService instance to be used when matching.
     */
    public static void setIntegrityService(IntegrityService aIs)
    {
        iIntegrityService = aIs;
    }

    /**
     * Searches for Jad file matching to given Jar file.
     *
     * @param aJar Jar file name.
     * @return JadJarFile containing matching file names.
     * @throws InstallerException if error occurs.
     */
    public static JadJarFile findJad(String aJar)
    {
        Log.log("JadJarMatcherBase: finding Jad for " + aJar);
        JadJarFile result = null;
        if (aJar != null)
        {
            result = getJarAttributes(aJar);
            if (result.iJadFilename != null)
            {
                // Jad file was found from inside midlet message package,
                // no need to find it.
                Log.log("JadJarMatcherBase: Found jad from midlet message");
                return result;
            }
            try
            {
                findJad(result, listFiles(aJar, new String[] { ".jad" }));
            }
            catch (IOException ioe)
            {
                InstallerException.internalError
                ("Finding Jad failed for " + aJar, ioe);
            }
            if (result.iJadFilename == null)
            {
                // Jad was not found from the same directory where Jar is.
                Log.log("JadJarMatcherBase: Jad not found from the same folder");
            }
        }
        return result;
    }

    /**
     * Searches for Jar file matching to given Jad file.
     *
     * @param aJad Jad file name.
     * @param aJadCharset Character encoding used in Jad file.
     * @return JadJarFile containing matching file names.
     * @throws InstallerException if error occurs.
     */
    public static JadJarFile findJar(String aJad, String aJadCharset)
    {
        Log.log("JadJarMatcherBase: finding Jar for " + aJad);
        JadJarFile result = null;
        if (aJad != null)
        {
            result = getJadAttributes(aJad, aJadCharset);
            try
            {
                findJar(result, listFiles(aJad, new String[] { ".jar", ".dcf", ".dm" }));
            }
            catch (IOException ioe)
            {
                InstallerException.internalError
                ("Finding Jar failed for " + aJad, ioe);
            }
            if (result.iJarFilename == null)
            {
                // Jar was not found from the same directory where Jad is.
                Log.log("JadJarMatcherBase: Jar not found from the same folder");
            }
        }
        return result;
    }

    /*** ---------------------------- PROTECTED --------------------------- */

    /**
     * Returns IntegrityService instance to be used when matching.
     */
    protected static IntegrityService getIntegrityService()
    {
        return iIntegrityService;
    }

    /**
     * Reads jad attributes from given file using given character encoding.
     *
     * @param aJad Jad file name.
     * @param aJadCharset Character encoding used in Jad file.
     * @return JadJarFile Jad file name and its attributes.
     * @throws InstallerException if error occurs.
     */
    protected static JadJarFile getJadAttributes(
        String aJad, String aJadCharset)
    {
        JadJarFile result = new JadJarFile();
        try
        {
            result.iJadFilename = aJad;
            result.iJadAttributes = JadReader.getAttributes(aJad, aJadCharset);
            if (result.iJadAttributes == null)
            {
                throw new IOException("No Jad attributes found");
            }
        }
        catch (IOException ioe)
        {
            Log.logError("Exception while getting jad attributes: " + ioe);
            throw new InstallerException
            (InstallerErrorMessage.INST_CORRUPT_PKG, null,
             InstallerDetailedErrorMessage.INTERNAL_ERROR,
             new String[] { "Reading Jad failed: " + aJad },
             OtaStatusCode.INVALID_DESCRIPTOR, ioe);
        }
        return result;
    }

    /**
     * Reads jar attributes from given aJar file. Also checks if the
     * attributes indicate that this jar file is a midlet message package,
     * and if that's the case then extracts jad and jar files and
     * initializes the returned JadJarFile accordingly.
     *
     * @param aJar Jar file name.
     * @return JadJarFile Jar file name.
     * @throws InstallerException if error occurs.
     */
    protected static JadJarFile getJarAttributes(String aJar)
    {
        JadJarFile result = new JadJarFile();
        try
        {
            result.iJarFilename = aJar;
            result.iJarAttributes = JarManifestReader.getAttributes(aJar);
            if (result.iJarAttributes == null)
            {
                throw new IOException("No Manifest attributes found");
            }
        }
        catch (IOException ioe)
        {
            Log.logError("Exception while getting jar attributes: " + ioe);
            throw new InstallerException
            (InstallerErrorMessage.INST_CORRUPT_PKG, null,
             InstallerDetailedErrorMessage.INTERNAL_ERROR,
             new String[] { "Reading Manifest failed: " + aJar },
             OtaStatusCode.INVALID_JAR, ioe);
        }
        // Check if jar file is a midlet message package.
        if (MidletMessageHandler.checkMidletMessage(result))
        {
            MidletMessageHandler.handleMidletMessage(aJar, result);
        }
        return result;
    }

    /**
     * Searches given aFiles for a match to Jar specified in aJadJarFile.
     * If a match is found, updates Jad details in aJadJarFile object.
     */
    protected static void findJad(JadJarFile aJadJarFile, FileList aFiles)
    throws IOException
    {
        MatchData jarMatch =
            MatchData.getMatchData(aJadJarFile.iJarAttributes);
        Log.log("JadJarMatcherBase: matching " + aJadJarFile.iJarFilename +
                " (" + jarMatch + ")");
        if (aFiles.iFilenames.length > 1)
        {
            // Sort files so that the file which has the same
            // basename as the jar file will be matched first.
            aFiles.sortByBasename(aJadJarFile.iJarFilename);
        }
        Log.log("JadJarMatcherBase: Found " + aFiles.iFilenames.length +
                " files:\n" + aFiles.toString());
        for (int i = 0; i < aFiles.iFilenames.length; i++)
        {
            aJadJarFile.iJadAttributes =
                matchJad(jarMatch, aFiles.iFilenames[i]);
            if (aJadJarFile.iJadAttributes != null)
            {
                aJadJarFile.iJadFilename = aFiles.iFilenames[i];
                break;
            }
        }
    }

    /**
     * Searches given aFiles for a match to Jad specified in aJadJarFile.
     * If a match is found, updates Jar details in aJadJarFile object.
     */
    protected static void findJar(JadJarFile aJadJarFile, FileList aFiles)
    throws IOException
    {
        MatchData jadMatch =
            MatchData.getMatchData(aJadJarFile.iJadAttributes);
        Log.log("JadJarMatcherBase: matching " + aJadJarFile.iJadFilename +
                " (" + jadMatch + ")");
        if (aFiles.iFilenames.length > 1)
        {
            // Sort files so that the file which has the same
            // basename as the jad file will be matched first.
            aFiles.sortByBasename(aJadJarFile.iJadFilename);
            // Sort files so that the file specified in
            // MIdlet-Jar-URL attribute will be matched first.
            String jarUrl = getMidletJarUrl(aJadJarFile);
            if (jarUrl != null)
            {
                aFiles.sortByName(jarUrl, 0);
            }
        }
        Log.log("JadJarMatcherBase: Found " + aFiles.iFilenames.length +
                " files:\n" + aFiles.toString());
        for (int i = 0; i < aFiles.iFilenames.length; i++)
        {
            aJadJarFile.iJarAttributes =
                matchJar(jadMatch, aFiles.iFilenames[i]);
            if (aJadJarFile.iJarAttributes != null)
            {
                aJadJarFile.iJarFilename = aFiles.iFilenames[i];
                break;
            }
        }
    }

    /**
     * Returns a Hashtable of attributes read from Jar specified by
     * given aFilename if attributes in Jar match to given aJarMatch
     * data. If data does not match, this method returns null.
     */
    protected static Hashtable matchJad(MatchData aJarMatch, String aFilename)
    {
        Hashtable attrs = null;
        try
        {
            attrs = JadReader.getAttributes(aFilename);
        }
        catch (IOException ioe)
        {
            // Not a Jad file ==> no match.
            Log.log("JadJarMatcherBase: " + ioe.toString());
        }
        MatchData jadMatch = null;
        if (attrs != null)
        {
            jadMatch = MatchData.getMatchData(attrs);
            if (jadMatch.equals(aJarMatch))
            {
                Log.log("JadJarMatcherBase: match " + aFilename);
                return attrs;
            }
        }
        Log.log("JadJarMatcherBase: no match " + aFilename +
                " (" + jadMatch + ")");
        return null;
    }

    /**
     * Returns a Hashtable of attributes read from Jad specified by
     * given aFilename if attributes in Jad match to given aJadMatch
     * data. If data does not match, this method returns null.
     */
    protected static Hashtable matchJar(MatchData aJadMatch, String aFilename)
    {
        Hashtable attrs = null;
        try
        {
            attrs = JarManifestReader.getAttributes(aFilename);
        }
        catch (IOException ioe)
        {
            // Not a Jar file ==> no match.
            Log.log("JadJarMatcherBase: " + ioe.toString());
        }
        MatchData jarMatch = null;
        if (attrs != null)
        {
            jarMatch = MatchData.getMatchData(attrs);
            if (jarMatch.equals(aJadMatch))
            {
                Log.log("JadJarMatcherBase: match " + aFilename);
                return attrs;
            }
        }
        Log.log("JadJarMatcherBase: no match " + aFilename +
                " (" + jarMatch + ")");
        return null;
    }

    /**
     * Returns the value of MIDlet-Jar-URL attribute from given JadJarFile
     * object or null if the attribute does not exist.
     */
    protected static String getMidletJarUrl(JadJarFile aJadJarFile)
    {
        if (aJadJarFile == null || aJadJarFile.iJadAttributes == null)
        {
            return null;
        }
        return getAttributeValue(aJadJarFile.iJadAttributes, "MIDlet-Jar-URL");
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Returns an array of file names with specified extensions
     * from the same directory where given file exists.
     * Extension comparison is case insensitive.
     *
     * @param aFile files will be returned from the directory where
     * this file is located
     * @param aExts allowed extensions for returned files
     * @return FileList object containing arrays of file realted info
     * @returns IOException if an I/O error occurs
     */
    private static FileList listFiles(String aFile, String[] aExts) throws IOException
    {
        String name = FileUtils.getName(aFile);
        String path = aFile.substring(0, aFile.length()-name.length());
        if (path == null || path.length() == 0)
        {
            path = ".";
            //} else {
            // Else branch is not needed when FileUtility is used.
            // Add "." to make sure that CDC File understands that
            // path is directory.
            //path = path + ".";
        }
        for (int i = 0; i < aExts.length; i++)
        {
            aExts[i] = (aExts[i] == null? "": aExts[i].toLowerCase());
        }
        FileUtility[] files = new FileUtility(path).listFiles();
        Vector filenames = new Vector();
        Vector fileTimestamps = new Vector();
        for (int i = 0; i < files.length; i++)
        {
            // Leave out directories and files with incorrect extension
            // from the returned file list.
            String filename = FileUtility.getCanonicalPath(
                                  files[i].getAbsolutePath());
            if (files[i].isFile() &&
                    extMatches(filename.toLowerCase(), aExts))
            {
                filenames.addElement(filename);
                fileTimestamps.addElement(new Long(files[i].lastModified()));
            }
        }
        String[] resultFilenames = new String[filenames.size()];
        long[] resultTimestamps = new long[fileTimestamps.size()];
        for (int i = 0; i < filenames.size(); i++)
        {
            resultFilenames[i] = (String)filenames.elementAt(i);
            resultTimestamps[i] = ((Long)fileTimestamps.elementAt(i)).longValue();
        }
        // Construct a FileList object to be returned and sort it by time
        // distance from the file where listing was started from.
        FileList fileList = new FileList(resultFilenames, null, resultTimestamps);
        fileList.initTimeDistances((new FileUtility(aFile)).lastModified());
        fileList.sortByTimeDistance();
        return fileList;
    }

    /**
     * Returns true if given file has one of the specified extensions,
     * false otherwise. Comparison is case sensitive.
     */
    private static boolean extMatches(String aFile, String[] aExts)
    {
        boolean result = false;
        for (int i = 0; i < aExts.length; i++)
        {
            if (aFile.endsWith(aExts[i]))
            {
                result = true;
                break;
            }
        }
        return result;
    }

    /**
     * Returns requested attribute value from given Hashtable,
     * or null if attribute does not exist.
     */
    private static String getAttributeValue(
        Hashtable aAttributes, String aName)
    {
        Attribute attr = (Attribute)aAttributes.get(aName);
        if (attr != null && attr.getValue() != null)
        {
            return attr.getValue().trim();
        }
        return null;
    }

    protected static class MatchData
    {
        private String iName = null;
        private String iVendor = null;
        private String iVersion = null;

        private MatchData()
        {
        }

        private MatchData(String aName, String aVendor, String aVersion)
        {
            iName = aName;
            iVendor = aVendor;
            iVersion = aVersion;
        }

        public static MatchData getMatchData(Hashtable aAttributes)
        {
            return new MatchData(
                       getAttributeValue(aAttributes, "MIDlet-Name"),
                       getAttributeValue(aAttributes, "MIDlet-Vendor"),
                       getAttributeValue(aAttributes, "MIDlet-Version"));
        }

        public int hashCode()
        {
            int result = 0;
            if (iName != null)
            {
                result += iName.hashCode();
            }
            if (iVendor != null)
            {
                result += iVendor.hashCode();
            }
            if (iVersion != null)
            {
                result += iVersion.hashCode();
            }
            return result;
        }

        public boolean equals(Object aObj)
        {
            if (!(aObj instanceof MatchData))
            {
                return false;
            }
            MatchData md = (MatchData)aObj;
            if (equals(this.iName, md.iName) &&
                    equals(this.iVendor, md.iVendor) &&
                    equals(this.iVersion, md.iVersion))
            {
                return true;
            }
            return false;
        }

        public String toString()
        {
            StringBuffer buf = new StringBuffer();
            buf.append("Name: " + iName);
            buf.append(", Vendor: " + iVendor);
            buf.append(", Version: " + iVersion);
            return buf.toString();
        }

        private static boolean equals(String aStr1, String aStr2)
        {
            if ((aStr1 == null && aStr2 == null) ||
                    (aStr1 != null && aStr1.equals(aStr2)))
            {
                return true;
            }
            return false;
        }
    }

    /*** ----------------------------- NATIVE ----------------------------- */
}
