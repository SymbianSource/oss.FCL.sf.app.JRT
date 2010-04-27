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

import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.GeneralSecuritySettings;
import com.nokia.mj.impl.security.midp.common.SecurityExtensionsReader;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.utils.ResourceUtil;
import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;
import java.io.ObjectInputStream;
import java.io.InputStream;
import java.io.IOException;

/**
 * The security policy module is the entity which manages function
 * groups/permissions and their allowed access levels corresponding to
 * each protection domain
 */
public final class SecurityPolicyModule
{
    /**
     * The pre-defined domains
     */
    public static final String MANUFACTURER_DOMAIN = "Manufacturer";
    public static final String OPERATOR_DOMAIN = "Operator";
    public static final String OPERATOR_EXTRA_DOMAIN = "OperatorExtra";
    public static final String IDENTIFIED_THIRD_PARTY_DOMAIN = "IdentifiedThirdParty";
    public static final String UNIDENTIFIED_THIRD_PARTY_DOMAIN = "UnidentifiedThirdParty";
    private static final String ALL_DOMAIN = "All";

    private static final String MANUFACTURER_DOMAIN_FILE_NAME_PATTERN = "manufacturer";
    private static final String OPERATOR_DOMAIN_FILE_NAME_PATTERN = "operator";
    private static final String OPERATOR_EXTRA_DOMAIN_FILE_NAME_PATTERN = "operatorextra";
    private static final String IDENTIFIED_THIRD_PARTY_DOMAIN_FILE_NAME_PATTERN = "trustedthirdparty";
    private static final String UNIDENTIFIED_THIRD_PARTY_DOMAIN_FILE_NAME_PATTERN = "untrusted";
    /**
     * Location (root) of the policies
     */
    static String policiesDir = null;

    /**
     * Pattern for the name of the policies in use
     */
    static String policiesFileNamePrefix = "s60_";
    static String policiesFileNameSuffix = ".ser";

    /**
     * The hashtable containing the policies for different domains
     */
    private static Hashtable policies = new Hashtable();

    /**
     * The hashtable containing the extended policies for different domains
     */
    private static Hashtable extPolicies = new Hashtable();

    static
    {
        String policy = GeneralSecuritySettings.getSecurityPolicy();
        if (policy != null)
        {
            policiesFileNamePrefix = policy + "_";
        }
    }

    /**
     * Creates an instance of the SecurityPolicyModule
     *
     * @return An instance of SecurityPolicyModule
     */
    public static SecurityPolicyModule getInstance()
    {
        if (self == null)
        {
            self = new SecurityPolicyModule();
        }
        return self;
    }

    public PolicyBasedPermission[] getPermissions(String protectionDomain)
    {
        if (protectionDomain == null)
        {
            return null;
        }
        SecurityPolicy policy = getPolicy(protectionDomain.toLowerCase());
        if (policy == null)
        {
            return null;
        }
        return policy.getPermissions();
    }

    private synchronized SecurityPolicy getPolicy(String protectionDomain)
    {
        // for a certain protection domain, read the policy only once and when
        // really needed
        if (policies.get(protectionDomain) == null)
        {
            initPolicies(protectionDomain);
        }
        // get the policy for the queried domain and append permissions
        // from the "All" domain, containing the default permissions
        SecurityPolicy policy = (SecurityPolicy)policies.get(
                                    protectionDomain);
        if (policy != null)
        {
            policy = policy.append((SecurityPolicy)policies.get(
                                       ALL_DOMAIN.toLowerCase()));
            // append the extensions permissions
            policy = policy.append((SecurityPolicy)extPolicies.get(
                                       protectionDomain));
            // append the extensions ALL permissions
            policy = policy.append((SecurityPolicy)extPolicies.get(
                                       ALL_DOMAIN.toLowerCase()));
        }
        return policy;
    }

    void initPolicies(String protectionDomain)
    {
        // collect the security policies from all the DRIVES,
        // and do replace existing ones (because the policies
        // are retrieved first from C: then Z: etc.)
        if (policiesDir == null)
        {
            policiesDir = ResourceUtil.getResourceDir(0);
        }
        Vector extPoliciesFileNames = SecurityExtensionsReader.getExtPoliciesFileNames();
        String domainFileNamePattern = null;
        if (extPoliciesFileNames.size() == 0)
        {
            // if there are no extensions, then it's enough to read only the
            // policy which corresponds to the requested domain
            if (UNIDENTIFIED_THIRD_PARTY_DOMAIN.equalsIgnoreCase(protectionDomain))
            {
                domainFileNamePattern = UNIDENTIFIED_THIRD_PARTY_DOMAIN_FILE_NAME_PATTERN;
            }
            else if (IDENTIFIED_THIRD_PARTY_DOMAIN.equalsIgnoreCase(protectionDomain))
            {
                domainFileNamePattern = IDENTIFIED_THIRD_PARTY_DOMAIN_FILE_NAME_PATTERN;
            }
            else if (OPERATOR_DOMAIN.equalsIgnoreCase(protectionDomain))
            {
                domainFileNamePattern = OPERATOR_DOMAIN_FILE_NAME_PATTERN;
            }
            else if (OPERATOR_EXTRA_DOMAIN.equalsIgnoreCase(protectionDomain))
            {
                domainFileNamePattern = OPERATOR_EXTRA_DOMAIN_FILE_NAME_PATTERN;
            }
            else if (MANUFACTURER_DOMAIN.equalsIgnoreCase(protectionDomain))
            {
                domainFileNamePattern = MANUFACTURER_DOMAIN_FILE_NAME_PATTERN;
            }
        }
        else
        {
            // if there are extensions, then we need to read all the policies
            // from all the domains, since we need to ensure that extension
            // policies can not collide with any of the base policies
            domainFileNamePattern = "*";
        }
        String masterPolicyFileNamePattern = policiesFileNamePrefix + domainFileNamePattern + policiesFileNameSuffix;
        for (int i = 1; policiesDir != null; i++)
        {
            if (!policiesDir.endsWith("/")
                    && !policiesDir.endsWith("\\"))
            {
                policiesDir = policiesDir + System.getProperty("file.separator");
            }
            policiesDir = policiesDir
                          + "security"
                          + System.getProperty("file.separator")
                          + "policies"
                          + System.getProperty("file.separator");
            try
            {
                FileUtility dir = new FileUtility(policiesDir);
                Vector entries = dir.listFiles(masterPolicyFileNamePattern, false);
                if (entries.size() > 0)
                {
                    // within the same policies dir we do collect all he permissions,
                    // but we overwrite when starting from new directory
                    Hashtable appendAllowed = new Hashtable();
                    for (int j=0; j<entries.size(); j++)
                    {
                        initPolicy(policiesDir + (String)entries.elementAt(j), appendAllowed);
                    }
                    // init the policy for default permissions
                    initPolicy(policiesDir + ALL_DOMAIN.toLowerCase() + ".ser", appendAllowed);
                }
            }
            catch (IOException e) {}
            // move on to the next directory
            policiesDir = ResourceUtil.getResourceDir(i);
        }

        // init extension policies: handle the policies as a group
        // do the validation against the existing collection of extension
        // policies and add all the policies from a group at once into
        // the collection of extensions policies. In this way policies
        // from the same group don't get validated against each other.
        // The SecurityExtensionsReader.POLICIES_SEPARATOR defines a group
        Hashtable tmpPolicies = new Hashtable();
        for (int i=0; i<extPoliciesFileNames.size(); i++)
        {
            if (((String)extPoliciesFileNames.elementAt(i)).equals(
                        SecurityExtensionsReader.POLICIES_SEPARATOR))
            {
                // pour the tmpPolicies into the extPolicies
                for (Enumeration e = tmpPolicies.keys() ; e.hasMoreElements() ;)
                {
                    String domain = (String)e.nextElement();
                    SecurityPolicy tmpPolicy = (SecurityPolicy)tmpPolicies
                                               .get(domain.toLowerCase());
                    SecurityPolicy extPolicy = (SecurityPolicy)extPolicies
                                               .get(domain.toLowerCase());
                    if (extPolicy != null)
                    {
                        extPolicy = extPolicy.append(
                                        tmpPolicy);
                    }
                    else
                    {
                        extPolicy = tmpPolicy;
                    }
                    extPolicies.put(
                        domain.toLowerCase(),
                        extPolicy);
                }
                // reset the tmpPolicies
                tmpPolicies = new Hashtable();
            }
            initExtPolicy((String)extPoliciesFileNames.elementAt(i), tmpPolicies);
        }
    }

    private void initPolicy(String policyFilePath, Hashtable appendAllowed)
    {
        InputStream policyStream = null;
        ObjectInputStream in = null;
        try
        {
            FileUtility entry = new FileUtility(policyFilePath);
            if (entry.isFile())
            {
                policyStream = entry.openInputStream();
                in = new ObjectInputStream(policyStream);
                SecurityPolicy policy = (SecurityPolicy)in.readObject();
                if (policy != null
                        && policy.getProtectionDomain() != null)
                {
                    String domain = policy.getProtectionDomain().toLowerCase();
                    if (appendAllowed.get(domain) !=  null)
                    {
                        // do append
                        SecurityPolicy currPolicy = (SecurityPolicy)policies
                                                    .get(domain);
                        if (currPolicy != null)
                        {
                            currPolicy = currPolicy.append(
                                             policy);
                        }
                        else
                        {
                            currPolicy = policy;
                        }
                        policies.put(domain, currPolicy);
                    }
                    else
                    {
                        // do replace
                        appendAllowed.put(domain, "");
                        policies.put(domain, policy);
                    }
                }
                in.close();
            }
        }
        catch (IOException ex) {}
        catch (ClassNotFoundException ex) {}
        catch (IllegalArgumentException ex) {}
        // recover the resources
        finally
        {
            try
            {
                if (policyStream != null)
                {
                    policyStream.close();
                }
            }
            catch (IOException e) {}
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (IOException e) {}
        }
    }

    private void initExtPolicy(String extPolicyFilePath, Hashtable tmpPolicies)
    {
        InputStream policyStream = null;
        ObjectInputStream in = null;
        try
        {
            FileUtility entry = new FileUtility(extPolicyFilePath);
            if (entry.isFile())
            {
                policyStream = entry.openInputStream();
                in = new ObjectInputStream(policyStream);
                SecurityPolicy policy = (SecurityPolicy)in.readObject();
                if (policy != null
                        && policy.getProtectionDomain() != null)
                {
                    String extPolicyDomain = policy.getProtectionDomain();
                    PolicyBasedPermission[] extPermissions = policy.getPermissions();
                    if (extPermissions != null)
                    {
                        Vector okPermissions = new Vector();
                        for (int i=0; i<extPermissions.length; i++)
                        {
                            SecurityPolicyPermissionSettings settings = null;
                            SecurityPolicyPermission foundPerm = find(
                                                                     extPolicyDomain,
                                                                     extPermissions[i], policies);
                            settings = (SecurityPolicyPermissionSettings)foundPerm
                                       .getUserSecuritySettings();
                            // discard permissions which exist in the base policy
                            if (foundPerm.getName() == null)
                            {
                                foundPerm = find(extPolicyDomain,
                                                 extPermissions[i], extPolicies);
                                if (settings == null)
                                {
                                    settings = (SecurityPolicyPermissionSettings)foundPerm
                                               .getUserSecuritySettings();
                                }
                            }
                            if (foundPerm.getName() == null)
                            {
                                // extPermissions[i] is a good one
                                if (settings == null)
                                {
                                    okPermissions.addElement(extPermissions[i]);
                                }
                                else
                                {
                                    okPermissions.addElement(new SecurityPolicyPermission(
                                                                 extPermissions[i].getName(),
                                                                 extPermissions[i].getTarget(),
                                                                 extPermissions[i].getActionList(),
                                                                 settings));
                                }
                            }
                            else
                            {
                                SecurityExtensionsReader.discard("policy "
                                                                 + extPermissions[i].getName()
                                                                 + " from "
                                                                 + extPolicyFilePath
                                                                 + " is discarded because it already exists");
                            }
                        }
                        // collect all the permissions from all the policies
                        if (okPermissions.size() > 0)
                        {
                            SecurityPolicyPermission[] okPerms = new SecurityPolicyPermission[okPermissions.size()];
                            okPermissions.copyInto(okPerms);
                            SecurityPolicy extPolicy = (SecurityPolicy)tmpPolicies.get(
                                                           extPolicyDomain.toLowerCase());
                            if (extPolicy != null)
                            {
                                extPolicy = extPolicy.append(
                                                new SecurityPolicy(extPolicyDomain,
                                                                   okPerms));
                            }
                            else
                            {
                                extPolicy = new SecurityPolicy(
                                    extPolicyDomain, okPerms);
                            }
                            tmpPolicies.put(
                                extPolicyDomain.toLowerCase(),
                                extPolicy);
                        }
                    }
                }
                in.close();
            }
        }
        catch (IOException ex) {}
        catch (ClassNotFoundException ex) {}
        catch (IllegalArgumentException ex) {}
        // recover the resources
        finally
        {
            try
            {
                if (policyStream != null)
                {
                    policyStream.close();
                }
            }
            catch (IOException e) {}
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (IOException e) {}
        }
    }

    private SecurityPolicyPermission find(String aExtPolicyDomain, PolicyBasedPermission aExtPerm, Hashtable aPolicies)
    {
        SecurityPolicyPermissionSettings settings = null;
        boolean settingsFound = (aExtPerm.getUserSecuritySettings() == null);
        boolean permissionFound = false;
        for (Enumeration e = aPolicies.keys() ; e.hasMoreElements() ;)
        {
            String domain = (String)e.nextElement();
            boolean sameDomain = (domain.equalsIgnoreCase(aExtPolicyDomain));
            PolicyBasedPermission[] policyPermissions = ((SecurityPolicy)aPolicies
                    .get(domain)).getPermissions();
            if (policyPermissions != null)
            {
                for (int j=0; j<policyPermissions.length; j++)
                {
                    // pick up the user settings (if any)
                    if (sameDomain
                            && !settingsFound
                            && policyPermissions[j].getUserSecuritySettings() != null)
                    {
                        if (aExtPerm.getUserSecuritySettings().getName()
                                .equalsIgnoreCase(policyPermissions[j]
                                                  .getUserSecuritySettings().getName()))
                        {
                            settingsFound = true;
                            settings = (SecurityPolicyPermissionSettings)policyPermissions[j]
                                       .getUserSecuritySettings();
                        }
                    }
                    if (!permissionFound
                            && policyPermissions[j].getName()
                            .equalsIgnoreCase(aExtPerm.getName()))
                    {
                        if (settingsFound)
                        {
                            return new SecurityPolicyPermission(
                                       aExtPerm.getName(),
                                       null, null,
                                       settings);
                        }
                        permissionFound = true;
                    }
                }
            }
        }
        return new SecurityPolicyPermission(
                   (permissionFound ? aExtPerm.getName() : null),
                   null, null, settings);
    }

    private static SecurityPolicyModule self;
}
