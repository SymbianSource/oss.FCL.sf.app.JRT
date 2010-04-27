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

package com.nokia.mj.impl.security.midp.authentication;

import com.nokia.mj.impl.security.common.Certificate;
import com.nokia.mj.impl.security.midp.common.AuthenticationCredentials;

public final class Credentials
        extends AuthenticationCredentials
{

    public Credentials(String protectionDomainName, String protectionDomainCategory, String jarHashValue, String rootHashValue, int validatedChainIndex, Certificate signingCert)
    {
        super(protectionDomainName, protectionDomainCategory);
        this.jarHashValue = jarHashValue;
        this.rootHashValue = rootHashValue;
        this.validatedChainIndex = validatedChainIndex;
        this.signingCert = signingCert;
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("\nDomain name:" + getProtectionDomainName());
        sb.append(" Domain category:" + getProtectionDomainCategory());
        sb.append("\nJar hash:" + jarHashValue);
        sb.append("\nRoot hash:" + rootHashValue);
        sb.append("\nValidated chain index:" + validatedChainIndex);
        sb.append("\nSigning cert:" + signingCert);
        return sb.toString();
    }

    String jarHashValue;
    String rootHashValue;
    int validatedChainIndex;
    Certificate signingCert;
}


