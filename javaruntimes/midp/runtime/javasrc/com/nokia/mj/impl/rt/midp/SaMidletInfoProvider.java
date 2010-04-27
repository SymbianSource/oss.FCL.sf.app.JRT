/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.rt.midp;

import java.io.InputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;

import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;
import com.nokia.mj.impl.utils.JadReader;
import com.nokia.mj.impl.utils.ManifestReader;
import com.nokia.mj.impl.utils.MidpAttributeValidator;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * A utility class for reading the standalone MIDlet specfic data from Jar and
 * Jad file (if Jad provided).
 *
 * @author Nokia Corporation
 * @version $Rev$
 */
final class SaMidletInfoProvider
{

    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Sets the MidletInfo to contain a set of static data related to the
     * MIDlet.
     * @param midletInfo where to store the data.
     */
    static void setMidletStartArguments(MidletInfo midletInfo)
    {
        String[] tokens = Tokenizer.split(midletInfo.getAttribute("MIDlet-1"), ",");

        // Set the main class of the MIDlet.
        midletInfo.setMainClass(tokens[2].trim());

        // Set the name of the MIDlet.
        midletInfo.setName(tokens[0].trim());

        // Set the UID of the MIDlet suite.
        midletInfo.setSuiteUid(midletInfo.getUid());

        // Set the suite name of the MIDlet suite.
        midletInfo.setSuiteName(midletInfo.getName());

        // Set the vendor of the MIDlet suite.
        midletInfo.setVendor(midletInfo.getAttribute("MIDlet-Vendor"));

        // Set the version of the MIDlet suite.
        midletInfo.setVersion(midletInfo.getAttribute("MIDlet-Version"));

        // Set the protection domain of the MIDlet suite.
        midletInfo.setProtectionDomain(ApplicationInfo.MANUFACTURER_DOMAIN);

        // Set the drm info.
        midletInfo.setDrm(false);
    }

    /**
     * Sets the MidletInfo to contain MIDlet attributes stored from manifest
     * and jad file (if exists) during installation.
     * @param midletInfo where to store the attributes.
     */
    static void setMidletAttributes(MidletInfo midletInfo, String jadFile)
    {
        Hashtable jarAttributes = null;
        Hashtable jadAttributes = null;
        try
        {
            InputStream is = midletInfo.getClass().getResourceAsStream("/META-INF/MANIFEST.MF");
            Log.logE("MIKKO is = " + is);
            jarAttributes = ManifestReader.getAttributes(is);
            is.close();
            is = null;
            if (jadFile != null)
            {
                try
                {
                    FileUtility fileUtil = new FileUtility(jadFile);
                    is = fileUtil.openInputStream();
                    jadAttributes = JadReader.getAttributes(is);
                    fileUtil = null;
                }
                catch (IOException ioe2)
                {
                    Log.logE("Error setting MIDlet attributes(jad) ", ioe2);
                    throw new StartupException("Error in start app: Getting MIDlet attributes from jad.", true);
                }
                finally
                {
                    if (is != null)
                    {
                        is.close();
                        is = null;
                    }
                }
            }
        }
        catch (IOException ioe)
        {
            Log.logE("Error setting MIDlet attributes ", ioe);
            throw new StartupException("Error in start app: Getting MIDlet attributes.", true);
        }
        try
        {
            MidpAttributeValidator validator = new MidpAttributeValidator();
            Enumeration e = validator.combine(jarAttributes, jadAttributes, false).elements();
            while (e != null && e.hasMoreElements())
            {
                Attribute attr = (Attribute)e.nextElement();
                midletInfo.addAttribute(attr.getName(), attr.getValue());
            }
        }
        catch (InvalidAttributeException iae)
        {
            Log.logE("Error validating MIDlet attributes ", iae);
            throw new StartupException("Error in start app: Validating MIDlet attributes. ", true);
        }
    }
}
