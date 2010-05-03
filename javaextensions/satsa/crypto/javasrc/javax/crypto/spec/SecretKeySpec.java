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



package javax.crypto.spec;

/**
 * Refer JSR-177 Specification for details
 */
public class SecretKeySpec implements java.security.spec.KeySpec,
        java.security.Key
{

    private static final String FORMAT_RAW = "RAW";

    /**
     * the secret key algorithm
     */
    private String iAlgorithm;

    /**
     * key material
     */
    private byte[] iEncoded;

    /**
    * Refer JSR-177 Specification for details
    */
    public SecretKeySpec(byte[] aKey, int aOffset, int aLen, String aAlgorithm)
    {
        iAlgorithm = aAlgorithm;

        // Create correct length byte array for key material.
        iEncoded = new byte[aLen];

        // Copy key material to iEncoded member variable.
        // Array copy will throw correct exceptions if offset or
        // length are invalid.
        System.arraycopy(aKey, aOffset, iEncoded, 0, aLen);
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
    public String getFormat()
    {
        return FORMAT_RAW;
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public byte[] getEncoded()
    {
        // make a copy of encoded key material
        byte[] encoded = new byte[iEncoded.length];
        System.arraycopy(iEncoded, // source
                         0, // source position
                         encoded, // destination
                         0, // destination position
                         iEncoded.length // data length
                        );

        // return a copy of encoded data, member variable cannot be returned
        // because changes to it would cause changes to this class.
        return encoded;
    }
}
