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

package com.nokia.mj.impl.rt.installer;

import com.nokia.mj.impl.installer.Installer;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.security.midp.authorization.AccessControllerFactoryImpl;
import com.nokia.mj.impl.security.midp.authorization.AccessControllerImpl;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.UserCancelException;

import java.security.Permission;
import java.security.AccessControlException;
import java.util.Hashtable;

/**
 * @author Nokia Corporation
 * @version 1.0
 */
public class ApplicationUtilsImpl extends ApplicationUtils
{
    private AccessControllerImpl iAccessController = null;
    private StorageSession iStorageSession = null;
    private boolean iSilent = false;
    private Uid iSuiteUid = null;
    private String iSuiteName = null;
    private Hashtable iAppUidNameTable = null;

    public void init(StorageSession aStorageSession, boolean aSilent,
                     Uid aSuiteUid, String aSuiteName,
                     Hashtable aAppUidNameTable)
    {
        Log.log("ApplicationUtilsImpl.init");
        iStorageSession = aStorageSession;
        iSuiteUid = aSuiteUid;
        iSuiteName = aSuiteName;
        iAppUidNameTable = aAppUidNameTable;
        iSilent = aSilent;
    }

    public void destroy()
    {
        Log.log("ApplicationUtilsImpl.destroy");
        iStorageSession = null;
        iSuiteUid = null;
        iSuiteName = null;
        iAppUidNameTable = null;
        if (iAccessController != null)
        {
            iAccessController.destroy();
            iAccessController = null;
        }
    }

    public static void doShutdownImpl()
    {
        Log.log("ApplicationUtilsImpl.doShutdownImpl");
        // Send shutdown notification to all registered listeners.
        ((ApplicationUtilsImpl)sInstance).doShutdown();
    }


    public void notifyExitCmd()
    {
        Log.log("ApplicationUtilsImpl.notifyExitCmd");
        // Cancel installation/uninstallation.
        Installer.cancel();
    }

    public void checkPermission(Permission aPermission)
    throws AccessControlException, NullPointerException
    {
        checkPermission(null, aPermission);
    }

    public void checkPermission(Uid aAppUid, Permission aPermission)
    throws AccessControlException, NullPointerException
    {
        if (aPermission == null)
        {
            throw new NullPointerException
            ("Check permission called with null Permission");
        }

        if (iSuiteUid == null)
        {
            // ApplicationUtilsImpl has not been initialized for
            // permission check, do nothing.
            Log.log("ApplicationUtilsImpl.checkPermission: appUid: " +
                    aAppUid + ", " + aPermission.toString() +
                    ": skipping check");
            return;
        }

        boolean userPromptAllowed = false;
        if (aPermission.toString().equals(
                    "javax.microedition.io.PushRegistry"))
        {
            // PushRegistryPermission is the only permission which
            // must be prompted from the user during installation.
            userPromptAllowed = true;
        }

        ApplicationInfoImpl appInfoImpl =
            (ApplicationInfoImpl)ApplicationInfo.getInstance();
        String protectionDomain = appInfoImpl.getProtectionDomain();
        if (!userPromptAllowed &&
                (protectionDomain == null ||
                 protectionDomain.equals(
                     ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN)))
        {
            // If user prompting is not allowed (i.e. other than
            // PushRegistryPermission is being requested) and
            // application is untrusted, do not make any
            // security checks; security checks for untrusted
            // application will be made at the application runtime.
            Log.log("ApplicationUtilsImpl.checkPermission: appUid: " +
                    aAppUid + ", " + aPermission.toString() +
                    ": no check for untrusted app");
            return;
        }

        if (iAccessController == null)
        {
            try
            {
                String appName = iSuiteName;
                if (aAppUid != null && iAppUidNameTable != null)
                {
                    appName = (String)iAppUidNameTable.get(aAppUid);
                }
                iAccessController =
                    AccessControllerFactoryImpl.
                    getAccessController(iStorageSession, iSuiteUid, appName);
            }
            catch (Throwable t)
            {
                throw new AccessControlException
                ("Error creating AccessControllerFactory. Reason " + t);
            }
            if (iAccessController == null)
            {
                throw new AccessControlException("Error creating AccessController");
            }
        }

        try
        {
            if (userPromptAllowed)
            {
                if (iSilent)
                {
                    // Do not prompt the user in case of silent installation.
                    int permissionResult =
                        iAccessController.checkPermission(aPermission.toString());
                    Log.log("ApplicationUtilsImpl.checkPermission: appUid: " +
                            aAppUid + ", " + aPermission.toString() +
                            " result " + permissionResult);
                    if (permissionResult == 0)
                    {
                        // Permission is denied.
                        throw new AccessControlException(
                            "Permission " + aPermission.toString() + " not allowed");
                    }
                }
                else
                {
                    // Not a silent installation, display user prompt.
                    iAccessController.checkPermission(aPermission);
                    Log.log("ApplicationUtilsImpl.checkPermission: appUid: " +
                            aAppUid + ", " + aPermission.toString() + " ok");
                }
            }
            else
            {
                // No user prompt allowed for this permission.
                int permissionResult =
                    iAccessController.checkPermission(aPermission.toString());
                Log.log("ApplicationUtilsImpl.checkPermission: appUid: " +
                        aAppUid + ", " + aPermission.toString() +
                        " result " + permissionResult);
                if (permissionResult <= 0)
                {
                    // Permission is either denied or requires user prompt.
                    throw new AccessControlException(
                        "Permission " + aPermission.toString() + " not allowed");
                }
            }
        }
        catch (UserCancelException uce)
        {
            Log.log("User cancelled security prompt");
            // Cancel installation/uninstallation.
            Installer.cancel();
        }
        catch (AccessControlException ace)
        {
            throw(ace);
        }
        catch (Throwable t)
        {
            Log.logError("Error during checkPermission", t);
            throw new AccessControlException
            ("Error in Security impl. Reason " + t);

        }
    }
}
