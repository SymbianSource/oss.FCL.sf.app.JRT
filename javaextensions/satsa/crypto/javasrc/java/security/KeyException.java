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
 * This is the basic key exception.
 */
public class KeyException extends GeneralSecurityException
{
    /**
    * Refer JSR-177 Specification for details
    */
    public KeyException()
    {
        super();
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public KeyException(String aMsg)
    {
        super(aMsg);
    }
}