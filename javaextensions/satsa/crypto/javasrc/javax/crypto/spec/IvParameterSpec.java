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
public class IvParameterSpec implements
        java.security.spec.AlgorithmParameterSpec
{
    /**
     * the initialization vector (IV)
     */
    private final byte[] iIV;

    /**
    * Refer JSR-177 Specification for details
    */
    public IvParameterSpec(byte[] aIv, int aOffset, int aLen)
    {
        // create correct length iv
        iIV = new byte[aLen];

        // copy iv to member variable
        // System.arraycopy will throws correct exceptions if
        // array offset or length are incorrect.
        System.arraycopy(aIv, aOffset, iIV, 0, aLen);
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public byte[] getIV()
    {
        // make a copy of iv
        byte[] iv = new byte[iIV.length];
        System.arraycopy(iIV, // source
                         0, // source position
                         iv, // destination
                         0, // destination position
                         iIV.length // data length
                        );

        // return a copy of iv, member variable cannot be returned because
        // changes to it would cause changes to this class.
        return iv;
    }
}
