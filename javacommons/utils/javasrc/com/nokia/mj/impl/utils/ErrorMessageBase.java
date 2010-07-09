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

import com.nokia.mj.impl.utils.ResourceLoader;

import java.util.Hashtable;

/**
 * Base class for classes defining localized error messages.
 *
 * @see InstallerExceptionBase
 */
abstract public class ErrorMessageBase
{
    /*** ----------------------------- PUBLIC ------------------------------ */

    public static final int NO_MSG = 0; // Used when the error message is not available.

    /** Start of Runtime error message ids range. */
    public static final int RUNTIME_RANGE_START = 0;
    /** End of Runtime error message ids range. */
    public static final int RUNTIME_RANGE_END = 99;
    /** Start of Installer error message ids range. */
    public static final int INSTALLER_RANGE_START = 100;
    /** End of Installer error message ids range. */
    public static final int INSTALLER_RANGE_END = 199;
    /** Start of Security error message ids range. */
    public static final int SECURITY_RANGE_START = 200;
    /** End of Security error message ids range. */
    public static final int SECURITY_RANGE_END = 299;

    /**
     * Method for retrieving the message of a certain error.
     *
     * @param errorCode The error code whose message is queried
     * @param errorMessageParameters The parameters to be filled into the
     * message
     * @return The corresponding message including the provided
     * parameters
     */
    public String get(int errorCode, String[] errorMessageParameters)
    {
        Hashtable messageTable = getMessageTable();
        String msgId = ((String)messageTable.get(new Integer(errorCode)));
        if (msgId == null)
        {
            return this.getClass().getName() + ": No message found for error " + errorCode;
        }
        ResourceLoader rl = getResourceLoader();
        if (rl == null)
        {
            rl = getResourceLoader(errorCode);
        }
        return rl.format(msgId, errorMessageParameters);
    }

    /*** ---------------------------- PROTECTED --------------------------- */

    /**
     * Method for getting message table where key is an Integer value for
     * the error code, and value is an error message String id.
     * This method must be overriden in inheriting class.
     */
    abstract protected Hashtable getMessageTable();

    /**
     * Method for retrieving the ResourceLoader instance that is used
     * to localise error messages. If this method returns null,
     * the ResourceLoader is obtained with getResourceLoader(int)
     * method variant which then must not return null.
     *
     * This method must be overriden in inheriting class.
     */
    abstract protected ResourceLoader getResourceLoader();

    /**
     * Method for retrieving the ResourceLoader instance that is used
     * to localise error message for specified error code.
     * This method must be overriden in inheriting class if
     * the getResourceLoader() method variant returns null.
     *
     * @param errorCode error code for which ResourceLoader is returned
     */
    protected ResourceLoader getResourceLoader(int errorCode)
    {
        return null;
    }

    /**
     * Method for retrieving the ResourceLoader instance.
     * This method is called from inheriting class
     * getResourceLoader(int) method variant.
     *
     * @param textFilename name for text file
     * @param textPrefix prefix for text ids
     */
    private static Hashtable iResourceLoaderTable = null;
    protected static ResourceLoader getResourceLoader(String textFilename, String textPrefix)
    {
        String key = textFilename + "::" + textPrefix;
        if (iResourceLoaderTable == null)
        {
            iResourceLoaderTable = new Hashtable();
        }
        ResourceLoader rl = (ResourceLoader)iResourceLoaderTable.get(key);
        if (rl == null)
        {
            rl = ResourceLoader.getInstance(textFilename, textPrefix);
            iResourceLoaderTable.put(key, rl);
        }
        return rl;
    }

    /**
     * Gets the Qt locale ID currently being used on the phone.
     *
     * @return Qt locale ID as provided by the platform
     */
    protected static String getLocaleIdQt()
    {
        // Change this after Qt localisation files are taken into use.
        //return ResourceLoader.getLocaleIdQt();
        return null;
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
