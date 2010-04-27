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

package com.nokia.mj.impl.rt.main;

import java.security.Permission;
import java.security.AccessControlException;
import java.security.AccessController;


import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.rt.midp.Log;


/**
 * A Main runtime specific implemetation of ApplicationUtils class of the
 * runtime support API.
 *
 * @author Nokia Corporation
 * @version $Rev$
 */

public class ApplicationUtilsImpl extends ApplicationUtils
{
    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Informs all the registered shutdown listeners about the coming
     * shutdown.
     */
    public static void doShutdownImpl()
    {
        ((ApplicationUtilsImpl)sInstance).doShutdown();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationUtils#
     *      releaseStartWaiterImpl().
     */
    public static void releaseStartWaiterImpl(boolean doStart)
    {
        if (Log.mOn) Log.logI("ApplicationUtilsImpl.releaseStartWaiterImpl("
                                  + doStart + ").");
        ((ApplicationUtilsImpl)sInstance).releaseStartWaiter(doStart);
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationUtils#
     *      notifyExitCmd().
     */
    public void notifyExitCmd()
    {
        // Informs the life cycle about a forceful exit request.
        if (Log.mOn) Log.logI("ApplicationUtilsImpl.notifyExitCmd().");
        System.exit(0);
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationUtils#
     *      pauseApplication().
     */
    public void pauseApplication()
    {
        if (Log.mOn) Log.logI("ApplicationUtilsImpl.pauseApplication().");
        // Not applicable in main runtime model.
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationUtils#
     *      resumeApplication().
     */
    public void resumeApplication()
    {
        if (Log.mOn) Log.logI("ApplicationUtilsImpl.resumeApplication().");
        // Not applicable in main runtime model.
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationUtils#
     *      checkPermission(java.security.Permission).
     */
    public void checkPermission(Permission p)
    throws AccessControlException,
                NullPointerException
    {
        if (p == null)
        {
            throw new NullPointerException("Check permission called with null "+
                                           "Permission");
        }
        try
        {
            // Check the permissions
            // AccessController.checkPermission(p);
        }
        catch (AccessControlException a)
        {
            throw(a);
        }
        catch (Throwable t)
        {
            Log.logE("Error in checking permission: " + p, t);
            throw new AccessControlException("Error in " +
                                             "Security impl. Reason "+t);

        }
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationUtils#
     *      checkPermission(java.security.Permission).
     */
    public void checkPermission(Uid appUid,Permission p)
    throws AccessControlException,
                NullPointerException
    {
        checkPermission(p);
    }
}
