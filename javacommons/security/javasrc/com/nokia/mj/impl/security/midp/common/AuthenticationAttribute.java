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

import com.nokia.mj.impl.utils.exception.InvalidAttributeException;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Base64;
import java.util.Vector;

/**
 * MIDP authentication is build around X.509 Public Key Infrastructure so that
 * MIDlet suites are signed using public key certificates.
 * This class helps in extracting the signing information, respectively a chain
 * of public certificates used and a corresponding JAR signature,
 * from the application descriptor (JAD)
 */
public class AuthenticationAttribute
        extends AuthenticationInfo
{
    /**
     * The JAD attribute holding the certificate information
     */
    public static final String MAIN_ATTRIBUTE_PREFIX = "MIDlet-Certificate-";

    /**
     * The JAD attribute holding the signature information
     */
    public static final String SECOND_ATTRIBUTE_PREFIX = "MIDlet-Jar-RSA-SHA1-";

    /**
     * The JAD legacy (MIDP2) attribute holding the signature information
     */
    public static final String SECOND_LEGACY_ATTRIBUTE_NAME
    = "MIDlet-Jar-RSA-SHA1";

    AuthenticationAttribute(String mainAttrName, String[] mainAttrValue, String secondAttrName, String secondAttrValue)
    {
        this(mainAttrName, mainAttrValue, secondAttrName, secondAttrValue, false);
    }

    AuthenticationAttribute(String mainAttrName, String[] mainAttrValue, String secondAttrName, String secondAttrValue, boolean skipSecondAttrValidation)
    {
        if (mainAttrValue == null)
        {
            if (secondAttrValue != null)
            {
                throw new InvalidAttributeException(
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.ATTR_MISSING,
                    new String[] {mainAttrName + 1},
                    OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE);
            }
            // nothing to validate or save
            return;
        }
        else
        {
            // do the validation of what we have so far
            String[] cChain = new String[mainAttrValue.length];
            for (int i=0; i<mainAttrValue.length; i++)
            {
                cChain[i] = mainAttrValue[i];
                if (Base64.decode(cChain[i]) == null)
                {
                    throw new InvalidAttributeException(
                        InstallerErrorMessage.INST_CORRUPT_PKG,
                        null, /* no params for short msg */
                        InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                        new String[] {(mainAttrName + (i+1))},
                        OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE);
                }
            }
            this.certChain = cChain;
            if (secondAttrValue == null)
            {
                // ignore the values
                this.certChain = null;
                this.signature = null;
                return;
            }
        }
        if (!skipSecondAttrValidation && Base64.decode(secondAttrValue) == null)
        {
            throw new InvalidAttributeException(
                InstallerErrorMessage.INST_CORRUPT_PKG,
                null, /* no params for short msg */
                InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                new String[] {(secondAttrName)},
                OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE);
        }
        this.signature = secondAttrValue;
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("Cert chain:");
        if (certChain != null)
        {
            for (int i=0; i<certChain.length; i++)
            {
                sb.append(" chain["+(i+1)+"]:"+certChain[i]);
            }
        }
        else
        {
            sb.append("null");
        }
        sb.append("\n Signature:");
        if (signature != null)
        {
            sb.append(" "+signature);
        }
        else
        {
            sb.append("null");
        }
        return sb.toString();
    }
}
