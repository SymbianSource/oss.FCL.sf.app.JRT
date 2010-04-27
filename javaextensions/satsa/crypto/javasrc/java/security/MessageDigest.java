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


package java.security;

import com.nokia.mj.impl.security.MessageDigestImpl;
import java.security.PrivMessageDigestImpl;

/**
 * Refer JSR-177 Specification for details
 */
public abstract class MessageDigest
{

    // MessageDigest implementation
    private MessageDigestImpl iDigest;

    // Package private constructor.
    MessageDigest(MessageDigestImpl aDigest)
    {
        iDigest = aDigest;
    }

    /**
    * Refer JSR-177 Specification for details
    */
    static public MessageDigest getInstance(String aAlgorithm)
    throws NoSuchAlgorithmException
    {
        MessageDigestImpl digest = new MessageDigestImpl(aAlgorithm);
        return new PrivMessageDigestImpl(digest);
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public int digest(byte[] aBuf, int aOffset, int aLength)
    throws DigestException
    {
        return iDigest.digest(aBuf, aOffset, aLength);
    }

    // Resets the digest for further use.
    public void reset()
    {
        iDigest.reset();
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public void update(byte[] aInput, int aOffset, int aLength)
    {
        iDigest.update(aInput, aOffset, aLength);
    }
}
