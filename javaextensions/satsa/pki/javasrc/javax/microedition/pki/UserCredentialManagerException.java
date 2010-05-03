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


package javax.microedition.pki;

/**
 * Refer JSR-177 Specification for details
 */
public final class UserCredentialManagerException extends java.lang.Exception
{

    /**
    * Code returned if an appropriate certificate can not be found.
    *
    */
    public static final byte CREDENTIAL_NOT_FOUND = 5;
    /**
    * Code returned if a credential can not be added. For example, it
    * is returned if there is insufficient memory to add additional
    * credentials
    *
    */
    public static final byte CREDENTIAL_NOT_SAVED = 0;
    /**
    * Code returned if a security element does not support key generation.
    */
    public static final byte SE_NO_KEYGEN = 1;
    /**
    * Code returned if a security element does not have keys
    * available for certificate requests.
    *
    */
    public static final byte SE_NO_KEYS = 2;
    /**
    * Code returned if a security element does not have any keys
    * available that are not already associated with a certificate,
    * and if the platform does not allow reuse of keys that are
    * associated with an existing certificate.
    *
    */
    public static final byte SE_NO_UNASSOCIATED_KEYS = 3;
    /**
    * Code returned if an appropriate security element can not be found.
    *
    */
    public static final byte SE_NOT_FOUND = 4;

    private byte iReason;

    /**
    * Refer JSR-177 Specification for details
    */
    public UserCredentialManagerException(byte aCode)
    {
        iReason = aCode;
    } // UserCredentialManagerException constructor

    /**
    * Refer JSR-177 Specification for details
    */
    public byte getReason()
    {
        return iReason;
    }

} // UserCredentialManagerException
