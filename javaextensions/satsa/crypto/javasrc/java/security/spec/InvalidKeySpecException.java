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


import java.security.GeneralSecurityException;

/**
 * This is the exception for invalid key specifications.
 */
public class InvalidKeySpecException extends GeneralSecurityException
{
    /**
    * Refer JSR-177 Specification for details
    */
    public InvalidKeySpecException()
    {
        super();
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public InvalidKeySpecException(String aMsg)
    {
        super(aMsg);
    }
}