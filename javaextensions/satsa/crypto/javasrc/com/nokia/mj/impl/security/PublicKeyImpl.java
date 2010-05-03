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



package com.nokia.mj.impl.security;

import java.security.PublicKey;
import com.nokia.mj.impl.rt.support.Jvm;

/**
 * Refer JSR-177 Specification for details
 */
class PublicKeyImpl implements PublicKey
{
    private String iAlgorithm;

    private byte[] iEncoded;

    private String iFormat;

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javasatsa");
        }
        catch (Exception e)
        {

        }

    }

    /**
    * Refer JSR-177 Specification for details
    */
    public PublicKeyImpl(String aAlgorithm, byte[] aEncoded, String aFormat)
    {
        iAlgorithm = aAlgorithm;
        iFormat = aFormat;

        // Create correct length byte array for key.
        iEncoded = new byte[aEncoded.length];

        // Copy key to iEncoded member variable.
        System.arraycopy(aEncoded, 0, iEncoded, 0, aEncoded.length);
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public String getAlgorithm()
    {
        return iAlgorithm;
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public byte[] getEncoded()
    {
        // make a copy of encoded key
        byte[] encoded = new byte[iEncoded.length];
        System.arraycopy(iEncoded, 0, encoded, 0, iEncoded.length);

        // return a copy of encoded data, member variable cannot be returned
        // because changes to it would cause changes to this class.
        return encoded;

    }

    /**
    * Refer JSR-177 Specification for details
    */
    public String getFormat()
    {
        return iFormat;
    }
}
