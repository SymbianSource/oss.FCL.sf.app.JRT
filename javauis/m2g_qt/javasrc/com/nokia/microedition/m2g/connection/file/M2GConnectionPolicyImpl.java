/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.m2g.connection.file;

import java.io.IOException;
import java.lang.NullPointerException;
import java.lang.IllegalArgumentException;
import java.lang.SecurityException;
import com.nokia.microedition.m2g.connection.*;
import com.nokia.microedition.m2g.M2GSVGConstants;
import com.nokia.mj.impl.utils.Logger;

/**
 * Connection policy
 * <b>DRM - DECRYPTED MODE</b>
 * <p>
 * <b>Description</b>
 * <br>
 * When a MIDlet tries to open a file using a URL that starts with file:// and
 * ends either with ?drm=dec+display or with ?drm=dec+display+preview,
 * this is interpreted as a request to open the file for reading of the
 * content in the plaintext form.
 * Example URLs:
 * file:///E:/foo.svg?drm=dec+display
 * file:///E:/foo.svg?drm=dec+display+preview
 * <br>
 * Supported only if the midlet belongs to MANUFACTURER DOMAIN otherwise
 * java.lang.SecurityException is thrown.
 * </p>
 * <br>
 * <br>
 * <b>DRM - ENCRYPTED MODE</b>
 * <p>
 * <b>Description</b>
 * <br>
 * When a MIDlet tries to open a file using a URL that starts with file:// and
 * ends with ?drm=enc, this is interpreted as a request to open the
 * file for reading of the content in the encrypted form.
 * Example URLs:
 * file:///E:/foo.svg?drm=enc
 * <br>
 * Not supported, java.lang.IllegalArgumentException is thrown always.
 * </p>
 * <br>
 * <br>
 * <b>DRM - DEFAULT MODE</b>
 * <p>
 * <b>Description</b>
 * <br>
 * When a MIDlet tries to open a file using either a normal file URL or a
 * file URL that ends with ?drm=preview, this is interpreted as a request to open the
 * file for reading of the content in the decrypted form.
 * NOTE: Preview mechanism is not available to MIDlets that don't
 * belong to MANUFACTURER DOMAIN.
 * Example URLs:
 * file:///E:/foo.svg
 * file:///E:/foo.svg?drm=preview
 * <br>
 * Access to SVG document via DOM API is NOT allowed, i.e. only rendering of the
 * content is allowed if a MIDlet uses a default mode URL.
 * If it's tryed to access to SVG document via DOM API then
 * java.lang.SecurityException is thrown.
 * </p>
 */
public class M2GConnectionPolicyImpl implements M2GConnectionPolicy
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    public static final String ENC_NOT_SUPPORTED_ESTR =
        "A URL with an encrypted mode is not supported.";
    public static final String DRM_PARAMETER =
        "drm";
    public static final String DECRYPTED_MODE =
        "dec";
    public static final String ENCRYPTED_MODE =
        "enc";
    public static final String DEFAULT_MODE =
        "def";
    public static final String DEFAULT_PREVIEW_MODE =
        "def_preview";
    public static final String DRM_PREVIEW_ATTRIBUTE =
        "preview";
    public static final String DRM_DEFAULT_VALUES =
        "?drm=dec+display";
    public static final String DRM_DEFAULT_VALUES_AND_PREVIEW =
        "?drm=dec+display+preview";
    public static final String DRM_FILE_SUFFIX =
        "dcf";

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private M2GUrlTokenizer iTokenizer;
    private boolean iAccessRight = true;
    private String iMode = null;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Ctor
     */
    public M2GConnectionPolicyImpl(M2GUrlTokenizer aTokenizer)
    {
        iTokenizer = aTokenizer;
    }

    /**
     * Check domain
     * @throws SecurityException if not access rights
     */
    public void checkDomain(boolean isManufacturerDomain) throws SecurityException
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "checkDomain(): manufacturer domain=" + isManufacturerDomain
                   + ", mode=" + iMode + ", access right=" + iAccessRight + " - begin");

        // If drm mode then
        if (iMode != null)
        {
            // If dec mode and not manufacturer domain
            if (iMode.equals(DECRYPTED_MODE))
            {
                if (!isManufacturerDomain)
                {
                    Logger.ELOG(Logger.EJavaUI,
                                "checkDomain() - Not manufacturer and dec: SecurityException");
                    throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
                }
            }
            // If default + preview mode
            else if (iMode.equals(DEFAULT_PREVIEW_MODE))
            {
                if (!isManufacturerDomain)
                {
                    Logger.ELOG(Logger.EJavaUI,
                                "checkDomain() - Not manufacturer and def+preview: SecurityException");
                    throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
                }
            }
            // If not default mode
            else if (!iMode.equals(DEFAULT_MODE))
            {
                Logger.ELOG(Logger.EJavaUI,
                            "checkDomain() - Not def nor dec: SecurityException");
                throw new SecurityException(M2GSVGConstants.ACCESS_RIGHTS_ESTR);
            }
        }
    }

    /**
     * Check protection type
     * @param aType If type is null then file is not drm protected.
     */
    public void checkProtectionType(String aType)
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "checkProtectionType() type=" + aType + ", mode=" + iMode +
                   ", access right=" + iAccessRight + " - begin");

        if ((aType != null) && (!aType.equals("")))
        {
            if (iMode == null)
            {
                iMode = DEFAULT_MODE;
                iAccessRight = false;
            }
        }

        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "checkProtectionType() mode=" + iMode + ", access right=" +
                   iAccessRight + " - end");
    }

    /**
       * Get access rights
       * @see M2GConnectionPolicy#getAccessRight()
       */
    public boolean getAccessRight()
    {
        return iAccessRight;
    }

    /**
       * Initialize
       * @param aTokenizer Tokenizer
       * @throws IllegalArgumentException if drm with encrypted mode
     */
    public void initialize(M2GUrlTokenizer aTokenizer)
    {
        iAccessRight = true;
        iMode = null; // no DRM at all
        iTokenizer = aTokenizer;
        String parameters = iTokenizer.getParameters();
        int pos = -1;
        // Check if a url is something more than just a normal url.
        // Is the drm parameter within the parameters?
        if ((parameters != null) && (pos = parameters.indexOf(DRM_PARAMETER)) != 1)
        {
            pos += DRM_PARAMETER.length();
            // Encrypted mode?
            if (parameters.indexOf(ENCRYPTED_MODE, pos) != -1)
            {
                Logger.ELOG(Logger.EJavaUI, "initialize(): drm - encrypted mode");
                iAccessRight = false;
                iMode = ENCRYPTED_MODE;
                throw new IllegalArgumentException(ENC_NOT_SUPPORTED_ESTR);
            }
            // Decrypted mode?
            else if (parameters.indexOf(DECRYPTED_MODE, pos) != -1)
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "initialize(): drm - decrypted mode");
                iAccessRight = true;
                iMode = DECRYPTED_MODE;
            }
            // Default + preview mode?
            else if (parameters.indexOf(DRM_PREVIEW_ATTRIBUTE, pos) != -1)
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "initialize(): drm - decrypted mode");
                iAccessRight = false;
                iMode = DEFAULT_PREVIEW_MODE;
            }
            // Default mode
            else
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "initialize(): drm - default mode");
                iAccessRight = false;
                iMode = DEFAULT_MODE;
            }
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "initialize() mode=" + iMode + ", access right="
                   + iAccessRight + " - end");
    }

    /**
     * Get drm mode
     * @return mode. Null if no drm
     */
    public String getMode()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "getMode(): " + iMode + "- begin");
        return iMode;
    }

    /**
     * Get url
     * @return File url
     */
    public String getUrl()
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "getUrl() mode=" + iMode + ", access right=" + iAccessRight + " - begin");

        String url = iTokenizer.getUrl();
        String parameters = iTokenizer.getParameters();
        // If normal url or default drm url
        if ((iMode == null) ||
                (iMode.equals(DEFAULT_MODE)) ||
                (iMode.equals(DEFAULT_PREVIEW_MODE)))
        {
            Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                       "getUrl() parse default url");
            // Url with preview attribute?
            if ((parameters != null) &&
                    (parameters.indexOf(DRM_PREVIEW_ATTRIBUTE) != -1))
            {
                url = (iTokenizer.getBasePart() + DRM_DEFAULT_VALUES_AND_PREVIEW);
            }
            // Normal file url?
            else
            {
                // File API is able to open normal file without drm protection even
                // when the url contains the drm parameter
                url = (iTokenizer.getBasePart() + DRM_DEFAULT_VALUES);
            }
        }
        Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                   "getUrl(): " + url + " - end");
        return url;
    }
}
