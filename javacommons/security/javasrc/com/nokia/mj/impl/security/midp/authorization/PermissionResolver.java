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

import java.util.Vector;
import java.util.Hashtable;
import java.security.Permission;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.security.midp.common.PermissionAttribute;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermissionImpl;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.midp.storage.*;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.security.utils.Logger;


/**
 * Permission resolver: it resolves a list of permissions against another list
 * of permissions, returning the list of permissions which resolved the
 * permissions. Resolving means callint the implies method of Permission
 * objects
 */
public final class PermissionResolver
{
    /*
     * Cache for Permission objects
     */
    // Add synchronization for access to these hashtables
    private static Hashtable policyPermissionInstances = new Hashtable();
    private static Hashtable grantedPermissionInstances = new Hashtable();

    public static Vector resolvePermissions(
        Uid msUID,
        PermissionAttribute[] requestedPermissions,
        PolicyBasedPermission[] policyPermissions)
    {
        if (requestedPermissions == null)
        {
            return null;
        }

        Vector resolvedPermissions = new Vector();
        for (int i=0; i<requestedPermissions.length; i++)
        {
            if (requestedPermissions[i].isLegacyAttribute())
            {
                Vector legacyPermissions = resolveLegacyPermission(requestedPermissions[i],
                                           policyPermissions);
                // add legacyPermissions into resolved permissions
                // Is it possible to provide the vector to resolveLegacyPermission method which will fill it?
                if (legacyPermissions != null && legacyPermissions.size() > 0)
                {
                    for (int j=0; j<legacyPermissions.size(); j++)
                    {
                        resolvedPermissions.addElement(
                            legacyPermissions.elementAt(j));
                    }
                }
                else if (requestedPermissions[i].getImportance()
                         == PermissionAttribute.MANDATORY_PERMISSION)
                {
                    Logger.logError("Permission " + requestedPermissions[i].getName() + " not supported");
                    throw new InvalidAttributeException(
                        InstallerErrorMessage.INST_CORRUPT_PKG,
                        null, /* no params for short msg */
                        InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                        new String[] {requestedPermissions[i].getAttributeName()},
                        OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE);
                }
                continue;
            }
            try
            {
                Permission p = (Permission)ClassInstantiator.newInstance(
                                   requestedPermissions[i].getName(),
                                   requestedPermissions[i].getTarget(),
                                   requestedPermissions[i].getActionList());
                Vector resolvedPermissionSet = resolvePermission(
                                                   msUID, p, policyPermissions);
                if (resolvedPermissionSet != null)
                {
                    if (identicalSettings(resolvedPermissionSet))
                    {
                        PolicyBasedPermission rPerm = (PolicyBasedPermission)
                                                      resolvedPermissionSet.elementAt(0);
                        if (rPerm.getUserSecuritySettings() == null)
                        {
                            resolvedPermissions.addElement(
                                new PolicyBasedPermissionImpl(
                                    requestedPermissions[i].getName(),
                                    requestedPermissions[i].getTarget(),
                                    requestedPermissions[i].getActionList(),
                                    rPerm.getType()));
                        }
                        else
                        {
                            resolvedPermissions.addElement(
                                new PolicyBasedPermissionImpl(
                                    requestedPermissions[i].getName(),
                                    requestedPermissions[i].getTarget(),
                                    requestedPermissions[i].getActionList(),
                                    rPerm.getUserSecuritySettings()));
                        }
                    }
                    else
                    {
                        for (int j=0; j<resolvedPermissionSet.size(); j++)
                        {
                            resolvedPermissions.addElement(
                                resolvedPermissionSet.elementAt(j));
                        }
                    }
                }
                else
                {
                    // the permission was not resovled
                    if (requestedPermissions[i].getImportance()
                            == PermissionAttribute.MANDATORY_PERMISSION)
                    {
                        Logger.logError("Permission " + requestedPermissions[i].getName() + " not supported");
                        throw new InvalidAttributeException(
                            InstallerErrorMessage.INST_CORRUPT_PKG,
                            null, /* no params for short msg */
                            InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                            new String[] {requestedPermissions[i].getAttributeName()},
                            OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE);
                    }
                }
            }
            catch (InstantiationException e)
            {
                if (requestedPermissions[i].getImportance()
                        == PermissionAttribute.MANDATORY_PERMISSION)
                {
                    Logger.logError("Permission " + requestedPermissions[i].getName() + " not supported");
                    throw new InvalidAttributeException(
                        InstallerErrorMessage.INST_CORRUPT_PKG,
                        null, /* no params for short msg */
                        InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                        new String[] {requestedPermissions[i].getAttributeName()},
                        OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE);
                }
                // ignore, since the permission was optional
            }
        }
        if (resolvedPermissions.size() == 0)
        {
            return null;
        }
        return resolvedPermissions;
    }

    public static Vector resolvePermission(
        StorageSession aStorageSession,
        Uid msUID,
        Permission requestedPermission)
    {
        return resolvePermission(requestedPermission,
                                 getGrantedPermissionInstances(aStorageSession, msUID));
    }

    /**
     * Indication that certain settings have changed. This is implemented
     * so that the new settings are updated into the cache
     */
    // Add synchronization
    static void settingsChanged(Uid msUID, UserSecuritySettings newSettings)
    {
        Vector oldPermissionInstancesAndSettings =
            (Vector)grantedPermissionInstances.remove(msUID);
        if (oldPermissionInstancesAndSettings != null)
        {
            Vector newPermissionInstancesAndSettings = new Vector();
            for (int i=0; i < oldPermissionInstancesAndSettings.size(); i++)
            {
                PermisionInstanceAndSettings p = (PermisionInstanceAndSettings)
                                                 oldPermissionInstancesAndSettings.elementAt(i);
                if (p.getSettings() != null && p.getSettings().getName()
                        .equals(newSettings.getName()))
                {
                    // add new settings
                    newPermissionInstancesAndSettings.addElement(
                        new PermisionInstanceAndSettings(
                            p.getPermissionInstance(), newSettings));
                }
                else
                {
                    // keep the old settings
                    newPermissionInstancesAndSettings.addElement(p);
                }
            }
            grantedPermissionInstances.put(msUID,
                                           newPermissionInstancesAndSettings);
        }
    }

    private static Vector resolvePermission(
        Permission requestedPermission,
        Vector permissionInstances)
    {
        if (permissionInstances == null
                || requestedPermission == null)
        {
            return null;
        }
        Vector resolvedPermissions = new Vector();
        // stage 1: try the simple permissions (simple permission = permission with single action)
        for (int i=0; i<permissionInstances.size(); i++)
        {
            PermisionInstanceAndSettings p =
                (PermisionInstanceAndSettings)permissionInstances
                .elementAt(i);
            Object policyPermission = p.getPermissionInstance();
            if (policyPermission.getClass().isInstance(requestedPermission)
                    && ((Permission)policyPermission).implies(requestedPermission))
            {
                resolvedPermissions.addElement(
                    getResolvedPermission(
                        requestedPermission,
                        policyPermission,
                        p.getSettings(),
                        requestedPermission.getClass().getName(),
                        requestedPermission.getName(),
                        requestedPermission.getActions()));
                return resolvedPermissions;
            }
        }
        // stage 2: try the requested complex permissions (complex permission = permission with multiple actions)
        String[] actions = Tokenizer.split(requestedPermission.getActions(),
                                           ",");
        if (actions != null && actions.length > 0)
        {
            for (int i=0; i<actions.length; i++)
            {
                boolean permissionResolved = false;
                try
                {
                    Permission p1 = (Permission)ClassInstantiator.newInstance(
                                        requestedPermission.getClass().getName(),
                                        requestedPermission.getName(), actions[i]);
                    for (int j=0; j<permissionInstances.size(); j++)
                    {
                        PermisionInstanceAndSettings p =
                            (PermisionInstanceAndSettings)permissionInstances
                            .elementAt(j);
                        Object policyPermission = p.getPermissionInstance();
                        if (policyPermission.getClass().isInstance(p1)
                                && ((Permission)policyPermission).implies(p1))
                        {
                            permissionResolved = true;
                            // add the permission to the result only of not found among the existing ones
                            PolicyBasedPermissionImpl permission1 =
                                getResolvedPermission(requestedPermission,
                                                      policyPermission,
                                                      p.getSettings(),
                                                      requestedPermission.getClass().getName(),
                                                      requestedPermission.getName(),
                                                      actions[i]);
                            if (permission1.getActionList() == null
                                    || permission1.getActionList().length() == 0)
                            {
                                resolvedPermissions.addElement(permission1);
                                continue;
                            }
                            UserSecuritySettings settings1 = permission1.getUserSecuritySettings();
                            boolean found = false;
                            for (int k=0; k<resolvedPermissions.size(); k++)
                            {
                                PolicyBasedPermissionImpl permission2 =
                                    (PolicyBasedPermissionImpl)(resolvedPermissions.elementAt(k));
                                if (permission2.getActionList() != null
                                        && permission2.getActionList().length() > 0)
                                {
                                    UserSecuritySettings settings2 =
                                        permission2.getUserSecuritySettings();
                                    // group resolve permissions by name and settings
                                    if (permission1.getClass().getName().equals(permission2.getClass().getName())
                                            && ((permission1.getName() != null && permission2.getName() != null
                                                 && permission1.getName().equals(permission2.getName()))
                                                || (permission1.getName() == null && permission2.getName() == null))
                                            && ((settings1 != null && settings2 != null && settings1.equals(settings2))
                                                || (settings1 == null && settings2 == null)))
                                    {
                                        // replace element at position k with another one having
                                        // the combined actions
                                        resolvedPermissions.setElementAt(getResolvedPermission(
                                                                             permission1.getPromptDetails(),
                                                                             p.getSettings(),
                                                                             requestedPermission.getClass().getName(),
                                                                             requestedPermission.getName(),
                                                                             permission2.getActionList() + "," + permission1.getActionList()),
                                                                         k);
                                        found = true;
                                        break;
                                    }
                                }
                            }
                            if (!found)
                            {
                                resolvedPermissions.addElement(permission1);
                            }
                            break;
                        }
                    }
                }
                catch (InstantiationException e)
                {
                    // ignore it
                }
                if (!permissionResolved)
                {
                    resolvedPermissions.removeAllElements();
                    break;
                }
            }
            if (resolvedPermissions.size() > 0)
            {
                return resolvedPermissions;
            }
        }
        // stage 3: try the granted complex permissions (complex permission = permission with multiple actions)
        //      - step 1: group permissions having same name, target and settings
        //                into a list of complex permissions (= permissions with
        //                same name, target and settings but with the composite
        //                action (comma separated list of individual actions)
        //      - step 2: try to resolve the requested permission against the
        //                list of complet permissions
        Vector tmp = new Vector();
        for (int i=0; i<permissionInstances.size(); i++)
        {
            tmp.addElement(permissionInstances.elementAt(i));
        }
        while (tmp.size() > 0)
        {
            PermisionInstanceAndSettings instance1 =
                (PermisionInstanceAndSettings)tmp.elementAt(0);
            tmp.removeElementAt(0);
            Permission permission1 = (Permission)instance1.getPermissionInstance();
            UserSecuritySettings settings1 = instance1.getSettings();
            String compositeAction = "";
            // put the individual actions into a vector so that the composite
            // action is made only by unique actions (duplicates are discarded)
            Vector uniqueActions = new Vector();
            int i=0;
            while (i<tmp.size())
            {
                PermisionInstanceAndSettings instance2 =
                    (PermisionInstanceAndSettings)tmp.elementAt(i);
                Permission permission2 = (Permission)instance2.getPermissionInstance();
                UserSecuritySettings settings2 = instance2.getSettings();
                if (permission1.getClass().getName().equals(permission2.getClass().getName())
                        && ((permission1.getName() != null && permission2.getName() != null
                             && permission1.getName().equals(permission2.getName()))
                            || (permission1.getName() == null && permission2.getName() == null))
                        && ((settings1 != null && settings2 != null && settings1.equals(settings2))
                            || (settings1 == null && settings2 == null)))
                {
                    tmp.removeElementAt(i);
                    if (permission1.getActions() != null && permission1.getActions().length() > 0)
                    {
                        if (!uniqueActions.contains(permission1.getActions()))
                        {
                            if (compositeAction.length() > 0)
                            {
                                compositeAction += ",";
                            }
                            compositeAction += permission1.getActions();
                            uniqueActions.addElement(permission1.getActions());
                        }
                    }
                    if (permission2.getActions() != null && permission2.getActions().length() > 0)
                    {
                        if (!uniqueActions.contains(permission2.getActions()))
                        {
                            if (compositeAction.length() > 0)
                            {
                                compositeAction += ",";
                            }
                            compositeAction += permission2.getActions();
                            uniqueActions.addElement(permission2.getActions());
                        }
                    }
                }
                else
                {
                    i++;
                }
            }
            if (compositeAction.length() > 0 && compositeAction.indexOf(',') != -1)
            {
                try
                {
                    Permission perm = (Permission)ClassInstantiator.newInstance(
                                          permission1.getClass().getName(),
                                          permission1.getName(),
                                          compositeAction);
                    if (perm.getClass().isInstance(requestedPermission)
                            && perm.implies(requestedPermission))
                    {
                        resolvedPermissions.addElement(
                            getResolvedPermission(
                                requestedPermission,
                                permission1,
                                settings1,
                                requestedPermission.getClass().getName(),
                                requestedPermission.getName(),
                                requestedPermission.getActions()));
                        return resolvedPermissions;
                    }
                }
                catch (InstantiationException ex) {}
            }
        }
        if (resolvedPermissions.size() == 0)
        {
            return null;
        }
        return resolvedPermissions;
    }

    private static Vector resolvePermission(
        Uid msUID,
        Permission requestedPermission,
        PolicyBasedPermission[] policyPermissions)
    {
        Vector permissionInstances = getPolicyPermissionInstances(
                                         msUID, policyPermissions);
        return resolvePermission(requestedPermission, permissionInstances);
    }

    private static PolicyBasedPermissionImpl getResolvedPermission(
        PermissionBase securityPromptDetails,
        UserSecuritySettings userSettings,
        String permissionName,
        String targetName,
        String actionList)
    {
        PermissionBase securityPromptDetails2 = securityPromptDetails;
        if (securityPromptDetails != null
                && (!PermissionBase.matchActions(securityPromptDetails.getActions(),actionList)
                    || !PermissionBase.matchActions(actionList, securityPromptDetails.getActions())))
        {
            // the new instance is created only if the actions are different
            try
            {
                securityPromptDetails2 = (PermissionBase)ClassInstantiator
                                         .newInstance(securityPromptDetails.getClass().getName(),
                                                      securityPromptDetails.getName(),
                                                      actionList);
            }
            catch (InstantiationException e2)
            {
                // ignore it (no details)
            }
        }
        return new PolicyBasedPermissionImpl(
                   permissionName,
                   targetName,
                   actionList,
                   userSettings,
                   securityPromptDetails2);
    }

    private static PolicyBasedPermissionImpl getResolvedPermission(
        Permission requestedPermission,
        Object policyPermission,
        UserSecuritySettings userSettings,
        String permissionName,
        String targetName,
        String actionList)
    {
        PermissionBase securityPromptDetails = null;
        if (requestedPermission instanceof PermissionBase)
        {
            securityPromptDetails = (PermissionBase)requestedPermission;
        }
        else if (policyPermission instanceof PermissionBase)
        {
            try
            {
                securityPromptDetails = (PermissionBase)ClassInstantiator
                                        .newInstance(policyPermission.getClass().getName(),
                                                     requestedPermission.getName(),
                                                     requestedPermission.getActions());
            }
            catch (InstantiationException e2)
            {
                // ignore it (no details)
            }
        }
        return new PolicyBasedPermissionImpl(
                   permissionName,
                   targetName,
                   actionList,
                   userSettings,
                   securityPromptDetails);
    }

    private static Vector getPolicyPermissionInstances(Uid msUID, PolicyBasedPermission[] policyPermissions)
    {
        // try retrieving the policy permissions from cache
        Vector policyPermissionInstancesAndSettings =
            (Vector)policyPermissionInstances.get(msUID);
        if (policyPermissionInstancesAndSettings != null)
        {
            return policyPermissionInstancesAndSettings;
        }

        // if the policy permissions are not in cache, then build it
        // from policy permissions
        policyPermissionInstancesAndSettings = new Vector();
        if (policyPermissions == null)
        {
            return null;
        }
        for (int i=0; i<policyPermissions.length; i++)
        {
            try
            {
                Object p = ClassInstantiator.newInstance(
                               policyPermissions[i].getName(),
                               policyPermissions[i].getTarget(),
                               policyPermissions[i].getActionList());
                policyPermissionInstancesAndSettings.addElement(
                    new PermisionInstanceAndSettings(p,
                                                     policyPermissions[i].getUserSecuritySettings()));
            }
            catch (InstantiationException e)
            {
                // This should not be leaked outside
                Logger.logError("getPolicyPermissionInstances failed: " + e.toString());
            }
        }
        policyPermissionInstances.put(msUID,
                                      policyPermissionInstancesAndSettings);
        return policyPermissionInstancesAndSettings;
    }

    private static Vector getGrantedPermissionInstances(StorageSession aStorageSession, Uid msUID)
    {
        // try retrieving the policy permissions from cache
        Vector grantedPermissionInstancesAndSettings =
            (Vector)grantedPermissionInstances.get(msUID);
        if (grantedPermissionInstancesAndSettings != null)
        {
            return grantedPermissionInstancesAndSettings;
        }

        // if the granted permissions are not in cache, then retrieve them
        // from storage
        grantedPermissionInstancesAndSettings = new Vector();
        SecurityStorage storage = new SecurityStorage(aStorageSession);
        Vector grantedPermissions =
            storage.readGrantedPermissions(msUID);
        if (grantedPermissions == null)
        {
            return null;
        }
        for (int i=0; i<grantedPermissions.size(); i++)
        {
            try
            {
                PolicyBasedPermission grantedPermission =
                    (PolicyBasedPermission)grantedPermissions.elementAt(i);
                Object p = ClassInstantiator.newInstance(
                               grantedPermission.getName(),
                               grantedPermission.getTarget(),
                               grantedPermission.getActionList());
                grantedPermissionInstancesAndSettings.addElement(
                    new PermisionInstanceAndSettings(p,
                                                     grantedPermission.getUserSecuritySettings()));
            }
            catch (InstantiationException e)
            {
                // This should not be leaked outside
            }
        }
        grantedPermissionInstances.put(msUID,
                                       grantedPermissionInstancesAndSettings);
        return grantedPermissionInstancesAndSettings;
    }

    private static boolean identicalSettings(
        Vector permissions)
    {
        UserSecuritySettings settings = ((PolicyBasedPermission)permissions
                                         .elementAt(0)).getUserSecuritySettings();
        for (int i=1; i<permissions.size(); i++)
        {
            PolicyBasedPermission p = (PolicyBasedPermission)permissions
                                      .elementAt(i);
            if ((settings == null
                    && p.getUserSecuritySettings() != null)
                    || !settings.equals(p.getUserSecuritySettings()))
            {
                return false;
            }
        }
        return true;
    }

    // if requested permission is a legacy one, then handle the wildcards:
    // - if the name is null -> grant all the permissions from the policy
    // - if name is not null -> then grant the permissions from policy with the same name
    //   - if target is null -> take the target from the policy permission, otherwise take the target of the requested permission
    //   - if actionList is null -> take the actionList from the policy permission, otherwise take the actionList of the requested
    //                              permission if it matches the actionList from the policy
    private static Vector resolveLegacyPermission(PermissionAttribute legacyPermission, PolicyBasedPermission[] policyPermissions)
    {
        if (policyPermissions == null)
        {
            return null;
        }

        Vector resolvedPermissions = new Vector();
        if (legacyPermission.getName() == null)
        {
            if (policyPermissions == null)
            {
                return null;
            }
            resolvedPermissions = new Vector();
            for (int i=0; i<policyPermissions.length; i++)
            {
                resolvedPermissions.addElement(
                    new PolicyBasedPermissionImpl(
                        policyPermissions[i].getName(),
                        policyPermissions[i].getTarget(),
                        policyPermissions[i].getActionList(),
                        policyPermissions[i].getUserSecuritySettings()));
            }
        }
        else
        {
            // return permissions from policy with same name
            for (int i=0; i<policyPermissions.length; i++)
            {
                if (legacyPermission.getName().equals(
                            policyPermissions[i].getName()))
                {
                    // choose the target: if the target does not include wildcards,
                    // then there must be an exact match between the requested
                    // permission and the policy permission
                    String target = legacyPermission.getTarget();
                    if (target != null
                            && target.indexOf("*") == -1
                            && !target.equals(policyPermissions[i].getTarget()))
                    {
                        continue;
                    }
                    // choose the action
                    String actionList = legacyPermission.getActionList();
                    if (actionList == null)
                    {
                        resolvedPermissions.addElement(
                            new PolicyBasedPermissionImpl(
                                policyPermissions[i].getName(),
                                policyPermissions[i].getTarget(),
                                policyPermissions[i].getActionList(),
                                policyPermissions[i].getUserSecuritySettings()));
                    }
                    else
                    {
                        if (PermissionBase.matchActions(actionList,
                                                        policyPermissions[i].getActionList()))
                        {
                            resolvedPermissions.addElement(
                                new PolicyBasedPermissionImpl(
                                    policyPermissions[i].getName(),
                                    policyPermissions[i].getTarget(),
                                    actionList,
                                    policyPermissions[i].getUserSecuritySettings()));
                        }
                    }
                }
            }
        }
        return resolvedPermissions;
    }

    private static class PermisionInstanceAndSettings
    {
        private Object permissionObject;
        private UserSecuritySettings settings;

        public PermisionInstanceAndSettings(Object permissionObject, UserSecuritySettings settings)
        {
            this.permissionObject = permissionObject;
            this.settings = settings;
        }

        public Object getPermissionInstance()
        {
            return permissionObject;
        }

        public UserSecuritySettings getSettings()
        {
            return settings;
        }
    }
}
