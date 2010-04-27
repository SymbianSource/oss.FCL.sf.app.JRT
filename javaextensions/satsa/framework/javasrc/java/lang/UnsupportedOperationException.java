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


package java.lang;

/**
 * Thrown to indicate that the requested operation is not supported.
 */
public class UnsupportedOperationException extends RuntimeException
{
    /**
     * Constructs an UnsupportedOperationException with no detail message.
     */
    public UnsupportedOperationException()
    {
    }

    /**
     * Constructs an UnsupportedOperationException with the specified detail
     * message.
     * @param aMsg the detail message
     */
    public UnsupportedOperationException(String aMsg)
    {
        super(aMsg);
    }
}
