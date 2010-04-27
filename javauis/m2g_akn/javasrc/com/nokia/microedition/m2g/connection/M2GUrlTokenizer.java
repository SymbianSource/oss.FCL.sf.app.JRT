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

package com.nokia.microedition.m2g.connection;

import java.io.IOException;
import java.lang.NullPointerException;
import java.lang.IllegalArgumentException;
import com.nokia.mj.impl.utils.Logger;

/**
 * Connection policy
 */
public class M2GUrlTokenizer
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    public static final String PROTOCOL_SEPARATOR =
        "://";
    public static final char PARAMETER_SEPARATOR =
        '?';
    public static final char SEPARATOR =
        '/';

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private String iUrl = null;
    private String iBaseUrl = null;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Ctor
     */
    public M2GUrlTokenizer()
    {
    }

    /**
     * Ctor
     * @param aUrl Locator
       * @throws NullPointerException if the locator is null.
       * @throws IllegalArgumentException if the locator is of unsupported type.
     */
    public M2GUrlTokenizer(String aUrl)
    {
        setUrl(aUrl);
    }

    /**
     * Check if the url begin with slash ('/')
     * @return true is the url begin with slash
       */
    public boolean beginWithSlash()
    {
        if ((iUrl != null) &&
                (iUrl.indexOf(SEPARATOR) == 0))
        {
            return true;
        }
        return false;
    }


    /**
     * Parses the base part of the locator
     * @return parsed base part
       */
    public String getBasePart()
    {
        if (iUrl == null)
        {
            return null;
        }
        int pos = iUrl.indexOf(PARAMETER_SEPARATOR);
        if (pos == -1)
        {
            pos = iUrl.length();
        }
        return iUrl.substring(0, pos);
    }

    /**
     * Parses the base url
     * @return parsed base url
       */
    public String getBaseUrl()
    {
        return iBaseUrl;
    }

    /**
     * Parses the end part of the locator
     * @return parsed end part
     * @throws IllegalArgumentException if base part cannot
     *       be parsed from the locator
       */
    public String getEndPart()
    {
        if (iUrl == null)
        {
            return null;
        }
        int pos = iUrl.lastIndexOf(SEPARATOR);
        if (pos == -1)
        {
            return iUrl;
        }
        return iUrl.substring(pos, iUrl.length());
    }

    /**
     * Parses the middle part of the locator
     * @return parsed middle part
     * @throws IllegalArgumentException if middle part cannot
     *       be parsed from the locator
       */
    public String getMiddlePart()
    {
        if (iUrl == null)
        {
            return null;
        }
        int pos = iUrl.indexOf(PARAMETER_SEPARATOR);
        if (pos == -1)
        {
            pos = iUrl.length();
        }
        return iUrl.substring(
                   getProtocol().length() + PROTOCOL_SEPARATOR.length(), pos);
    }

    /**
     * Parses possible parameters of the locator
     * @return Parsed parameters or null if there are not parameters
     */
    public String getParameters()
    {
        if (iUrl == null)
        {
            return null;
        }
        int pos = iUrl.indexOf(PARAMETER_SEPARATOR);
        String parameters = null;
        if (pos != -1)
        {
            parameters = iUrl.substring(pos + 1);
        }
        return parameters;
    }

    /**
       * Parses the protocol part of the locator.
       * @see M2GUrlTokenizer#isAbsolutURL()
       * @return Protocol
       * @throws IllegalArgumentException If the locator is of
       * unsupported type.
       */
    public String getProtocol()
    {
        int pos = -1;
        if ((iUrl == null) || ((pos = iUrl.indexOf(PROTOCOL_SEPARATOR)) == -1))
        {
            throw new IllegalArgumentException("Unable to parse protocol");
        }
        return iUrl.substring(0, pos);
    }

    /**
     * Returns a whole locator string
     * @return Locator used to create this instance
     */
    public String getUrl()
    {
        return iUrl;
    }

    /**
     * Resolve URL according to existing base url and
     * the given relative url
     * TODO: Should be resolved by using RFC 2396
     * @aRelativeUrl Relative URL.
     * @return Resolved URL
     */
    public String resolveUrl(String aRelativeUrl)
    {
        check(aRelativeUrl);
        if (iBaseUrl != null)
        {
            return (iBaseUrl + aRelativeUrl);
        }
        else
        {
            return aRelativeUrl;
        }
    }


    /**
     * Set a locator string
     * @param aUrl Locator
       * @throws NullPointerException if the locator is null.
       * @throws IllegalArgumentException if the locator is of unsupported type.
     */
    public void setUrl(String aUrl)
    {
        check(aUrl);
        iUrl = aUrl;
        int pos = iUrl.lastIndexOf(SEPARATOR);
        if (pos == -1)
        {
            iBaseUrl = null;
        }
        else
        {
            iBaseUrl = iUrl.substring(0, (pos + 1));
        }
    }

    /**
     * Check url
     * @param aUrl Locator
       * @throws NullPointerException if the locator is null.
       * @throws IllegalArgumentException if the locator is of unsupported type.
     */
    private void check(String aUrl)
    {
        if (aUrl == null)
        {
            Logger.ELOG(Logger.EJavaUI, "setUrl() - url is null");
            throw new NullPointerException();
        }
        if (aUrl.length() == 0)
        {
            Logger.ELOG(Logger.EJavaUI, "setUrl() - url is illegal");
            throw new IllegalArgumentException();
        }
    }

    //--------------------------------------------------
    // STATIC METHODS
    //--------------------------------------------------
    /**
       * Check if contains protocol.
       * An absolute URL contains the name of the scheme being used (<scheme>)
       * followed by a colon (":") and then a string (the <scheme-specific-
       * // part>) whose interpretation depends on the scheme.
       * Also if the URL begins with a slash character "/" then the URL is treated
       * as absolute.
       * @param aUrl URL
       * @return true if URL contains a protocol
       */
    static public boolean isAbsolutURL(String aUrl)
    {
        if ((aUrl == null) ||
                ((aUrl.indexOf(PROTOCOL_SEPARATOR) == -1) &&
                 (aUrl.indexOf(SEPARATOR) != 0)))
        {
            return false;
        }
        return true;
    }
}
