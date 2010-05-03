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


package javax.crypto;

import java.security.NoSuchAlgorithmException;
import com.nokia.mj.impl.crypto.CipherImpl;

/**
 * Refer JSR-177 Specification for details
 */
public class Cipher
{
    /**
     * Constant used to initialize cipher to encryption mode.
     */
    static public final int ENCRYPT_MODE = 1;

    /**
     * Constant used to initialize cipher to decryption mode.
     */
    static public final int DECRYPT_MODE = 2;

    /**
     * This class delegates all method calls to CipherImpl object.
     */
    private CipherImpl iCipher;

    /**
     * Private constructor. This class must be created using getInstance method
     *
     * @param aTransformation -
     *            the name of the transformation
     */
    private Cipher(String aTransformation) throws NoSuchAlgorithmException,
                NoSuchPaddingException
    {
        iCipher = new CipherImpl(aTransformation);
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public static final Cipher getInstance(String aTransformation)
    throws NoSuchAlgorithmException, NoSuchPaddingException
    {
        return new Cipher(aTransformation);
    }


    /**
    * Refer JSR-177 Specification for details
    */
    public final int doFinal(byte[] aInput, int aInputOffset, int aInputLen,
                             byte[] aOutput, int aOutputOffset) throws IllegalStateException,
                ShortBufferException, IllegalBlockSizeException,
                BadPaddingException
    {
        return iCipher.doFinal(aInput, aInputOffset, aInputLen, aOutput,
                               aOutputOffset);
    }


    /**
    * Refer JSR-177 Specification for details
    */
    public final byte[] getIV()
    {
        return iCipher.getIV();
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public final void init(int aOpMode, java.security.Key aKey)
    throws java.security.InvalidKeyException
    {
        iCipher.init(aOpMode, aKey);
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public final void init(int aOpMode, java.security.Key aKey,
                           java.security.spec.AlgorithmParameterSpec aParams)
    throws java.security.InvalidKeyException,
                java.security.InvalidAlgorithmParameterException
    {
        iCipher.init(aOpMode, aKey, aParams);
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public final int update(byte[] aInput, int aInputOffset, int aInputLen,
                            byte[] aOutput, int aOutputOffset) throws IllegalStateException,
                ShortBufferException
    {
        return iCipher.update(aInput, aInputOffset, aInputLen, aOutput,
                              aOutputOffset);
    }
};
