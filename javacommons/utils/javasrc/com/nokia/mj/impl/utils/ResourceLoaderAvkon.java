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
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.Hashtable;

/**
 * Resource loader to get localised strings and Formatter patterns.
 * <br>
 * Usage:
 * <pre>
 *   ResourceLoader res = ResourceLoader.getInstance("javainstaller", "qtn_java_installer_");
 *   Label subjectLabel = createLabel(
 *       res.format("subject").arg(certificate.getSubject()).toString(),
 *       horizontalSpan, labelStyle);
 * </pre>
 * @see ResourceLoader
 */
class ResourceLoaderAvkon extends ResourceLoader
{
    /** Localisation resource basepath */
    private static final String LOC_RESOURCE_BASE = "/resources/com/nokia/mj/impl/";

    /**
     * Resource string map for the resources that have been fetched
     * at least once.
     */
    private Hashtable iResourceMap = new Hashtable();

    /**
     * Resource string maps for resources read from each resource file
     * specified in comma separated list of resource file names.
     */
    private Hashtable[] iResourceMaps = null;

    /** Resource name prefix passed when ResourceLoader is constructed. */
    private String iPrefix = null;

    /** Array of resource name prefixes. */
    private String[] iPrefixes = null;

    /** Platform localisation type. */
    private int iLocType = -1;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Returns a resource loader instance.
     *
     * @param avkonFileName name of the Avkon resource file
     * @param avkonPrefix prefix added before each id when retrieving
     * @param qtFileName name of the Qt resource file
     * @param qtPrefix prefix added before each id when retrieving
     * @return resource loader instance
     */
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
        ResourceLoader result = (ResourceLoader)iResourceLoaders.get(key);
        if (result == null)
        {
            result = new ResourceLoaderAvkon(
                avkonFileName, avkonPrefix, qtFileName, qtPrefix);
            iResourceLoaders.put(key, result);
        }
        return result;
    }

    /**
     * Returns a resource loader instance.
     *
     * @param resourceName comma separated list of resource file names
     * @param prefix comma separated list of prefixes added before each
     * id when retrieving
     * @return resource loader instance
     */
    public static ResourceLoader getInstance(String resourceName, String prefix)
    {
        String key = resourceName + ":" + prefix;
        ResourceLoader result = (ResourceLoader)iResourceLoaders.get(key);
        if (result == null)
        {
            result = new ResourceLoaderAvkon(resourceName, prefix);
            iResourceLoaders.put(key, result);
        }
        return result;
    }

    /**
     * Get a string formatter of a given resource id.
     *
     * @param id comma separated list of resource ids
     * @return formatter instance
     * @see Formatter
     */
    public Formatter format(String id)
    {
        return new FormatterAvkon(string(id), iLocType);
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
        return new FormatterAvkon(string(id.getString(iLocType)), iLocType);
    }

    /**
     * Formats localised text with specified parameters from an array.
     *
     * @param id comma separated list of resource ids
     * @param textParameters parameters to be filled into the text
     * @return localised text formatted with the provided parameters
     * @see Formatter
     */
    public String format(String id, Object[] textParameters)
    {
        return new FormatterAvkon(string(id), iLocType).format(textParameters);
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
        return new FormatterAvkon(string(id.getString(iLocType)), iLocType).format(textParameters);
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
        String str = (String)iResourceMap.get(id);
        if (str == null)
        {
            str = getResourceStringByList(id);
            // Put to resource map with original key for quick retrieval.
            str = decode(str);
            str = replaceCharacterCodes(str);
            iResourceMap.put(id, str);
        }
        return str;
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
    private ResourceLoaderAvkon(String avkonFileName, String avkonPrefix,
                                String qtFileName, String qtPrefix)
    {
        String localeId = getLocaleIdQt();
        if (localeId == null)
        {
            iLocType = AVKON;
            setPrefix(avkonPrefix);
            loadFiles(avkonFileName, true);
        }
        else
        {
            if (!loadFiles(qtFileName, false))
            {
                // Fallback to Avkon
                iLocType = AVKON;
                setPrefix(avkonPrefix);
                loadFiles(avkonFileName, true);
            }
            else
            {
                iLocType = QT;
                setPrefix(qtPrefix);
            }
        }
    }

    /**
     * Creates resource loader, using the current locale of the environment.
     *
     * @param resourceName name of the resource
     * @param aPrefix prefix added before each id when retrieving
     */
    private ResourceLoaderAvkon(String resourceName, String aPrefix)
    {
        iLocType = AVKON;
        setPrefix(aPrefix);
        loadFiles(resourceName, true); // Avkon
    }

    /**
     * Initializes member variables according to given prefix.
     *
     * @param aPrefix comma separated list of text id prefixes
     */
    private void setPrefix(String aPrefix)
    {
        iPrefix = aPrefix;
        iPrefixes = Tokenizer.split(iPrefix, SEPARATOR);
        if (iPrefixes == null)
        {
            iPrefixes = new String[0];
        }
        for (int i = 0; i < iPrefixes.length; i++)
        {
            iPrefixes[i] = iPrefixes[i].trim();
        }
    }

    /**
     * Loads the resources from .loc type files.
     *
     * @param resourceNames comma separated list of resource filenames.
     * @param avkon true if Avkon based localisation is in use
     * @param true if operation succeed.
     */
    private boolean loadFiles(String resourceNames, boolean avkon)
    {
        if (resourceNames == null)
        {
            return false;
        }
        boolean result = false;
        String[] filenames = Tokenizer.split(resourceNames, SEPARATOR);
        iResourceMaps = new Hashtable[filenames.length];
        for (int i = 0; i < filenames.length; i++)
        {
            iResourceMaps[i] = new Hashtable();
            if (loadFile(filenames[i].trim(), avkon, iResourceMaps[i]))
            {
                // Return true if loading succeeds for at least one file.
                result = true;
            }
        }
        return result;
    }

    /**
     * Loads the resources from .loc type file.
     *
     * @param resourceName name of the resource file
     * @param avkon true if Avkon based localisation is in use
     * @param resourceMap Map where the resources are stored to
     * @param true if operation succeed.
     */
    private boolean loadFile(String resourceName, boolean avkon, Hashtable resourceMap)
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
                // Load the reference without locale id
                is = this.getClass().getResourceAsStream(
                         LOC_RESOURCE_BASE + resourceName + ".loc");
            }
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
                            "ResourceLoaderAvkon: Cannot load resource file: " +
                            resourceName);
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

                    str = decode(str);
                    str = replaceCharacterCodes(str);
                    resourceMap.put(id, str);

                }
                catch (IndexOutOfBoundsException ex)
                {
                    String error = "ResourceLoaderAvkon: Incorrect line " +
                        resourceName + ":" + lr.getLineNumber() + "\"" +
                        line + "\"";
                    Logger.WLOG(Logger.EUtils, error);
                }
            }
            is.close();

        }
        catch (IOException ex)
        {
            Logger.WLOG(Logger.EUtils,
                        "ResourceLoaderAvkon: Resource file " + resourceName +
                        " handling failed: " + ex.getMessage());
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
                startIndex += 7;

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
                            "ResourceLoaderAvkon: Cannot replace character from string: " +
                            str);
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

    /**
     * Get a string from resource map with a given comma
     * separated resource id list.
     *
     * @param id comma separated resource id list, either with prefix or without
     * @return resource string, or the id if does not exist
     */
    private String getResourceStringByList(String id)
    {
        String str = null;
        String[] ids = Tokenizer.split(id, SEPARATOR);
        for (int i = 0; i < ids.length && str == null; i++)
        {
            // Search text ids from resource maps.
            for (int j = 0; j < iResourceMaps.length && str == null; j++)
            {
                str = (String)iResourceMaps[j].get(ids[i]);
                if (str == null && iPrefixes != null && iPrefixes.length > j)
                {
                    // Try with prefix.
                    str = (String)iResourceMaps[j].get(
                        iPrefixes[j] + ids[i]);
                }
            }
        }
        if (str == null)
        {
            // Not found. Use the id itself.
            str = id;
            Logger.WLOG(Logger.EUtils,
                        "ResourceLoaderAvkon: Cannot find resource: " + id +
                        ", prefix: " + iPrefix);
        }
        return str;
    }
}
