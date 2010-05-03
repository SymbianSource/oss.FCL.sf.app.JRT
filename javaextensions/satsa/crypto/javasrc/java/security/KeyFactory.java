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


import java.security.spec.InvalidKeySpecException;
import java.security.spec.KeySpec;
import com.nokia.mj.impl.security.KeyFactoryImpl;

/**
 * Refer JSR-177 Specification for details
 */
public class KeyFactory
{
    /**
     * Key factory implementation.
     */
    private KeyFactoryImpl iFactory;

    /**
     * Private constructor creates KeyFactoryImpl object.
     */
    private KeyFactory(String aAlgorithm) throws NoSuchAlgorithmException
    {
        iFactory = new KeyFactoryImpl(aAlgorithm);
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public static KeyFactory getInstance(String aAlgorithm)
    throws NoSuchAlgorithmException
    {
        return new KeyFactory(aAlgorithm);
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public final PublicKey generatePublic(KeySpec aKeySpec)
    throws InvalidKeySpecException
    {
        return iFactory.generatePublic(aKeySpec);
    }

}