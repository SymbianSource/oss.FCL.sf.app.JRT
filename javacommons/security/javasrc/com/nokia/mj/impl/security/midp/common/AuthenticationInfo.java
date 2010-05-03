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
 * MIDP authentication is build around X.509 Public Key Infrastructure so that
 * MIDlet suites are signed using public key certificates.
 * This class encapsulates the signing information, respectively a chain of
 * public certificates used and a corresponding JAR signature
 */
public class AuthenticationInfo
{
    /**
     * The chain of public certificates (the chains are base64 encoded)
     */
    protected String[] certChain;

    /**
     * The JAR signature (the signature is base64 encoded)
     */
    protected String signature;

    /**
     * Flag for specifying if the auth info corresponds to
     * a legacy MIDlet suite
     */
    private boolean legacyInfo;

    public AuthenticationInfo()
    {
    }

    public AuthenticationInfo(String[] certChain, String signature)
    {
        this.certChain = certChain;
        this.signature = signature;
    }

    public String[] getCertChain()
    {
        return certChain;
    }

    public String getSignature()
    {
        return signature;
    }

    public void setLegacyInfo(boolean legacyInfo)
    {
        this.legacyInfo = legacyInfo;
    }

    public boolean getLegacyInfo()
    {
        return legacyInfo;
    }

    public boolean equals(Object o)
    {
        if (this == o)
        {
            return true;
        }
        if (!(o instanceof AuthenticationInfo))
        {
            return false;
        }
        AuthenticationInfo i = (AuthenticationInfo)o;
        if ((this.signature == null && i.getSignature() != null)
                || (this.signature != null && !this.signature.equals(i.getSignature()))
                || !equal(this.certChain, i.getCertChain()))
        {
            return false;
        }
        return true;
    }

    private boolean equal(String[] str1, String[] str2)
    {
        if (str1 == null && str2 == null)
        {
            return true;
        }
        if ((str1 == null && str2 != null)
                || (str1 != null && str2 == null)
                || str1.length != str2.length)
        {
            return false;
        }
        for (int i=0; i<str1.length; i++)
        {
            if (!str1[i].equals(str2[i]))
            {
                return false;
            }
        }
        return true;
    }

}
