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

import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.security.utils.Logger;
import com.nokia.mj.impl.security.packageprotection.PackageNames;

import java.util.Hashtable;
import java.util.Enumeration;
import java.util.Vector;

/**
 * This class reads the security extensions from the extensions
 * deployment packages.
 *
 * A security extension is a group of permissions and their usage
 * with regard to protection domains, function groups and security
 * prompt texts.
 *
 * Security extensions are contained into the extensions ODC files
 * as system properties, as follows:
 *   -Dcom.nokia.mj.addon.protected.<odcname>=<comma separated list
 *                                             of protected packages>
 *   -Dcom.nokia.mj.addon.restricted.<odcname>=<comma separated list
 *                                             of restricted packages>
 *   -Dcom.nokia.mj.addon.permissions.<odcname>=<";" separated list
 *                                             of permission mappings>
 *   -Dcom.nokia.mj.addon.policies.<odcname>=<";" separated list
 *                                             of policies file names>
 * where:
 *   <odcname> is always the odc filename without path and extension
 *   <permission mappings> has following syntax:
 *      <named-permission>","<permission-class-name>","<taget>","<actionList>
 *
 * This class validates the security extensions and discards the invalid
 * elements of the security extension by writing a WARNING log entry into
 * JavaSecurity.log
 */
public final class SecurityExtensionsReader
{
    /**
     * The extensions of the ODC file containing the security
     * extensions definitions
     */
    public final static String POLICIES_SEPARATOR = "";

    /**
     * The extensions of the ODC file containing the security
     * extensions definitions
     */
    private final static String ODC_EXTENSIONS = ".odc";

    /**
     * The names of the system properties defining the
     * security extension
     */
    private final static String PROTECTED_PACKAGE_PROPERTY    = "com.nokia.mj.addon.protected.";
    private final static String RESTRICTED_PACKAGE_PROPERTY   = "com.nokia.mj.addon.restricted.";
    private final static String PERMISSION_MAPPINGS_PROPERTY  = "com.nokia.mj.addon.permissions.";
    private final static String SECURITY_POLICIES_PROPERTY    = "com.nokia.mj.addon.policies.";

    /**
     * The allowed locations for the security extensions policies
     */
    private final static String ALLOWED_POLICY_FILE_LOCATION1 = "c:\\resource\\";
    private final static String ALLOWED_POLICY_FILE_LOCATION2 = "c:/resource/";

    /**
     * Internal data members
     */
    private static final int PROTECTED_PACKAGES = 1;
    private static final int RESTRICTED_PACKAGES = 2;
    private static Vector extProtectedPackageNames = new Vector();
    private static Vector extRestrictedPackageNames = new Vector();
    private static Vector extPoliciesFileNames = new Vector();
    private static Hashtable extPermissionMappings = new Hashtable();

    /**
     * Boolean used in lazy initialization: the system properties
     * are read only once when they are needed for the first time
     */
    private static boolean initialized = false;


    /**
     * Retrieves the names of all the installed security extensions
     */
    public static Vector getExtPoliciesFileNames()
    {
        lazyInit();
        return extPoliciesFileNames;
    }

    /**
     * Retrieves the names of all the protected packages brought in by
     * security extensions
     */
    public static Vector getExtProtectedPackages()
    {
        lazyInit();
        return extProtectedPackageNames;
    }

    /**
     * Retrieves the names of all the restricted packages brought in by
     * security extensions
     */
    public static Vector getExtRestrictedPackages()
    {
        lazyInit();
        return extRestrictedPackageNames;
    }

    /**
     * Retrieves the permission mapping of a certain named-permission,
     * if found among the permission mappings brought in by all the
     * security extensions
     */
    public static MIDPPermission getExtPermission(String permissionName)
    {
        lazyInit();
        return (MIDPPermission)extPermissionMappings.get(permissionName);
    }

    /**
     * Loggs a discard operation of an invalid security extension element
     */
    public static void discard(String discardReason)
    {
        Logger.logWarning(discardReason);
    }

    /*** ----------------------------- PACKAGE ---------------------------- */


    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Lazy initialization of the security extension definitions
     */
    private static void lazyInit()
    {
        if (!initialized)
        {
            handleExtensions();
            initialized = true;
        }
    }

    /**
     * Reads the list of extensions and goes through each of the extension's
     * (ODC-based) system properties, reading and validating the
     * details of the security extension.
     */
    private static void handleExtensions()
    {
        String extOdcList = System.getProperty("com.nokia.mj.addon.list");
        if (extOdcList != null)
        {
            //Split the list to vector
            String[] result = Tokenizer.split(extOdcList, ";");
            if (result != null)
            {
                for (int i=0; i<result.length; i++)
                {
                    String extOdcFile = result[i];

                    // Support also plain .jar extensions (for Java 1.3 compatibility)
                    if (extOdcFile.endsWith(ODC_EXTENSIONS))
                    {
                        handleAddOnSystemProperties(extOdcFile);
                    }
                }
            }
        }
    }

    /**
     * Validates the security extension read as an (ODC-based) system property
     */
    private static void handleAddOnSystemProperties(String extOdcFile)
    {
        char pathSeparator = System.getProperty("file.separator").charAt(0);

        // Locate the last file separator
        int lastFileSeparator =
            extOdcFile.lastIndexOf(pathSeparator);

        // The line must contain at least one file separator and the
        // name of the file must end with .odc.
        if (lastFileSeparator >= 0)
        {
            // Strip away the path and file name extension. E.g.
            // c:\foo\eswtmobile.odc would be eswtmobile
            String propertyName = extOdcFile.substring(
                                      lastFileSeparator+1,
                                      extOdcFile.length()-
                                      ODC_EXTENSIONS.length());

            // validate&set the security extensions
            setExtPackageNames(extOdcFile,
                               PROTECTED_PACKAGES, System.getProperty(
                                   PROTECTED_PACKAGE_PROPERTY + propertyName));
            setExtPackageNames(
                extOdcFile,
                RESTRICTED_PACKAGES, System.getProperty(
                    RESTRICTED_PACKAGE_PROPERTY + propertyName));
            setExtPermissionMappings(
                extOdcFile,
                System.getProperty(
                    PERMISSION_MAPPINGS_PROPERTY + propertyName));
            setExtPoliciesFileNames(extOdcFile,
                                    System.getProperty(
                                        SECURITY_POLICIES_PROPERTY + propertyName));
        }
    }

    /**
     * Validates the security extension package names applying the following
     * rule: package names collisions are only allowed for same type of
     * packages (e.g. an extension can not declare as protected a package
     * name which is already declared restricted)
     */
    private static void setExtPackageNames(String odcFile, int packageType,
                                           String packageNamesSystemProperty)
    {
        // sanity check
        if (packageNamesSystemProperty == null)
        {
            return;
        }

        String[] basePackageNames = (packageType == PROTECTED_PACKAGES
                                     ? PackageNames.restrictedPackages : PackageNames.protectedPackages);
        String[] extPackageNames = Tokenizer.split(packageNamesSystemProperty,
                                   ",");
        Vector okPackageNames = new Vector();
        for (int i=0; i<extPackageNames.length; i++)
        {
            boolean found = find(extPackageNames[i], basePackageNames);
            if (!found)
            {
                // one more check against the already existing extensions
                // package names
                Vector otherPackageNames = (packageType == PROTECTED_PACKAGES
                                            ? extRestrictedPackageNames : extProtectedPackageNames);
                found = find(extPackageNames[i], otherPackageNames);
            }
            if (!found)
            {
                okPackageNames.addElement(extPackageNames[i]);
            }
            else
            {
                discard(odcFile, extPackageNames[i]
                        + " is declared as "
                        + (packageType == PROTECTED_PACKAGES ? "protected": "restricted")
                        + " but it is already declared as "
                        + (packageType == PROTECTED_PACKAGES ? "restricted": "protected"));
            }
        }
        Vector packageNames = (packageType == PROTECTED_PACKAGES
                               ? extProtectedPackageNames : extRestrictedPackageNames);
        for (int i=0; i<okPackageNames.size(); i++)
        {
            packageNames.addElement(okPackageNames.elementAt(i));
        }
    }

    /**
     * Validates the names of the security extension's policies applying the
     * following rule: the policies can only be loaded from "c:\\resource\\"
     * or "c:/resource/"
     */
    private static void setExtPoliciesFileNames(String odcFile,
            String policiesSystemProperty)
    {
        String[] policiesFileNames = Tokenizer.split(policiesSystemProperty,
                                     ";");
        if (policiesFileNames != null)
        {
            for (int i=0; i<policiesFileNames.length; i++)
            {
                String policyFileName = policiesFileNames[i];
                if (policyFileName != null && policyFileName.length() > 0)
                {
                    policyFileName = policyFileName.toLowerCase();
                    if (policyFileName.startsWith(ALLOWED_POLICY_FILE_LOCATION1)
                            || (policyFileName.startsWith(ALLOWED_POLICY_FILE_LOCATION2)))
                    {
                        extPoliciesFileNames.addElement(policyFileName);
                    }
                    else
                    {
                        discard(odcFile, policyFileName + " points to unsafe location");
                    }
                }
            }
            // add the separator for new set of policies, so that the policies
            // could be treated as a group of related policies
            extPoliciesFileNames.addElement(POLICIES_SEPARATOR);
        }
    }

    /**
     * Validates the security extension's permission mappings applying the
     * following rule: permission mappings can not refer to any existing
     * named permissions nor to existing class names
     */
    private static void setExtPermissionMappings(String odcFile,
            String mappingsSystemProperty)
    {
        String[] mappings = Tokenizer.split(mappingsSystemProperty, ";");
        Vector validatedPermNames = new Vector();
        if (mappings != null)
        {
            for (int i=0; i<mappings.length; i++)
            {
                String mapping = mappings[i];
                String[] tokens = Tokenizer.split(mapping, ",");
                if (tokens != null && tokens.length >= 2)
                {
                    String namedPermName = tokens[0];
                    String classBasedPermName = null;
                    String classBasedPermTarget = null;
                    String classBasedPermActionList = null;
                    switch (tokens.length)
                    {
                    case 4:
                        classBasedPermActionList = tokens[3];
                    case 3:
                        classBasedPermTarget = tokens[2];
                    case 2:
                        classBasedPermName = tokens[1];
                    }
                    if (classBasedPermName != null)
                    {
                        boolean found = find(namedPermName,
                                             classBasedPermName,
                                             PermissionMappingTable.PERMISSION_MAPPING_TABLE);
                        if (!found)
                        {
                            // one more check against the existing extensions
                            // permission mappings
                            found = find(namedPermName,
                                         (validatedPermNames.contains(classBasedPermName) 
                                         ? null: classBasedPermName),
                                         extPermissionMappings);
                        }
                        if (!found)
                        {
                            validatedPermNames.addElement(classBasedPermName);
                            extPermissionMappings.put(
                                namedPermName,
                                new MIDPPermission(
                                    classBasedPermName,
                                    classBasedPermTarget,
                                    classBasedPermActionList));
                        }
                        else
                        {
                            discard(odcFile, mappings[i] + " refers to existing permission mapping");
                        }
                    }
                }
            }
        }
    }

    private static boolean find(String namedPermName, String classBasedPermName, Hashtable permMapping)
    {
        for (Enumeration e = permMapping.keys() ; e.hasMoreElements() ;)
        {
            Object key = e.nextElement();
            Object value = permMapping.get(key);
            if (((String)key).equalsIgnoreCase(namedPermName)
                    || ( classBasedPermName != null 
                    && (((MIDPPermission)value)).getName()
                    .equalsIgnoreCase(classBasedPermName)))
            {
                return true;
            }
        }
        return false;
    }

    private static boolean find(String packageName, Vector packageNames)
    {
        String[] tmp = new String[packageNames.size()];
        packageNames.copyInto(tmp);
        return find(packageName, tmp);
    }

    private static boolean find(String packageName, String[] packageNames)
    {
        packageName =  packageName.replace('.', '/');
        for (int i=0; i<packageNames.length; i++)
        {
            String otherPackageName = packageNames[i].replace('.', '/');
            if (otherPackageName.startsWith(packageName)
                    || packageName.startsWith(otherPackageName))
            {
                return true;
            }
        }
        return false;
    }

    private static void discard(String odcFile, String discardReason)
    {
        Logger.logWarning(odcFile + " contains unsafe properties. Details: " + discardReason);
    }
}
