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
* Description: java://taskmanager scheme handler.
*
*/
package com.nokia.mj.impl.rt.taskmanager;

import javax.microedition.io.ConnectionNotFoundException;

import com.nokia.mj.impl.coreui.CoreUi;
import com.nokia.mj.impl.rt.midp.SchemeHandlerBase;

/**
 * TaskManager platform request scheme handler. These are java: taskmanager?hide
 * java: taskmanager?show. Using hide URL query application is hided
 * from the TaskManager and brought visible using show query.
 */
public class SchemeHandler extends SchemeHandlerBase
{

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Execute scheme. If url contains query ?hide application is removed
     * from the TaskManager and if it contains ?show application is shown on
     * TaskManager.
     *
     * @return false. Since MIDlet needs never be closed before content fetch.
     * @throws ConnectionNotFoundException if URL was invalid.
     */
    public boolean execute(String url) throws ConnectionNotFoundException
    {
        if (url.indexOf("?hide") != -1)
        {
            CoreUi.hideApplication(true);
        }
        else if (url.indexOf("?show") != -1)
        {
            CoreUi.hideApplication(false);
        }
        else
        {
            throw new ConnectionNotFoundException("Invalid URL");
        }

        return false;
    }
}

