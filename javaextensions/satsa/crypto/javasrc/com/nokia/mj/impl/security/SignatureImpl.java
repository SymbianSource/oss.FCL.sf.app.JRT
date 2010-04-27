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
import java.security.InvalidKeyException;
import java.security.SignatureException;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.rt.support.Finalizer;


/**
 * Refer JSR-177 Specification for details
 */
public class SignatureImpl
{

    // Native handle. Set in constructor.
    private final int iHandle;
    private Finalizer iFinalizer;

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
    public SignatureImpl(String aAlgorithm) throws NoSuchAlgorithmException
    {
        // Native side accept only upper case strings.
        iHandle = _create(aAlgorithm.toUpperCase());

        if (iHandle <= 0)
        {
            // Failed to create native object.
            throw new NoSuchAlgorithmException("Unsupported algorithm: "
                                               + aAlgorithm);

        }
        // else native object is created and handle points to native object
        iFinalizer = registerForFinalization();

    }

    /**
     * Registers with Finalizer to call a method when the object gets collected
     * by GC
     *
     * @return Finalizer object that will be notified when GC happens
     */
    private Finalizer registerForFinalization()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    void doFinalize()
    {

        if (iFinalizer == null)
        {
            return;
        }
        iFinalizer = null;
        _dispose(iHandle);

    }


    /**
     * From Signature
     * @see java.security.Signature
     */
    public void initVerifyImpl(PublicKey aPublicKey) throws InvalidKeyException
    {
        if (aPublicKey == null)
        {
            // Key cannot be null.
            throw new IllegalArgumentException("PublicKey is null.");
        }

        // Do native verify with key contents.
        // Native side accept only upper case strings.
        int err = _initVerify(iHandle, aPublicKey.getAlgorithm().toUpperCase(),
                              aPublicKey.getFormat().toUpperCase(), aPublicKey.getEncoded());

        if (err < 0)
        {
            // verify did not succeed.
            throw new InvalidKeyException("Invalid key: encoding or length is false, or it is uninitialized");
        }
        // else initVerify succeed
    }

    /**
     * From Signature
     * @see java.security.Signature
     */
    public void updateImpl(byte[] aData, int aOffset, int aLength)
    throws SignatureException
    {
        if (aData == null)
        {
            // Data cannot be null.
            throw new IllegalArgumentException("Data is null.");
        }

        if (aLength == 0)
        {
            // Nothing to update, just return.
            return;
        }

        // Check array boundaries.
        if ((aData.length < aOffset + aLength) || (aOffset < 0)
                || (aLength < 0) || (aOffset >= aData.length))
        {
            // Data do not fit to input buffer
            throw new IllegalArgumentException("Array out of bounds.");
        }

        int err = _update(iHandle, aData, aOffset, aLength);

        if (err <= 0)
        {
            // Native update failed
            throw new SignatureException("Update failed");
        }
        // else update succeed
    }

    /**
     * From Signature
     * @see java.security.Signature
     */
    public boolean verifyImpl(byte[] aSignature) throws SignatureException
    {
        if (aSignature == null)
        {
            // Signature cannot be null
            throw new IllegalArgumentException("Signature is null.");
        }

        // Do verify to native object
        int retVal = _verify(iHandle, aSignature);
        if (retVal < 0)
        {
            // Native verify failed.
            throw new SignatureException("Verification failed: invalid signature");
        }

        // There was no errors and now retVal is >= 0.
        // If retVal == 0 return false and if retVal > 0 return true.
        return retVal > 0;
    }

    // NATIVE METHODS

    /**
     * Creates new native object for algorithm.
     *
     * @param aAlgorithm
     *            The standard name of the algorithm requested. Whole string
     *            must be in upper case letters.
     * @return Native handle ( value > 0 ) or error (value <= 0)
     */
    static private native int _create(String aAlgorithm);

    /**
     * Initializes native signature object with a key.
     *
     * @param aHandle
     *            Handle to corresponding native object.
     * @param aKeyAlgorithm
     *            The name of the algorithm associated with this key. Must be in
     *            upper case letters.
     * @param aKeyFormat
     *            The primary encoding format of the key. Must be in upper case
     *            letters.
     * @param aKeyEncoded
     *            The encoded key, or null if the key does not support encoding.
     * @return Negative value if an error or >= 0 if success.
     */
    static private native int _initVerify(int aHandle, String aKeyAlgorithm,
                                          String aKeyFormat, byte[] aKeyEncoded);

    /**
     * Updates native object.
     *
     * @param aHandle
     *            Handle to corresponding native object.
     * @param aData
     *            The array of bytes.
     * @param aOffset
     *            The offset to start from in the array of bytes.
     * @param aLength
     *            The number of bytes to use, starting at offset.
     * @return Negative value if an error or >= 0 if success.
     */
    static private native int _update(int aHandle, byte[] aData, int aOffset,
                                      int aLength);

    /**
     * Makes native verify operation.
     *
     * @param aHandle
     *            Handle to corresponding native object.
     * @param aSignature
     *            The signature bytes to be verified.
     * @return value 0: false, value > 0: true, value < 0: error
     */
    static private native int _verify(int aHandle, byte[] aSignature);

    static private native void _dispose(int aHandle);

}

