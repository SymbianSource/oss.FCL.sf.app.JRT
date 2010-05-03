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
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;

/**
 * Specifies the allowed MCC (3 digits) and MNC (2 or 3 digits) tuples,
 * separated by a hyphen (Unicode U+002D), to limit the execution to certain
 * networks for any MIDlets within a MIDlet suite bound to the Operator
 * Protection Domain
 */
public class NetworkRestrictionsAttribute
{
    /**
     * The JAR attribute holding the network restriction info
     */
    public static final String ATTRIBUTE_NAME
    = "MIDlet-Operator-Allowed";

    /* The network restrictions contained in the attribute */
    private NetworkRestriction[] restrictions;

    public NetworkRestrictionsAttribute(
        String attrValue)
    {
        if (attrValue == null)
        {
            // nothing to validate
            return;
        }

        // mainAttrValue: MCC-MNC MCC-MNC ...
        String[] tokens = Tokenizer.split(attrValue, " ");
        if (tokens == null || tokens.length == 0)
        {
            throw new InvalidAttributeException(
                InstallerErrorMessage.INST_CORRUPT_PKG,
                null, /* no params for short msg */
                InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                new String[] {ATTRIBUTE_NAME},
                OtaStatusCode.INVALID_JAR);
        }
        restrictions = new NetworkRestriction[tokens.length];
        // validate the tuples
        for (int i=0; i<tokens.length; i++)
        {
            // the format of a token must be MCC-MNC, where MCC
            // is 3 digits and MNC is 2 or 3 digits
            boolean validToken = (tokens[i].length() == 6
                                  || tokens[i].length() == 7)
                                 && tokens[i].charAt(3) == '-'
                                 && tokens[i].charAt(0) >= '0' && tokens[i].charAt(0)<='9'
                                 && tokens[i].charAt(1) >= '0' && tokens[i].charAt(1)<='9'
                                 && tokens[i].charAt(2) >= '0' && tokens[i].charAt(2)<='9'
                                 && tokens[i].charAt(4) >= '0' && tokens[i].charAt(4)<='9'
                                 && tokens[i].charAt(5) >= '0' && tokens[i].charAt(5)<='9'
                                 && (tokens[i].length() == 6 ||
                                     (tokens[i].charAt(6) >= '0' && tokens[i].charAt(6)<='9'));
            if (!validToken)
            {
                throw new InvalidAttributeException(
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                    new String[] {ATTRIBUTE_NAME},
                    OtaStatusCode.INVALID_JAR);
            }
            restrictions[i] = new NetworkRestriction(
                tokens[i].substring(0,3),
                tokens[i].substring(4));
        }
    }

    public NetworkRestriction[] getRestrictions()
    {
        return restrictions;
    }
}
