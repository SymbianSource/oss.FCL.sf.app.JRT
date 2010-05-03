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

package com.nokia.mj.impl.security.midp.common;

import java.util.Hashtable;
import java.util.Enumeration;
import java.util.Vector;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.security.utils.Logger;

/**
 * This class extracts all the security related attributes from raw attributes
 * read from either JAD and/or JAR
 */
public class SecurityAttributes
{
    public static final String MIDP_VERSION_ATTRIBUTE_NAME
    = "MicroEdition-Profile";
    public static final String MIDP3_VERSION_ATTRIBUTE_VALUE = "MIDP-3.0";
    private DescriptorAttributes descriptorAttributes;
    private ManifestAttributes manifestAttributes;

    public void addDescriptorAttributes(Hashtable attrNamesAndValues)
    {
        descriptorAttributes = new DescriptorAttributes(attrNamesAndValues);
        // try to validate also attributes which are targeted for manifest
        //(detect errors as soon as possible if all the attributes are in JAD)
        boolean signedSuite = (descriptorAttributes != null
                               && descriptorAttributes.authenticationAttributes != null);
        manifestAttributes = new ManifestAttributes(attrNamesAndValues,
                signedSuite);
    }

    public void addManifestAttributes(Hashtable attrNamesAndValues)
    {
        boolean signedSuite = (descriptorAttributes != null
                               && descriptorAttributes.authenticationAttributes != null);
        // if we already have JAD attributes, make sure the
        // JAD attributes are a subset of the manifest
        // attributes and do the validation only for the ones which
        // are in manifest but not in descriptor
        Hashtable manifestOnlyAttributes;
        int optPermsStartIndex = 1;
        int mandatoryPermsStartIndex = 1;
        int authorizationRestrictionsStartIndex = 1;
        // save the jad attributes
        AuthorizationRestrictionAttribute[] authRestrictionAttributes =
            getAuthorizationRestrictionAttributes();
        NetworkRestrictionsAttribute networkRestrictionsAttribute =
            getNetworkRestrictionsAttribute();
        if (authRestrictionAttributes != null)
        {
            authorizationRestrictionsStartIndex =
                authRestrictionAttributes.length + 1;
        }
        if (signedSuite)
        {
            PermissionAttribute[] permissionAttributes =
                getPermissionAttributes();
            manifestOnlyAttributes = getManifestOnlyAttributes(
                                         permissionAttributes,
                                         networkRestrictionsAttribute,
                                         authRestrictionAttributes,
                                         attrNamesAndValues);
            // update the start index for opt and mandatory perms
            if (permissionAttributes != null)
            {
                for (int i=0; i< permissionAttributes.length; i++)
                {
                    if (permissionAttributes[i].getImportance()
                            == PermissionAttribute.MANDATORY_PERMISSION)
                    {
                        mandatoryPermsStartIndex++;
                    }
                    else
                    {
                        optPermsStartIndex++;
                    }
                }
            }
        }
        else
        {
            manifestOnlyAttributes = getManifestOnlyAttributes(
                                         null /* no check for permissions */,
                                         null, /* no check for network restrictions */
                                         authRestrictionAttributes,
                                         attrNamesAndValues);
        }
        descriptorAttributes = new DescriptorAttributes(
            manifestOnlyAttributes,
            optPermsStartIndex,
            mandatoryPermsStartIndex,
            false /* parse authentication info */,
            signedSuite /* parse permissions */);
        manifestAttributes = new ManifestAttributes(manifestOnlyAttributes,
                authorizationRestrictionsStartIndex, signedSuite);
        // no need to combine he jad with jar attributes
    }

    public PermissionAttribute[] getPermissionAttributes()
    {
        return (descriptorAttributes == null
                ? null : descriptorAttributes.permissionAttributes);
    }

    public AuthenticationAttribute[] getAuthenticationAttributes()
    {
        return (descriptorAttributes == null
                ? null : descriptorAttributes.authenticationAttributes);
    }

    public AuthorizationRestrictionAttribute[] getAuthorizationRestrictionAttributes()
    {
        return (manifestAttributes == null
                ? null : manifestAttributes.authorizationRestrictionAttributes);
    }

    public NetworkRestrictionsAttribute getNetworkRestrictionsAttribute()
    {
        return (manifestAttributes == null
                ? null : manifestAttributes.networkRestrictionsAttribute);
    }

    /**
    * Returns a boolean indicating if the attributes correspond to a trusted
    * application suite
    *
    * @return True if the attributes correspond to a trusted suite and False
    *         otherwise
    */
    public boolean isTrusted()
    {
        return (descriptorAttributes != null
                && descriptorAttributes.authenticationAttributes != null
                && descriptorAttributes.authenticationAttributes.length > 0);
    }

    /**
     * Checks if JAD permissions are a subset of JAR permissions
     */
    private Hashtable getManifestOnlyAttributes(PermissionAttribute[] jadPermissions,
            NetworkRestrictionsAttribute jadNetworkRestrictions,
            AuthorizationRestrictionAttribute[] jadAuthorizations,
            Hashtable jarAttributes)
    {
        // setup a flag indicating if the attributes belong to a legacy or
        // MIDP3 application. For MIDP3 applications, the JAR permissions
        // MUST be a subset of JAD permissions. For legacy applications
        // this does not hold true
        boolean legacyApp = false;
        String midpVersion = getValue(jarAttributes,
                                      MIDP_VERSION_ATTRIBUTE_NAME);
        if (midpVersion == null || !midpVersion.equalsIgnoreCase(
                    MIDP3_VERSION_ATTRIBUTE_VALUE))
        {
            legacyApp = true;
        }

        // permissions
        if (jadPermissions != null)
        {
            // make sure we don't remove duplicates
            Vector permAttrNames = new Vector();
            for (int i=0; i<jadPermissions.length; i++)
            {
                if (permAttrNames.contains(jadPermissions[i]
                                           .getAttributeName()))
                {
                    continue;
                }
                permAttrNames.addElement(jadPermissions[i]
                                         .getAttributeName());
                Object o = jarAttributes.remove(
                               jadPermissions[i].getAttributeName());
                if (o == null)
                {
                    jadAttributeNotFound(legacyApp,
                                         jadPermissions[i].getAttributeName());
                }
            }
        }
        // network restrictions
        if (jadNetworkRestrictions != null)
        {
            Object o = jarAttributes.remove(NetworkRestrictionsAttribute
                                            .ATTRIBUTE_NAME);
            if (o == null)
            {
                jadAttributeNotFound(false,
                                     NetworkRestrictionsAttribute.ATTRIBUTE_NAME);
            }
        }
        // authorization restrictions
        if (jadAuthorizations != null)
        {
            for (int i=0; i<jadAuthorizations.length; i++)
            {
                Object o = jarAttributes.remove(
                               jadAuthorizations[i].getMainAttributeName());
                if (o == null)
                {
                    jadAttributeNotFound(false,
                                         jadAuthorizations[i].getMainAttributeName());
                }
                // delete quitely the second attribute name
                jarAttributes.remove(
                    jadAuthorizations[i].getSecondAttributeName());
            }
        }
        // return what's left from the jar attributes
        return jarAttributes;
    }

    /**
     * This class extracts all the security related attributes from raw attributes
     * read from a JAD
     */
    private class DescriptorAttributes
    {
        public PermissionAttribute[] permissionAttributes;
        public AuthenticationAttribute[] authenticationAttributes;

        public DescriptorAttributes(Hashtable attrNamesAndValues)
        {
            this(attrNamesAndValues,
                 1 /* opt permission attributes start index */,
                 1 /* mandatory permission attributes start index */,
                 true, true);
        }

        public DescriptorAttributes(Hashtable attrNamesAndValues,
                                    int optPermsStartIndex,
                                    int mandatoryPermsStartIndex,
                                    boolean parseAuthenticationInfo,
                                    boolean parsePermissions)
        {
            String midpVersion = getValue(attrNamesAndValues,
                                          MIDP_VERSION_ATTRIBUTE_NAME);
            if (midpVersion == null
                    || !midpVersion.equalsIgnoreCase(MIDP3_VERSION_ATTRIBUTE_VALUE))
            {
                // MIDP2 or MIDP1 MIDlet suite
                handleLegacyAttributes(attrNamesAndValues,
                                       parseAuthenticationInfo, parsePermissions);
            }
            else
            {
                // MIDP3 MIDlet suite
                handleMIDP3Attributes(attrNamesAndValues, optPermsStartIndex,
                                      mandatoryPermsStartIndex, parseAuthenticationInfo);
            }
        }

        private void handleLegacyAttributes(Hashtable attrNamesAndValues,
                                            boolean parseAuthenticationInfo,
                                            boolean parsePermissions)
        {
            // make sure there are no MIDP3 security attributes
            String midp3Attr = findFirstAppearance(attrNamesAndValues.keys(),
                                                   new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX,
                                                                 PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX,
                                                                 AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX
                                                                });
            if (midp3Attr != null)
            {
                Logger.logWarning(midp3Attr + " is a MIDP3 specific descriptor attribute, therefore it can only be placed into a MIDP3 descriptor");
                throw new InvalidAttributeException(
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                    new String[] {midp3Attr},
                    OtaStatusCode.INVALID_DESCRIPTOR);
            }
            PermissionAttribute[] optPerms = null;
            PermissionAttribute[] mandatoryPerms = null;
            if (parseAuthenticationInfo)
            {
                saveAuthenticationInfo(attrNamesAndValues, true);
            }
            if (parsePermissions
                    && ((parseAuthenticationInfo && authenticationAttributes != null)
                        || !parseAuthenticationInfo))
            {
                // Note: no need to check for signature, permission duplicates,
                // since it was done for all the attributes
                optPerms = OptionalPermissionAttribute
                           .HandleLegacyAttribute(getValue(attrNamesAndValues,
                                                           PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME));
                mandatoryPerms = MandatoryPermissionAttribute
                                 .HandleLegacyAttribute(getValue(attrNamesAndValues,
                                                                 PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME));
            }
            else
            {
                // MIDP2 unsigned MIDlets don't need to request permissions
                // If any permissions were present, they are ignored
                // create the PermissionAttribute with the flag on for legacy
                // attribute -> this flag will be used when granting permissions,
                // so that the policy permissions will be granted as such
                mandatoryPerms = new PermissionAttribute[1];
                mandatoryPerms[0] = new MandatoryPermissionAttribute();
            }
            // save the permissions
            savePermissions(optPerms, mandatoryPerms);
        }

        private void handleMIDP3Attributes(Hashtable attrNamesAndValues,
                                           int optPermsStartIndex,
                                           int mandatoryPermsStartIndex,
                                           boolean parseAuthenticationInfo)
        {
            // make sure there are no legacy signature,permission attributes
            String legacySigAttr = findFirstAppearance(attrNamesAndValues.keys(),
                                   new String[] {AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME,
                                                 PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME,
                                                 PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME
                                                },
                                   true);
            if (legacySigAttr != null)
            {
                Logger.logWarning(legacySigAttr + " is a MIDP2 specific descriptor attribute, therefore it can only be placed into a MIDP2 descriptor");
                throw new InvalidAttributeException(
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                    new String[] {legacySigAttr},
                    OtaStatusCode.INVALID_DESCRIPTOR);
            }
            String[] optPermValues = getAttributeValues(
                                         attrNamesAndValues,
                                         PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX,
                                         optPermsStartIndex);
            String[] mandatoryPermValues = getAttributeValues(
                                               attrNamesAndValues,
                                               PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX,
                                               mandatoryPermsStartIndex);
            // save the permissions
            savePermissions(optPermValues, mandatoryPermValues);
            // save the authentication attributes
            if (parseAuthenticationInfo)
            {
                saveAuthenticationInfo(attrNamesAndValues, false);
            }
        }

        private void savePermissions(Object[] optPerms, Object[] mandatoryPerms)
        {
            if (optPerms == null && mandatoryPerms == null)
            {
                // nothing to save
                return;
            }
            int length =
                (optPerms != null ? optPerms.length : 0) +
                (mandatoryPerms != null ? mandatoryPerms.length : 0);
            if (length == 0)
            {
                // nothing to save
                return;
            }
            permissionAttributes = new PermissionAttribute[length];
            int i= 0;
            if (optPerms != null && optPerms.length > 0)
            {
                boolean strings = (optPerms[0] instanceof String);
                for (i = 0; i<optPerms.length; i++)
                {
                    permissionAttributes[i] = (strings ?
                                               new OptionalPermissionAttribute(
                                                   PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + (i+1),
                                                   (String)optPerms[i]) : (PermissionAttribute)optPerms[i]);
                }
            }
            if (mandatoryPerms != null && mandatoryPerms.length > 0)
            {
                boolean strings = (mandatoryPerms[0] instanceof String);
                for (int j=0; j< mandatoryPerms.length; j++)
                {
                    permissionAttributes[i + j] = (strings ?
                                                   new MandatoryPermissionAttribute(
                                                       PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + (j+1),
                                                       (String)mandatoryPerms[j]) : (PermissionAttribute)mandatoryPerms[j]);
                }
            }
        }

        private void saveAuthenticationInfo(
            Hashtable attrNamesAndValues,
            boolean legacyAttributes)
        {
            Vector authAttributes = new Vector();
            int i=1;
            String signature = null;
            // give the attribute names to AuthenticationAttribute class
            // for logging pusposes in case of errors (e.g. signal the
            // missing of an attribute by pointing out the name of the
            // attribute which is missing)
            String certAttributeName = null;
            String signatureAttributeName = null;
            // if legacy, then signature appears only once
            if (legacyAttributes)
            {
                signatureAttributeName = AuthenticationAttribute
                                         .SECOND_LEGACY_ATTRIBUTE_NAME;
                signature = getValue(attrNamesAndValues,
                                     signatureAttributeName);
            }
            InvalidAttributeException firstException = null;
            while (true)
            {
                AuthenticationAttribute authAttribute = null;
                String[] certChain = getAttributeValues(attrNamesAndValues,
                                                        AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + i + "-");
                certAttributeName = AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX
                                    + i + "-";
                if (!legacyAttributes)
                {
                    signatureAttributeName =
                        AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + i;
                    signature = getValue(attrNamesAndValues,
                                         signatureAttributeName);
                }
                if (certChain == null && signature == null)
                {
                    break;
                }
                try
                {
                    if (legacyAttributes && i > 1)
                    {
                        if (certChain == null)
                        {
                            break;
                        }
                        authAttribute = new AuthenticationAttribute(
                            certAttributeName, certChain,
                            signatureAttributeName, signature,
                            true /* skip validation of signature*/);
                    }
                    else
                    {
                        authAttribute = new AuthenticationAttribute(
                            certAttributeName, certChain,
                            signatureAttributeName, signature);
                        authAttribute.setLegacyInfo(legacyAttributes);
                    }
                    if (!authAttributes.contains(authAttribute)
                            && authAttribute.certChain != null
                            && authAttribute.signature != null)
                    {
                        authAttributes.addElement(authAttribute);
                    }
                }
                catch (InvalidAttributeException e)
                {
                    // just collect the first invalid attribute exception.
                    // If all the attributes are invalid, then throw this
                    // exception. Otherwise ignore the invalid attributes
                    // and go on with the valid ones
                    if (firstException == null)
                    {
                        firstException = e;
                    }
                    Logger.log("The validation of the following attribute pair {" + certAttributeName + ", " + signatureAttributeName + "} failed with error " + e.getMessage());
                }
                // move on to the next attribute pair
                i++;
            }
            if (authAttributes.size() != 0)
            {
                this.authenticationAttributes =
                    new AuthenticationAttribute[authAttributes.size()];
                authAttributes.copyInto(this.authenticationAttributes);
            }
            else if (firstException != null)
            {
                // all the attributes were invalid -> signal the first error we found
                throw firstException;
            }
        }
    }

    /**
     * This class extracts all the security related attributes from raw attributes
     * read from a Manifest
     */
    private class ManifestAttributes
    {
        public AuthorizationRestrictionAttribute[] authorizationRestrictionAttributes;
        public NetworkRestrictionsAttribute networkRestrictionsAttribute;

        public ManifestAttributes(Hashtable attrNamesAndValues, boolean signedSuite)
        {
            this(attrNamesAndValues,
                 1 /* authorizationRestrictionsStartIndex */,
                 signedSuite);
        }

        public ManifestAttributes(Hashtable attrNamesAndValues,
                                  int authorizationRestrictionsStartIndex,
                                  boolean signedSuite)
        {
            // MIDP3 MIDlet suite
            handleMIDP3Attributes(attrNamesAndValues,
                                  authorizationRestrictionsStartIndex,
                                  signedSuite);
        }

        private void handleLegacyAttributes(Hashtable attrNamesAndValues)
        {
            // make sure there are no MIDP3 security attributes
            String midp3Attr = findFirstAppearance(attrNamesAndValues.keys(),
                                                   new String[]
                                                   {AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX,
                                                    AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX,
                                                    NetworkRestrictionsAttribute.ATTRIBUTE_NAME
                                                   });
            if (midp3Attr != null)
            {
                Logger.logWarning(midp3Attr + " is a MIDP3 specific descriptor attribute, therefore it can only be placed into a MIDP3 descriptor");
                throw new InvalidAttributeException(
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                    new String[] {midp3Attr},
                    OtaStatusCode.INVALID_JAR);
            }
        }

        private void handleMIDP3Attributes(Hashtable attrNamesAndValues,
                                           int authorizationRestrictionsStartIndex,
                                           boolean signedSuite)
        {
            Vector authRestrictions = new Vector();
            int i=authorizationRestrictionsStartIndex;
            // give the attribute names to AutheorizationRestrictionAttribute
            // class for logging pusposes in case of errors (e.g. signal the
            // missing of an attribute by pointing out the name of the
            // attribute which is missing)
            String authRestrictionAttributeName = null;
            String authRestrictionCertAttributeName = null;
            while (true)
            {
                authRestrictionAttributeName =
                    AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + i;
                String authRestriction = getValue(attrNamesAndValues,
                                                  authRestrictionAttributeName);
                authRestrictionCertAttributeName =
                    AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + i;
                String authRestrictionCert = getValue(attrNamesAndValues,
                                                      authRestrictionCertAttributeName);
                if (authRestriction == null && authRestrictionCert == null)
                {
                    break;
                }
                // validate the pair
                AuthorizationRestrictionAttribute authRestrictionPair =
                    new AuthorizationRestrictionAttribute(
                    authRestrictionAttributeName, authRestriction,
                    authRestrictionCertAttributeName, authRestrictionCert);
                // ignore duplicates
                if (!authRestrictions.contains(authRestrictionPair))
                {
                    authRestrictions.addElement(authRestrictionPair);
                }
                // move further
                i++;
            }
            if (authRestrictions.size() != 0)
            {
                this.authorizationRestrictionAttributes =
                    new AuthorizationRestrictionAttribute[authRestrictions.size()];
                authRestrictions.copyInto(this.authorizationRestrictionAttributes);
            }
            // if the suite is signed then parse&validate the network
            // restriction attribute
            if (signedSuite)
            {
                String networkRestrictions = getValue(attrNamesAndValues,
                                                      NetworkRestrictionsAttribute.ATTRIBUTE_NAME);
                if (networkRestrictions != null)
                {
                    networkRestrictionsAttribute =
                        new NetworkRestrictionsAttribute(networkRestrictions);
                }
            }
        }
    }

    /**
     * Searches for the appearance of any of the provided strings from the
     * string list inside the strings from the collection/Enumeration
     * @param strings A collection of strings
     * @param attrList A list of strings whose appearance (inside the
     *                 collection of strings) is searched
     * @param exactMatch Boolean specifying the match criteria: exact match or
     *                   substring
     * @return The first string from the string list which was found among
     *         the strings from the collection or null if none of the strings
     *         from the string list could be found among the collection of
     *         strings
     */
    private String findFirstAppearance(Enumeration strings,
                                       String[] strList,
                                       boolean exactMatch)
    {
        while (strings.hasMoreElements())
        {
            String str = ((String)strings.nextElement());
            String lStr = str.toLowerCase();
            for (int i=0; i<strList.length; i++)
            {
                if ((!exactMatch && lStr.startsWith(strList[i].toLowerCase()))
                        || (exactMatch && lStr.equals(strList[i].toLowerCase())))
                {
                    return str;
                }
            }
        }
        return null;
    }

    private String findFirstAppearance(Enumeration strings, String[] strList)
    {
        return findFirstAppearance(strings, strList, false);
    }

    private String[] getAttributeValues(
        Hashtable attrNamesAndValues,
        String attrPrefix)
    {
        return getAttributeValues(
                   attrNamesAndValues,
                   attrPrefix,
                   1 /* startIndex */);
    }

    private String[] getAttributeValues(
        Hashtable attrNamesAndValues,
        String attrPrefix,
        int startIndex)
    {
        Vector values = new Vector();
        int i=startIndex;
        while (true)
        {
            String value = getValue(attrNamesAndValues,
                                    attrPrefix + i);
            if (value != null)
            {
                // remove duplicates
                value = value.trim();
                if (!values.contains(value))
                {
                    values.addElement(value);
                }
            }
            else
            {
                break;
            }
            i++;
        }
        if (values.size() == 0)
        {
            return null;
        }
        String[] strValues = new String[values.size()];
        values.copyInto(strValues);
        return strValues;
    }

    private String getValue(Hashtable attrNamesAndValues, String key)
    {
        Attribute attr = (Attribute)attrNamesAndValues.get(key);
        if (attr != null)
        {
            return attr.getValue();
        }
        return null;
    }

    private void jadAttributeNotFound(boolean legacyApp, String attrName)
    {
        if (!legacyApp)
        {
            Logger.logWarning(attrName + " is missing from JAD");
            throw new InvalidAttributeException(
                InstallerErrorMessage.INST_CORRUPT_PKG,
                null, /* no params for short msg */
                InstallerDetailedErrorMessage.ATTR_MISMATCH,
                new String[] {attrName},
                OtaStatusCode.ATTRIBUTE_MISMATCH);
        }
    }
}
