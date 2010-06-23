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

package com.nokia.mj.impl.security.midp.storage;

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.security.midp.common.AuthorizationRestriction;
import com.nokia.mj.impl.security.midp.common.AuthorizationRestrictionAttribute;
import com.nokia.mj.impl.security.midp.common.AuthenticationAttribute;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermissionImpl;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettingsImpl;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.security.midp.common.GeneralSecuritySettings;
import com.nokia.mj.impl.storage.*;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.utils.Tokenizer;
import java.util.Vector;
import java.util.Hashtable;

/**
 * Class for handling the security related storage data. There are three types
 * of security data:
 *    - authentication data, which includes:
 *        - security domain
 *        - jar hash
 *        - root hash
 *        - succesfully validated certificates
 *    - authorization data, which includes the set of granted permissions
 *    - app access authorization data, which includes:
 *        - vendor name
 *        - domain name
 *        - signers list (the end-entity certificates from the succesfully
 *          validated certificate chains)
 */
public final class SecurityStorage
{
    /* Session to storage */
    private StorageSession session = null;

    // boolean indicating if the storage session needs to be destroyed.
    // This flag is modified if the storage session has been provided
    // to this class, so that the one providing the storage session
    // will destroy it
    private boolean destroyStorageSession = true;

    /*
     * The allowed modes info is stored as a 4-bit constant:
     *     X(oneshot)X(session)X(blanket)X(no)
     * e.g. 1011 (=11) means that oneshot, blanket and no are allowed.
     * The following constants are used to encode/decode the allowed modes
     * into/from a 4-bit number
     */
    private static final int NO_MASK = 1;
    private static final int BLANKET_MASK = 2;
    private static final int SESSION_MASK = 4;
    private static final int ONESHOT_MASK = 8;

    /*
     * The actual interaction with the storage session is kept in one place
     * (so that error handling is centralized, instead of being spread here
     * and there). These constants specify which storage operation to be
     * executed
     */
    private static final int STORAGE_OPEN = 1;
    private static final int STORAGE_CLOSE = 2;
    private static final int STORAGE_SEARCH = 3;
    private static final int STORAGE_WRITE = 4;
    private static final int STORAGE_UPDATE = 5;
    private static final int STORAGE_REMOVE = 6;

    /*
     * Filters for specifying which app access authorization data to be read
     * from storage
     */
    public static final int APP_ACCESS_AUTH_DOMAIN_QUERY = 1;
    public static final int APP_ACCESS_AUTH_VENDOR_QUERY = 2;
    public static final int APP_ACCESS_AUTH_SIGNERS_LIST_QUERY = 4;

    /*
     * Filters for specifying which authentication data to be read
     * from storage
     */
    public static final int AUTHENTICATION_DOMAIN_NAME_QUERY = 1;
    public static final int AUTHENTICATION_DOMAIN_CATEGORY_QUERY = 2;
    public static final int AUTHENTICATION_ROOT_HASH_QUERY = 4;
    public static final int AUTHENTICATION_JAR_HASH_QUERY = 8;
    public static final int AUTHENTICATION_VALID_CERTS_QUERY = 16;

    private static final int NOT_FOUND = -2;
    private static final int REMOVED = -1;

    /**
     * Constructor
     */
    public SecurityStorage()
    {
        doStorageOpen();
    }

    /**
     * Constructor
     */
    public SecurityStorage(StorageSession session)
    {
        if (session != null)
        {
            destroyStorageSession = false;
            this.session = session;
        }
        else
        {
            doStorageOpen();
        }
    }

    /**
     * Destroies the session to the storage
     */
    public void close()
    {
        doStorageClose();
    }

    /**
     * Retrieves the storage session
     */
    public StorageSession getStorageSession()
    {
        return session;
    }

    /**
     * Retrieves the authorization restrictions of a certain application
     *
     * @param appUID the UID of the application whose authorization
     *               restrictions are retrieved
     * @return       an array of all the authorization restrictions or null if
     *               no restrictions found
     */
    public AuthorizationRestriction[] readAuthorizationRestrictions(Uid appUID)
    {
        if (appUID == null)
        {
            return null;
        }
        Vector authRestrictions = new Vector();
        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               appUID.getStringValue()));
        int i=1;
        while (true)
        {
            String mainAttrName = AuthorizationRestrictionAttribute
                                  .MAIN_ATTRIBUTE_PREFIX  + i;
            StorageAttribute mainAttr = new StorageAttribute(
                StorageNames.NAME, mainAttrName);
            query.addAttribute(mainAttr);
            query.addAttribute(new StorageAttribute(StorageNames.VALUE, ""));
            StorageEntry[] entries = doStorageSearch(
                                         StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, query);
            if (entries != null && entries.length > 0)
            {
                String mainAttrValue = getStorageAttributeValue(entries[0],
                                       StorageNames.VALUE);
                String secondAttrName = AuthorizationRestrictionAttribute
                                        .SECOND_ATTRIBUTE_PREFIX + i;
                query.removeAttribute(mainAttr);
                StorageAttribute secondAttr = new StorageAttribute(
                    StorageNames.NAME, secondAttrName);
                query.addAttribute(secondAttr);
                entries = doStorageSearch(
                              StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE,
                              query);
                if (entries != null && entries.length > 0)
                {
                    String secondAttrValue = getStorageAttributeValue(
                                                 entries[0], StorageNames.VALUE);
                    authRestrictions.addElement(
                        new AuthorizationRestrictionAttribute(
                            mainAttrName, mainAttrValue,
                            secondAttrName, secondAttrValue));
                }
                else
                {
                    authRestrictions.addElement(
                        new AuthorizationRestrictionAttribute(
                            mainAttrName, mainAttrValue,
                            null, null));
                }
                query.removeAttribute(secondAttr);
            }
            else
            {
                break;
            }
            i++;
        }
        if (authRestrictions.size() == 0)
        {
            return null;
        }
        AuthorizationRestriction[] authorizationRestrictions =
            new AuthorizationRestriction[authRestrictions.size()];
        authRestrictions.copyInto(authorizationRestrictions);
        return authorizationRestrictions;
    }

    public AppAccessAuthorizationStorageData readAppAccessAuthorizationStorageData(Uid appUID)
    {
        // read everything
        return readAppAccessAuthorizationStorageData(appUID,
                APP_ACCESS_AUTH_DOMAIN_QUERY | APP_ACCESS_AUTH_VENDOR_QUERY | APP_ACCESS_AUTH_SIGNERS_LIST_QUERY);
    }

    /**
     * Retrieves the app access authorization data of a certain application
     *
     * @param aAppName    the name of the application for which the
     *                    authentication data is queried
     * @param aAppVersion the version of the application for which the
     *                    authentication data is queried
     * @param aAppVendor  the vendor of the application for which the
     *                    authentication data is queried
     * @param readFilter  specifies which app access authorization data to be
     *                    read
     * @return            the app access authorization data associated with the
     *                    queried application
     */
    public AppAccessAuthorizationStorageData readAppAccessAuthorizationStorageData(
        String aAppName,
        String aAppVersion,
        String aAppVendor,
        int readFilter)
    {
        return readAppAccessAuthorizationStorageData(null /* appUID */,
                aAppName,
                aAppVersion,
                aAppVendor,
                readFilter);
    }

    /**
     * Retrieves the app access authorization data of a certain application
     *
     * @param appUID     the UID of the application whose app access
     *                   authorization
     *                   data is retrieved
     * @param readFilter specifies which app access authorization data to be
     *                   read
     * @return           the app access authorization data
     */
    public AppAccessAuthorizationStorageData readAppAccessAuthorizationStorageData(
        Uid appUID, int readFilter)
    {
        return readAppAccessAuthorizationStorageData(appUID,
                null /* appName */,
                null /* appVersion */,
                null /* appvendor*/,
                readFilter);
    }

    /**
     * Retrieves the permissions granted to a certain application.
     * The user permissions are retrieved first and then the rest
     * of the permissions
     *
     * @param appUID the UID of the application whose granted permissions
     *               are retrieved
     * @return      aAn array of the granted permissions
     */
    public Vector readGrantedPermissions(Uid appUID)
    {
        if (appUID == null)
        {
            return null;
        }
        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               appUID.getStringValue()));
        StorageEntry[] permEntries = doStorageSearch(
                                         StorageNames.MIDP_PERMISSIONS_TABLE, query);
        Vector permissions = new Vector();
        Vector nonUserPermissions = new Vector();
        // put all the function group names into a hashtable and use it
        // to check if details of a certain function group have already
        // been read from storage (it's faster than querying the
        // storage)
        Hashtable fGroups = new Hashtable();
        if (permEntries != null && permEntries.length > 0)
        {
            for (int i=0; i<permEntries.length; i++)
            {
                String className = getStorageAttributeValue(permEntries[i],
                                   StorageNames.CLASS);
                String target = getStorageAttributeValue(permEntries[i],
                                StorageNames.NAME);
                String actionList = getStorageAttributeValue(permEntries[i],
                                    StorageNames.ACTION);
                FunctionGroup fg = decodeFunctionGroup(
                                getStorageAttributeValue(permEntries[i],
                                StorageNames.FUNCTION_GROUP));
                if (fg != null)
                {
                    UserSecuritySettings cachedSettings =
                        (UserSecuritySettings)fGroups.get(fg.name);
                    if (cachedSettings != null)
                    {
                        permissions.addElement(
                            new PolicyBasedPermissionImpl(
                                className,
                                target,
                                actionList,
                                fg.type,
                                cachedSettings));
                    }
                    else
                    {
                        // query the settings details
                        StorageEntry fgQuery = new StorageEntry();
                        fgQuery.addAttribute(new StorageAttribute(
                                                 StorageAttribute.ID,
                                                 appUID.getStringValue()));
                        fgQuery.addAttribute(new StorageAttribute(
                                                 StorageNames.FUNCTION_GROUP,
                                                 fg.name));
                        fgQuery.addAttribute(new StorageAttribute(
                                                 StorageNames.ALLOWED_SETTINGS,
                                                 ""));
                        fgQuery.addAttribute(new StorageAttribute(
                                                 StorageNames.CURRENT_SETTING,
                                                 ""));
                        fgQuery.addAttribute(new StorageAttribute(
                                                 StorageNames.BLANKET_PROMPT,
                                                 ""));
                        StorageEntry[] fgEntries = doStorageSearch(
                                                       StorageNames.MIDP_FUNC_GRP_SETTINGS_TABLE,
                                                       fgQuery);
                        if (fgEntries != null && fgEntries.length > 0)
                        {
                            try
                            {
                                int allowedSettings = Integer.valueOf(
                                                          getStorageAttributeValue(fgEntries[0],
                                                                                   StorageNames.ALLOWED_SETTINGS)).intValue();
                                int currentSetting = Integer.valueOf(
                                                         getStorageAttributeValue(fgEntries[0],
                                                                                  StorageNames.CURRENT_SETTING)).intValue();
                                String blanketPrompt = getStorageAttributeValue(fgEntries[0],
                                                       StorageNames.BLANKET_PROMPT);
                                UserSecuritySettings settings = new UserSecuritySettingsImpl(
                                    fg.name,
                                    currentSetting,
                                    decodeAllowedSettings(allowedSettings),
                                    ("1".equals(blanketPrompt) ? true : false),
                                    fg.isActive);
                                permissions.addElement(
                                    new PolicyBasedPermissionImpl(
                                        className,
                                        target,
                                        actionList,
                                        fg.type,
                                        settings));
                                fGroups.put(fg.name, settings);
                            }
                            catch (NumberFormatException e)
                            {
                                Logger.ELOG(Logger.EJavaSecurity, "Permission read failed", e);
                            }
                        }
                    }
                }
                else
                {
                    // is not a user permission
                    nonUserPermissions.addElement(
                        new PolicyBasedPermissionImpl(
                            className,
                            target,
                            actionList,
                            null));
                }
                // cleanup the query, so it can be used at the next interation
                if (fg != null)
                {
                    query.removeAttribute(fg.name);
                }
            }
        }
        // add the non user permissions into the set of returned permissions
        for (int i=0; i<nonUserPermissions.size(); i++)
        {
            permissions.addElement(nonUserPermissions.elementAt(i));
        }
        if (permissions.size() == 0)
        {
            return null;
        }
        return permissions;
    }

    /**
     * Stores the set of permissions granted to a certain application.
     * If this is an update (oldAppUID parameter is not null), then the
     * settings of the old application are retained
     *
     * @param appUID               the UID of the application to whome the
     *                             permissions are granted
     * @param oldAppUID            the UID if the application being updated
     *                             (if applicable) or null otherwise
     * @param grantedPermissions   the set of granted permissions
     */
    public void writeGrantedPermissions(Uid appUID, Uid oldAppUID, Vector grantedPermissions, boolean preserveSettings)
    {
        if (grantedPermissions != null
                && grantedPermissions.size() > 0
                && appUID != null)
        {
            if (oldAppUID != null)
            {
                updateGrantedPermissions(
                    appUID,
                    oldAppUID,
                    grantedPermissions);
                return;
            }
            // put all the function group names into a vector and use it
            // to check if details of a certain function group have already
            // been writen into storage (it's faster than querying the
            // storage or overwriting the storage data)
            Vector fGroups = new Vector();
            for (int i=0; i<grantedPermissions.size(); i++)
            {
                PolicyBasedPermission p = (PolicyBasedPermission)
                                          grantedPermissions.elementAt(i);
                if (p == null)
                {
                    // move to the next permission
                    continue;
                }
                StorageEntry permEntry = new StorageEntry();
                permEntry.addAttribute(new StorageAttribute(
                                           StorageAttribute.ID,
                                           appUID.getStringValue()));
                permEntry.addAttribute(new StorageAttribute(
                                           StorageNames.CLASS,
                                           p.getName()));
                if (p.getTarget() != null
                        && p.getTarget().length() > 0)
                {
                    permEntry.addAttribute(new StorageAttribute(
                                               StorageNames.NAME,
                                               p.getTarget()));
                }
                if (p.getActionList() != null
                        && p.getActionList().length() > 0)
                {
                    permEntry.addAttribute(new StorageAttribute(
                                               StorageNames.ACTION,
                                               p.getActionList()));
                }
                UserSecuritySettings settings =
                    p.getUserSecuritySettings();
                if (settings != null)
                {
                    StorageEntry fgEntry = new StorageEntry();
                    fgEntry.addAttribute(new StorageAttribute(
                                             StorageAttribute.ID,
                                             appUID.getStringValue()));
                    // update the MIDP_FUNC_GRP_SETTINGS_TABLE table as well
                    // (if not already done)
                    String fgName = settings.getName();
                    StorageAttribute fgAttribute = new StorageAttribute(
                        StorageNames.FUNCTION_GROUP,
                        fgName);
                    if (p.getType() == PolicyBasedPermission.USER_ASSIGNED_TYPE 
                        || !settings.isActive())
                    {
                        // the inactive settings or the settings of assigned 
                        // permissions with user settings should be activated 
                        // only after the first usage of the permission. 
                        // Otherwise it creates confusion for the user, 
                        // like in the following use cases:
                        // 1) the installation time dialog would include the assigned
                        //    permissions even though the MIDlet did not request it
                        // 2) after installing a MIDlet suite and before accessing 
                        //    some protected functionality (which is guarder by 
                        //    assigned permission with user setting) the user sees 
                        //    the settings of the permissions in the MIDlet settings 
                        //   (even though the MIDlet might never access such 
                        //   functionality)
                        //
                        // disabling the settings = alter the name of the function 
                        // group stored in PERMISSIONS table
                        permEntry.addAttribute(new StorageAttribute(
                            StorageNames.FUNCTION_GROUP,
                            encodeFunctionGroup(p.getType(),fgName)));
                    }
                    else
                    {
                        permEntry.addAttribute(fgAttribute);
                    }
                    // Do we need to check if the settings which already exist
                    // are equal to the ones which are to be inserted?
                    // This won't even happen, since all the settings are read
                    // from the security policy
                    // If we don't check, then the first settings are stored and are
                    // available for all the permissions
                    if (!fGroups.contains(fgName))
                    {
                        fGroups.addElement(fgName);
                        if (!preserveSettings
                                || readUserSecuritySettings(appUID, fgName) == null)
                        {
                            fgEntry.addAttribute(new StorageAttribute(
                                                     StorageNames.FUNCTION_GROUP,
                                                     fgName));
                            fgEntry.addAttribute(new StorageAttribute(
                                                     StorageNames.ALLOWED_SETTINGS,
                                                     "" + encodeAllowedSettings(
                                                         settings.getAllowedInteractionModes())));
                            fgEntry.addAttribute(new StorageAttribute(
                                                     StorageNames.CURRENT_SETTING,
                                                     "" + settings.getCurrentInteractionMode()));
                            fgEntry.addAttribute(new StorageAttribute(
                                                     StorageNames.BLANKET_PROMPT,
                                                     settings.getBlanketPrompt() ? "1" : "0"));
                            doStorageWrite(
                                StorageNames.MIDP_FUNC_GRP_SETTINGS_TABLE,
                                fgEntry);
                        }
                    }
                }
                doStorageWrite(StorageNames.MIDP_PERMISSIONS_TABLE,
                               permEntry);
            }
        }
    }

    public void writeGrantedPermissions(Uid appUID, Uid oldAppUID, Vector grantedPermissions)
    {
        writeGrantedPermissions(appUID,oldAppUID,grantedPermissions, false /*preserveSettings*/);
    }

    /**
     * Removes from storage all the permissions granted to a certain
     * application
     *
     * @param appUID The UID of the application whose permissions are
     *               removed
     */
    public void removeGrantedPermissions(Uid appUid)
    {
        if (appUid != null)
        {
            // delete everything from MIDP_PERMISSIONS_TABLE
            doStorageRemove(StorageNames.MIDP_PERMISSIONS_TABLE, appUid);
            // delete everything from MIDP_FUNC_GRP_SETTINGS_TABLE
            doStorageRemove(StorageNames.MIDP_FUNC_GRP_SETTINGS_TABLE, appUid);
        }
    }

    /**
     * Reads the authentication data associated with certain application
     *
     * @param aAppName    the name of the application for which the
     *                    authentication data is queried
     * @param aAppVersion the version of the application for which the
     *                    authentication data is queried
     * @param aAppVendor  the vendor of the application for which the
     *                    authentication data is queried
     * @return            the authentication data associated with the
     *                    queried application
     */
    public AuthenticationStorageData readAuthenticationStorageData(
        String aAppName,
        String aAppVersion,
        String aAppVendor,
        int readFilter)
    {
        return readAuthenticationStorageData(null /* appUID */,
                                             aAppName,
                                             aAppVersion,
                                             aAppVendor,
                                             readFilter);
    }

    /**
     * Reads the authentication data
     *
     * @param appUID the UID of the application whose authentication data is
     *               retrieved
     * @return       the authentication data associated with the application
     *               identified by the appUID
     */
    public AuthenticationStorageData readAuthenticationStorageData(Uid appUID)
    {
        return readAuthenticationStorageData(appUID,
                                             null /* appName */,
                                             null /* appVersion */,
                                             null /* appvendor*/,
                                             AUTHENTICATION_DOMAIN_NAME_QUERY
                                             | AUTHENTICATION_DOMAIN_CATEGORY_QUERY
                                             | AUTHENTICATION_ROOT_HASH_QUERY
                                             | AUTHENTICATION_JAR_HASH_QUERY
                                             | AUTHENTICATION_VALID_CERTS_QUERY/* read everything */);
    }

    /**
     * Reads the protection domain associated with certain application
     *
     * @param appUID the UID of the application whose protection domain is
     *               retrieved
     * @return       the protection domain associated with the application
     *               identified by the appUID
     */
    public String readProtectionDomain(Uid appUID)
    {
        if (appUID == null)
        {
            return null;
        }
        StorageEntry query = new StorageEntry();
        AuthenticationStorageData authData = null;
        query.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               appUID.getStringValue()));
        query.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_DOMAIN,
                               ""));
        StorageEntry[] entries = doStorageSearch(
                                     StorageNames.MIDP_PACKAGE_TABLE, query);
        if (entries != null && entries.length > 0)
        {
            return getStorageAttributeValue(entries[0],
                                            StorageNames.SECURITY_DOMAIN);
        }
        return null;
    }

    /**
     * Reads the protection domain category associated with certain application
     *
     * @param appUID the UID of the application whose protection domain category
     *               is retrieved
     * @return       the protection domain category associated with the
     *               application identified by the appUID
     */
    public String readProtectionDomainCategory(Uid appUID)
    {
        if (appUID == null)
        {
            return null;
        }
        StorageEntry query = new StorageEntry();
        AuthenticationStorageData authData = null;
        query.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               appUID.getStringValue()));
        query.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_DOMAIN_CATEGORY,
                               ""));
        StorageEntry[] entries = doStorageSearch(
                                     StorageNames.MIDP_PACKAGE_TABLE, query);
        if (entries != null && entries.length > 0)
        {
            return getStorageAttributeValue(entries[0],
                                            StorageNames.SECURITY_DOMAIN_CATEGORY);
        }
        return null;
    }

    /**
     * Stores the authentication data
     *
     * @param appUID the UID of the application whose authentication data is
     *               stored
     * @param data   the authentication data to be stored
     */
    public void writeAuthenticationStorageData(Uid appUID, AuthenticationStorageData data)
    {
        writeAuthenticationStorageData(appUID, data,
                                       false /* this is not an update*/);
    }

    /**
     * Stores the authentication data
     *
     * @param appUID the UID of the application whose authentication data is
     *               stored
     * @param data   the authentication data to be stored
     * @param update boolean specifying if the DB operation should be an
     *               update or not
     */
    public void writeAuthenticationStorageData(Uid appUID, AuthenticationStorageData data, boolean update)
    {
        // protection domain is mandatory values,
        // no sense to try storing something useless
        if (data == null || data.getProtectionDomain() == null)
        {
            return;
        }
        StorageEntry entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(StorageAttribute.ID,
                                                appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_DOMAIN,
                               data.getProtectionDomain()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_DOMAIN_CATEGORY,
                               data.getProtectionDomainCategory()));
        if (data.getJarHashValue() != null
                && data.getJarHashValue().length() > 0)
        {
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.HASH,
                                   data.getJarHashValue()));
        }
        else
        {
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.HASH,
                                   null, StorageAttribute.NULL_TYPE));
        }
        if (data.getRootHashValue() != null
                && data.getRootHashValue().length() > 0)
        {
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.CERT_HASH,
                                   data.getRootHashValue()));
        }
        String validCerts = encodeValidatedChainIndexes(
                                data.getValidatedChainIndexes());
        if (validCerts != null && validCerts.length() > 0)
        {
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.VALID_CERTS,
                                   validCerts));
        }
        int securityWarningsMode = data.getSecurityWarningsMode();
        if (update)
        {
            securityWarningsMode = readSecurityWarningsMode(appUID);
        }
        if (securityWarningsMode == GeneralSecuritySettings.DEFAULT_SECURITY_MODE
                || securityWarningsMode == GeneralSecuritySettings.USER_SECURITY_MODE)
        {
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.SECURITY_WARNINGS,
                                   "" + securityWarningsMode));
        }
        if (update)
        {
            StorageEntry oldEntry = new StorageEntry();
            oldEntry.addAttribute(new StorageAttribute(StorageAttribute.ID,
                                  appUID.getStringValue()));
            doStorageUpdate(StorageNames.MIDP_PACKAGE_TABLE, entry, oldEntry);
        }
        else
        {
            doStorageWrite(StorageNames.MIDP_PACKAGE_TABLE, entry);
        }
    }

    /**
     * Removes from storage all the authentication data related to a certain
     * application
     *
     * @param appUID The UID of the application whose authentication data is
     *               removed
     */
    public void removeAuthenticationStorageData(Uid appUid)
    {
        // delete from MIDP_PACKAGE_TABLE
        if (appUid != null)
        {
            doStorageRemove(StorageNames.MIDP_PACKAGE_TABLE, appUid);
        }
    }

    /**
     * Retrieves the flag indicating if the security prompt was shown in blanket
     * mode
     *
     * @param aAppUID       The UID of the application whose info about the
     *                      prompt is retrieved
     * @param aSettingsName The name of the settings whose info about the
     *                      prompt is retrieved
     * @return              A boolean indicating if the security prompt in
     *                      blanket mode was shown
     */
    public boolean readUserSecuritySettingsPromptFlag(Uid aAppUID, String aSettingsName)
    {
        StorageEntry query = new StorageEntry();
        UserSecuritySettings settings = null;
        query.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               aAppUID.getStringValue()));
        query.addAttribute(new StorageAttribute(
                               StorageNames.FUNCTION_GROUP,
                               aSettingsName));
        query.addAttribute(new StorageAttribute(
                               StorageNames.BLANKET_PROMPT,
                               ""));
        StorageEntry[] entries = doStorageSearch(
                                     StorageNames.MIDP_FUNC_GRP_SETTINGS_TABLE,
                                     query);
        if (entries != null && entries.length > 0)
        {
            String blanketPrompt = getStorageAttributeValue(entries[0],
                                   StorageNames.BLANKET_PROMPT);
            if (blanketPrompt!= null && blanketPrompt.equals("1"))
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Stores the flag indicating if the security prompt was shown in blanket
     * mode
     *
     * @param aAppUID       the UID of the application whose flag for blanket
     *                      prompt is stored
     * @param aSettingsName the name of the settings whose whose flag for
     *                      blanket prompt is stored
     * @param aPromptFlag   true is prompt in blanket mode was shown and false
     *                      otherwise
     */
    public void writeUserSecuritySettingsPromptFlag(Uid aAppUID, String aSettingsName, boolean aPromptFlag)
    {
        StorageEntry query = new StorageEntry();
        StorageEntry newEntry = new StorageEntry();
        UserSecuritySettings settings = null;
        query.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               aAppUID.getStringValue()));
        query.addAttribute(new StorageAttribute(
                               StorageNames.FUNCTION_GROUP,
                               aSettingsName));
        newEntry.addAttribute(new StorageAttribute(
                                  StorageNames.BLANKET_PROMPT,
                                  (aPromptFlag ? "1":"0")));
        doStorageUpdate(StorageNames.MIDP_FUNC_GRP_SETTINGS_TABLE,
                        newEntry, query);
    }

    /**
     * Retrieves certain settings associated to a certain application
     *
     * @param appUID       the UID of the application whose security settings
     *                     are retrieved
     * @param settingsName the name of the settings to be retrieved
     * @return             user security settings
     */
    public UserSecuritySettings readUserSecuritySettings(
        Uid msUID,
        String settingsName)
    {
        if (settingsName == null)
        {
            return null;
        }
        StorageEntry query = new StorageEntry();
        UserSecuritySettings settings = null;
        query.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               msUID.getStringValue()));
        query.addAttribute(new StorageAttribute(StorageNames.FUNCTION_GROUP,
                                                settingsName));
        query.addAttribute(new StorageAttribute(
                               StorageNames.ALLOWED_SETTINGS,
                               ""));
        query.addAttribute(new StorageAttribute(
                               StorageNames.CURRENT_SETTING,
                               ""));
        query.addAttribute(new StorageAttribute(
                               StorageNames.BLANKET_PROMPT,
                               ""));
        StorageEntry[] entries = doStorageSearch(
                                     StorageNames.MIDP_FUNC_GRP_SETTINGS_TABLE,
                                     query);
        if (entries != null && entries.length > 0)
        {
            try
            {
                int allowedSettings = Integer.valueOf(getStorageAttributeValue(
                                                          entries[0],StorageNames.ALLOWED_SETTINGS)).intValue();
                int currentSetting = Integer.valueOf(getStorageAttributeValue(
                                                         entries[0],StorageNames.CURRENT_SETTING)).intValue();
                String blanketPrompt = getStorageAttributeValue(entries[0],
                                       StorageNames.BLANKET_PROMPT);
                settings = new UserSecuritySettingsImpl(
                    settingsName,
                    currentSetting,
                    decodeAllowedSettings(allowedSettings),
                    ("1".equals(blanketPrompt) ? true : false));
            }
            catch (NumberFormatException e)
            {
                Logger.ELOG(Logger.EJavaSecurity, "SecuritySettings read failed", e);
            }
        }
        return settings;
    }

    /**
     * Retrieves ALL the settings associated to a certain application
     *
     * @param appUID the UID of the application whose security settings
     *               are retrieved
     * @return       an array of all the user security settings
     */
    public UserSecuritySettings[] readUserSecuritySettings(
        Uid msUID)
    {

        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               msUID.getStringValue()));
        query.addAttribute(new StorageAttribute(
                               StorageNames.FUNCTION_GROUP,
                               ""));
        StorageEntry[] entries = doStorageSearch(
                                     StorageNames.MIDP_PERMISSIONS_TABLE, query);
        Vector vSettings = new Vector();
        if (entries != null)
        {
            for (int i=0; i<entries.length; i++)
            {
                UserSecuritySettings settings =
                    readUserSecuritySettings(msUID,getStorageAttributeValue(
                                                 entries[i],StorageNames.FUNCTION_GROUP));
                if (settings != null)
                {
                    vSettings.addElement(settings);
                }
            }
        }
        if (vSettings.size() == 0)
        {
            return null;
        }
        UserSecuritySettings[] settings =
            new UserSecuritySettings[vSettings.size()];
        vSettings.copyInto(settings);
        return settings;
    }

    /**
     * Stores settings info associated to a certain application
     *
     * @param appUID                 the UID of the application whose security
     *                               settings info are stored
     * @param settingsName           the name of the settings to be modified
     * @param currentInteractionMode the new value of the settings
     */
    public void writeUserSecuritySettings(
        Uid msUID,
        String settingsName,
        int currentInteractionMode)
    {
        StorageEntry entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               msUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.FUNCTION_GROUP,
                               settingsName));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.CURRENT_SETTING,
                               "" + currentInteractionMode));
        updateFunctionGroupTable(msUID, entry);
    }

    /**
     * Stores settings info associated to a certain application
     *
     * @param appUID                 the UID of the application whose security
     *                               settings info are stored
     * @param settingsName           the name of the settings to be modified
     * @param currentInteractionMode the new value of the settings
     * @param blanketPromptShown     the new value of the settings
     */
    public void writeUserSecuritySettings(
        Uid msUID,
        String settingsName,
        int currentInteractionMode,
        boolean blanketPromptShown)
    {
        StorageEntry entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               msUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.FUNCTION_GROUP,
                               settingsName));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.CURRENT_SETTING,
                               "" + currentInteractionMode));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.BLANKET_PROMPT,
                               (blanketPromptShown ? "1":"0")));
        updateFunctionGroupTable(msUID, entry);
    }

    /**
     * Sets all the settings associated to a certain application
     * to a certain value
     *
     * @param appUID                 the UID of the application whose security
     *                               settings are set
     * @param currentInteractionMode the new value of the settings
     * @param blanketPromptShown     the new value of the settings
     */
    public void writeUserSecuritySettings(
        Uid msUID,
        int currentInteractionMode,
        boolean blanketPromptShown)
    {
        // read all the settings
        UserSecuritySettings[] settings = readUserSecuritySettings(msUID);
        if (settings != null)
        {
            for (int i=0; i<settings.length; i++)
            {
                // for each of the settings write the new value
                writeUserSecuritySettings(msUID,
                                          settings[i].getName(),
                                          currentInteractionMode,
                                          blanketPromptShown);
            }
        }
    }
    
    /**
     * Activates user security settings
     *
     * @param appUID       the UID of the application whose user 
                           settings are activatesversion is read
     * @param settingsName the name of permission whose settings 
     *                     which are activated
     */
    public void activateUserSecuritySettings(
        Uid msUID,
        PolicyBasedPermission permission)
    {
        if (permission.getUserSecuritySettings() == null)
        {
            return;
        }
        StorageEntry oldEntry = new StorageEntry();
        oldEntry.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               msUID.getStringValue()));
        oldEntry.addAttribute(new StorageAttribute(
                               StorageNames.FUNCTION_GROUP,
                               encodeFunctionGroup(permission.getType(), 
                               permission.getUserSecuritySettings().getName())));
        StorageEntry newEntry = new StorageEntry();
        newEntry.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               msUID.getStringValue()));
        newEntry.addAttribute(new StorageAttribute(
                               StorageNames.FUNCTION_GROUP,
                               permission.getUserSecuritySettings().getName()));
        doStorageUpdate(StorageNames.MIDP_PERMISSIONS_TABLE,
                        newEntry,
                        oldEntry);
    }

    /**
     * Reads the value of the suite version attribute
     *
     * @param appUID the UID of the application whose version is read
     * @return       the value of the version attribute
     */
    public String readSuiteVersion(Uid appUID)
    {
        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               appUID.getStringValue()));
        StorageAttribute versionAttr = new StorageAttribute(
            StorageNames.NAME, SecurityAttributes.MIDP_VERSION_ATTRIBUTE_NAME);
        query.addAttribute(versionAttr);
        query.addAttribute(new StorageAttribute(StorageNames.VALUE, ""));
        StorageEntry[] entries = doStorageSearch(
                                     StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, query);
        if (entries != null && entries.length > 0)
        {
            return getStorageAttributeValue(entries[0],
                                            StorageNames.VALUE);
        }
        return null;
    }

    /**
     * Reads the value of the MEDIA_ID attribute
     *
     * @param appUID the UID of the application whose MEDIA_ID is retrieved
     * @return       the value of the MEDIA_ID attribute
     */
    public String readSuiteMediaId(Uid appUID)
    {
        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               appUID.getStringValue()));
        query.addAttribute(new StorageAttribute(
                               StorageNames.MEDIA_ID,
                               ""));
        StorageEntry[] entries = doStorageSearch(
                                     StorageNames.APPLICATION_PACKAGE_TABLE,
                                     query);
        if (entries != null && entries.length > 0)
        {
            return getStorageAttributeValue(entries[0],
                                            StorageNames.MEDIA_ID);
        }
        return null;
    }

    /**
     * Reads the network restrictions
     *
     * @param  appUID the UID of the application whose network restrictions
     *         are retrieved
     * @return A string containing the network restrictions or null if
     *         there are no network restrictions associated
     */
    public String readNetworkRestrictions(Uid appUID)
    {
        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               appUID.getStringValue()));
        query.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               "MIDlet-Operator-Allowed"));
        query.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               ""));
        StorageEntry[] entries = doStorageSearch(
                                     StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE,
                                     query);
        if (entries != null && entries.length > 0)
        {
            return getStorageAttributeValue(entries[0],
                                            StorageNames.VALUE);
        }
        return null;
    }

    /**
     * Reads security warnings mode value
     * @return One of the values
     *         SECURITY_WARNINGS_USER_DEFINED_MODE
     *         SECURITY_WARNINGS_DEFAULT_MODE
     */
    public int readSecurityWarningsMode(Uid appUid)
    {
        try
        {
            StorageEntry query = new StorageEntry();
            query.addAttribute(new StorageAttribute(
                                   StorageAttribute.ID,
                                   appUid.getStringValue()));
            query.addAttribute(new StorageAttribute(
                                   StorageNames.SECURITY_WARNINGS,
                                   ""));
            StorageEntry[] entries = doStorageSearch(
                                         StorageNames.MIDP_PACKAGE_TABLE,
                                         query);
            if (entries != null && entries.length > 0)
            {
                try
                {
                    int value = Integer.valueOf(getStorageAttributeValue(entries[0],
                                                StorageNames.SECURITY_WARNINGS)).intValue();
                    if (value == GeneralSecuritySettings.DEFAULT_SECURITY_MODE
                            || value == GeneralSecuritySettings.USER_SECURITY_MODE)
                    {
                        return value;
                    }
                }
                catch (NumberFormatException e)
                {
                    Logger.ELOG(Logger.EJavaSecurity, "SecurityWarnings read failed", e);
                }
            }
        }
        catch (StorageException e)
            {/* move on with defaults */}
        return GeneralSecuritySettings.DEFAULT_SECURITY_MODE;
    }

    /**
     * Writes security warnings mode value
     * @param aSecurityWarningsMode One of the values
     *        SECURITY_WARNINGS_USER_DEFINED_MODE
     *        SECURITY_WARNINGS_DEFAULT_MODE
     */
    public void writeSecurityWarningsMode(Uid appUid, int aSecurityWarningsMode)
    {
        try
        {
            switch (aSecurityWarningsMode)
            {
            case GeneralSecuritySettings.DEFAULT_SECURITY_MODE:
            case GeneralSecuritySettings.USER_SECURITY_MODE:
                StorageEntry entry = new StorageEntry();
                entry.addAttribute(new StorageAttribute(
                                       StorageNames.SECURITY_WARNINGS,
                                       "" + aSecurityWarningsMode));
                StorageEntry oldEntry = new StorageEntry();
                oldEntry.addAttribute(new StorageAttribute(
                                          StorageAttribute.ID,
                                          appUid.getStringValue()));
                doStorageUpdate(StorageNames.MIDP_PACKAGE_TABLE,
                                entry,
                                oldEntry);
                break;
            }
        }
        catch (StorageException e)
            {/* move on with defaults */}
    }


    private void updateGrantedPermissions(Uid newAppUID, Uid oldAppUID, Vector grantedPermissions)
    {
        // the vector containing the newGrantedPermissions
        Vector newGrantedPermissions = new Vector();

        // get the old permissions & settings
        Vector oldPermissions = readGrantedPermissions(oldAppUID);

        // filter out the the brand new permissions
        // (permissions which are not found among the old permissions)
        if (oldPermissions != null)
        {
            int index=0;
            while (index < grantedPermissions.size())
            {
                // instead of calling Vector.removeElement(p) we will do the
                // remove manually, since the search is to be based on
                // the permission without the settings
                PolicyBasedPermission p = (PolicyBasedPermission)
                                          grantedPermissions.elementAt(index);
                int status = removeElement(oldPermissions, p);
                switch (status)
                {
                case NOT_FOUND:
                    index++;
                    break;
                case REMOVED:
                    grantedPermissions.removeElementAt(index);
                    break;
                default:
                    // different settings
                    UserSecuritySettings oldSettings
                    = ((PolicyBasedPermission)oldPermissions
                       .elementAt(status)).getUserSecuritySettings();
                    UserSecuritySettings newSettings
                    = p.getUserSecuritySettings();
                    if (oldSettings != null
                            && newSettings != null)
                    {
                        boolean activeSettings = false;
                        if (oldSettings.isActive() 
                            || newSettings.isActive())
                        {
                            activeSettings = true;
                        }
                        newGrantedPermissions.addElement(
                            new PolicyBasedPermissionImpl(
                                p.getName(),
                                p.getTarget(),
                                p.getActionList(),
                                p.getType(),
                                new UserSecuritySettingsImpl(
                                    newSettings.getName(),
                                    oldSettings.getCurrentInteractionMode(),
                                    newSettings.getAllowedInteractionModes(),
                                    oldSettings.getBlanketPrompt(),
                                    activeSettings)));
                    }
                    else
                    {
                        newGrantedPermissions.addElement(p);
                    }
                    grantedPermissions.removeElementAt(index);
                    break;
                }
            }
        }
        // write what's left from the granted permissions
        writeGrantedPermissions(newAppUID, null, grantedPermissions, true /* preserveSettings */);
        for (int i=0; i<newGrantedPermissions.size(); i++)
        {
            grantedPermissions.addElement(newGrantedPermissions.elementAt(i));
        }

        // remove what's left from the old permissions
        if (oldPermissions != null)
        {
            for (int i=0; i<oldPermissions.size(); i++)
            {
                PolicyBasedPermission p = (PolicyBasedPermission)
                                          oldPermissions.elementAt(i);
                StorageEntry removePermissionQuery = new StorageEntry();
                removePermissionQuery.addAttribute(new StorageAttribute(
                                                       StorageAttribute.ID,
                                                       oldAppUID.getStringValue()));
                removePermissionQuery.addAttribute(new StorageAttribute(
                                                       StorageNames.CLASS,
                                                       p.getName()));
                if (p.getTarget() != null
                        && p.getTarget().length() > 0)
                {
                    removePermissionQuery.addAttribute(new StorageAttribute(
                                                           StorageNames.NAME,
                                                           p.getTarget()));
                }
                if (p.getActionList() != null
                        && p.getActionList().length() > 0)
                {
                    removePermissionQuery.addAttribute(new StorageAttribute(
                                                           StorageNames.ACTION,
                                                           p.getActionList()));
                }
                doStorageRemove(StorageNames.MIDP_PERMISSIONS_TABLE,
                                removePermissionQuery);
                // remove the setting also if not used by some other permission
                UserSecuritySettings settings =
                    p.getUserSecuritySettings();
                if (settings != null)
                {
                    StorageEntry permissionsQuery = new StorageEntry();
                    permissionsQuery.addAttribute(new StorageAttribute(
                                                      StorageAttribute.ID,
                                                      newAppUID.getStringValue()));
                    permissionsQuery.addAttribute(new StorageAttribute(
                                                      StorageNames.FUNCTION_GROUP,
                                                      settings.getName()));
                    StorageEntry[] permissions = doStorageSearch(
                                                     StorageNames.MIDP_PERMISSIONS_TABLE, permissionsQuery);
                    if (permissions == null || (permissions != null
                                                && permissions.length == 0))
                    {
                        // remove the orphaned settings from settings table
                        StorageEntry removeSettingsQuery = new StorageEntry();
                        removeSettingsQuery.addAttribute(new StorageAttribute(
                                                             StorageAttribute.ID,
                                                             newAppUID.getStringValue()));
                        removeSettingsQuery.addAttribute(new StorageAttribute(
                                                             StorageNames.FUNCTION_GROUP,
                                                             settings.getName()));
                        doStorageRemove(StorageNames.MIDP_FUNC_GRP_SETTINGS_TABLE,
                                        removeSettingsQuery);
                    }
                }
            }
        }
        // write the new permissions
        writeGrantedPermissions(newAppUID, null, newGrantedPermissions, true /* preserveSettings */);
    }

    private AuthenticationStorageData readAuthenticationStorageData(
        Uid appUID, String aAppName, String aAppVersion,
        String aAppVendor, int readFilter)
    {
        if (appUID == null && (aAppName == null
                               || aAppVersion == null || aAppVendor == null))
        {
            return null;
        }
        // decode the filter
        boolean readDomainName = ((readFilter & AUTHENTICATION_DOMAIN_NAME_QUERY) != 0);
        boolean readDomainCategory = ((readFilter & AUTHENTICATION_DOMAIN_CATEGORY_QUERY) != 0);
        boolean readRootHash = ((readFilter & AUTHENTICATION_ROOT_HASH_QUERY) != 0);
        boolean readJarHash = ((readFilter & AUTHENTICATION_JAR_HASH_QUERY) != 0);
        boolean readValidCerts = ((readFilter & AUTHENTICATION_VALID_CERTS_QUERY) != 0);
        if (!readDomainName && !readDomainCategory
                && !readRootHash && !readJarHash && !readValidCerts)
        {
            // nothing has been requested
            return null;
        }
        StorageAttribute ID = null;
        if (appUID != null)
        {
            ID = new StorageAttribute(
                StorageAttribute.ID,
                appUID.getStringValue());
        }
        else
        {
            ID = readApplicationID(aAppName, aAppVersion, aAppVendor);
            if (ID == null)
            {
                return null;
            }
        }
        StorageEntry query = new StorageEntry();
        query.addAttribute(ID);
        AuthenticationStorageData authData = null;
        if (readDomainName)
        {
            query.addAttribute(new StorageAttribute(
                                   StorageNames.SECURITY_DOMAIN,
                                   ""));
        }
        if (readDomainCategory)
        {
            query.addAttribute(new StorageAttribute(
                                   StorageNames.SECURITY_DOMAIN_CATEGORY,
                                   ""));
        }
        if (readJarHash)
        {
            query.addAttribute(new StorageAttribute(
                                   StorageNames.HASH,
                                   ""));
        }
        if (readRootHash)
        {
            query.addAttribute(new StorageAttribute(
                                   StorageNames.CERT_HASH,
                                   ""));
        }
        if (readValidCerts)
        {
            query.addAttribute(new StorageAttribute(
                                   StorageNames.VALID_CERTS,
                                   ""));
        }
        StorageEntry[] entries = doStorageSearch(
                                     StorageNames.MIDP_PACKAGE_TABLE, query);
        if (entries != null && entries.length > 0)
        {
            try
            {
                authData = new AuthenticationStorageData(
                    getStorageAttributeValue(entries[0],
                                             StorageNames.SECURITY_DOMAIN),
                    getStorageAttributeValue(entries[0],
                                             StorageNames.SECURITY_DOMAIN_CATEGORY),
                    getStorageAttributeValue(entries[0],
                                             StorageNames.HASH),
                    getStorageAttributeValue(entries[0],
                                             StorageNames.CERT_HASH),
                    decodeValidatedChainIndexes(getStorageAttributeValue(
                                                    entries[0],StorageNames.VALID_CERTS)));
            }
            catch (NumberFormatException e)
            {
                Logger.ELOG(Logger.EJavaSecurity, "Authentication read failed", e);
            }
        }
        return authData;
    }

    private AppAccessAuthorizationStorageData readAppAccessAuthorizationStorageData(
        Uid appUID, String aAppName, String aAppVersion,
        String aAppVendor, int readFilter)
    {
        if (appUID == null && (aAppName == null
                               || aAppVersion == null || aAppVendor == null))
        {
            return null;
        }
        // decode the filter
        boolean readDomain = ((readFilter & APP_ACCESS_AUTH_DOMAIN_QUERY) != 0);
        boolean readVendor = ((readFilter & APP_ACCESS_AUTH_VENDOR_QUERY) != 0);
        boolean readSignersList = ((readFilter & APP_ACCESS_AUTH_SIGNERS_LIST_QUERY) != 0);
        if (!readDomain && !readVendor && !readSignersList)
        {
            // nothing has been requested
            return null;
        }
        StorageAttribute ID = null;
        if (appUID != null)
        {
            ID = new StorageAttribute(
                StorageAttribute.ID,
                appUID.getStringValue());
        }
        else
        {
            ID = readApplicationID(aAppName, aAppVersion, aAppVendor);
            if (ID == null)
            {
                return null;
            }
        }
        StorageEntry query = new StorageEntry();
        query.addAttribute(ID);
        StorageEntry[] entries = null;
        String vendorName = null;
        String domainName = null;
        String[] signersList = null;
        // read the vendor name
        if (readVendor)
        {
            if (aAppVendor != null)
            {
                vendorName = aAppVendor;
            }
            else
            {
                query.addAttribute(new StorageAttribute(
                                       StorageNames.VENDOR,
                                       ""));
                entries = doStorageSearch(
                              StorageNames. APPLICATION_PACKAGE_TABLE, query);
                if (entries != null && entries.length > 0)
                {
                    vendorName = getStorageAttributeValue(entries[0],
                                                          StorageNames.VENDOR);
                }
            }
        }
        if (readDomain || readSignersList)
        {
            query.removeAll();
            query.addAttribute(ID);
            if (readDomain)
            {
                query.addAttribute(new StorageAttribute(
                                       StorageAttribute.SECURITY_DOMAIN,
                                       ""));
            }
            if (readSignersList)
            {
                query.addAttribute(new StorageAttribute(
                                       StorageAttribute.VALID_CERTS,
                                       ""));
            }
            entries = doStorageSearch(
                          StorageNames.MIDP_PACKAGE_TABLE, query);
            if (entries != null && entries.length > 0)
            {
                domainName = getStorageAttributeValue(entries[0],
                                                      StorageNames.SECURITY_DOMAIN);
                Vector validatedChainIndexes = decodeValidatedChainIndexes(
                                                   getStorageAttributeValue(entries[0],
                                                                            StorageNames.VALID_CERTS));
                if (validatedChainIndexes != null)
                {
                    Vector vSignersList = new Vector();
                    for (int i=0; i<validatedChainIndexes.size(); i++)
                    {
                        // retrieve the end entity cert from the succesfully
                        // validated certs
                        String attrName = AuthenticationAttribute
                                          .MAIN_ATTRIBUTE_PREFIX
                                          + ((Integer)validatedChainIndexes.elementAt(i))
                                          .intValue() + "-1";
                        query.removeAll();
                        query.addAttribute(ID);
                        query.addAttribute(new StorageAttribute(
                                               StorageNames.NAME, attrName));
                        query.addAttribute(new StorageAttribute(
                                               StorageNames.VALUE, ""));
                        entries = doStorageSearch(StorageNames
                                                  .APPLICATION_PACKAGE_ATTRIBUTES_TABLE,query);
                        if (entries != null && entries.length > 0)
                        {
                            vSignersList.addElement(getStorageAttributeValue(
                                                        entries[0],StorageNames.VALUE));
                        }
                    }
                    if (vSignersList.size() > 0)
                    {
                        signersList = new String[vSignersList.size()];
                        vSignersList.copyInto(signersList);
                    }
                }
            }
        }
        if (domainName == null && vendorName == null && signersList == null)
        {
            return null;
        }
        return new AppAccessAuthorizationStorageData(
                   vendorName, domainName, signersList);
    }

    private StorageAttribute readApplicationID(String aAppName,
            String aAppVersion,
            String aAppVendor)
    {
        StorageEntry query = new StorageEntry();
        StorageAttribute NAME = new StorageAttribute(
            StorageAttribute.PACKAGE_NAME,
            aAppName);
        StorageAttribute VERSION = new StorageAttribute(
            StorageAttribute.VERSION,
            aAppVersion);
        StorageAttribute VENDOR = new StorageAttribute(
            StorageAttribute.VENDOR,
            aAppVendor);
        query.addAttribute(NAME);
        query.addAttribute(VERSION);
        query.addAttribute(VENDOR);
        query.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               ""));
        StorageEntry[] entries = doStorageSearch(
                                     StorageNames. APPLICATION_PACKAGE_TABLE, query);
        if (entries != null && entries.length > 0)
        {
            return entries[0].getAttribute(StorageNames.ID);
        }
        return null;
    }

    private void updateFunctionGroupTable(Uid msUID, StorageEntry entry)
    {
        StorageEntry oldEntry = new StorageEntry();
        oldEntry.addAttribute(new StorageAttribute(
                                  StorageAttribute.ID,
                                  msUID.getStringValue()));
        oldEntry.addAttribute(new StorageAttribute(
                                  StorageNames.FUNCTION_GROUP,
                                  getStorageAttributeValue(entry,
                                                           StorageNames.FUNCTION_GROUP)));
        doStorageUpdate(StorageNames.MIDP_FUNC_GRP_SETTINGS_TABLE,
                        entry,
                        oldEntry);
    }

    private void doStorageOpen()
    {
        doStorageOperation(STORAGE_OPEN, null, null, null, null);
    }

    private void doStorageClose()
    {
        doStorageOperation(STORAGE_CLOSE, null, null, null, null);
    }

    private StorageEntry[] doStorageSearch(String tableName, StorageEntry entry)
    {
        return doStorageOperation(STORAGE_SEARCH, tableName, entry, null, null);
    }

    private void doStorageWrite(String tableName, StorageEntry entry)
    {
        doStorageOperation(STORAGE_WRITE, tableName, entry, null, null);
    }

    private void doStorageUpdate(String tableName, StorageEntry newEntry, StorageEntry oldEntry)
    {
        doStorageOperation(STORAGE_UPDATE, tableName, newEntry, oldEntry, null);
    }

    private void doStorageRemove(String tableName, StorageEntry removeEntry)
    {
        doStorageOperation(STORAGE_REMOVE, tableName, null, removeEntry, null);
    }

    private void doStorageRemove(String tableName, Uid removeKey)
    {
        doStorageOperation(STORAGE_REMOVE, tableName, null, null, removeKey);
    }

    // have all the session operations in one place, so that error handling is centralized
    private StorageEntry[] doStorageOperation(int opID, String tableName, StorageEntry newEntry, StorageEntry oldEntry, Uid removeKey)
    {
        switch (opID)
        {
        case STORAGE_OPEN:
            session = StorageFactory.createSession();
            session.open();
            break;
        case STORAGE_CLOSE:
            if (destroyStorageSession)
            {
                session.close();
                session.destroySession();
            }
            break;
        case STORAGE_SEARCH:
            return session.search(tableName, newEntry);
        case STORAGE_WRITE:
            session.write(tableName, newEntry);
            break;
        case STORAGE_UPDATE:
            session.update(tableName, newEntry, oldEntry);
            break;
        case STORAGE_REMOVE:
            if (removeKey != null)
            {
                session.remove(tableName, removeKey);
            }
            else
            {
                session.remove(tableName, oldEntry);
            }
            break;
        }
        return null;
    }

    private String encodeValidatedChainIndexes(int[] validatedChainIndexes)
    {
        String strValidatedChainIndexes = null;
        if (validatedChainIndexes != null && validatedChainIndexes.length > 0)
        {
            strValidatedChainIndexes = "";
            for (int i=0; i<validatedChainIndexes.length -1; i++)
            {
                strValidatedChainIndexes = strValidatedChainIndexes
                                           + validatedChainIndexes[i] + ",";
            }
            strValidatedChainIndexes = strValidatedChainIndexes
                                       + validatedChainIndexes[validatedChainIndexes.length - 1];
        }
        return strValidatedChainIndexes;
    }

    private Vector decodeValidatedChainIndexes(String strValidatedChainIndexes)
    throws NumberFormatException
    {
        if (strValidatedChainIndexes == null
                || strValidatedChainIndexes.length() == 0)
        {
            return null;
        }
        String[] tokens = Tokenizer.split(strValidatedChainIndexes, ",");
        Vector validatedChainIndexes = null;
        if (tokens != null && tokens.length > 0)
        {
            validatedChainIndexes = new Vector();
            for (int i=0; i<tokens.length; i++)
            {
                validatedChainIndexes.addElement(Integer.valueOf(tokens[i]));
            }
        }
        return validatedChainIndexes;
    }

    private int[] decodeAllowedSettings(int allowedSettings)
    {
        Vector vAllowedSettings = new Vector();
        int tmp = allowedSettings & BLANKET_MASK;
        if (tmp > 0)
        {
            vAllowedSettings.addElement(
                new Integer(UserSecuritySettings.BLANKET_INTERACTION_MODE));
        }
        tmp = allowedSettings & SESSION_MASK;
        if (tmp > 0)
        {
            vAllowedSettings.addElement(
                new Integer(UserSecuritySettings.SESSION_INTERACTION_MODE));
        }
        tmp = allowedSettings & ONESHOT_MASK;
        if (tmp > 0)
        {
            vAllowedSettings.addElement(
                new Integer(UserSecuritySettings.ONESHOT_INTERACTION_MODE));
        }
        tmp = allowedSettings & NO_MASK;
        if (tmp > 0)
        {
            vAllowedSettings.addElement(
                new Integer(UserSecuritySettings.NO_INTERACTION_MODE));
        }
        int[] settings = new int[vAllowedSettings.size()];
        for (int i=0; i<vAllowedSettings.size(); i++)
        {
            settings[i] = ((Integer)vAllowedSettings.elementAt(i)).intValue();
        }
        return settings;
    }

    private int encodeAllowedSettings(int[] allowedSettings)
    {
        int settings = 0;
        if (allowedSettings != null)
        {
            for (int i=0; i<allowedSettings.length; i++)
            {
                switch (allowedSettings[i])
                {
                case UserSecuritySettings.NO_INTERACTION_MODE:
                    settings = settings | NO_MASK;
                    break;
                case UserSecuritySettings.ONESHOT_INTERACTION_MODE:
                    settings = settings | ONESHOT_MASK;
                    break;
                case UserSecuritySettings.SESSION_INTERACTION_MODE:
                    settings = settings | SESSION_MASK;
                    break;
                case UserSecuritySettings.BLANKET_INTERACTION_MODE:
                    settings = settings | BLANKET_MASK;
                    break;
                }
            }
        }
        return settings;
    }

    private String getStorageAttributeValue(StorageEntry entry, String attrName)
    {
        String attrValue = null;
        StorageAttribute attr = entry.getAttribute(attrName);
        if (attr != null)
        {
            String tmp = attr.getValue();
            if (tmp != null && tmp.length() > 0)
            {
                return attrValue = tmp;
            }
        }
        return attrValue;
    }

    private int removeElement(Vector elements, PolicyBasedPermission element)
    {
        PolicyBasedPermissionImpl p1 = new PolicyBasedPermissionImpl(
            element.getName(),
            element.getTarget(),
            element.getActionList(),
            null);
        for (int i=0; i<elements.size(); i++)
        {
            PolicyBasedPermission tmp = (PolicyBasedPermission)elements
                                        .elementAt(i);
            PolicyBasedPermissionImpl p2 = new PolicyBasedPermissionImpl(
                tmp.getName(),
                tmp.getTarget(),
                tmp.getActionList(),
                null);
            if (p1.equals(p2))
            {
                UserSecuritySettings s1 = element.getUserSecuritySettings();
                UserSecuritySettings s2 = tmp.getUserSecuritySettings();
                if ((s1 == null && s2 == null)
                        || (s1 != null
                            && s2 != null
                            && s1.equals(s2)))
                {
                    // identical permissions
                    elements.removeElementAt(i);
                    return REMOVED;
                }
                return i;
            }
        }
        return NOT_FOUND;
    }
    
    private String encodeFunctionGroup(int type, String name)
    {
        return (type + ";" + name);
    }
    
    private FunctionGroup decodeFunctionGroup(String fg)
    {
        boolean isActive = true;
        if (fg != null)
        {
            int sepIndex = fg.indexOf(";");
            int type = PolicyBasedPermission.USER_TYPE;
            if (sepIndex != -1)
            {
                isActive = false;
                try
                {
                    type = Integer.valueOf(
                        fg.substring(0, sepIndex)).intValue();
                }
                catch (NumberFormatException e)
                {
                }
            }
            return new FunctionGroup(type, fg.substring(sepIndex + 1), isActive);
        }
        return null;
    }
    
    private class FunctionGroup
    {
        FunctionGroup(int type, String name, boolean isActive)
        {
            this.type = type;
            this.name = name;
            this.isActive = isActive;
        }
        private boolean isActive;
        private int type;
        private String name;
    }
}
