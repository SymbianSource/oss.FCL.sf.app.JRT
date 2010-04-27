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

package com.nokia.mj.impl.utils;

import java.io.InputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

/**
 * JarManifestReader is used to parse Manifest files inside .jar file.
 * This class extends the functionality of
 * com.nokia.mj.impl.utils.ManifestReader
 * by adding possibility to parse from given .jar file.
 *
 * @see com.nokia.mj.impl.utils.ManifestReader
 */
public class JarManifestReader extends com.nokia.mj.impl.utils.ManifestReader
{
    /**
     * Reads Manifest attributes from given jar file.
     * If duplicate attributes are found,
     * the last value of the attribute is used.
     *
     * @param aFilename .jar file to be parsed
     * @return hashtable that contains all attributes read from Manifest file
     * @throws IOException if the file cannot be read or file is not valid
     *  Manifest according to BNF
     */
    public static Hashtable getAttributes(String aFilename) throws IOException
    {
        ZipFile jarFile = null;
        try
        {
            jarFile = new ZipFile(aFilename);
            return (new JarManifestReader()).parseManifest(jarFile, aFilename);
        }
        finally
        {
            if (jarFile != null)
            {
                jarFile.close();
                jarFile = null;
            }
        }
    }

    /*** ---------------------------- PROTECTED --------------------------- */

    /**
     * Default constructor.
     */
    protected JarManifestReader()
    {
        super();
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    private Hashtable parseManifest(ZipFile jarFile, String aFilename)
    throws IOException
    {
        //Log.log("JarManifestReader: Going to parse Manifest file " + aFilename);

        // Get Manifest entry
        ZipEntry manifestEntry = jarFile.getEntry("META-INF/MANIFEST.MF");
        if (manifestEntry == null)
        {
            // Allow also file names in lower cases.
            manifestEntry = jarFile.getEntry("meta-inf/manifest.mf");
            if (manifestEntry == null)
            {
                // The name of the manifest or meta-inf has been written
                // using strange cases. Must scan all entries to find the
                // manifest.
                Enumeration entries = jarFile.entries();
                while (entries.hasMoreElements())
                {
                    manifestEntry = (ZipEntry)entries.nextElement();
                    //Log.log("JarManifestReader.parseManifest: entry name is " +
                    //        manifestEntry.getName());
                    if (manifestEntry.getName().compareToIgnoreCase(
                                "meta-inf/manifest.mf") == 0)
                    {
                        break;
                    }
                    manifestEntry = null;
                }
            }
        }
        if (manifestEntry == null)
        {
            throw new IOException("Manifest was not found in " + aFilename);
        }

        // Do some sanity checks.
        long manifestLength = manifestEntry.getSize();
        // Use this default value in sanity checks if cannot get real amount.
        long freeRamMemory = 1048576; // 1MB
        String tmpStr = System.getProperty("com.nokia.memoryramfree");
        //Log.log("JarManifestReader.parseManifest: com.nokia.memoryramfree is " + tmpStr);
        if ((tmpStr != null) && (tmpStr.length() > 0))
        {
            Integer freeSystemMem = new Integer(tmpStr);
            freeRamMemory = freeSystemMem.longValue();
        }

        // At least 'MIDlet-Name:' must be present.
        if ((manifestLength < 12) && (manifestLength != -1))
        {
            throw new IOException("Manifest is too short.");
        }
        else if (manifestLength > freeRamMemory)
        {
            throw new IOException("Not enough memory to read Manifest.");
        }

        // Choose the buffer size needed for parsing attribute values.
        // If Manifest is small, choose size that is "large enough"
        // (that can contain even the largest attribute value
        // that can be within Manifest).
        int bufSize = (int)manifestLength;
        // Make some sanity checks.
        if (bufSize < 12)
        {
            bufSize = 1024;
        }
        else if (bufSize > MAX_ATTR_VALUE_LEN)
        {
            bufSize = MAX_ATTR_VALUE_LEN;
        }

        // Open file for buffered reading.
        InputStream is = jarFile.getInputStream(manifestEntry);
        // Read the attributes. Manifest files always use
        // UTF-8 character encoding, which is the default for
        // readAttributes method.
        Hashtable result = readAttributes(is, null, bufSize);
        //Log.log("JarManifestReader returns attributes:\n" + result);
        return result;
    }
}
