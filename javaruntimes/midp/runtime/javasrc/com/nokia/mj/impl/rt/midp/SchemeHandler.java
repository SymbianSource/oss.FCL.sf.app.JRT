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

package com.nokia.mj.impl.rt.midp;

import javax.microedition.io.ConnectionNotFoundException;

/**
 * JRT based platform request scheme handler interface.
 */
public interface SchemeHandler
{

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Execute scheme.
     *
     * @param url to be invoked.
     * @return  true if MIDlet must be closed prior content fetch.
     * @throws ConnectionNotFoundException if URL execution fails.
     */
    public boolean execute(String url) throws ConnectionNotFoundException;
}

