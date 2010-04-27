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
import java.security.NoSuchAlgorithmException;
import java.security.spec.EncodedKeySpec;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.KeySpec;
import com.nokia.mj.impl.rt.support.Jvm;


/**
 * Refer JSR-177 Specification for details
 */
public class KeyFactoryImpl
{

    // Presents the algorithm this KeyFactory produces.
    private String iAlgorithm;

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
     *
     *
     */
    public KeyFactoryImpl(String aAlgorithm) throws NoSuchAlgorithmException
    {

        // aAlgorithm.toUpperCase() throwns NullPointerException if aAlgorithm
        // is null.
        if (!_isValidAlgorithm(aAlgorithm.toUpperCase()))
        {
            throw new NoSuchAlgorithmException("Unsupported algorithm: "
                                               + aAlgorithm);
        }

        iAlgorithm = aAlgorithm;
    }

    /**
     *
     *
     */
    public final PublicKey generatePublic(KeySpec aKeySpec)
    throws InvalidKeySpecException
    {
        byte[] encoded = null;
        String format = null;

        if (aKeySpec instanceof EncodedKeySpec)
        {
            // EncodedKeySpec is the only supported specification
            EncodedKeySpec encodedKeySpec = (EncodedKeySpec) aKeySpec;

            //
            encoded = encodedKeySpec.getEncoded();

            // 'X.509' if type is X509EncodedKeySpec
            format = encodedKeySpec.getFormat();
        }
        else
        {
            // Specification is not supported.
            throw new InvalidKeySpecException("PublicKey generation failed: KeySpec type is not supported");
        }

        if (encoded == null || format == null)
        {
            // there must encoding and format
            throw new InvalidKeySpecException("PublicKey generation failed: format or encoded key is null");
        }

        int ret = _isValidKey(iAlgorithm.toUpperCase(), format.toUpperCase(),
                              encoded);
        if (ret < 0)
        {
            throw new InvalidKeySpecException("PublicKey generation failed: invalid key specification");
        }

        // Create public key from key material and return it.
        return new PublicKeyImpl(iAlgorithm, encoded, format);
    }

    /**
     * Checks if algorithm is supported by the implementation.
     *
     * @param aAlgorithm
     *            Algorithm name in upper case letters.
     * @return true if algorithm is supported.
     */
    static private native boolean _isValidAlgorithm(String aAlgorithm);

    /**
     *
     */
    static private native int _isValidKey(String aKeyAlgorithm, String aFormat,
                                          byte[] aKeyEncoded);
}
