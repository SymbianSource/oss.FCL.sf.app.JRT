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


package com.nokia.mj.impl.rt.utils;

import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.Vector;
import java.util.Enumeration;

import com.nokia.mj.impl.rt.support.JvmInternal;
import com.nokia.mj.impl.rt.midp.Log;
import com.nokia.mj.impl.utils.LineReader;
import com.nokia.mj.impl.fileutils.FileUtility;

/**
 * This class provides utilities for handling the add-on JSRs
 *
 * The add-on JSR always has one or more ODC files.
 * Each ODC file defines the JAR/DLL names and packages. Each ODC file
 * is added to the classpath (instead of JARs), while JAR files are
 * located in "odc" subdirectory relative to the odc file. ODC files
 * defines two system properties, as follows:
 *   -Dcom.nokia.mj.addon.protected.<odcname>=<comma separated list
 *                                             of protected packages>
 *   -Dcom.nokia.mj.addon.restricted.<odcname>=<comma separated list
 *                                             of restricted packages>
 * The <odcname> is always the odc filename without path and extension.
 * (same list with full paths and extensions are defined as
 * -bootclasspath/p parameter).
 *
 * The starter will define system property (from central repository)
 *   -Dcom.nokia.mj.addon.list=<semicolon separated list of odcfiles
 *                              with full path names>.
 *
 * On runtime, the protected and restricted packages/classes are added
 * to the JVM's protected/restricted set.
 *
 * The J9 JVM is not able to read the system properties from the odc
 * file added to boot classpath. This class provides utility for reading
 * the system properties from the odc files.
 */

public final class ExtensionUtil
{
    private final static String ODC_EXTENSIONS = ".odc";
    private final static String PROTECTED_PACKAGE_PROPERTY =
        "com.nokia.mj.addon.protected.";
    private final static String RESTRICTED_PACKAGE_PROPERTY =
        "com.nokia.mj.addon.restricted.";

    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Sets the protected/restricted packages and the system properties of
     * the add-on JSRs if present.
     */
    public static void handleExtensions()
    {
        if (Log.mOn) Log.logI("Handling extensions.");
        // Get the whole list of odc files who define protected/restricted
        // packages. E.g. the odcList could be
        // c:\foo\eswtmobile.odc;c:\foo\eswtexpanded.odc;c:\foo\eswtcore.odc
        String odcList = System.getProperty("com.nokia.mj.addon.list");
        if (odcList != null)
        {
            if (Log.mOn) Log.logI(" ODC list: " + odcList);
            //Split the list to vector
            Vector result = split(odcList, ';');
            Enumeration odcFiles = result.elements();
            while (odcFiles.hasMoreElements())
            {
                String odcFile = (String)odcFiles.nextElement();

                // Support also plain .jar extensions (for Java 1.3 compatibility)
                if (odcFile.endsWith(ODC_EXTENSIONS))
                {
                    setAddOnSystemProperties(odcFile);
                }
            }
        }
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Adds the system properties of the add-on JSRs. This method will read
     * all the properties from the odc file of the add-on JSR.
     *
     * @param odcFile absolute path to odcfile.
     */
    private static void setAddOnSystemProperties(String odcFile)
    {
        if (Log.mOn) Log.logI("  Setting system properties: " + odcFile);

        InputStream fis = null;
        try
        {
            // Open the file.
            fis = new FileUtility(odcFile).openInputStream();

            // Use LineReader to read the content line by line.
            LineReader lr = new LineReader(new InputStreamReader(fis));
            String line;

            // Ignore lines until we reach to [properties] line or to the
            // end of file.
            while ((line = lr.readLine()) != null)
            {
                if (!line.startsWith("[properties]"))
                {
                    continue;
                }
                else
                {
                    break;
                }
            }

            // Loop the reat of lines an consider properties only those that
            // do start with '-D'
            while ((line = lr.readLine()) != null)
            {
                // Ignore lines that doesn't start with -D
                if (!line.startsWith("-D"))
                {

                    continue;
                }

                int ind = line.indexOf('=');
                String propertyKey = null;
                String propertyValue = "";
                if (ind == -1)
                {
                    // The property doesn't have a value.
                    propertyKey = line.substring(2);
                }
                else
                {
                    // The property has also a value.
                    propertyKey = line.substring(2,ind);
                    propertyValue = line.substring(ind + 1);
                }
                if (Log.mOn) Log.logI("   Property found. Key: '"+propertyKey+
                                          "', value: '" + propertyValue + "'");

                // Store the found property.
                JvmInternal.setSystemProperty(propertyKey, propertyValue);
            }
        }
        catch (IOException ioe)
        {
            Log.logE("Error in opening file: " + odcFile, ioe);
        }

        // Close the file.
        try
        {
            if (fis != null)
            {
                fis.close();
                fis = null;
            }
        }
        catch (IOException ioe)
        {
            Log.logE("setAddOnSystemProperties: error while closing file " +
                     odcFile, ioe);
        }
    }

    /**
     * This method will split the text into Vector.
     *
     * @param text       String to be splitted.
     * @param delimiter  The delimiter that is used to separate the text
     *                   partitions.
     */
    private static Vector split(String text, char delimiter)
    {
        if (text == null || text.equals(""))
        {
            return null;
        }

        //Split the content to String array.
        int startIndex = 0;
        boolean loop = true;
        Vector result = new Vector(10);
        while (loop)
        {
            String splitString = null;
            int endIndex = text.indexOf(delimiter, startIndex);
            if (endIndex == -1)
            {
                // No more delimiters found - store the last found sub string.
                splitString = text.substring(startIndex);
                loop = false;
            }
            else
            {
                // Delimiter found - store the sub string to array and look for
                // next one delimiter.
                splitString = text.substring(startIndex, endIndex);
                startIndex = endIndex + 1;
            }

            // Store the found sub string into array.
            result.addElement(splitString.trim());
        }
        return result;
    }
}

