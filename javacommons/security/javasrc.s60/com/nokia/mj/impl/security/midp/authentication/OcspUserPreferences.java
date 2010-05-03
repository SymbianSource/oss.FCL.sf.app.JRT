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


package com.nokia.mj.impl.security.midp.authentication;

/**
 * Encapsulation of the ocsp settings which could be set by the user:
 *
 * <ul>
 * <li>ocspUrl: the URL of the Ocsp responder</li>
 *
 * <li>ocspMode: the mode in which the ocsp checks are done. Possible values:
 * <li>ON: the ocsp checks should be performed and some errors are tollerated (e.g. with user's approval)</li>
 * <li>MUST: the ocsp checks should be performed and no errors are tollerated</li>
 * <li>OFF: the ocsp checks should not be performed</li>
 * </ul>
 *
 */
final class OcspUserPreferences
{
    // constants for the ocsp mode
    static final int OCSP_MODE_ON = 1;
    static final int OCSP_MODE_MUST = 2;
    static final int OCSP_MODE_OFF = 3;

    // the ocsp mode
    private int ocspMode;

    // the ocsp Url
    private String ocspUrl = null;

    OcspUserPreferences()
    {
        ocspMode = _getOcspMode();
        ocspUrl = _getOcspUrl();
    }

    int getOcspMode()
    {
        return ocspMode;
    }

    String getOcspUrl()
    {
        return ocspUrl;
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append("ocspMode: ").append(ocspMode);
        buf.append(", ocspUrl: ").append(ocspUrl);
        return buf.toString();
    }

    private native int _getOcspMode();
    private native String _getOcspUrl();
}
