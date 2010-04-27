/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.properties.fileapi;

import java.util.Hashtable;
import java.util.Vector;

import com.nokia.mj.impl.file.FileConstants;
import com.nokia.mj.impl.file.FileSystemUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.SystemPropertyProvider;

/**
 * Implements Property handler for File API
 */
public final class FileDynamicPropertyHandler implements SystemPropertyProvider
{
    /*
     * Constants for identifying the asked property.
     */
    private static final String FILECONN_DIR_GRAPHICS = "fileconn.dir.graphics";
    private static final String FILECONN_DIR_GRAPHICS_NAME = "fileconn.dir.graphics.name";
    private static final String FILECONN_DIR_MEMORYCARD = "fileconn.dir.memorycard";
    private static final String FILECONN_DIR_MEMORYCARD_NAME = "fileconn.dir.memorycard.name";
    private static final String FILECONN_DIR_MUSIC = "fileconn.dir.music";
    private static final String FILECONN_DIR_MUSIC_NAME = "fileconn.dir.music.name";
    private static final String FILECONN_DIR_PHOTOS = "fileconn.dir.photos";
    private static final String FILECONN_DIR_PHOTOS_NAME = "fileconn.dir.photos.name";
    private static final String FILECONN_DIR_PRIVATE = "fileconn.dir.private";
    private static final String FILECONN_DIR_PRIVATE_NAME = "fileconn.dir.private.name";
    private static final String FILECONN_DIR_RECORDINGS = "fileconn.dir.recordings";
    private static final String FILECONN_DIR_RECORDINGS_NAME = "fileconn.dir.recordings.name";
    private static final String FILECONN_DIR_ROOTS_NAMES = "fileconn.dir.roots.names";
    private static final String FILECONN_DIR_TONES = "fileconn.dir.tones";
    private static final String FILECONN_DIR_TONES_NAME = "fileconn.dir.tones.name";
    private static final String FILECONN_DIR_VIDEOS = "fileconn.dir.videos";
    private static final String FILECONN_DIR_VIDEOS_NAME = "fileconn.dir.videos.name";

    /**
     * Hashtable that contains keys to properties that return file URLs
     */
    private static Hashtable iPropertyKeys;

    /**
     * Hashtable that contains keys to propertis that return localized names
     */
    private static Hashtable iLocalizedKeys;

    // Initialize the Hashtables when this class is first loaded.
    static
    {
        // Prepare the hashtable for URL returning keys
        iPropertyKeys = new Hashtable();
        iPropertyKeys.put(FILECONN_DIR_GRAPHICS, new Integer(
                              FileConstants.GRAPHICS_PROPERTY));
        iPropertyKeys.put(FILECONN_DIR_MEMORYCARD, new Integer(
                              FileConstants.MEMORY_CARD_PROPERTY));
        iPropertyKeys.put(FILECONN_DIR_MUSIC, new Integer(
                              FileConstants.MUSIC_PROPERTY));
        iPropertyKeys.put(FILECONN_DIR_PHOTOS, new Integer(
                              FileConstants.PHOTOS_PROPERTY));
        iPropertyKeys.put(FILECONN_DIR_RECORDINGS, new Integer(
                              FileConstants.RECORDINGS_PROPERTY));
        iPropertyKeys.put(FILECONN_DIR_TONES, new Integer(
                              FileConstants.TONES_PROPERTY));
        iPropertyKeys.put(FILECONN_DIR_VIDEOS, new Integer(
                              FileConstants.VIDEOS_PROPERTY));

        // Prepare hashtable for localized name properties
        iLocalizedKeys = new Hashtable();
        iLocalizedKeys.put(FILECONN_DIR_GRAPHICS_NAME, new Integer(
                               FileConstants.GRAPHICS_PROPERTY));
        iLocalizedKeys.put(FILECONN_DIR_MEMORYCARD_NAME, new Integer(
                               FileConstants.MEMORY_CARD_PROPERTY));
        iLocalizedKeys.put(FILECONN_DIR_MUSIC_NAME, new Integer(
                               FileConstants.MUSIC_PROPERTY));
        iLocalizedKeys.put(FILECONN_DIR_PHOTOS_NAME, new Integer(
                               FileConstants.PHOTOS_PROPERTY));
        iLocalizedKeys.put(FILECONN_DIR_RECORDINGS_NAME, new Integer(
                               FileConstants.RECORDINGS_PROPERTY));
        iLocalizedKeys.put(FILECONN_DIR_TONES_NAME, new Integer(
                               FileConstants.TONES_PROPERTY));
        iLocalizedKeys.put(FILECONN_DIR_VIDEOS_NAME, new Integer(
                               FileConstants.VIDEOS_PROPERTY));
        iLocalizedKeys.put(FILECONN_DIR_PRIVATE_NAME, new Integer(
                               FileConstants.PRIVATE_PROPERTY));
    }

    /**
     * Returns System Properties requested.
     *
     * @param key
     *            published file connection properties. See file spec and MSA
     * @return value of the system properties.
     */
    public String getProperty(String key)
    {
        String propertyValue = getURLValue(key);
        if (null != propertyValue)
        {
            return propertyValue;
        }

        // Property might be one of the localized names
        propertyValue = getLocalizedValue(key);
        if (null != propertyValue)
        {
            return propertyValue;
        }

        // Property is either root name list or private directory
        if (key.equals("fileconn.dir.roots.names"))
        {
            return getRootNames();
        }

        if (key.equals("fileconn.dir.private"))
        {
            return "file:///"+FileSystemUtils.getAppPrivateDir();
        }

        return null;
    }

    public boolean isStatic(String key)
    {
        if (key.equals("fileconn.dir.roots.names"))
        {
            return false;
        }
        if (key.equals("fileconn.dir.private"))
        {
            return false;
        }
        return true;
    }

    /**
     * Returns a URL that points to a valid path based on the propery requested.
     *
     * @param propertyName
     *            value that matches one of the System Properties exposed by
     *            JSR-75
     * @return a valid URL pointing to a path requested by the property, null
     *         otherwise.
     */
    private String getURLValue(String propertyName)
    {
        Integer property = (Integer) iPropertyKeys.get(propertyName);
        if (null == property)
        {
            return null;
        }

        return FileSystemUtils.getPathOfProperty(property.intValue());
    }

    /**
     * Returns the localized name of the propery requested.
     *
     * @param propertyName
     *            value that matches one of the System Properties exposed by
     *            JSR-75
     * @return Localized name of the property, null in case the requested name
     *         does not match with one of the properties returning localized
     *         names
     */
    private String getLocalizedValue(String propertyName)
    {
        Integer property = (Integer) iLocalizedKeys.get(propertyName);
        if (null == property)
        {
            return null;
        }

        return FileSystemUtils.getLocalizedName(property.intValue());
    }

    /**
     * Returns roots currently available to the application for use.
     *
     * @return string containing all the roots delimited by ';' character
     */
    private String getRootNames()
    {
        String retVal = "";

        try
        {
            Vector rootList = FileSystemUtils.getRootNames();
            for (int index = 0; index < rootList.size(); index++)
            {
                retVal += rootList.elementAt(index);
                retVal += ";";
            }
        }
        catch (Exception e)
        {
            // Nothing to do. Return whatever has been added to return value
        }
        return retVal;
    }
}