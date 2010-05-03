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


// PACKAGE
package java.security.spec;

/**
 * This class represents a public key in encoded format.
 */
public abstract class EncodedKeySpec implements KeySpec
{
    /**
     * Encoded key material
     */
    private byte[] iEncodedKey;

    /**
    * Refer JSR-177 Specification for details
    */
    public EncodedKeySpec(byte[] aEncodedKey)
    {
        if (aEncodedKey == null)
        {
            throw new NullPointerException("Key is null.");
        }

        // Create correct length byte array for key.
        iEncodedKey = new byte[aEncodedKey.length];

        // Copy key to iEncodedKey member variable.
        System.arraycopy(aEncodedKey, // source
                         0, // source position
                         iEncodedKey, // destination
                         0, // destination position
                         aEncodedKey.length // data length
                        );
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public byte[] getEncoded()
    {
        // make a copy of encoded key
        byte[] encoded = new byte[iEncodedKey.length];
        System.arraycopy(iEncodedKey, // source
                         0, // source position
                         encoded, // destination
                         0, // destination position
                         iEncodedKey.length // data length
                        );

        // return a copy of encoded data, member variable cannot be returned
        // because changes to it would cause changes to this class.
        return encoded;
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public abstract String getFormat();
}
