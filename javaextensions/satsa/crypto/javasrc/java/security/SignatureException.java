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

/**
 * This is the generic Signature exception.
 */
public class SignatureException extends GeneralSecurityException
{
    /**
    * Refer JSR-177 Specification for details
    */
    public SignatureException()
    {
        super();
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public SignatureException(String aMsg)
    {
        super(aMsg);
    }
}