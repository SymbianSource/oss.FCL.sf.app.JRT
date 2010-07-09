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

package com.nokia.mj.impl.security.midp.authorization;

import java.security.Permission;
import java.util.Vector;
import java.security.AccessControlException;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermissionImpl;
import com.nokia.mj.impl.security.midp.common.UserPermission;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.common.PermissionMappingTable;
import com.nokia.mj.impl.security.midp.common.MIDPPermission;
import com.nokia.mj.impl.security.midp.common.GeneralSecuritySettings;
import com.nokia.mj.impl.security.midp.common.SecurityExtensionsReader;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.midp.storage.*;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.utils.exception.UserCancelException;

/**
 * MIDP specific AccessController.
 * At installation time, a MIDlet suite is granted a set of permissions.
 * At runtime, when the MIDlet is trying to access some protected
 * functionality, AccessController consults the set of granted permissions
 * to decide if the MIDlet is allowed to access the protected
 * functionality or not.
 * The MIDP access control is based on the imply mechanism applied on the
 * MIDlet suite's set of effective permissions.
 */
public final class AccessControllerImpl
{
    // each instance has a list of granted permissions and an associated prompt handler
    private Uid iAppUID;
    private SecurityPromptHandler iSecurityPromptHandler;
    private StorageSession iStorageSession;

    /**
     * Checks if the calling entity (e.g. MIDlet suite) has access to
     * functionality protected by given permission. If the permission is
     * allowed by the security policy, this method returns normally;
     * otherwise, it throws an AccessControlException.
     * Caller of this method is prompted if the given permission is
     * implied by a USER permission
     * @param p Permission to be checked
     * @throws AccessControlException is permission is not granted
     */
    public void checkPermission(Permission p)
    {
        SecurityStorage storage = new SecurityStorage(iStorageSession);
        try
        {
            // resolve the given permission against the set of granted permissions
            Vector resolvedPermissions = queryPermission(p, storage);

            // if the permission was resolved, check if granting it requires
            // user interaction
            boolean unresolvedUserPermission = false;
            for (int i=0; i<resolvedPermissions.size(); i++)
            {
                PolicyBasedPermissionImpl resolvedPermission =
                    (PolicyBasedPermissionImpl)resolvedPermissions.elementAt(i);
                if (resolvedPermission.getUserSecuritySettings() != null)
                {
                    if (resolvedPermission.getPromptDetails() != null)
                    {
                        handleUserPermission(p, resolvedPermission, storage);
                        return;
                    }
                    else
                    {
                        // user permission without prompt info
                        unresolvedUserPermission = true;
                    }
                }
                else
                {
                    // the permission was implied by a non user permission as well
                    // -> reset the flag for user permission
                    unresolvedUserPermission = false;
                }
            }
            if (unresolvedUserPermission)
            {
                // the checked permission was implied by only by user permission(s)
                // for which there were no prompt details (user could not be
                // prompted) -> fail
                fail(p);
            }
        }
        finally
        {
            storage.close();
        }
    }

    /**
     * Checks if the calling entity (e.g. MIDlet suite) has access to
     * functionality protected by given permission (identified by name).
     *
     * @param permissionName Name of the permission to be checked
     * @return               0 if the permission is denied
     *                       1 if the permission is allowed
     *                       -1 if the status of the permission is
     *                       unknown due to e.g. it requires user interaction
     */
    public int checkPermission(String permissionName)
    {
        Permission permission = getPermissionInstance(permissionName);
        if (permission == null)
        {
            // unknown permission -> it is denied
            return 0;
        }
        SecurityStorage storage = new SecurityStorage(iStorageSession);
        try
        {
            // resolve the given permission against the set of granted permissions
            Vector resolvedPermissions = PermissionResolver.resolvePermission(
                                             storage.getStorageSession(), iAppUID, permission);

            if (resolvedPermissions == null)
            {
                // denied permission
                return 0;
            }

            // if the permission was resolved, check if granting it requires
            // user interaction
            for (int i=0; i<resolvedPermissions.size(); i++)
            {
                PolicyBasedPermissionImpl resolvedPermission =
                    (PolicyBasedPermissionImpl)resolvedPermissions.elementAt(i);
                if (resolvedPermission.getUserSecuritySettings() != null)
                {
                    // user permission: check the user settings
                    UserSecuritySettings settings = resolvedPermission
                                                    .getUserSecuritySettings();
                    switch (settings.getCurrentInteractionMode())
                    {
                    case UserSecuritySettings.ONESHOT_INTERACTION_MODE:
                        // requires user interaction
                        return -1;
                    case UserSecuritySettings.NO_INTERACTION_MODE:
                        // denied
                        return 0;
                    case UserSecuritySettings.BLANKET_INTERACTION_MODE:
                        // if prompt in blanket mode was shown then return 1
                        // otherwise return -1
                        boolean blanketPromptShown = storage
                                                     .readUserSecuritySettingsPromptFlag(
                                                         iAppUID, settings.getName());
                        return (blanketPromptShown ? 1 : -1);
                    case UserSecuritySettings.SESSION_INTERACTION_MODE:
                        return iSecurityPromptHandler
                               .getSessionPromptStatus(settings.getName());
                    }
                }
            }
        }
        finally
        {
            storage.close();
        }
        return 1;
    }


    /**
     * Checks if the calling entity (e.g. MIDlet suite) has access to
     * functionality protected by given permission (identified by name).
     *
     * @param permissionName Name of the permission to be checked
     * @return               true if the permission is allowed
     *                       false if the permission is denied
     */
    public boolean isPermissionAllowed(String permissionName)
    {
        Permission permission = getPermissionInstance(permissionName);
        if (permission == null)
        {
            // unknown permission -> it is denied
            return false;
        }
        try
        {
            checkPermission(permission);
        }
        catch (AccessControlException e)
        {
            return false;
        }
        return true;
    }

    public AccessControllerImpl(StorageSession aStorageSession, Uid aAppSuiteUid, String aAppName)
    {
        init(aStorageSession, aAppSuiteUid, aAppName);
    }

    /**
     * Closes the storage session
     */
    public void destroy()
    {
        iSecurityPromptHandler.destroy();
    }


    /**
     * Does the initialization: initializes the prompt handler and and
     * the storage
     */
    private void init(StorageSession aStorageSession, Uid appUID, String appName)
    {
        this.iAppUID = appUID;
        this.iStorageSession = aStorageSession;
        iSecurityPromptHandler = new SecurityPromptHandler(appName,
                GeneralSecuritySettings.getSecurityWarningsMode(iStorageSession, appUID));
    }

    private Vector queryPermission(Permission p, SecurityStorage storage)
    {
        // resolve the given permission against the set of granted permissions
        Vector resolvedPermissions =
            PermissionResolver.resolvePermission(
                storage.getStorageSession(), iAppUID, p);

        // if permission not resovled FAIL
        if (resolvedPermissions == null)
        {
            fail(p);
        }
        return resolvedPermissions;
    }


    /**
     * Handles the user settings by delegating the prompt handler to handle
     * the settings and stores the result as the new settings
     */
    private void handleUserPermission(
        Permission checkedPermission,
        PolicyBasedPermissionImpl resolvedPermission,
        SecurityStorage storage)
    {
        UserSecuritySettings settings = resolvedPermission
                                        .getUserSecuritySettings();

        // check if the settings are set to NO
        if (settings.getCurrentInteractionMode()
                == UserSecuritySettings.NO_INTERACTION_MODE)
        {
            fail(checkedPermission);
        }

        // check if the permission explicitly denies showing of the prompt
        if (!resolvedPermission.getPromptDetails().isSecurityPromptNeeed(
                    settings.getCurrentInteractionMode()))
        {
            // no sense in going further
            return;
        }

        boolean OFF = false;
        boolean ON = true;

        // "blanket" mode MUST prompt the user only once
        // on or before the first invocation of the API or function protected.
        boolean blanketPromptShown = storage
                                     .readUserSecuritySettingsPromptFlag(iAppUID, settings.getName());

        // give it to PromptHandler
        UserSecuritySettings newSettings;
        try
        {
            newSettings = iSecurityPromptHandler.handleUserPermission(
                              resolvedPermission, blanketPromptShown);
        }
        catch (UserCancelException e)
        {
            // add the AccessControlException as embedded exception
            throw new UserCancelException(
                e.getMessage(),
                new java.security.AccessControlException(
                    "Permission " + checkedPermission + " not allowed"));
        }
        
        // if the permission was assigned or the settings were not active, 
        // then activate the user settings
        if (resolvedPermission.getType() != PolicyBasedPermission.USER_TYPE 
            || !settings.isActive())
        {
            storage.activateUserSecuritySettings(iAppUID, resolvedPermission);
        }

        // if settings have not changed, still do one check on the current
        // interaction mode: if it's blanket, mark down that the prompt in
        // blanket mode was shown and then simply return
        if (newSettings.equals(settings))
        {
            if (newSettings.getCurrentInteractionMode()
                    == UserSecuritySettings.BLANKET_INTERACTION_MODE
                    && !blanketPromptShown)
            {
                storage.writeUserSecuritySettingsPromptFlag(iAppUID,
                        settings.getName(), ON);
            }
            return;
        }

        if (newSettings.getCurrentInteractionMode()
                == UserSecuritySettings.NO_INTERACTION_MODE)
        {
            // denying from runtime prompts will not change the settings
            // to NO -> it is always a onetime deny (this has been chosen
            // for a better user experience)
            fail(checkedPermission);
        }

        if (settings.getCurrentInteractionMode()
                == UserSecuritySettings.BLANKET_INTERACTION_MODE
                && newSettings.getCurrentInteractionMode()
                != UserSecuritySettings.BLANKET_INTERACTION_MODE)
        {
            // if mode is changed from blanket to something else than blanket,
            // reset the flag indicating if prompting happened in blanket mode
            blanketPromptShown = OFF;
        }
        else if (newSettings.getCurrentInteractionMode()
                 == UserSecuritySettings.BLANKET_INTERACTION_MODE)
        {
            // if mode is changed to blanket, set the flag indicating that
            // prompt happened in blanket mode, since the change to blanket
            // mode was made via a prompt
            blanketPromptShown = ON;
        }

        // write the new settings to storage
        storage.writeUserSecuritySettings(iAppUID, newSettings.getName(),
                                          newSettings.getCurrentInteractionMode(), blanketPromptShown);
        // inform the PermissionResolver that the settings have changed
        PermissionResolver.settingsChanged(iAppUID, newSettings);
        /* Uncomment if the mutual exclusive rules are to be applied in runtime security prompts
        // apply the mutually exclusive rules
        applyMutualExclusiveRules(storage, newSettings);
        */
    }

    private Permission getPermissionInstance(String permissionName)
    {
        if (permissionName != null)
        {
            MIDPPermission internalPermission = PermissionMappingTable.get(
                                                    permissionName);
            if (internalPermission == null)
            {
                internalPermission = SecurityExtensionsReader.getExtPermission(permissionName);
            }
            if (internalPermission != null)
            {
                // make an instance of the internal permission
                try
                {
                    return (Permission)ClassInstantiator.newInstance(
                               internalPermission.getName(),
                               internalPermission.getTarget(),
                               internalPermission.getActionList());
                }
                catch (InstantiationException e)
                {
                    // fall-through
                }
            }
        }
        // fall-through
        return null;
    }

    private void fail(Permission p)
    {
        throw new java.security.AccessControlException(
            "Permission " + p + " not allowed");
    }

    private void applyMutualExclusiveRules(SecurityStorage storage, UserSecuritySettings settings)
    {
        // Rule1: if the settings belongs to list 1, change the settings
        //        to Session or Oneshot or No for all the settings belonging
        //        to list 2
        // Rule2: if the settings belongs to list 2, change the settings
        //        to Session or Oneshot or No for all the settings belonging
        //        to list 1
        // where
        //        list 1: Application Auto Invocation
        //        list 2: Net Access
        boolean settings_in_list1 = (settings.getName().equalsIgnoreCase(
                                         UserSecuritySettings.APPLICATION_AUTO_INVOCATION_SETTINGS));
        boolean settings_in_list2 = (settings.getName().equalsIgnoreCase(
                                         UserSecuritySettings.NET_ACCESS_SETTINGS));
        if (settings.getCurrentInteractionMode()
                != UserSecuritySettings.BLANKET_INTERACTION_MODE
                || (settings_in_list1 == false && settings_in_list2 == false))
        {
            // not applicable
            return;
        }
        String mutualExclusiveSettingsName = (settings_in_list1
                                              ? UserSecuritySettings.NET_ACCESS_SETTINGS
                                              : UserSecuritySettings.APPLICATION_AUTO_INVOCATION_SETTINGS);
        UserSecuritySettings[] allSettings =
            storage.readUserSecuritySettings(iAppUID);
        for (int i=0; i<allSettings.length; i++)
        {
            if (allSettings[i].getName().equals(mutualExclusiveSettingsName)
                    && allSettings[i].getCurrentInteractionMode()
                    == UserSecuritySettings.BLANKET_INTERACTION_MODE)
            {
                // change the mutualExclusiveSettings to either
                // Session or Oneshot or No
                int mutualExclusiveSettingsIntractionMode;
                if (allSettings[i].isInteractionModeAllowed(
                            UserSecuritySettings.SESSION_INTERACTION_MODE))
                {
                    mutualExclusiveSettingsIntractionMode =
                        UserSecuritySettings.SESSION_INTERACTION_MODE;
                }
                else if (allSettings[i].isInteractionModeAllowed(
                             UserSecuritySettings.ONESHOT_INTERACTION_MODE))
                {
                    mutualExclusiveSettingsIntractionMode =
                        UserSecuritySettings.ONESHOT_INTERACTION_MODE;
                }
                else
                {
                    mutualExclusiveSettingsIntractionMode =
                        UserSecuritySettings.NO_INTERACTION_MODE;
                }
                storage.writeUserSecuritySettings(iAppUID,
                                                  mutualExclusiveSettingsName,
                                                  mutualExclusiveSettingsIntractionMode);
                // inform the PermissionResolver that the settings have changed
                PermissionResolver.settingsChanged(iAppUID, allSettings[i]);
            }
        }
    }
}
