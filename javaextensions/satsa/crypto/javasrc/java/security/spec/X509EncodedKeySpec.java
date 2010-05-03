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



package java.security.spec;

/**
 * Refer JSR-177 Specification for details
 */
public class X509EncodedKeySpec extends EncodedKeySpec
{
    /**
     * Key encoding format.
     */
    private static final String FORMAT_X509 = "X.509";

    /**
    * Refer JSR-177 Specification for details
    */
    public X509EncodedKeySpec(byte[] aEncodedKey)
    {
        super(aEncodedKey);
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public final String getFormat()
    {
        return FORMAT_X509;
    }
}