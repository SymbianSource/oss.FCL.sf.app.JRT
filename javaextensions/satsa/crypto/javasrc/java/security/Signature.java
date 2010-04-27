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


import com.nokia.mj.impl.security.SignatureImpl;
import java.security.PrivSignatureImpl;

/**
 * Refer JSR-177 Specification for details
 */
public abstract class Signature
{
    /**
     * Signature implementation
     */
    private SignatureImpl iSignature;

    /**
     * Package private constructor.
     */
    Signature(SignatureImpl aSignature)
    {
        iSignature = aSignature;
    }

    /**
    * Refer JSR-177 Specification for details
    */
    static public Signature getInstance(String aAlgorithm)
    throws NoSuchAlgorithmException
    {
        SignatureImpl signatureImpl = new SignatureImpl(aAlgorithm);
        return new PrivSignatureImpl(signatureImpl);
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public final void initVerify(PublicKey aPublicKey)
    throws InvalidKeyException
    {
        iSignature.initVerifyImpl(aPublicKey);
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public final void update(byte[] aData, int aOffset, int aLength)
    throws SignatureException
    {
        iSignature.updateImpl(aData, aOffset, aLength);
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public final boolean verify(byte[] aSignature) throws SignatureException
    {
        return iSignature.verifyImpl(aSignature);
    }

}
