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

import com.nokia.mj.impl.security.common.Certificate;

/**
 * Class which bound together certificate info with protection
 * domain info.
 */
public final class SigningCertificate
        extends Certificate
{
    private ProtectionDomain protectionDomain;

    private String protectionDomainRoot;

    public SigningCertificate(String issuer,
                              String subject,
                              String organization,
                              String notBefore,
                              String notAfter,
                              String serialNumber,
                              String fingerprint,
                              String protectionDomainRoot,
                              String protectionDomainName,
                              String protectionDomainCategory)
    {
        super(issuer,
              subject,
              organization,
              notBefore,
              notAfter,
              serialNumber,
              fingerprint);
        protectionDomain = new ProtectionDomain(protectionDomainName, protectionDomainCategory);
        this.protectionDomainRoot = protectionDomainRoot;
    }

    public SigningCertificate(Certificate cert,
                              String protectionDomainRoot,
                              String protectionDomainName,
                              String protectionDomainCategory)
    {
        super(cert);
        protectionDomain = new ProtectionDomain(protectionDomainName, protectionDomainCategory);
        this.protectionDomainRoot = protectionDomainRoot;
    }

    public ProtectionDomain getProtectionDomain()
    {
        return protectionDomain;
    }

    public String getProtectionDomainRoot()
    {
        return protectionDomainRoot;
    }

    /**
     * Checks if this signing certificate and the provided
     * signing certificate have the same signer: common
     * signer is defined as matching the Organization field
     * and have the the same Protection Domain Root Certificate
     */
    public boolean isSameSigner(SigningCertificate cert)
    {
        if (cert == null)
        {
            return (this.getOrganization() == null
                    && this.protectionDomainRoot == null);
        }
        return (((this.getOrganization() == null
                  && cert.getOrganization() == null)
                 || (this.getOrganization() != null
                     && this.getOrganization().equals(cert.getOrganization())))
                && ((this.protectionDomainRoot == null
                     && cert.getProtectionDomainRoot() == null)
                    || (this.protectionDomainRoot != null
                        && this.protectionDomainRoot.equals(
                            cert.getProtectionDomainRoot()))));
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer(super.toString());
        sb.append("\nProtection Domain Root:" + protectionDomainRoot);
        sb.append(" Protection Domain:").append(getProtectionDomain().toString());
        return sb.toString();
    }
}
