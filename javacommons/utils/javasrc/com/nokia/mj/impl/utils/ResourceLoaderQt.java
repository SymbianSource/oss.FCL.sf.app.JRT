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


package com.nokia.mj.impl.utils;

import com.nokia.mj.impl.coreui.CoreUi;
import java.util.Hashtable;


/**
 * Resource loader to get localised strings and Formatter patterns.
 * <br>
 * Usage:
 * <pre>
 *   ResourceLoader res = ResourceLoader.getInstance("javaapplicationinstaller", "txt_java_inst_");
 *   Label subjectLabel = createLabel(
 *       res.format("subject").arg(certificate.getSubject()).toString(),
 *       horizontalSpan, labelStyle);
 * </pre>
 * @see ResourceLoader
 */
class ResourceLoaderQt extends ResourceLoader
{
    /**
     * Resource string map for the resources that have been fetched
     * at least once.
     */
    private Hashtable iResourceMap = new Hashtable();

    /** Resource name passed when ResourceLoader is constructed. */
    private String iResourceName = null;

    /** Array of resource names. */
    private String[] iResourceNames = null;

    /** Resource name prefix passed when ResourceLoader is constructed. */
    private String iPrefix = null;

    /** Array of resource name prefixes. */
    private String[] iPrefixes = null;

    /** Array of handles to native translator objects. */
    private int[] iTranslators = null;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Returns a resource loader instance.
     *
     * @param avkonFileName this parameter is ignored
     * @param avkonPrefix this parameter is ignored
     * @param qtFileName name of the Qt resource file
     * @param qtPrefix prefix added before each id when retrieving
     * @return resource loader instance
     */
    public static ResourceLoader getInstance(String avkonFileName,
                                             String avkonPrefix,
                                             String qtFileName,
                                             String qtPrefix)
    {
        return getInstance(qtFileName, qtPrefix);
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
            result = new ResourceLoaderQt(resourceName, prefix);
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
        return new FormatterQt(this, id);
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
        return new FormatterQt(this, id.getString(QT));
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
        return new FormatterQt(this, id).format(textParameters);
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
        return new FormatterQt(this, id.getString(QT)).format(textParameters);
    }


    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Releases resources and destroys this ResourceLoader instance.
     */
    void destroy()
    {
        deleteTranslators();
    }

    /**
     * Get a plain string resource with a given resource id.
     *
     * @param id resource id, either with prefix or without
     * @param n used to identify plural forms
     * @return resource string, or the id if does not exist
     */
    synchronized String string(final String id, final int n)
    {
        String str = (String)iResourceMap.get(id+n);
        if (str == null)
        {
            str = getResourceStringByList(id, n);
            // Put to resource map with original key for quick retrieval.
            iResourceMap.put(id, str+n);
        }
        return str;
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Creates resource loader, using the current locale of the environment.
     *
     * @param resourceName name of the resource
     * @param aPrefix prefix added before each id when retrieving
     */
    private ResourceLoaderQt(String aResourceName, String aPrefix)
    {
        setResourceName(aResourceName);
        setPrefix(aPrefix);
        if (!loadTranslators())
        {
            throw new IllegalArgumentException(
                "Translations not found for " + aResourceName);
        }
    }

    /**
     * Initializes member variables according to given resource name.
     *
     * @param aResourceName comma separated list of resource names
     */
    private void setResourceName(String aResourceName)
    {
        iResourceName = aResourceName;
        iResourceNames = Tokenizer.split(iResourceName, SEPARATOR);
        if (iResourceNames == null)
        {
            iResourceNames = new String[0];
        }
        for (int i = 0; i < iResourceNames.length; i++)
        {
            iResourceNames[i] = iResourceNames[i].trim();
        }
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
     * Initializes iTranslators array from given resource files.
     *
     * @param true if operation succeeds.
     */
    private boolean loadTranslators()
    {
        if (iResourceName == null)
        {
            return false;
        }
        iTranslators = new int[iResourceNames.length];
        CoreUi.runInSyncUiThread(new Runnable() {
            public void run()
            {
                for (int i = 0; i < iResourceNames.length; i++)
                {
                    iTranslators[i] = _createTranslator(iResourceNames[i]);
                    if (iTranslators[i] < 0)
                    {
                        Logger.ILOG(Logger.EUtils,
                                    "ResourceLoaderQt: Cannot load resource file " +
                                    iResourceNames[i] + ", error: " +
                                    iTranslators[i]);
                    }
                }
            }
        });
        boolean result = false;
        for (int i = 0; i < iTranslators.length; i++)
        {
            if (iTranslators[i] > 0)
            {
                // Return true if loading succeeds for at least one translator.
                result = true;
                break;
            }
        }
        return result;
    }

    /**
     * Deletes translator from iTranslators array.
     */
    private void deleteTranslators()
    {
        CoreUi.runInSyncUiThread(new Runnable() {
            public void run()
            {
                for (int i = 0; i < iTranslators.length; i++)
                {
                    if (iTranslators[i] <= 0)
                    {
                        continue;
                    }
                    int result = _deleteTranslator(iTranslators[i]);
                    if (result < 0)
                    {
                        Logger.WLOG(Logger.EUtils,
                                    "ResourceLoaderQt: Deleting translator for resource " +
                                    iResourceNames[i] + " failed, error: " +
                                    result);
                    }
                }
            }
        });
    }

    /**
     * Get a string with a given comma separated resource id list.
     *
     * @param id comma separated resource id list, either with prefix or without
     * @param n used to identify plural forms
     * @return resource string, or the id if does not exist
     */
    private String getResourceStringByList(String id, int n)
    {
        String str = null;
        String[] ids = Tokenizer.split(id, SEPARATOR);
        String strIn = null;
        String strOut = null;
        for (int i = 0; i < ids.length && str == null; i++)
        {
            for (int j = 0; j < iTranslators.length && str == null; j++)
            {
                if (iTranslators[j] <= 0)
                {
                    // Skip invalid translator handles.
                    continue;
                }
                // Check if id already has prefix.
                if (iPrefixes != null && iPrefixes.length > j &&
                    !ids[i].startsWith(iPrefixes[j]))
                {
                    // Try with prefix and id.
                    strIn = iPrefixes[j] + ids[i];
                }
                else
                {
                    // Try with id.
                    strIn = ids[i];
                }
                strOut = _translate(iTranslators[j], strIn, n);
                if (!strIn.equals(strOut))
                {
                    // Translation was found.
                    str = strOut;
                }
            }
        }
        if (str == null)
        {
            // Not found. Use the id itself.
            str = id;
            Logger.WLOG(Logger.EUtils,
                        "ResourceLoaderQt: Cannot find resource: " + id +
                        ", prefix: " + iPrefix);
        }
        return str;
    }

    /*** ----------------------------- NATIVE ----------------------------- */

    /**
     * Creates native translator object.
     *
     * @param aResourceFilename resource filename
     * @return handle to native side translator object, or negative
     * error code
     */
    private static native int _createTranslator(String aResourceFilename);

    /**
     * Deletes native translator object.
     *
     * @param handle to native side translator object
     * @return 0 if deletion was successful, or negative error code
     */
    private static native int _deleteTranslator(int aHandle);

    /**
     * Fetches localized text for given text id.
     *
     * @param aHandle translator to be used
     * @param aId text id
     * @param aN used to identify plural forms
     * @return localized text, or aId if text is not found
     */
    private static native String _translate(int aHandle, String aId, int aN);
}
