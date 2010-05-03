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

public final class OcspData
{
    // constants for different responses
    public static final int RESPONSE_GOOD = 1;
    public static final int RESPONSE_REVOKED = 2;
    public static final int RESPONSE_UNKNOWN = 3;
    public static final int RESPONSE_CANNOT_OBTAIN_CERT_STATUS = 4;
    public static final int RESPONSE_INVALID_REVOCATION_SERVER_URI = 5;
    public static final int RESPONSE_SIGNATURE_VALIDATION_FAILURE = 6;
    public static final int RESPONSE_INVALID_REVOCATION_SERVER_RESPONSE = 7;
    public static final int RESPONSE_MISSING_NONCE = 8;
    public static final int RESPONSE_INVALID_CERT_STATUS_INFO = 9;

    // constant for the default response
    public static final int DEFAULT_RESPONSE = RESPONSE_CANNOT_OBTAIN_CERT_STATUS;

    // the cert chains
    private String[] certChain = null;

    // the summary response
    private int summary = DEFAULT_RESPONSE;

    // the individual responses corresponding to each
    // of the cert from the chain
    private int[] individualResponses = null;

    public OcspData(String[] certChain)
    {
        this.certChain = certChain;
    }

    public void setIndividualResponses(int[] individualResponses)
    {
        this.individualResponses = individualResponses;
    }

    public void setSummary(int summary)
    {
        this.summary = summary;
    }

    public int[] getIndividualResponses()
    {
        return this.individualResponses;
    }

    public int getSummary()
    {
        return this.summary;
    }

    public String[] getCertChain()
    {
        return this.certChain;
    }
}


