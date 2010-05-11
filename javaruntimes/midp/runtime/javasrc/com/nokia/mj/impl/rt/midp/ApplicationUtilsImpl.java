/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

import java.security.Permission;
import java.security.AccessControlException;

import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.security.midp.authorization.AccessControllerFactoryImpl;
import com.nokia.mj.impl.security.midp.authorization.AccessControllerImpl;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.UserCancelException;


/**
 * A MIDP runtime specific implementation of ApplicationUtils class of the
 * runtime support API.
 */

public class ApplicationUtilsImpl extends ApplicationUtils
{
    private AccessControllerImpl mAccessController;
    private boolean              mStandAlone = false;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Informs all the registered shutdown listeners about the coming
     * shutdown.
     */
    public static void doShutdownImpl()
    {
        try
        {
            ((ApplicationUtilsImpl)sInstance).doShutdown();
        }
        catch (Throwable t)
        {
            Log.logE("Error in doShutdownImpl: " , t);
        }
    }

    public static void setStandAloneMode()
    {
        ((ApplicationUtilsImpl)sInstance).mStandAlone = true;
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
        MidletLifeCycle.getInstance().terminateMidletRequest(true);
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationUtils#
     *      pauseApplication().
     */
    public void pauseApplication()
    {
        if (Log.mOn) Log.logI("ApplicationUtilsImpl.pauseApplication().");
        MidletLifeCycle.getInstance().pauseApplication();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationUtils#
     *      resumeApplication().
     */
    public void resumeApplication()
    {
        if (Log.mOn) Log.logI("ApplicationUtilsImpl.resumeApplication().");
        MidletLifeCycle.getInstance().resumeRequest();
    }

    /**
     * @see com.nokia.mj.impl.rt.support.ApplicationUtils#
     *      checkPermission(java.security.Permission).
     */
    public void checkPermission(Permission p)
    throws AccessControlException,
                NullPointerException
    {
        if (mStandAlone)
        {
            // For standalone MIDlet everything is allowed.
            return;
        }
        if (p == null)
        {
            throw new NullPointerException("Check permission called with null "+
                                           "Permission");
        }
        if (mAccessController == null)
        {
            // If this is a first time, get the access controller.
            try
            {
                mAccessController =
                    AccessControllerFactoryImpl.
                    getAccessController(ApplicationInfo.getInstance().
                                        getSuiteUid(), ApplicationInfo.
                                        getInstance().getName());
            }
            catch (Throwable t)
            {
                throw new AccessControlException("Error creating " +
                                                 "AccessControllerFactory. Reason "+t);
            }
            if (mAccessController == null)
            {
                throw new AccessControlException("Error in AccessController");
            }
        }
        try
        {
            // Check the permissions
            mAccessController.checkPermission(p);
        }
        catch (UserCancelException uce)
        {
            // Do not propagate UserCancelException further,
            // throw AccessControlException instead.
            if (uce.getRootException() instanceof AccessControlException)
            {
                throw(AccessControlException)uce.getRootException();
            }
            else
            {
                throw new AccessControlException("User cancelled");
            }
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
