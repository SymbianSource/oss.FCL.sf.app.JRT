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
 * Encapsulation of the ocsp settings:
 *
 * <ul>
 * <li>ocspMode: the mode in which the ocsp checks are done</li>
 * <li>enabled: the ocsp checks should be performed</li>
 * <li>disabled: the ocsp checks should not be performed</li>
 * <li>undefined: the mode for ocsp checks is not defined
 *     (e.g. could be done according to some other configuration)</li>
 *
 * <li>ocspWarning: the mode in which the ocsp warnings are handled</li>
 * <li>confirm: the ocsp warnings should be confirmed
 *     (e.g. via user prompts)</li>
 * <li>ignore: the ocsp warnings should be simply ignored</li>
 * <li>undefined: the handling for the ocsp warnings is not defined
 *     (e.g. could be done according to some other configuration)</li>
 *
 * <li>silent: the ocsp checks are done as specified by ocspMode and
 *     ocspWarning settings, but silently (e.g. no user prompts)</li>
 *
 * <li>iap: the Internet Access Point which could be used when conducting the
 *     ocsp checks</li>
 * <li>snap: the Service Network Access Point to be used when conducting the
 *     ocsp checks</li>
 * </ul>
 *
 */
public final class OcspSettings
{
    // constants for the ocsp mode
    public static final int OCSP_MODE_UNDEFINED = 0;
    public static final int OCSP_MODE_ENABLED = 1;
    public static final int OCSP_MODE_DISABLED = 2;

    // constants for the ocsp warning
    public static final int OCSP_WARNING_UNDEFINED = 0;
    public static final int OCSP_WARNING_CONFIRM = 1;
    public static final int OCSP_WARNING_IGNORE = 2;

    // the ocsp mode
    public int ocspMode;

    // the ocsp warning
    public int ocspWarning;

    // the silent mode
    public boolean silent;

    // the iap
    public String iap;

    // the snap
    public String snap;

    /**
     * Constructor
     */
    public OcspSettings(int ocspMode, int ocspWarning, boolean silent, String iap, String snap)
    {
        this.ocspMode = ocspMode;
        this.ocspWarning = ocspWarning;
        this.iap = iap;
        this.snap = snap;
        this.silent = silent;
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append("ocspMode: ").append(ocspMode);
        buf.append(", ocspWarning: ").append(ocspWarning);
        buf.append(", silent: ").append(silent);
        buf.append(", iap: ").append(iap);
        buf.append(", snap: ").append(snap);
        return buf.toString();
    }
}


