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

package com.nokia.mj.impl.security.midp.appaccessauthorization;

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.security.midp.common.AuthorizationRestriction;
import com.nokia.mj.impl.security.midp.storage.SecurityStorage;
import com.nokia.mj.impl.security.midp.storage.AppAccessAuthorizationStorageData;

/**
 * Application Access Authorization is about securing the inter-MIDlet suite
 * communication and shared RMS data access at runtime. A MIDlet suite which
 * intends to restrict access to its resources declares a list of authorized
 * identities on the basis of the domain of the accessing MIDlet, the vendor
 * of the accessing MIDlet, the signer of the accessing MIDlet, or any
 * combination of these three. This list will be saved at MIDlet suite
 * installation time. At runtime a MIDlet suite will be granted access to
 * another MIDlet suite if its identities (any of the validated signing
 * certificates resulted from the MIDlet suite authentication and/or the
 * domain name and/or the vendor name) are found among the accessed MIDlet
 * suite's list of authorized identities.
 */
public final class ApplicationAccessAuthorization
{
    /**
     * Constant for access being allowed
     */
    public static final int ACCESS_ALLOWED = 1;

    /**
     * Constant for access being denied
     */
    public static final int ACCESS_DENIED = 2;

    /**
     * Performs the application access authorization
     *
     * @param msUID_A The identifier of the accessing MIDlet
     * @param msUID_B The identifier of the accessed MIDlet
     * @return An integer (ACCESS_ALLOWED or ACCESS_DENIED) specifying if the
     *         msUID_A can access msUID_B
     */
    public static int checkAccess(Uid msUID_A, Uid msUID_B)
    {
        if (msUID_A == null)
        {
            return ACCESS_DENIED;
        }
        if (msUID_B == null)
        {
            return ACCESS_ALLOWED;
        }

        if (msUID_A.equals(msUID_B))
        {
            return ACCESS_ALLOWED;
        }

        SecurityStorage storage =
            new SecurityStorage();
        AuthorizationRestriction[] restrictionList =
            storage.readAuthorizationRestrictions(msUID_B);
        if (restrictionList == null || restrictionList.length == 0)
        {
            return ACCESS_ALLOWED;
        }

        AppAccessAuthorizationStorageData sData =
            storage.readAppAccessAuthorizationStorageData(msUID_A);
        String vendorName = null;
        String domainName = null;
        String[] signers = null;
        if (sData != null)
        {
            vendorName = sData.getVendorName();
            domainName = sData.getDomainName();
            signers = sData.getSignersList();
        }
        for (int i=0; i<restrictionList.length; i++)
        {
            String vendor = restrictionList[0].getVendor();
            if (vendor.equalsIgnoreCase("any")
                    || vendor.equalsIgnoreCase(vendorName))
            {
                String domain = restrictionList[0].getDomain();
                if (domain.equalsIgnoreCase("self"))
                {
                    domain = storage.readProtectionDomain(msUID_B);
                }
                if (domain != null
                        && (domain.equalsIgnoreCase("any")
                            || domain.equalsIgnoreCase(domainName)))
                {
                    String signer =  restrictionList[0].getSigner();
                    if (signer.equalsIgnoreCase("any"))
                    {
                        return ACCESS_ALLOWED;
                    }
                    if (signers != null)
                    {
                        for (int j=0; j<signers.length; j++)
                        {
                            if (signer.equalsIgnoreCase(signers[j]))
                            {
                                return ACCESS_ALLOWED;
                            }
                        }
                    }
                }
            }
        }
        return ACCESS_DENIED;
    }
}
