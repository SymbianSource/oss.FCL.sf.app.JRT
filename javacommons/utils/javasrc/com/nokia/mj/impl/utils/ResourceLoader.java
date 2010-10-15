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

import com.nokia.mj.impl.coreui.CoreUi;

import java.util.Enumeration;
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
 * <br>
 * It is possible to pass a comma separated list of text ids to format
 * method and also to pass a comma separated list of resource filenames
 * and text id prefixes to getInstance method. In this case the texts
 * are searched in the order they are listed and the first text
 * that is found will be returned. This can be used to specify texts
 * for different platforms in a single parameter, provided that the
 * text ids, prefixes and resource filenames are unique in each platform.
 * <br>
 * If the text ids, prefixes and resource filenames are not unique
 * in each platform, then a separate ResourceLoader instance must
 * be obtained for every platform.
 * <br>
 * When more than one resource filenames and text id prefixes
 * are used, the Nth prefix is applied only to the Nth resource file.
 * This means that the number of comma separated resource filenames and
 * prefixes must be the same.
 * <br>
 * If the localised text for given id is not found, ResourceLoader
 * returns the id itself.
 */
abstract public class ResourceLoader
{
    /** AVKON UI identifier. */
    protected static final int AVKON = 1;
    /** QT UI identifier. */
    protected static final int QT = 2;

    /** Separator in text id, resource filename, and prefix strings. */
    protected static final String SEPARATOR = ",";

    /** Map for ResourceLoader instances. */
    protected static Hashtable iResourceLoaders = new Hashtable();

    /**
     * Flag telling if ResourceLoader has already ensured that
     * UI thread exists.
     */
    private static boolean iUiThreadExists = false;

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
    synchronized public static ResourceLoader getInstance(
        String avkonFileName, String avkonPrefix,
        String qtFileName, String qtPrefix)
    {
        ResourceLoader rl = null;
        if (getLocaleIdQt() == null || qtFileName == null)
        {
            rl = ResourceLoaderAvkon.getInstance(avkonFileName, avkonPrefix);
        }
        else
        {
            try
            {
                rl = ResourceLoaderQt.getInstance(qtFileName, qtPrefix);
            }
            catch (Throwable t)
            {
                Logger.WLOG(Logger.EUtils,
                            "ResourceLoader: Creating ResourceLoaderQt for " +
                            qtFileName + " failed: " + t);
            }
            if (rl == null)
            {
                rl = ResourceLoaderAvkon.getInstance(
                    avkonFileName, avkonPrefix, qtFileName, qtPrefix);
            }
        }
        return rl;
    }

    /**
     * Returns a resource loader instance.
     *
     * @param resourceName comma separated list of resource file names
     * @param prefix comma separated list of prefixes added before each
     * id when retrieving
     * @return resource loader instance
     */
    synchronized public static ResourceLoader getInstance(
        String resourceName, String prefix)
    {
        if (getLocaleIdQt() == null)
        {
            return getInstance(resourceName, prefix, null, null);
        }
        else
        {
            return getInstance(resourceName, prefix, resourceName, prefix);
        }
    }

    /**
     * Get a string formatter of a given resource id.
     *
     * @param id comma separated list of resource ids
     * @return formatter instance
     * @see Formatter
     */
    public abstract Formatter format(String id);

    /**
     * Get a string formatter of a given resource id.
     *
     * @param id resource id
     * @return formatter instance
     * @see Formatter
     */
    public abstract Formatter format(Id id);

    /**
     * Formats localised text with specified parameters from an array.
     *
     * @param id comma separated list of resource ids
     * @param textParameters parameters to be filled into the text
     * @return localised text formatted with the provided parameters
     * @see Formatter
     */
    public abstract String format(String id, Object[] textParameters);

    /**
     * Formats localised text with specified parameters from an array.
     *
     * @param id resource id
     * @param textParameters parameters to be filled into the text
     * @return localised text formatted with the provided parameters
     * @see Formatter
     */
    public abstract String format(Id id, Object[] textParameters);

    /**
     * Gets the locale ID currently being used on the phone.
     * This can be used e.g. to load a localized icon file, by
     * adding the locale id as suffix.
     *
     * @return Locale ID as provided by the platform
     */
    public static String getLocaleId()
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
        if (!iUiThreadExists)
        {
            // Do something in UI thread to ensure that it exists
            // before _getLocaleIdQt() is called. If _getLocaleIdQt()
            // has been called before creating QApplication,
            // QApplication creation will panic.
            try
            {
                CoreUi.runInSyncUiThread(new Runnable() {
                    public void run()
                    {
                        iUiThreadExists = true;
                    }
                });
            }
            catch (Throwable t)
            {
                // Assume that UI thread already exists.
                iUiThreadExists = true;
            }
        }
        return _getLocaleIdQt();
    }

    /**
     * Releases resources and destroys all ResourceLoader instances.
     */
    synchronized public static void destroyAll()
    {
        Enumeration e = iResourceLoaders.keys();
        while (e.hasMoreElements())
        {
            String key = (String)e.nextElement();
            ResourceLoader rl = (ResourceLoader)iResourceLoaders.get(key);
            if (rl instanceof ResourceLoaderQt)
            {
                ((ResourceLoaderQt)rl).destroy();
            }
        }
        iResourceLoaders.clear();
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Default constructor.
     */
    protected ResourceLoader()
    {
    }

    /*** ----------------------------- NATIVE ----------------------------- */

    /**
     * Get device language code.
     *
     * @return languege code.
     */
    private static native int _getLocaleId();

    /**
     * Get Locale Id on Qt platform.
     *
     * @return locale Id string. If not in Qt null.
     */
    private static native String _getLocaleIdQt();
}
