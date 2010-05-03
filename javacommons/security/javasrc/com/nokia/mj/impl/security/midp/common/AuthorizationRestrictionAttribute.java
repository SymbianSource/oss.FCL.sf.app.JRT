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
import com.nokia.mj.impl.utils.Base64;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;

/**
 * A MIDlet suite which intends to restrict access to its resources declares
 * a list of authorized identities on the basis of the domain of the accessing
 * MIDlet, the vendor of the accessing MIDlet, the signer of the accessing
 * MIDlet, or any combination of these three.
 * This class helps in extracting the authorization restriction info,
 * respectively a vendor name, a domain name and a signer info,
 * from the application descriptor (JAD)
 */
public class AuthorizationRestrictionAttribute
        extends AuthorizationRestriction
{
    /**
     * The JAD attribute holding the authorization restriction info
     */
    public static final String MAIN_ATTRIBUTE_PREFIX
    = "MIDlet-Access-Auth-Type-";

    /**
     * The JAD attribute holding the authorization restriction's signer info
     */
    public static final String SECOND_ATTRIBUTE_PREFIX
    = "MIDlet-Access-Auth-Cert-";

    /**
     * Constant specifying a general value for any of the domain, vendor or
     * signer
     */
    public static final String ANY = "ANY";

    /**
     * Constant specifying a general value for the domain
     */
    public static final String SELF = "SELF";

    /**
     * The name of the main attribute
     */
    private String mainAttrName  = null;

    /**
     * The name of the second attribute
     */
    private String secondAttrName  = null;

    public AuthorizationRestrictionAttribute(
        String mainAttrName,
        String mainAttrValue,
        String secondAttrName,
        String secondAttrValue)
    {
        if (mainAttrValue == null)
        {
            if (secondAttrValue == null)
            {
                // nothing to do
                return;
            }
            // validate what we have so far
            String[] aliasTokens = Tokenizer.split(secondAttrValue, " ");
            if (aliasTokens == null ||
                    aliasTokens.length != 2 ||
                    Base64.decode(aliasTokens[1]) == null)
            {
                throw new InvalidAttributeException(
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                    new String[] {secondAttrName},
                    OtaStatusCode.INVALID_JAR);
            }
            // signal the missing attribute
            throw new InvalidAttributeException(
                InstallerErrorMessage.INST_CORRUPT_PKG,
                null, /* no params for short msg */
                InstallerDetailedErrorMessage.ATTR_MISSING,
                new String[] {mainAttrName},
                OtaStatusCode.INVALID_JAR);
        }
        // mainAttrValue: domain=SELF|ANY;vendor=<something>|ANY;signer=<alias>|ANY
        String[] tokens = Tokenizer.split(mainAttrValue, ";");
        if (tokens == null || tokens.length != 3)
        {
            throw new InvalidAttributeException(
                InstallerErrorMessage.INST_CORRUPT_PKG,
                null, /* no params for short msg */
                InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                new String[] {mainAttrName},
                OtaStatusCode.INVALID_JAR);
        }
        String[] domainTokens = Tokenizer.split(tokens[0], "=");
        String[] vendorTokens = Tokenizer.split(tokens[1], "=");
        String[] signerTokens = Tokenizer.split(tokens[2], "=");
        if (domainTokens == null
                || domainTokens.length != 2
                || vendorTokens == null
                || vendorTokens.length != 2
                || signerTokens == null
                || signerTokens.length != 2)
        {
            throw new InvalidAttributeException(
                InstallerErrorMessage.INST_CORRUPT_PKG,
                null, /* no params for short msg */
                InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                new String[] {mainAttrName},
                OtaStatusCode.INVALID_JAR);
        }
        // get rid of the leading and trailing whitespaces
        domainTokens[0] = domainTokens[0].trim();
        domainTokens[1] = domainTokens[1].trim();
        vendorTokens[0] = vendorTokens[0].trim();
        vendorTokens[1] = vendorTokens[1].trim();
        signerTokens[0] = signerTokens[0].trim();
        // alias can not contain whitespaces, so don't trim it
        if (!domainTokens[0].equalsIgnoreCase("domain") ||
                (!domainTokens[1].equalsIgnoreCase(SELF) &&
                 !domainTokens[1].equalsIgnoreCase(ANY)) ||
                !vendorTokens[0].equalsIgnoreCase("vendor") ||
                !signerTokens[0].equalsIgnoreCase("signer"))
        {
            throw new InvalidAttributeException(
                InstallerErrorMessage.INST_CORRUPT_PKG,
                null, /* no params for short msg */
                InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                new String[] {mainAttrName},
                OtaStatusCode.INVALID_JAR);
        }
        this.domain = domainTokens[1];
        this.vendor = vendorTokens[1];
        this.signer = signerTokens[1];
        // if signer is something different that ANY -> secondAttrValue must
        // be present as well
        if (!signerTokens[1].equalsIgnoreCase(ANY))
        {
            // signerTokens[1] must be a string of alpha numeric
            // characters,excluding whitespace
            for (int i=0; i<signerTokens[1].length(); i++)
            {
                char c=signerTokens[1].charAt(i);
                boolean isAlpha = (c >= 'A' && c <= 'Z')
                                  || (c >= 'a' && c <= 'z') || (c>= '0' && c<='9');
                if (!isAlpha || c == ' ')
                {
                    throw new InvalidAttributeException(
                        InstallerErrorMessage.INST_CORRUPT_PKG,
                        null, /* no params for short msg */
                        InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                        new String[] {mainAttrName},
                        OtaStatusCode.INVALID_JAR);
                }
            }
            if (secondAttrValue == null)
            {
                throw new InvalidAttributeException(
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.ATTR_MISSING,
                    new String[] {secondAttrName},
                    OtaStatusCode.INVALID_JAR);
            }
            // secondAttrValue: <alias> <base64 encoded>
            // we do not base64 decode it, because this is going to be used
            // in a simple equality comparison, so it is simpler to compare
            // base64 encoded that base64 decoded data. Plus that the size
            // of the base64 encoded is smaller than the size of the base64
            // decoded, so we also save some space
            String[] aliasTokens = Tokenizer.split(secondAttrValue, " ");
            if (aliasTokens == null ||
                    aliasTokens.length != 2 ||
                    !aliasTokens[0].equalsIgnoreCase(signerTokens[1]) ||
                    (Base64.decode(aliasTokens[1]) == null))
            {
                throw new InvalidAttributeException(
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                    new String[] {secondAttrName},
                    OtaStatusCode.INVALID_JAR);
            }
            this.signer = aliasTokens[1];
        }
        this.mainAttrName = mainAttrName;
        this.secondAttrName = secondAttrName;
    }

    public String getMainAttributeName()
    {
        return mainAttrName;
    }

    public String getSecondAttributeName()
    {
        return secondAttrName;
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("\nDOMAIN: " + domain);
        sb.append("\nVENDOR: " + vendor);
        sb.append("\nSIGNER: " + signer);
        return sb.toString();
    }
}
