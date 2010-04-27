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
 *
 *   Label noteLabel = createLabel(
 *       res.string("note"), horizontalSpan, labelStyle);
 * </pre>
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public class ResourceLoader
{
    /** Localisation resource basepath */
    private static final String LOC_RESOURCE_BASE = "/resources/com/nokia/mj/impl/";

    /** Map for ResourceLoader instances. */
    private static Hashtable resourceLoaders = new Hashtable();

    /** Resource string map. Null if resource could not be loaded. */
    private Hashtable resourceMap = new Hashtable();

    /** Resource name prefix */
    private String prefix;

    /*** ----------------------------- PUBLIC ------------------------------ */

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
     * Creates resource loader, using the current locale of the environment.
     *
     * @param resourceName name of the resource
     * @param aPrefix prefix added before each id when retrieving
     */
    public ResourceLoader(String resourceName, String aPrefix)
    {
        prefix = aPrefix;
        loadFile(resourceName);
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
        return new Formatter(string(id));
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
        return new Formatter(string(id)).format(textParameters);
    }

    /**
     * Get a plain string resource with a given resource id.
     *
     * @param id resource id, either with prefix or without
     * @return resource string, or the id if does not exist
     */
    public String string(String id)
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

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Loads the resources from .loc type file
     */
    private void loadFile(String resourceName)
    {
        InputStream is = null;

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
            return;
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

}