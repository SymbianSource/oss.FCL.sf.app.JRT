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

import java.io.*;
import java.util.*;

/**
 * Resource loader to get localised strings and Formatter patterns.
 * <br>
 * Usage:
 * <pre>
 *   ResourceLoader res = new ResourceLoader("javainstaller", "qtn_java_installer_");
 *   Label subjectLabel = createLabel(
 *       res.format("subject").arg(certificate.getSubject()).toString(),
 *       horizontalSpan, labelStyle);
 * </pre>
 */
public class ResourceLoader
{
    /** AVKON UI identifier. */
    public static final int AVKON = 1;
    /** QT UI identifier. */
    public static final int QT = 2;

    /** Localisation resource basepath */
    private static final String LOC_RESOURCE_BASE = "/resources/com/nokia/mj/impl/";

    /** Map for ResourceLoader instances. */
    private static Hashtable resourceLoaders = new Hashtable();

    /** Resource string map. Null if resource could not be loaded. */
    private Hashtable resourceMap = new Hashtable();

    /** Resource name prefix */
    private String prefix = null;

    /** Platform localisation type. */
    private int locType = -1;

    /*** ----------------------------- PUBLIC ------------------------------ */

    public static ResourceLoader getInstance(String avkonFileName,
                                             String avkonPrefix,
                                             String qtFileName,
                                             String qtPrefix)
    {
        // Construct key from filenames and prefixes, this is the same
        // between platforms.
        String key = (new StringBuffer()).append(avkonFileName).append(":")
            .append(avkonPrefix).append(":").append(qtFileName).append(":")
            .append(qtPrefix).toString();
        ResourceLoader result = (ResourceLoader)resourceLoaders.get(key);

        if (result == null)
        {
            result = new ResourceLoader(avkonFileName, avkonPrefix, qtFileName, qtPrefix);
            resourceLoaders.put(key, result);
        }
        return result;
    }

    /**
     * Returns a resource loader instance.
     *
     * @param resourceName name of the resource
     * @param prefix prefix added before each id when retrieving
     * @return resource loader instance
     */
    public static ResourceLoader getInstance(String resourceName, String prefix)
    {
        String key = resourceName + ":" + prefix;
        ResourceLoader result = (ResourceLoader)resourceLoaders.get(key);
        if (result == null)
        {
            result = new ResourceLoader(resourceName, prefix);
            resourceLoaders.put(key, result);
        }
        return result;
    }

    /**
     * Private constructor. Loads localisation resource file.
     * On Avkon UI it's resources are loaded. On Qt platfor it's
     * resource is first read and if that fails Avkon one is read.
     *
     * @param avkonFileName Avkon localisation resource file.
     * @param avkonPrefix   Avkon logical string prefix.
     * @param qtFileName    Qt localisation resource file.
     * @param qtPrefix      Qt logical string prefix.
     */
    private ResourceLoader(String avkonFileName,
                           String avkonPrefix,
                           String qtFileName,
                           String qtPrefix)
    {
        String localeId = getLocaleIdQt();

        if (localeId == null)
        {
            locType = AVKON;
            prefix = avkonPrefix;
            loadFile(avkonFileName, true);
        }
        else
        {
            if (!loadFile(qtFileName, false))
            {
                // Fallback to Avkon
                locType = AVKON;
                prefix = avkonPrefix;
                loadFile(avkonFileName, true);
            }
            else
            {
                locType = QT;
                prefix = qtPrefix;
            }
        }
    }

    /**
     * Creates resource loader, using the current locale of the environment.
     *
     * @param resourceName name of the resource
     * @param aPrefix prefix added before each id when retrieving
     */
    ResourceLoader(String resourceName, String aPrefix)
    {
        locType = AVKON;
        prefix = aPrefix;
        loadFile(resourceName, true);  // Avkon
    }

    /**
     * Get a string formatter of a given resource id.
     *
     * @param id resource id
     * @return formatter instance
     * @see Formatter
     */
    public Formatter format(String id)
    {
        return new Formatter(string(id), locType);
    }

    /**
     * Get a string formatter of a given resource id.
     *
     * @param id resource id
     * @return formatter instance
     * @see Formatter
     */
    public Formatter format(Id id)
    {
        return new Formatter(string(id.getString(locType)), locType);
    }

    /**
     * Formats localised text with specified parameters from an array.
     *
     * @param id resource id
     * @param textParameters parameters to be filled into the text
     * @return localised text formatted with the provided parameters
     * @see Formatter
     */
    public String format(String id, Object[] textParameters)
    {
        return new Formatter(string(id), locType).format(textParameters);
    }

    /**
     * Formats localised text with specified parameters from an array.
     *
     * @param id resource id
     * @param textParameters parameters to be filled into the text
     * @return localised text formatted with the provided parameters
     * @see Formatter
     */
    public String format(Id id, Object[] textParameters)
    {
        return new Formatter(string(id.getString(locType)), locType).format(textParameters);
    }


    /**
     * Gets the locale ID currently being used on the phone. This can be used
     * e.g. to load a localized icon file, by adding the locale id as suffix.
     *
     * @return Locale ID as provided by the platform
     */
    public String getLocaleId()
    {
        int localeId = _getLocaleId();
        if (localeId > 0)
        {
            if (localeId < 10)
            {
                // Ensure that the returned locale ID has at least two digits.
                return "0" + Integer.toString(localeId);
            }
            else
            {
                return Integer.toString(localeId);
            }
        }
        return "sc";
    }

    /**
     * Return locale id string on Qt platform.
     *
     * @return Qt Locale Id String, null if not in Qt.
     */
    public static String getLocaleIdQt()
    {
        return _getLocaleIdQt();
    }


    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Get a plain string resource with a given resource id.
     *
     * @param id resource id, either with prefix or without
     * @return resource string, or the id if does not exist
     */
    private String string(String id)
    {
        String str = (String)resourceMap.get(id);
        if (str == null)
        {
            // Try with prefix
            str = (String)resourceMap.get(prefix + id);
            if (str == null)
            {
                // Not found even with prefix. Use the id itself
                if (!id.startsWith(prefix))
                {
                    str = prefix + id;
                }
                else
                {
                    str = id;
                }

                Logger.WLOG(Logger.EUtils, "Cannot find resource: " + id);
            }

            // Put back to hash with original key for quick retrieval
            resourceMap.put(id, str);
        }

        str = decode(str);
        str = replaceCharacterCodes(str);

        return str;
    }

    /**
     * Loads the resources from .loc type file.
     *
     * @param resourceName name of the resource file.
     * @param aIs InputStream pointing to resource. It will be closed after use.
     * @param true if operation succeed.
     */
    private boolean loadFile(String resourceName, boolean avkon)
    {
        InputStream is = null;

        if (!avkon)  // Qt resources.
        {
            String langName = getLocaleIdQt();

            // Emulator returns falsely en_GB as engineering English locale name.
            if (langName.equals("en_GB"))
            {
                langName = "en";
            }

            // Load with real locale id
            is = this.getClass().getResourceAsStream(
                LOC_RESOURCE_BASE + resourceName + "_" + langName + ".loc");

            if (is == null)
            {
                /*
                 * Does not exist. No need to continue as avkon file cannot
                 * found using qt name.
                 */
                return false;
            }
        }
        else  // Avkon resources.
        {
            // Load with real locale id
            is = this.getClass().getResourceAsStream(
                 LOC_RESOURCE_BASE + resourceName + "_" + getLocaleId() + ".loc");

            if (is == null)
            {
                // Load the engineering english
                is = this.getClass().getResourceAsStream(
                         LOC_RESOURCE_BASE + resourceName + "_sc" + ".loc");
            }
            if (is == null)
            {
                // Load the reference engineering english
                is = this.getClass().getResourceAsStream(
                         LOC_RESOURCE_BASE + resourceName + ".loc");
            }
            if (is == null)
            {
                Logger.WLOG(Logger.EUtils,
                            "Cannot load resource file: " + resourceName);
                return false;
            }
        }

        try
        {
            // Loc-files area always on UTF8 format
            LineReader lr = new LineReader(
                new BufferedReader(new InputStreamReader(is, "UTF-8")));
            String line;

            while ((line = lr.readLine()) != null)
            {
                // Ignore lines which are not #define's
                if (!line.startsWith("#define "))
                {
                    continue;
                }
                try
                {
                    // Skip "#define" + any whitespace
                    line = line.substring(line.indexOf(' ')).trim();

                    int idEnd = line.indexOf(' ');
                    String id = line.substring(0, idEnd);

                    int strStart = line.indexOf('"', idEnd);
                    int strEnd = line.lastIndexOf('"');
                    String str = line.substring(strStart + 1, strEnd);

                    resourceMap.put(id, str);

                }
                catch (IndexOutOfBoundsException ex)
                {
                    String error = "Incorrect line " + lr.getLineNumber() + "\"" +
                                   line + "\"";
                    Logger.WLOG(Logger.EUtils, error);
                }
            }
            is.close();

        }
        catch (IOException ex)
        {
            Logger.WLOG(Logger.EUtils,
                        "Resource file " + resourceName + " handling failed: "
                        + ex.getMessage());
        }

        return true;
    }

    /**
     * Decode given string. Decoding means unescaping escaped characters.
     * Currently \n, \t, \', \\ and \" patterns are decoded to respective
     * characters.
     *
     * @param str to be decoded.
     * @return decoded String.
     */
    private String decode(String str)
    {
        str = replacePattern(str, "\\n", '\n');
        str = replacePattern(str, "\\\\", '\\');
        str = replacePattern(str, "\\\"", '\"');
        str = replacePattern(str, "\\t", '\t');
        str = replacePattern(str, "\\'", '\'');

        return str;
    }

    /**
     * Replace all occurrences of the pattern in the given String.
     *
     * @param resource string to be replaced.
     * @param pattern to replace.
     * @param replacement replacement character.
     * @return String where all occurrences of the pattern are replaced.
     */
    private String replacePattern(
        String resource, String pattern, char replacement)
    {
        StringBuffer sb = new StringBuffer();

        int startIndex = resource.indexOf(pattern);
        if (startIndex != -1)
        {
            sb.append(resource.substring(0, startIndex)).append(replacement);
            startIndex = startIndex + pattern.length();
            int endIndex = 0;

            while ((endIndex = resource.indexOf(pattern, startIndex)) != -1)
            {
                sb.append(resource.substring(startIndex, endIndex))
                .append(replacement);
                startIndex = endIndex + pattern.length();
            }

            if (startIndex < resource.length())
            {
                sb.append(resource.substring(startIndex, resource.length()));
            }
            return sb.toString();
        }
        return resource;
    }

    /**
     * Replace character codes. They are given as <0x0000> format. Where 0x0000
     * contains character unicode as hex representation.
     *
     * @param str to replace characters.
     * @return String where characters are replaced.
     */
    private String replaceCharacterCodes(String str)
    {
        StringBuffer sb = new StringBuffer();
        int startIndex = str.indexOf("<0x");

        if (startIndex != -1 && str.charAt(startIndex + 7) == '>')
        {
            sb.append(str.substring(0, startIndex));
            try
            {
                int charint = Integer.parseInt(
                                  str.substring(startIndex + 3, startIndex + 7), 16);
                sb.append((char) charint);

                int endIndex = 0;
                startIndex+= 7;

                while ((endIndex = str.indexOf("<0x", startIndex)) != -1
                        && str.charAt(endIndex + 7) == '>')
                {
                    sb.append(str.substring(startIndex + 1, endIndex));

                    charint = Integer.parseInt(
                                  str.substring(endIndex + 3, endIndex + 7), 16);
                    sb.append((char) charint);
                    startIndex = endIndex + 7;
                }
            }
            catch (NumberFormatException nfe)
            {
                Logger.ELOG(Logger.EUtils,
                            "Cannot replace character from string: " + str);
                return str;
            }

            if (startIndex < str.length())
            {
                sb.append(str.substring(startIndex + 1, str.length()));
            }
            return sb.toString();
        }
        return str;
    }


    /*** ----------------------------- NATIVE ----------------------------- */

    /**
     * Get device language code.
     *
     * @return languege code.
     */
    private native int _getLocaleId();

    /**
     * Get Locale Id on Qt platform.
     *
     * @return locale Id string. If not in Qt null.
     */
    private static native String _getLocaleIdQt();

}
