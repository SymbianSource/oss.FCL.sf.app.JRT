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

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;
import com.nokia.mj.impl.security.midp.common.MIDPPermission;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettingsImpl;
import com.nokia.mj.impl.security.midp.common.PermissionAttribute;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermissionImpl;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.AuthenticationCredentials;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.security.midp.common.ProtectionDomain;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.security.midp.storage.*;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.security.utils.Logger;
import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;

/**
 * Grants permissions to MIDlet suites at installation time based on the
 * MIDlet's signing information and based on the permissions requested by the
 * MIDlet suite
 */
public final class PermissionGranter
{
    // self
    private static PermissionGranter self;

    /*
     * Hashtable containing the granted permissions of
     * different aplications being installed
     */
    private Hashtable iGrantedPermissions = new Hashtable();

    /*
     * Hashtable containing the details of the blanket permissions of
     * different aplications being installed
     */
    private Hashtable iBlanketPermissionsDetails = new Hashtable();

    /**
     * Creates an instance of the PermissionGranter
     *
     * @return An instance of PermissionGranter
     */
    public static PermissionGranter getInstance()
    {
        if (self == null)
        {
            self = new PermissionGranter();
        }
        return self;
    }

    /**
     * Grants permissions to a certain MIDlet suite. This method is called
     * when/if the JAD is available
     *
     * @param msUID                the UID if the MIDlet suite to whom the
     *                             permissions are granted
     * @param oldMSUID             the UID if the MIDlet suite being updated
     *                             (if applicable) or null otherwise
     * @param requestedPermissions the permissions requested in the JAD by
     *                             the MIDlet (the set of requested permissions
     *                             is retrieved from
     *                             SecurityAttributes.getPermissionAttributes()
     *                             called after calling
     *                             SecurityAttributes.addDescriptorAttributes())
     * @param authCredentials      a set of credentials assigned to the MIDlet
     *                             suite as a result of calling
     *                             AuthenticationModule.authenticateJAD
     */
    public void grantJadPermissions(
        Uid msUID,
        Uid oldMSUID,
        PermissionAttribute[] requestedPermissions,
        AuthenticationCredentials[] authCredentials)
    {
        if (msUID == null
                || authCredentials == null)
        {
            return;
        }

        Vector grantedPermissions = new Vector();
        InvalidAttributeException excThrown = null;
        boolean validPermissionsGranted = false;
        for (int i=0; i< authCredentials.length; i++)
        {
            if (iGrantedPermissions.containsKey(msUID.getStringValue()
                                                + authCredentials[i].getProtectionDomainName()))
            {
                continue;
            }
            try
            {
                // try resolving the requested permissions
                Vector resolvedPermissions = resolvePermissions(
                                                 msUID,
                                                 authCredentials[i].getProtectionDomainName(),
                                                 requestedPermissions,
                                                 false /* skip adding the assigned permissions */);
                if (resolvedPermissions == null)
                {
                    continue;
                }

                // store granted permissions to cache
                iGrantedPermissions.put(msUID.getStringValue()
                                        + authCredentials[i].getProtectionDomainName(),
                                        resolvedPermissions);
                // mark down that we have granted valid permissions
                validPermissionsGranted = true;
            }
            catch (InvalidAttributeException e)
            {
                // keep the exception instead of the granted permissions
                // and throw it later if needed
                iGrantedPermissions.put(msUID.getStringValue()
                                        + authCredentials[i].getProtectionDomainName(), e);
                if (excThrown == null)
                {
                    excThrown = e;
                }
            }
        }
        // if granting permissions resulted in nothing but exceptions,
        // just throw the first one
        if (!validPermissionsGranted && excThrown != null)
        {
            throw excThrown;
        }
    }

    /**
     * Grants permissions to a certain MIDlet suite. This method is called
     * when the JAR is available
     *
     * @param storageSession       the JavaStorage session to be used when
     *                             storing the granted permissions
     * @param msUID                the UID if the MIDlet suite to whom the
     *                             permissions are granted
     * @param oldMSUID             the UID if the MIDlet suite being updated
     *                             (if applicable) or null otherwise
     * @param requestedPermissions the permissions requested in the JAR by
     *                             the MIDlet (the set of requested permissions
     *                             is retrieved from
     *                             SecurityAttributes.getPermissionAttributes()
     *                             called after calling
     *                             SecurityAttributes.addManifestAttributes())
     */
    public void grantJarPermissions(
        StorageSession storageSession,
        Uid msUID,
        Uid oldMSUID,
        PermissionAttribute[] requestedPermissions)
    {
        if (storageSession == null
                || msUID == null)
        {
            return;
        }

        // take the domain from storage
        SecurityStorage storage = new SecurityStorage(storageSession);
        String protectionDomainName = storage.readProtectionDomain(msUID);
        // filter the already granted permissions
        Object o = iGrantedPermissions.remove(
                       msUID.getStringValue() + protectionDomainName);
        Vector preGrantedPermissions = null;
        if (o != null)
        {
            if (o instanceof InvalidAttributeException)
            {
                throw(InvalidAttributeException)o;
            }
            preGrantedPermissions = (Vector)o;
        }
        // resolved the requested permissions
        Vector resolvedPermissions = resolvePermissions(msUID,
                                     protectionDomainName,
                                     requestedPermissions);

        // store the pre-granted and the resolved permissions
        if (preGrantedPermissions != null)
        {
            for (int i=0; i<preGrantedPermissions.size(); i++)
            {
                if (!resolvedPermissions.contains(
                            preGrantedPermissions.elementAt(i)))
                {
                    resolvedPermissions.addElement(
                        preGrantedPermissions.elementAt(i));
                }
            }
        }
        storage.writeGrantedPermissions(
            msUID,
            oldMSUID,
            resolvedPermissions);
        // update the cache with the full set of permissions
        iGrantedPermissions.put(msUID.getStringValue() + protectionDomainName,
                                resolvedPermissions);
        Logger.logGrantedPermissions(resolvedPermissions);
    }

    /**
     * Grants all the permissions from a specified protection domain to a
     * certain MIDlet suite.
     *
     * @param storageSession       the JavaStorage session to be used when
     *                             storing the granted permissions
     * @param uid                  the Uid if the MIDlet suite to whom the
     *                             permissions are granted
     * @param oldUid               the Uid if the MIDlet suite being updated
     *                             (if applicable) or null otherwise
     * @param protectionDomain     the protection domain containing the
     *                             permissions which are to be assigned to
     *                             the MIDlet suite.
     */
    public void grantJarPermissions(
        StorageSession storageSession,
        Uid uid,
        Uid oldUid,
        ProtectionDomain protectionDomain)
    {
        if (uid == null || protectionDomain == null)
        {
            return;
        }

        // Build in the securityAttributes object with MIDP version only
        // (same as MIDP2 unsigned MIDlets suites do). this combined with
        // a certain protection domain guarantees that all the permissions
        // from a certain protection domain are retrieved
        SecurityAttributes securityAttributes = new SecurityAttributes();
        Hashtable allAttributes = new Hashtable();
        allAttributes.put(SecurityAttributes.MIDP_VERSION_ATTRIBUTE_NAME, new Attribute("", "MIDP-2.0"));
        securityAttributes.addDescriptorAttributes(allAttributes);

        // try resolving the requested permissions
        Vector resolvedPermissions = resolvePermissions(
                                         uid,
                                         protectionDomain.getName(),
                                         securityAttributes.getPermissionAttributes(),
                                         true /* add the assigned permissions */);

        // store the permissions
        SecurityStorage storage = new SecurityStorage(storageSession);
        storage.writeGrantedPermissions(
            uid,
            oldUid,
            resolvedPermissions);
        // update the cache with the full set of permissions
        iGrantedPermissions.put(uid.getStringValue() + protectionDomain,
                                resolvedPermissions);
        Logger.logGrantedPermissions(resolvedPermissions);
    }

    /**
     * Retrieves the localized names of the "blanket permission"s granted to a
     * certain MIDlet suite.
     * The concept of blanket permission granted to a MIDlet suite is defined
     * by the following conditions:
     * <ul>
     * <li>The MIDlet suite is signed</li>
     * <li>The maximum allowed setting of each of the granted permissions
     *     requested by the MIDlet suite is Blanket</li>
     * <li>The default setting of at least one of the requested permissions
     *     is something else than Blanket</li>
     * <li>The MIDlet suite is not granted permissions in the following
     *     combination: Application Auto Invocation AND Net Access</li>
     * </ul>
     *
     * @param msUID          the UID of the MIDlet suite for whom the Blanket
     *                       permissions are queried
     * @return               the localized names of the blanket permissions
     *                       granted to the MIDlet suite or null otherwise
     */
    public String[] getBlanketPermissions(
        Uid msUID)
    {
        // sanity checks
        if (msUID == null)
        {
            return null;
        }
        // get the set of granted permissions
        String msUidKey = msUID.getStringValue();
        Vector grantedPermissions = null;
        for (Enumeration e = iGrantedPermissions.keys() ;
                e.hasMoreElements() ;)
        {
            String key = (String)e.nextElement();
            if (key.startsWith(msUidKey))
            {
                grantedPermissions = (Vector)iGrantedPermissions.remove(key);
                if (key.endsWith("UnidentifiedThirdParty"))
                {
                    // if the suite is not signed, there is no blanket
                    // permissions concept either
                    Logger.log("Suite " + msUID.toString() + " is not signed, therefore there are no blanket permissions returned");
                    return null;
                }
                break;
            }
        }
        // Step1: go through each of the granted permissions and do the
        //        following:
        //        a) ensure that the maximum interaction mode is Blanket
        //        b) mark down if the default setting is something else than
        //           Blanket
        //        c) mark down to which of the following lists it belongs
        //           - list 1: Application Auto Invocation
        //           - list 2: Net Access
        //           - list 3: Net Access, Low Level Net Access, Messaging, Restricted messaging, Call control, Local connectivity
        //           - list 4: Multimedia, Read user data
        // Step 2: check the following combinations:
        //        a) permissions in list 1 and permissions list 2
        //        b) permissions in list 3 and permissions list 4
        if (grantedPermissions != null && grantedPermissions.size() > 0)
        {
            boolean defaultNotBlanket = false;
            boolean permissions_from_mutually_exclusive_list_1 = false;
            boolean permissions_from_mutually_exclusive_list_2 = false;
            boolean permissions_from_sensitive_combination_list_1 = false;
            boolean permissions_from_sensitive_combination_list_2 = false;
            boolean local_connectivity = false;
            boolean net_access = false;
            Vector blanketPermissions =
                new Vector();
            for (int i=0; i<grantedPermissions.size(); i++)
            {
                PolicyBasedPermission permission =
                    ((PolicyBasedPermission)grantedPermissions.elementAt(i));
                UserSecuritySettings settings =
                    permission.getUserSecuritySettings();
                if (settings == null)
                {
                    // not a user permission -> move on to the next permission
                    Logger.log("Permission " + permission.getName() + " is not a user permission, therefore is is not returned as part of the group of blanket permissions");
                    continue;
                }
                if (!settings.isInteractionModeAllowed(
                            UserSecuritySettings.BLANKET_INTERACTION_MODE))
                {
                    // blanket is not an allowed interaction mode
                    // -> no blanket permission
                    Logger.log("Permission " + permission.getName() + " does not allow Blanket, therefore there are no blanket permissions returned");
                    return null;
                }
                String settingsLocName = UserSecuritySettingsImpl
                                         .getLocalizedName(settings.getName());
                if (settings.getCurrentInteractionMode()
                        != UserSecuritySettings.BLANKET_INTERACTION_MODE)
                {
                    if (!blanketPermissions.contains(settingsLocName))
                    {
                        blanketPermissions.addElement(settingsLocName);
                    }
                    defaultNotBlanket = true;
                }
                // mark down to which mutually exclusive list the permission belongs
                if (settings.getName().equalsIgnoreCase(
                            UserSecuritySettings.APPLICATION_AUTO_INVOCATION_SETTINGS))
                {
                    permissions_from_mutually_exclusive_list_1 = true;
                }
                else if (settings.getName().equalsIgnoreCase(
                             UserSecuritySettings.NET_ACCESS_SETTINGS))
                {
                    permissions_from_mutually_exclusive_list_2 = true;
                    permissions_from_sensitive_combination_list_1 = true;
                    net_access = true;
                }
                else if (settings.getName().equalsIgnoreCase(
                             UserSecuritySettings.LOW_LEVEL_NET_ACCESS_SETTINGS))
                {
                    permissions_from_sensitive_combination_list_1 = true;
                    net_access = true;
                }
                else if (settings.getName().equalsIgnoreCase(
                             UserSecuritySettings.MESSAGING_SETTINGS)
                         || settings.getName().equalsIgnoreCase(
                             UserSecuritySettings.RESTRICTED_MESSAGING_SETTINGS)
                         || settings.getName().equalsIgnoreCase(
                             UserSecuritySettings.CALL_CONTROL_SETTINGS))
                {
                    permissions_from_sensitive_combination_list_1 = true;
                }
                else if (settings.getName().equalsIgnoreCase(
                             UserSecuritySettings.LOCAL_CONNECTIVITY_SETTINGS))
                {
                    permissions_from_sensitive_combination_list_1 = true;
                    local_connectivity = true;
                }
                else if (settings.getName().equalsIgnoreCase(
                             UserSecuritySettings.MULTIMEDIA_RECORDING_SETTINGS)
                         || settings.getName().equalsIgnoreCase(
                             UserSecuritySettings.READ_USER_DATA_ACCESS_SETTINGS))
                {
                    permissions_from_sensitive_combination_list_2 = true;
                }
            }
            if (!defaultNotBlanket)
            {
                // none of the granted permissions has the default set to
                // something else than Blanket -> there is no blanket
                // permission concept
                Logger.log("All of the granted permissions are set to Blanket already, therefore there are no blanket permissions returned");
                return null;
            }
            // check for "forbidden" combinations
            if (permissions_from_mutually_exclusive_list_1
                    && permissions_from_mutually_exclusive_list_2)
            {
                // forget about blanket permissions
                Logger.log("The mutual exclusive rules would be violated if the granted permissions were all set to Blanket, therefore there are no blanket permissions returned");
                return null;
            }
            // check for sensitive combinations
            if (permissions_from_sensitive_combination_list_1
                    && permissions_from_sensitive_combination_list_2)
            {
                String blanketPermissionsDetails = ((local_connectivity && !net_access) ? 
                    "settings_inst_query_perm_sec" : 
                    "settings_inst_query_perm_net");
                iBlanketPermissionsDetails.put(msUidKey,
                                               UserSecuritySettingsImpl.getLocalizedString(
                                                   blanketPermissionsDetails));
            }
            if (blanketPermissions.size() > 0)
            {
                String[] ret = new String[blanketPermissions.size()];
                blanketPermissions.copyInto(ret);
                Logger.logBlanketPermissions(ret);
                return ret;
            }
        }
        return null;
    }

    /**
     * Retrieves details associated with the "blanket permission"s granted to a
     * certain MIDlet suite.
     *
     * @param msUID          the UID of the MIDlet suite for whom the Blanket
     *                       permissions's details are queried
     * @return               the details (as localized string) associated with
     *                       the blanket permissions or null otherwise
     */
    public String getBlanketPermissionsDetails(
        Uid msUID)
    {
        // sanity checks
        if (msUID == null)
        {
            return null;
        }
        return (String)iBlanketPermissionsDetails.remove(
                   msUID.getStringValue());
    }


    /**
     * Sets the permissions granted to a certain MIDlet suite to Blanket
     *
     * @param storageSession the JavaStorage session to be used when setting
     *                       the permissions to Blanket
     * @param msUID          the UID of the MIDlet suite whose permissions
     *                       are set to Blanket
     */
    public void setPermissionsToBlanket(
        StorageSession storageSession,
        Uid msUID)
    {
        Logger.log("The granted permissions are all set to Blanket, therefore there won't be any runtime security prompts");
        SecurityStorage storage = new SecurityStorage(storageSession);
        storage.writeUserSecuritySettings(msUID,
                                          UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                          true /* blanket prompt shown */);
    }

    /**
     * Performs a cleanup (e.g. on cached data)
     *
     */
    public void cleanup()
    {
        Logger.log("Cleanup permission granter cache");
        iGrantedPermissions.clear();
        iBlanketPermissionsDetails.clear();
    }

    /**
     * Removes all the security data related to a certain MIDlet suite
     *
     * @param storageSession the JavaStorage session to be used when
     *                       removing the security data
     * @param msUID          the UID if the MIDlet suite whose security data
     *                       is being removed
     */
    public void removeSecurityData(StorageSession storageSession, Uid msUID)
    {
        Logger.log("Remove granted permissions");
        SecurityStorage storage = new SecurityStorage(storageSession);
        storage.removeGrantedPermissions(msUID);
        // clear the cache
        String msUidKey = msUID.getStringValue();
        for (Enumeration e = iGrantedPermissions.keys() ;
                e.hasMoreElements() ;)
        {
            String key = (String)e.nextElement();
            if (key.startsWith(msUidKey))
            {
                iGrantedPermissions.remove(key);
                break;
            }
        }
        iBlanketPermissionsDetails.remove(msUidKey);
    }

    private Vector resolvePermissions(Uid msUID,
                                      String protectionDomainName,
                                      PermissionAttribute[] requestedPermissions,
                                      boolean addAssignedPerms)
    {
        // retrieve the policy permissions
        PolicyBasedPermission[] policyPermissions = SecurityPolicyModule
                .getInstance().getPermissions(protectionDomainName);

        // resovle requested permissions (if not already done)
        Vector resolvedPermissions = PermissionResolver
                                     .resolvePermissions(msUID, requestedPermissions, policyPermissions);

        // add the "assigned" permissions from the policy to the set of granted
        // permissions
        if (addAssignedPerms)
        {
            if (resolvedPermissions == null)
            {
                resolvedPermissions = new Vector();
            }
            if (policyPermissions != null)
            {
                for (int i=0; i<policyPermissions.length; i++)
                {
                    if (policyPermissions[i].getType()
                            == PolicyBasedPermission.ASSIGNED_TYPE)
                    {
                        PolicyBasedPermissionImpl p1 = new PolicyBasedPermissionImpl(
                            policyPermissions[i]);
                        // add it only if not contained already
                        boolean found = false;
                        for (int j=0; j<resolvedPermissions.size(); j++)
                        {
                            PolicyBasedPermissionImpl p2 = new PolicyBasedPermissionImpl(
                                (PolicyBasedPermission)resolvedPermissions.elementAt(j));
                            if (p1.equals(p2))
                            {
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                            resolvedPermissions.addElement(
                                new PolicyBasedPermissionImpl(
                                    policyPermissions[i].getName(),
                                    policyPermissions[i].getTarget(),
                                    policyPermissions[i].getActionList(),
                                    policyPermissions[i].getUserSecuritySettings()));
                        }
                    }
                }
            }
        }
        return resolvedPermissions;
    }

    private Vector resolvePermissions(Uid msUID,
                                      String protectionDomainName,
                                      PermissionAttribute[] requestedPermissions)
    {
        return resolvePermissions(msUID, protectionDomainName,
                                  requestedPermissions, true /* add assigned permissions */);
    }
}
