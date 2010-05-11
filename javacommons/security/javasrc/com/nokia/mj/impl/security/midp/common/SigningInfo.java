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
 * Encapsulation of MIDP signing details:
 *  - protection domain
 *  - signing certificate (e.k.a end-entity certificate)
 *  - root certificate
 */
public class SigningInfo
{

    private Certificate signingCertificate;
    private Certificate rootCertificate;
    private ProtectionDomain protectionDomain;

    /**
     * Constructor
     */
    public SigningInfo(Certificate signingCertificate,
                       Certificate rootCertificate,
                       ProtectionDomain protectionDomain)
    {
        this.signingCertificate = signingCertificate;
        this.rootCertificate = rootCertificate;
        this.protectionDomain = protectionDomain;
    }

    /**
     * Getter for the protection domain
     *
     * @return The protection domain
     */
    public ProtectionDomain getProtectionDomain()
    {
        return protectionDomain;
    }

    /**
     * Getter for the signing certificate
     *
     * @return The signing certificate
     */
    public Certificate getSigningCertificate()
    {
        return signingCertificate;
    }

    /**
     * Getter for the root certificate
     *
     * @return The root certificate
     */
    public Certificate getRootCertificate()
    {
        return rootCertificate;
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("\n  Signing certificate: \n").append(signingCertificate);
        sb.append("\n  Root certificate: ").append(rootCertificate);
        sb.append("\n  Protection domain: ").append(protectionDomain);
        return sb.toString();
    }
}