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

/**
 * A MIDlet suite which intends to restrict access to its resources declares
 * a list of authorized identities on the basis of the domain of the accessing
 * MIDlet, the vendor of the accessing MIDlet, the signer of the accessing
 * MIDlet, or any combination of these three.
 * This class encapsulates an authorization restriction, respectively a
 * domain name, a vendor name and a signer info.
 */
public class AuthorizationRestriction
{
    /**
     * The domain name
     */
    protected String domain;

    /**
     * The vendor name
     */
    protected String vendor;

    /**
     * The signer (the signer is base64 encoded)
     */
    protected String signer;

    public AuthorizationRestriction()
    {
    }

    public AuthorizationRestriction(
        String domain,
        String vendor,
        String signer)
    {
        this.domain = domain;
        this.vendor = vendor;
        this.signer = signer;
    }

    public String getDomain()
    {
        return domain;
    }

    public String getVendor()
    {
        return vendor;
    }

    public String getSigner()
    {
        return signer;
    }

    public boolean equals(Object o)
    {
        if (this == o)
        {
            return true;
        }
        if (!(o instanceof AuthorizationRestriction))
        {
            return false;
        }
        AuthorizationRestriction r = (AuthorizationRestriction)o;
        if ((this.domain == null && r.getDomain() != null)
                || (this.domain != null && !this.domain.equals(r.getDomain()))
                || (this.vendor == null && r.getVendor() != null)
                || (this.vendor != null && !this.vendor.equals(r.getVendor()))
                || (this.signer == null && r.getSigner() != null)
                || (this.signer != null && !this.signer.equals(r.getSigner())))
        {
            return false;
        }
        return true;
    }
}
