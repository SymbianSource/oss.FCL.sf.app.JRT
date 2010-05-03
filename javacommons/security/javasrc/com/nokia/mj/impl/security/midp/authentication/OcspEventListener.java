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
 * Interface defining callback functions for listening to different events
 * related to ocsp checks.
 * As a general rule for the implementations of this interface, the callbacks
 * should return quickly.
 * The following events are currently supported:
 * <ul>
 * <li>OCSP_START_EVENT: the ocsp checks just started</li>
 * <li>OCSP_FINISH_EVENT: the ocsp checks just finished</li>
 * </ul>
 *
 */
public interface OcspEventListener
{
    // constants for the events
    public static final int OCSP_START_EVENT = 1;
    public static final int OCSP_FINISH_EVENT = 2;

    /**
     * Callback upon different ocsp events
     *
     * @param eventId    Identifier for the ocsp event being propagated
     *                   (OCSP_START_EVENT or OCSP_FINISH_EVENT)
     * @param eventError If there are any errors related to the ocsp event,
     *                   they are propagated as well via this parameter.
     *                   If there are no errors to be signaled, this parameter
     *                   is null
     */
    public void ocspEvent(int eventId, Throwable eventError);
}


