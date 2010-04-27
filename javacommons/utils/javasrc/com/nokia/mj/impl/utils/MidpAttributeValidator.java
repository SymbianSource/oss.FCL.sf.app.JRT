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


package com.nokia.mj.impl.utils;

import com.nokia.mj.impl.utils.exception.InvalidAttributeException;

import java.util.Enumeration;
import java.util.Hashtable;

/**
 * MidpAttributeValidator makes syntax checks for JAD and JAR (Manifest)
 * attributes. It also combines JAD and JAR attributes according to rules
 * from MIDP specification.
 */
public class MidpAttributeValidator
{
    // File type can be either jad or jar.
    /** Jad file type. */
    public static final int JAD = 0;
    /** Jar file type. */
    public static final int JAR = 1;

    private int iFileType = -1;
    private String iFileTypeStr = "";

    /** Array of flags telling what file types have been validated. */
    private boolean[] iValidated = null;
    /** Flag telling if jad and jar attributes have been combined. */
    private boolean iCombined = false;

    /**
     * Default constructor.
     */
    public MidpAttributeValidator()
    {
        iValidated = new boolean[] { false, false };
    }

    /**
     * Returns true if specified file type has been validated with
     * validate method of this MidpAttributeValidator instance.
     * @param aFileType file type (JAD or JAR)
     */
    public boolean isValidated(int aFileType)
    {
        if (aFileType == JAD || aFileType == JAR)
        {
            return iValidated[aFileType];
        }
        return false;
    }

    /**
     * Validates attributes in given attribute table.
     */
    public synchronized void validate(Hashtable aAttrs, int aFileType)
    {
        if (aAttrs == null)
        {
            return;
        }
        // Initialize member variables according to specified file type.
        iFileType = aFileType;
        if (iFileType == JAD)
        {
            iFileTypeStr = "Jad";
        }
        else if (iFileType == JAR)
        {
            iFileTypeStr = "Jar";
        }
        else
        {
            iFileTypeStr = "[" + aFileType + "]";
        }
        logInfo("Validating " + iFileTypeStr + " attributes:\n" + aAttrs);

        // Trim all the attributes.
        trim(aAttrs);

        // Check values for MIDP and proprietary attributes.
        checkRuntimeExecutionEnvironment(aAttrs);
        checkMicroEditionProfile(aAttrs);
        checkMicroEditionConfiguration(aAttrs);
        checkMidletN(aAttrs);
        checkMidletVersion(aAttrs);
        checkValidInteger(aAttrs, "MIDlet-Jar-Size");
        checkValidInteger(aAttrs, "MIDlet-Data-Size");
        checkMidletInstallNotify(aAttrs);
        checkMidletDeleteNotify(aAttrs);
        checkExtensions(aAttrs);

        // The following attributes must be identical and present both
        // in JAD and JAR [MIDP 2.1 spec 12.2.3.3 and 12.2.4]:
        // - MIDlet-Name
        // - MIDlet-Version
        // - MIDlet-Vendor
        // In addition to three attributes listed above, the following
        // attributes must be present in JAD [MIDP 2.1 spec 12.2.3.3
        // and 12.2.4]:
        // - MIDlet-Jar-URL
        // - MIDlet-Jar-Size
        if (iFileType == JAD)
        {
            // Check that mandatory attributes are present in JAD.
            checkAttributeExists("MIDlet-Name", aAttrs);
            checkAttributeExists("MIDlet-Version", aAttrs);
            checkAttributeExists("MIDlet-Vendor", aAttrs);
            checkAttributeExists("MIDlet-Jar-URL", aAttrs);
            checkAttributeExists("MIDlet-Jar-Size", aAttrs);
        }
        else if (iFileType == JAR)
        {
            // Check that mandatory attributes are present in JAR.
            checkAttributeExists("MIDlet-Name", aAttrs);
            checkAttributeExists("MIDlet-Version", aAttrs);
            checkAttributeExists("MIDlet-Vendor", aAttrs);
        }

        // Mark that validation was succesful.
        iValidated[aFileType] = true;
        iFileType = -1;
        iFileTypeStr = "";
    }

    /**
     * Returns true if jad and jar attributes have been combined.
     */
    public boolean isCombined()
    {
        return iCombined;
    }

    /**
     * Combines JAD and JAR attributes according to MIDP specification rules.
     * Attributes must be validated with validate method before they can be
     * combined.
     */
    public synchronized Hashtable combine(Hashtable aJadAttrs, Hashtable aJarAttrs, boolean aTrusted)
    {
        // If Jar attributes are not given, we only have attributes from
        // Jad and this method will be called again later when also Jar
        // attributes are available. On the other hand if Jad attributes
        // are not given but Jar attributes are, it means that installation
        // is made without Jad file and attribute combination checks
        // must be performed at this point.
        if (aJarAttrs == null)
        {
            return aJadAttrs;
        }
        logInfo("Combining Jad and Jar attributes, trusted=" + aTrusted);

        // Check that mandatory attributes are present either in JAD or JAR.
        // The manifest or jad must contain the following
        // attributes [MIDP 2.1 spec 12.2.3.3]:
        // - MIDlet-<n> for each MIDlet
        // - MicroEdition-Profile
        // - MicroEdition-Configuration
        checkAttributeExists("MIDlet-1", aJadAttrs, aJarAttrs);
        checkAttributeExists("MicroEdition-Profile", aJadAttrs, aJarAttrs);
        checkAttributeExists("MicroEdition-Configuration", aJadAttrs, aJarAttrs);

        Hashtable combinedAttrs = new Hashtable();
        if (aTrusted)
        {
            // Check that all attributes present both in JAD and JAR
            // are identical.
            checkAttributesIdentical(aJadAttrs, aJarAttrs);
            // JAR attributes override JAD attributes.
            addAttributes(combinedAttrs, aJadAttrs, false);
            addAttributes(combinedAttrs, aJarAttrs, true);
        }
        else
        {
            // Check attributes which must be identical in JAD and JAR.
            checkAttributeIdentical("MIDlet-Name", aJadAttrs, aJarAttrs);
            checkAttributeIdentical("MIDlet-Vendor", aJadAttrs, aJarAttrs);
            checkAttributeIdentical("MIDlet-Version", aJadAttrs, aJarAttrs);
            // JAD attributes override JAR attributes.
            addAttributes(combinedAttrs, aJarAttrs, false);
            addAttributes(combinedAttrs, aJadAttrs, false);
        }
        // After combining attributes check that required
        // attribute combinations are present.
        checkAttributeCombinations(combinedAttrs);
        iCombined = true;
        return combinedAttrs;
    }

    /**
     * Method for making info log entries. Subclasses can override
     * this method if they want info log entries from validation.
     */
    protected void logInfo(String aMsg)
    {
    }

    /**
     * Method for making error log entries. Subclasses can override
     * this method if they want error log entries from validation.
     */
    protected void logError(String aMsg)
    {
    }

    /**
     * Makes extended check for attributes. Subclasses can add their
     * own attribute validation checks by overriding this method.
     * This method gets called when validate method is called.
     *
     * @param aAttrs attribute table to be validated
     * @throws InvalidAttributeException if attribute validation fails
     */
    protected void checkExtensions(Hashtable aAttrs)
    throws InvalidAttributeException
    {
        // This method is implemented in the subclass.
    }

    /**
     * Reports that an invalid attribute was found.
     * @param aAttr invalid attribute
     * @param aReason reason why attribute is invalid
     */
    protected void invalidAttribute(Attribute aAttr, String aReason)
    {
        logError("Invalid attribute " + aAttr.getName() +
                 ": " + aAttr.getValue() +
                 (aReason != null? " (" + aReason + ")": ""));
        if (iFileType == JAD)
        {
            throw new InvalidAttributeException(InstallerErrorMessage.INST_CORRUPT_PKG,
                                                null,
                                                InstallerDetailedErrorMessage.ATTR_BAD_SYNTAX,
                                                new String[] { aAttr.getName() },
                                                OtaStatusCode.INVALID_DESCRIPTOR);
        }
        else
        {
            throw new InvalidAttributeException(InstallerErrorMessage.INST_CORRUPT_PKG,
                                                null,
                                                InstallerDetailedErrorMessage.ATTR_BAD_SYNTAX,
                                                new String[] { aAttr.getName() },
                                                OtaStatusCode.INVALID_JAR);
        }
    }

    /**
     * Reports that an unsupported attribute was found.
     * @param aAttr unsupported attribute
     * @param aReason reason why attribute is unsupported
     */
    protected void unsupportedAttribute(Attribute aAttr, String aReason)
    {
        logError("Unsupported attribute " + aAttr.getName() +
                 ": " + aAttr.getValue() +
                 (aReason != null? " (" + aReason + ")": ""));
        if (iFileType == JAD)
        {
            throw new InvalidAttributeException(InstallerErrorMessage.INST_CORRUPT_PKG,
                                                null,
                                                InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                new String[] { aAttr.getName() },
                                                OtaStatusCode.INVALID_DESCRIPTOR);
        }
        else
        {
            throw new InvalidAttributeException(InstallerErrorMessage.INST_CORRUPT_PKG,
                                                null,
                                                InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                new String[] { aAttr.getName() },
                                                OtaStatusCode.INVALID_JAR);
        }
    }

    /**
     * Reports that a mandatory attribute is missing.
     */
    protected void missingAttribute(String aName)
    {
        logError("Missing mandatory attribute: " + aName);
        if (iFileType == JAD)
        {
            throw new InvalidAttributeException(InstallerErrorMessage.INST_CORRUPT_PKG,
                                                null,
                                                InstallerDetailedErrorMessage.ATTR_MISSING,
                                                new String[] { aName },
                                                OtaStatusCode.INVALID_DESCRIPTOR);
        }
        else
        {
            throw new InvalidAttributeException(InstallerErrorMessage.INST_CORRUPT_PKG,
                                                null,
                                                InstallerDetailedErrorMessage.ATTR_MISSING,
                                                new String[] { aName },
                                                OtaStatusCode.INVALID_JAR);
        }
    }

    /**
     * Reports that attribute values in JAD nad JAR mismatch.
     */
    private void attributeMismatch(String aName)
    {
        logError("Attribute values in Jad and Jar mismatch: " + aName);
        throw new InvalidAttributeException(InstallerErrorMessage.INST_CORRUPT_PKG,
                                            null,
                                            InstallerDetailedErrorMessage.ATTR_MISMATCH,
                                            new String[] { aName },
                                            OtaStatusCode.ATTRIBUTE_MISMATCH);
    }

    /**
     * Reports that an incompatible Configuration or Profile was found.
     * @param aAttr incompatible attribute
     * @param aReason reason why attribute is incompatible
     */
    private void incompatibleConfigurationOrProfile
    (Attribute aAttr, String aReason)
    {
        logError("Incompatible Configuration or Profile " +
                 aAttr.getName() + ": " + aAttr.getValue() +
                 (aReason != null? " (" + aReason + ")": ""));
        throw new InvalidAttributeException
        (InstallerErrorMessage.INST_COMPAT_ERR, null,
         InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
         new String[] { aAttr.getName() },
         OtaStatusCode.INCOMPATIBLE_CONFIGURATION_OR_PROFILE);
    }

    /**
     * Removes elements with empty values and trims all the remaining keys and values.
     * Table key is an attribute name string and value is an Attribute object.
     * @param aAttrs table to be trimmed
     */
    private void trim(Hashtable aAttrs)
    {
        if (aAttrs == null)
        {
            return;
        }
        Enumeration e = aAttrs.keys();
        while (e.hasMoreElements())
        {
            String key = (String)e.nextElement();
            Attribute attr = (Attribute)aAttrs.get(key);
            aAttrs.remove(key);
            if (attr == null)
            {
                continue;
            }
            String name = attr.getName().trim();
            String value = attr.getValue().trim();
            if (name.length() == 0)
            {
                continue;
            }
            aAttrs.put(name, new Attribute(name, value, attr.isTrusted()));
        }
        // Call trim3Tokens for MIDlet-n and MIDlet-Push-n attribute
        // values to detect error cases where the values don't have
        // three parts. However do not replace the trimmed values to
        // aAttrs table because MSA 1.1 TCK test cases
        //   api/OTA/javax_microedition/midlet/MIDlet/indexOTA.html#JarNumAttribues
        //   api/OTA/javax_microedition/midlet/MIDlet/indexOTA.html#JadNumAttribues
        // will fail if whitespace within the attribute value is not
        // returned as it is.
        for (int i = 1; true; i++)
        {
            String key = "MIDlet-" + i;
            Attribute attr = (Attribute)aAttrs.get(key);
            if (attr == null)
            {
                break; // no more MIDlet-n attributes
            }
            trim3Tokens(attr);
        }
        for (int i = 1; true; i++)
        {
            String key = "MIDlet-Push-" + i;
            Attribute attr = (Attribute)aAttrs.get(key);
            if (attr == null)
            {
                break; // no more MIDlet-Push-n attributes
            }
            trim3Tokens(attr);
        }
    }

    /**
     * Trims whitespace from within three part attribute value
     * (for example MIDlet-n and MIDlet-Push-n).
     */
    private Attribute trim3Tokens(Attribute aAttr)
    {
        String[] tokens = Tokenizer.split(aAttr.getValue(), ",");
        if (tokens == null || tokens.length != 3)
        {
            invalidAttribute(aAttr, "value must have three parts");
        }
        for (int i = 0; i < tokens.length; i++)
        {
            tokens[i] = tokens[i].trim();
        }
        return new Attribute(aAttr.getName(),
                             tokens[0] + ", " + tokens[1] + ", " + tokens[2],
                             aAttr.isTrusted());
    }

    /**
     * Adds all the attributes from aTbl2 into aTbl1.
     */
    private void addAttributes(Hashtable aTbl1, Hashtable aTbl2, boolean aTrusted)
    {
        if (aTbl2 == null)
        {
            // Nothing to add.
            return;
        }
        Enumeration e = aTbl2.keys();
        while (e.hasMoreElements())
        {
            String name = (String)e.nextElement();
            Attribute attr = (Attribute)aTbl2.get(name);
            if (attr.isTrusted() != aTrusted)
            {
                attr = new Attribute(attr.getName(), attr.getValue(), aTrusted);
            }
            aTbl1.put(name, attr);
        }
    }

    /**
     * Checks that attributes present both in JAD and JAR have identical values.
     */
    private void checkAttributesIdentical(Hashtable aJadAttrs, Hashtable aJarAttrs)
    {
        Enumeration e = null;
        if (aJadAttrs != null)
        {
            // Check all attributes from JAD.
            e = aJadAttrs.keys();
            while (e.hasMoreElements())
            {
                String name = (String)e.nextElement();
                checkAttributeIdentical(name, aJadAttrs, aJarAttrs);
            }
        }
        if (aJarAttrs != null)
        {
            // Check all attributes from JAR.
            e = aJarAttrs.keys();
            while (e.hasMoreElements())
            {
                String name = (String)e.nextElement();
                checkAttributeIdentical(name, aJadAttrs, aJarAttrs);
            }
        }
    }

    /**
     * Checks that if given attribute is present both in JAD and JAR it has identical values.
     */
    private void checkAttributeIdentical(String aName, Hashtable aJadAttrs, Hashtable aJarAttrs)
    {
        Attribute jadAttr = null;
        Attribute jarAttr = null;
        if (aJadAttrs != null)
        {
            jadAttr = (Attribute)aJadAttrs.get(aName);
        }
        if (aJarAttrs != null)
        {
            jarAttr = (Attribute)aJarAttrs.get(aName);
        }
        if (jadAttr != null && jarAttr != null)
        {
            if (endsWithDigits(aName, "MIDlet-") ||
                    endsWithDigits(aName, "MIDlet-Push-"))
            {
                // Trim whitespaces within MIDlet-n and MIdlet-Push-n
                // attribute values before comparing them.
                jadAttr = trim3Tokens(jadAttr);
                jarAttr = trim3Tokens(jarAttr);
            }
            if (!jadAttr.getValue().equalsIgnoreCase(jarAttr.getValue()))
            {
                attributeMismatch(aName);
            }
        }
    }

    /**
     * Returns true if given name begins with given prefix and
     * has only digits after prefix.
     */
    private boolean endsWithDigits(String aName, String aPrefix)
    {
        boolean result = false;
        if (aName != null && aPrefix != null && aName.startsWith(aPrefix))
        {
            String suffix = aName.substring(aPrefix.length());
            if (suffix.length() > 0)
            {
                result = true;
            }
            for (int i = 0; i < suffix.length(); i++)
            {
                int ch = suffix.charAt(i);
                if (ch >= '0' && ch <= '9')
                {
                    // Digit found, continue checking.
                }
                else
                {
                    // Not a digit, return false.
                    result = false;
                    break;
                }
            }
        }
        return result;
    }

    /**
     * Checks that mandatory attribute is present.
     */
    private void checkAttributeExists(String aName, Hashtable aAttrs)
    {
        if (aAttrs == null)
        {
            missingAttribute(aName);
        }
        Attribute attr = (Attribute)aAttrs.get(aName);
        if (attr == null ||
                attr.getValue() == null ||
                attr.getValue().length() == 0)
        {
            missingAttribute(aName);
        }
    }

    /**
     * Checks that mandatory attribute is present.
     */
    private void checkAttributeExists(String aName, Hashtable aJadAttrs, Hashtable aJarAttrs)
    {
        if (aJadAttrs != null)
        {
            Attribute attr = (Attribute)aJadAttrs.get(aName);
            if (attr != null &&
                    attr.getValue() != null &&
                    attr.getValue().length() > 0)
            {
                return;
            }
        }
        if (aJarAttrs != null)
        {
            Attribute attr = (Attribute)aJarAttrs.get(aName);
            if (attr != null &&
                    attr.getValue() != null &&
                    attr.getValue().length() > 0)
            {
                return;
            }
        }
        missingAttribute(aName);
    }

    /**
     * Checks MIDlet-n attribute value validity.
     */
    private void checkMidletN(Hashtable aAttrs)
    {
        // Check MIDlet-n attribute values
        for (int i = 1; true; i++)
        {
            String key = "MIDlet-" + i;
            Attribute attr = (Attribute)aAttrs.get(key);
            if (attr == null)
            {
                break; // no more MIDlet-n attributes
            }
            String[] tokens = Tokenizer.split(attr.getValue(), ",");
            for (int j = 0; j < tokens.length; j++)
            {
                tokens[j] = tokens[j].trim();
            }
            if (tokens[0].length() == 0)
            {
                invalidAttribute(attr, "missing MIDlet name");
            }
            if (tokens[2].length() == 0)
            {
                invalidAttribute(attr, "missing MIDlet class");
            }
            if (!JavaLanguageSyntaxChecker.checkClassName(tokens[2]))
            {
                invalidAttribute(attr, "invalid MIDlet class name");
            }
        }
    }

    /**
     * Checks MIDlet-Version attribute value validity.
     */
    private void checkMidletVersion(Hashtable aAttrs)
    {
        Attribute attr = (Attribute)aAttrs.get("MIDlet-Version");
        if (attr == null)
        {
            return;
        }
        Version version = null;
        try
        {
            version = Version.getVersion(attr.getValue());
        }
        catch (NumberFormatException nfe)
        {
        }
        if (version == null)
        {
            invalidAttribute(attr, "not valid version");
        }
    }

    /**
     * Checks Runtime-Execution-Environment attribute value validity.
     */
    private void checkRuntimeExecutionEnvironment(Hashtable aAttrs)
    {
        Attribute attr = (Attribute)aAttrs.get("Runtime-Execution-Environment");
        if (attr == null)
        {
            return;
        }
        if (attr.getValue().length() == 0 ||
                attr.getValue().equals("MIDP.CLDC"))
        {
            // Attribute has valid value.
        }
        else
        {
            unsupportedAttribute(attr, "unsupported value " + attr.getValue());
        }
    }

    /**
     * Checks MicroEdition-Profile attribute value validity.
     */
    private void checkMicroEditionProfile(Hashtable aAttrs)
    {
        Attribute attr = (Attribute)aAttrs.get("MicroEdition-Profile");
        if (attr == null)
        {
            return;
        }

        if (attr.getValue() == null || attr.getValue().length() == 0)
        {
            incompatibleConfigurationOrProfile(attr, "not supported profile");
        }

        // Loop through all the profiles in attribute value.
        String midpPrefix = "MIDP-";
        String[] values = Tokenizer.split(attr.getValue(), " ");
        for (int i = 0; i < values.length; i++)
        {
            String value = values[i].trim();
            if (value.length() == 0)
            {
                continue; // skip empty spaces in value
            }
            if (!value.startsWith(midpPrefix))
            {
                incompatibleConfigurationOrProfile(attr, "not supported profile");
            }
            Version version = null;
            try
            {
                version = Version.getVersion(value.substring(midpPrefix.length()));
            }
            catch (NumberFormatException nfe)
            {
            }
            if (version == null)
            {
                invalidAttribute(attr, "invalid version");
            }
            if (Version.getVersion("2.1").compareTo(version) < 0)
            {
                incompatibleConfigurationOrProfile(attr, "not supported version");
            }
        }
    }

    /**
     * Checks MicroEdition-Configuration attribute value validity.
     */
    private void checkMicroEditionConfiguration(Hashtable aAttrs)
    {
        Attribute attr = (Attribute)aAttrs.get("MicroEdition-Configuration");
        if (attr == null)
        {
            return;
        }
        String cldcPrefix = "CLDC-";
        String value = attr.getValue();
        if (!value.startsWith(cldcPrefix))
        {
            incompatibleConfigurationOrProfile(attr, "not supported configuration");
        }
        Version version = null;
        try
        {
            version = Version.getVersion(value.substring(cldcPrefix.length()));
        }
        catch (NumberFormatException nfe)
        {
        }
        if (version == null)
        {
            invalidAttribute(attr, "invalid version");
        }
        if (Version.getVersion("1.1").compareTo(version) < 0)
        {
            incompatibleConfigurationOrProfile(attr, "not supported version");
        }
    }

    /**
     * Checks MIDlet-Install-Notify attribute value validity.
     */
    private void checkMidletInstallNotify(Hashtable aAttrs)
    {
        Attribute attr = (Attribute)aAttrs.get("MIDlet-Install-Notify");
        if (attr == null)
        {
            return;
        }
        String value = attr.getValue();
        // MIDP2.1 spec chapter 2.1.9 "Additional Descriptor Attributes"
        // limits length of this attribute to 256 characters.
        if (value != null && value.length() > 256)
        {
            invalidAttribute(attr, "too long attribute value");
        }
    }

    /**
     * Checks MIDlet-Delete-Notify attribute value validity.
     */
    private void checkMidletDeleteNotify(Hashtable aAttrs)
    {
        Attribute attr = (Attribute)aAttrs.get("MIDlet-Delete-Notify");
        if (attr == null)
        {
            return;
        }
        String value = attr.getValue();
        // MIDP2.1 spec chapter 2.1.9 "Additional Descriptor Attributes"
        // limits length of this attribute to 256 characters.
        if (value != null && value.length() > 256)
        {
            invalidAttribute(attr, "too long attribute value");
        }
    }

    /**
     * Checks that attribute value is a valid integer.
     */
    private void checkValidInteger(Hashtable aAttrs, String aName)
    {
        Attribute attr = (Attribute)aAttrs.get(aName);
        if (attr == null)
        {
            return;
        }
        try
        {
            int value = Integer.parseInt(attr.getValue());
        }
        catch (NumberFormatException nfe)
        {
            invalidAttribute(attr, "not valid integer");
        }
    }

    /**
     * Checks that mandatory attribute combinations are present.
     * This method is called after the jad and jar attributes
     * have been combined in the end of combine method.
     */
    private void checkAttributeCombinations(Hashtable aAttrs)
    {
        // Presence of Runtime-Execution-Environment attribute
        // value MIDP.CLDC requires that MicroEdition-Configuration
        // attribute specifies CLDC value.
        // Currently CLDC is the only supported configuration
        // and values for aforementioned attributes are already
        // checked, there is nothing left to be checked here.
        // This method is just a placeholder for future
        // attribute combination checks.
    }
}
