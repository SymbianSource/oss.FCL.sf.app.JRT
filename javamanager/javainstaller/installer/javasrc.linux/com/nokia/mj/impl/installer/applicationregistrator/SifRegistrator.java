/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.applicationregistrator;

import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.ComponentId;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.Uid;

/**
 * Registers (and unregisters) Java application to platform's
 * software installation framework.
 */
public final class SifRegistrator
{
    /** Added application. Used with notifyAppChange() method. */
    public static final int APP_ADDED = 0;
    /** Removed application. Used with notifyAppChange() method. */
    public static final int APP_REMOVED = 1;
    /** Updated application. Used with notifyAppChange() method. */
    public static final int APP_UPDATED = 2;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Returns greater than zero if application registration to
     * software installation framework is enabled.
     */
    public static int getSifMode()
    {
        return 0;
    }

    /**
     * Launches the application view. If launching application view
     * fails this method does not throw exception but produces an
     * error log entry.
     */
    public static void launchAppView()
    {
    }

    /**
     * Notifies system about added/updated/removed applications.
     * This method should be called only after the changes have been
     * committed.
     *
     * @param aAppUids application uids
     * @param aAppChange change type: APP_ADDED, APP_UPDATED, or APP_REMOVED
     * @throws InstallerException if notification fails
     */
    public static void notifyAppChange(Uid[] aAppUids, int aAppChange)
    {
    }

    /**
     * Get component uid basing on component id.
     *
     * @param aCid component id
     * @return uid for the component, or null if component is not found
     * @throws InstallerException if an error occurs
     */
    public static Uid getUid(int aCid)
    {
        return null;
    }

    /**
     * Registers or unregisters Java software type to software
     * installation framework.
     *
     * @param aRegister true for registration, false for unregistration
     */
    public static void registerJavaSoftwareType(boolean aRegister)
    {
    }

    /**
     * Starts application registration session.
     * The registrations and unregistrations are done only
     * when commitSession is called.
     * If you want to discard the registrations and unregistrations
     * call rollbackSession.
     * Does nothing if session has already been successfully started
     * from this SifRegistrator instance.
     *
     * @param aTransaction true if also transaction for this session should
     * be opened, false otherwise
     * @throws InstallerException if the session cannot created
     * @see commitSession
     * @see rollbackSession
     */
    public void startSession(boolean aTransaction)
    {
    }

    /**
     * Registers Java application suite.
     *
     * @param aRegInfo Information needed to register the application
     * @param aIsUpdate true in case of an update, false in case of a new
     * installation
     * @throws InstallerException if registration cannot done or
     *  startSession has not been called successfully
     * @see startSession
     * @see SuiteInfo
     */
    public void registerSuite(SuiteInfo aSuiteInfo, boolean aIsUpdate)
    {
    }

    /**
     * Unregisters Java application suite.
     *
     * @param aRegInfo Information needed to unregister the application,
     * @throws InstallerException if unregistration cannot done or
     *  startSession has not been called successfully
     * @see startSession
     * @see SuiteInfo
     */
    public void unregisterSuite(SuiteInfo aSuiteInfo)
    {
    }

    /**
     * Commits the registrations and unregistrations.
     * Ends the current session if commit is successfull.
     * If commit fails the session is kept open so that
     * rollbackSession can be called.
     *
     * @throws InstallerException if session cannot be committed
     */
    public void commitSession()
    {
    }

    /**
     * Rolls back the registrations and unregistrations.
     * Ends the current session.
     *
     * @throws InstallerException if session cannot be rolled back.
     */
    public void rollbackSession()
    {
    }

    /**
     * Closes the current session if it still open.
     * If registerComponent or unregisterComponent has been called,
     * commitSession or rollbackSession must be called instead of this method.
     */
    public void closeSession()
    {
    }

    /**
     * Returns the component id of the application.
     *
     * @param aGlobalId the global id for the application.
     * @return the component id for the application, or null if the
     * application with given global id cannot be found.
     * @throws InstallerException if an error occurs
     */
    public ComponentId getComponentId(String aGlobalId)
    {
        return null;
    }

    /**
     * Returns the component id of the application.
     *
     * @param aAppUid the uid for the application.
     * @return the component id for the application, or null if the
     * application with given uid cannot be found.
     * @throws InstallerException if an error occurs
     */
    public ComponentId getComponentId(Uid aAppUid)
    {
        return null;
    }

    /**
     * Writes information of the given application to JavaInstaller log.
     *
     * @param aGlobalId the global id for the application.
     */
    public void logComponent(String aGlobalId)
    {
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
