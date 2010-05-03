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


import java.security.NoSuchAlgorithmException;
import java.security.DigestException;
import com.nokia.mj.impl.rt.support.Jvm;
import java.lang.String;
import com.nokia.mj.impl.satsa.SecurityError;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.utils.Logger;

/**
 * Refer JSR-177 Specification for details
 */
public class MessageDigestImpl
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

    public MessageDigestImpl(String aAlgorithm) throws NoSuchAlgorithmException
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ MessageDigestImpl::MessageDigestImpl");
        String algorithm = aAlgorithm.toUpperCase();

        // validate the algorithm
        if (algorithm.equals("SHA-1") || algorithm.equals("MD2")
                || algorithm.equals("MD5"))
        {
            iHandle = _create(aAlgorithm.toUpperCase());

            // Check error
            if (iHandle <= 0)
            {
                // Failed to create native object.
                if (iHandle == SecurityError.ERR_NO_MEMORY)
                {
                    throw new NoSuchAlgorithmException("Failed to create MessageDigest for algorithm: "
                                                       + aAlgorithm);
                }
            }
        }
        else
        {
            throw new NoSuchAlgorithmException("Unsupported algorithm: "
                                               + aAlgorithm);
        }

        // Native object OK
        iFinalizer = registerForFinalization();
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "-- MessageDigestImpl::MessageDigestImpl");

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
     * From MessageDigest
     * @see java.security.MessageDigest
     */
    public int digest(byte[] aBuf, int aOffset, int aLength)
    throws DigestException
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ MessageDigestImpl::digest");
        if (aBuf == null)
        {
            // TCK requires DigestException instead of NullPointerException
            throw new DigestException("Digest method failed: buffer is null");
        }
        else if ((aBuf.length < aOffset + aLength) && (aBuf.length <= aOffset))
        {
            // Data do not fit to output buffer
            throw new IllegalArgumentException("Digest method failed: output buffer out of bounds");
        }
        else if (aOffset < 0 || aLength < 0)
        {
            // offset and length must be positive
            throw new IllegalArgumentException("Digest method failed: offset or length is negative");
        }

        int retVal = _digest(iHandle, aBuf, aOffset, aLength);

        if (retVal < 0)
        {
            // Error occured
            if (retVal == SecurityError.ERR_NO_MEMORY)
            {
                throw new DigestException("Digest method failed: cannot allocate memory");
            }
        }

        // Return length of the bytes processed.
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "-- MessageDigestImpl::digest");
        return retVal;
    }

    /**
     * From MessageDigest
     * @see java.security.MessageDigest
     */
    public void reset()
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ MessageDigestImpl::reset");
        // Reset should always success, but checked to be sure.
        if (_reset(iHandle) < 0)
        {
            throw new OutOfMemoryError("Internal error");
        }
    }

    /**
     * From MessageDigest
     * @see java.security.MessageDigest
     */
    public void update(byte[] aInput, int aOffset, int aLength)
    {
        Logger.LOG(Logger.ESATSA, Logger.EInfo,
                   "+ MessageDigestImpl::update");
        if (aLength == 0)
        {
            // Nothing to update.
            return;
        }

        // aInput.length throws correct NullPointerException if aInput is null

        if ((aInput.length < aOffset + aLength) && (aInput.length <= aOffset))
        {
            // Data do not fit to input buffer
            throw new IllegalArgumentException("Update method failed: input array out of bounds.");
        }

        if (aOffset < 0 || aLength < 0)
        {
            // offset and length must be positive
            throw new IllegalArgumentException("Update method failed: offset or length is negative.");
        }

        int err = _update(iHandle, aInput, aOffset, aLength);

        if (err < 0)
        {
            // Error occured
            if (err == SecurityError.ERR_NO_MEMORY)
            {
                throw new IllegalArgumentException("Update method failed: cannot allocate memory");
            }
        }
    }

    /**
     * Creates new native object for algorithm.
     *
     * @param aAlgorithm
     *            Algorithm name in upper case letters.
     * @return Native handle ( value > 0 ) or error (value <= 0)
     */
    static private native int _create(String aAlgorithm);

    /**
     * Do native update operation.
     *
     * @param aHandle
     *            Handle to corresponding native object.
     * @param aInput
     *            The array of bytes.
     * @param aOffset
     *            The offset to start from in the array of bytes.
     * @param aLength
     *            The number of bytes to use, starting at offset.
     * @return Negative value if an error or >= 0 if success.
     */
    static private native int _update(int aHandle, byte[] aInput, int aOffset,
                                      int aLength);

    /**
     * Do native digest operation.
     *
     * @param aHandle
     *            Handle to corresponding native object.
     * @param aBuf
     *            The array of bytes.
     * @param aOffset
     *            The offset to start from in the array of bytes.
     * @param aLength
     *            The number of bytes to use, starting at offset.
     * @return Negative value if an error or data length written.
     */
    static private native int _digest(int aHandle, byte[] aBuf, int aOffset,
                                      int aLength);

    /**
     * Resets native object.
     *
     * @param aHandle
     *            Handle to corresponding native object.
     * @return Error code.
     */
    static private native int _reset(int aHandle);
    static private native void _dispose(int aHandle);
}
