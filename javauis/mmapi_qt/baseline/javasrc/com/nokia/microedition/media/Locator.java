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


package com.nokia.microedition.media;

import javax.microedition.media.MediaException;

/**
 * Locator parsing class
 */
public class Locator
{
    private static final String PROTOCOL_SEPARATOR = "://";
    private static final char PARAMETER_SEPARATOR = '?';
    private final String iLocator;

    /**
     * Constructor
     * @param aLocator Locator to be parsed
     */
    public Locator(String aLocator)
    {
        iLocator = aLocator;
    }
    /**
     * Parses the protocol part of the locator e.g. returns
     * "capture" from "capture://audio?encoding=amr" locator
     * @return parsed protocol
     * @throw MediaException if Protocol cannot
     *         be parsed from the locator
     */
    public String getProtocol() throws MediaException
    {
        // Find protocol separator
        int pSep = iLocator.indexOf(PROTOCOL_SEPARATOR);

        // Protocol must exist
        if (pSep == -1)
        {
            throw new MediaException(
                "Could not create player, URI separator not found");
        }
        return iLocator.substring(0, pSep);
    }

    /**
     * Parses the middle part of the locator e.g. returns
     * "audio" from "capture://audio?encoding=amr" locator
     * @return parsed middle part
     * @throw MediaException if middle part cannot
     *         be parsed from the locator
     */
    public String getMiddlePart() throws MediaException
    {
        // Find possible parameter separator
        int parSep = iLocator.indexOf(PARAMETER_SEPARATOR);

        // No parameters
        if (parSep == -1)
        {
            parSep = iLocator.length();
        }

        return iLocator.substring(getProtocol().length() +
                                  PROTOCOL_SEPARATOR.length(), parSep);
    }

    /**
     * Parses possible parameters of the locator e.g. returns
     * "encoding=amr" from "capture://audio?encoding=amr" locator
     * @return parsed parameters or <code>null</code> if there is
     *         not parameters
     */
    public String getParameters()
    {
        // Find possible parameter separator
        int parSep = iLocator.indexOf(PARAMETER_SEPARATOR);

        String locator = null;

        // Null is returned if there is no parameters
        if (parSep != -1)
        {
            locator = iLocator.substring(parSep + 1);
        }

        return locator;
    }

    /**
     * Returns the whole locator string
     * @return locator used to create this instance
     */
    public String getLocatorString()
    {
        return iLocator;
    }
}
