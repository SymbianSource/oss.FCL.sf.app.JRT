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
* Description:  Cipher interface implementation
 *
*/



package com.nokia.mj.impl.crypto;


import java.security.InvalidKeyException;
import java.security.InvalidAlgorithmParameterException;
import java.security.NoSuchAlgorithmException;
import javax.crypto.Cipher;
import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.ShortBufferException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.IvParameterSpec;
import com.nokia.mj.impl.satsa.SecurityError;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Finalizer;



/**
 * Refer JSR-177 Specification for details
 */
public class CipherImpl
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
    public CipherImpl(String aTransformation) throws NoSuchAlgorithmException,
                NoSuchPaddingException
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ CipherImpl::CipherImpl");
        if (aTransformation == null)
        {
            // TCK requires NoSuchAlgorithmException to be thrown if
            // transformation is null.
            throw new NoSuchAlgorithmException("Transformation is null");
        }

        // Create native object
        // Native side accepts only upper case strings
        iHandle = _create(aTransformation.toUpperCase());

        // Check error
        if (iHandle <= 0)
        {

            // Failed to create native object.
            if (iHandle == SecurityError.ERR_NO_MEMORY)
            {
                throw new OutOfMemoryError();
            }
            else if (iHandle == SecurityError.ERR_NO_SUCH_PADDING)
            {
                // invalid padding
                throw new NoSuchPaddingException("Invalid or unsupported padding scheme");
            }
            else
            {
                // algorithm or mode is not supported or
                // invalid transformation string
                throw new NoSuchAlgorithmException("No such algorithm");
            }
        }
        // else Native object is OK
        iFinalizer = registerForFinalization();
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "-- CipherImpl::CipherImpl");
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
     * From Cipher.
     *
     * @see javax.crypto.Cipher
     */
    public int doFinal(byte[] aInput, int aInputOffset, int aInputLen,
                       byte[] aOutput, int aOutputOffset) throws IllegalStateException,
                ShortBufferException, IllegalBlockSizeException,
                BadPaddingException
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ CipherImpl::doFinal");
        // the number of bytes stored in output
        int bytes = 0;
        try
        {
            bytes = privUpdate(aInput, aInputOffset, aInputLen, aOutput,
                               aOutputOffset, true);
        }
        catch (IllegalBlockSizeException ibse)
        {
            throw new IllegalBlockSizeException("doFinal method failed: " + ibse.getMessage());
        }
        catch (IllegalStateException ise)
        {
            throw new IllegalStateException("doFinal method failed: " + ise.getMessage());
        }
        catch (ShortBufferException sbe)
        {
            throw new ShortBufferException("doFinal method failed: " + sbe.getMessage());
        }
        catch (BadPaddingException bpe)
        {
            throw new BadPaddingException("doFinal method failed: " + bpe.getMessage());
        }

        return bytes;
    }


    /**
     * From Cipher.
     *
     * @see javax.crypto.Cipher
     */
    public byte[] getIV()
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ CipherImpl::getIV");
        byte[] nativeIV = _getIV(iHandle);

        // null if the underlying algorithm does not use an IV,
        // or if the IV has not yet been set.
        byte[] retIV = null;

        if (nativeIV != null)
        {
            // Copy iv to new buffer, it is not a reference to a native buffer.
            retIV = new byte[nativeIV.length];

            // src, src_position, dst, dst_position, length
            System.arraycopy(nativeIV, 0, retIV, 0, nativeIV.length);
        }
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "-- CipherImpl::getIV");
        return retIV;
    }

    /**
     * From Cipher.
     *
     * @see javax.crypto.Cipher
     */
    public void init(int aOpMode, java.security.Key aKey)
    throws java.security.InvalidKeyException
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ CipherImpl::init");
        try
        {
            // Parameters are checked in privInit.
            privInit(aOpMode, aKey, null);
        }
        catch (java.security.InvalidAlgorithmParameterException iape)
        {
            // If this cipher requires any algorithm parameters that cannot be
            // derived from the given key, the underlying cipher implementation
            // is supposed to generate the required parameters itself
            // (using provider-specific default or random values) if it is
            // being initialized for encryption, and raise an
            // InvalidKeyException
            // if it is being initialized for decryption.
            throw new InvalidKeyException(iape.getMessage());
        }
    }

    /**
     * From Cipher.
     *
     * @see javax.crypto.Cipher
     */
    public void init(int aOpMode, java.security.Key aKey,
                     java.security.spec.AlgorithmParameterSpec aParams)
    throws java.security.InvalidKeyException,
                java.security.InvalidAlgorithmParameterException
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ CipherImpl::init with params");
        // If this cipher requires any algorithm parameters and params is
        // null, the underlying cipher implementation is supposed to
        // generate the required parameters itself
        byte[] params = null;

        if (aParams != null)
        {
            // IvParameterSpec is the only algorithm specification
            // defined in 177
            if (!(aParams instanceof javax.crypto.spec.IvParameterSpec))
            {
                throw new InvalidAlgorithmParameterException(
                    "Cipher initializaton failed: algorithm parameter type is not supported");
            }

            // aParams is typeof IvParameterSpec. Get byte array presenting
            // the IV.
            params = ((IvParameterSpec) aParams).getIV();

            if (params == null)
            {
                throw new InvalidAlgorithmParameterException("Cipher initializaton failed: initialization vector is null");
            }
        }

        privInit(aOpMode, aKey, params);
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "-- CipherImpl::init");
    }

    /**
     * From Cipher.
     *
     * @see javax.crypto.Cipher
     */
    public int update(byte[] aInput, int aInputOffset, int aInputLen,
                      byte[] aOutput, int aOutputOffset) throws IllegalStateException,
                ShortBufferException
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ CipherImpl::update");
        // the number of bytes stored in output
        int bytes = 0;

        try
        {
            bytes = privUpdate(aInput, aInputOffset, aInputLen, aOutput,
                               aOutputOffset, false);

            // update cannot throw IllegalBlockSizeException or
            // BadPaddingException. When updating these should never occur,
            // but it is not necessarily to throw runtime exception.
        }
        catch (IllegalBlockSizeException ibse)
        {
            throw new ShortBufferException("Update method failed: " + ibse.getMessage());
        }
        catch (BadPaddingException bpe)
        {
            throw new ShortBufferException("Update method failed: " + bpe.getMessage());
        }
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "-- CipherImpl::update");
        return bytes;
    }

    // PRIVATE METHODS

    /**
     * Private update method. Called from update or doFinal methods.
     *
     */
    private int privUpdate(byte[] aInput, int aInputOffset, int aInputLen,
                           byte[] aOutput, int aOutputOffset, boolean aDoFinal)
    throws IllegalStateException, ShortBufferException,
                IllegalBlockSizeException, BadPaddingException
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ CipherImpl::privUpdate");
        // array offsets and length must be positive
        if (aInputOffset < 0 || aInputLen < 0 || aOutputOffset < 0)
        {
            throw new IllegalArgumentException("Array offset or length is negative");
        }
        else if (aInput.length < aInputOffset + aInputLen)
        {
            // Data do not fit to input buffer
            throw new IllegalArgumentException("Array is out of bounds: data do not fit to input buffer");
        }

        // If aInputLen is 0, offset values may not be checked because of TCK
        if (aInputLen > 0)
        {
            // Offset must fit to array boundary.
            if (aInput.length <= aInputOffset)
            {
                // offset cannot exceed the length
                throw new IllegalArgumentException(
                    "Input offset exceeds array length");
            }
            else if (aOutput.length <= aOutputOffset)
            {
                // offset cannot exceed the length
                throw new IllegalArgumentException(
                    "Output offset exceeds array length");
            }
        }

        // retVal is bytes written or error code ( negative value ).
        int retVal = _update(iHandle, aInput, aInputOffset, aInputLen, aOutput,
                             aOutputOffset, aDoFinal);

        // Check error. 0 is not an error, because it is allowed that update
        // does not make any output
        if (retVal < 0)
        {
            // Native operation failed
            if (retVal == SecurityError.ERR_NO_MEMORY)
            {
                throw new OutOfMemoryError();
            }
            else if (retVal == SecurityError.ERR_ILLEGAL_STATE)
            {
                throw new IllegalStateException("init not called successfully");
            }
            else if (retVal == SecurityError.ERR_ILLEGAL_STATE_INPUT)
            {
                throw new IllegalStateException("invalid input length");
            }
            else if (retVal == SecurityError.ERR_SHORT_BUFFER)
            {
                throw new ShortBufferException("Updating cipher failed: output buffer too short");
            }
            else if (retVal == SecurityError.ERR_ILLEGAL_BLOCK_SIZE)
            {
                throw new IllegalBlockSizeException("illegal block size");
            }
            else
            {
                // Otherwise padding exception
                throw new BadPaddingException("bad padding");
            }
        }
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "-- CipherImpl::privUpdate");
        // else Update OK and returned bytes processed.
        return retVal;
    }

    /**
     * Private init method. Used from both overloaded init methods.
     *
     */
    private void privInit(int aOpMode, java.security.Key aKey, byte[] aParams)
    throws java.security.InvalidKeyException,
                java.security.InvalidAlgorithmParameterException
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ CipherImpl::privInit");
        if (aOpMode != Cipher.DECRYPT_MODE && aOpMode != Cipher.ENCRYPT_MODE)
        {
            // Invalid mode
            throw new IllegalArgumentException("Invalid mode: only encrypt and decrypt are supported");
        }
        else if (aKey == null)
        {
            // key cannot be null
            throw new InvalidKeyException("Key is null");
        }

        // make native init with given key and params
        int err = _init(iHandle, aOpMode, aKey.getAlgorithm().toUpperCase(),
                        aKey.getFormat().toUpperCase(), aKey.getEncoded(), aParams);

        // Check error
        if (err < 0)
        {
            if (err == SecurityError.ERR_NO_MEMORY)
            {
                throw new OutOfMemoryError();
            }
            else if (err == SecurityError.ERR_INVALID_ALGORITHM_PARAMETER)
            {
                throw new InvalidAlgorithmParameterException(
                    "Invalid parameter");
            }
            else
            {
                // Default exception
                throw new InvalidKeyException("Invalid key");
            }
        }
        // else init OK
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "-- CipherImpl::privInit");
    }

    // NATIVE METHODS

    /**
     * Creates native instance or returns error if construction fails.
     *
     * @param aTransformation
     *            The name of the transformation in upper case.
     * @return Native handle or error code.
     */
    static private native int _create(String aTransformation);

    /**
     * Initializes native cipher with key material, algorithm parameters and
     * operation mode ( DECRYPT_MODE or ENCRYPT_MODE ).
     */
    static private native int _init(int aHandle, int aOpMode,
                                    String aKeyAlgorithm, String aKeyFormat, byte[] aKeyEncoded,
                                    byte[] aParams);

    /**
     * Makes Updates or DoFinal operation to native object accordingly to
     * aDoFinal parameter.
     */
    static private native int _update(int aHandle, byte[] aInput,
                                      int aInputOffset, int aInputLen, byte[] aOutput, int aOutputOffset,
                                      boolean aDoFinal);

    /**
     * Returns initialization vector from native implementation.
     *
     * @param aHandle
     *            Handle to corresponding native object.
     * @return Initialization vector or null if vector is not available.
     */
    static private native byte[] _getIV(int aHandle);

    static private native void _dispose(int aHandle);

}
