/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mid.cenrep;

/**
 * Generic exception class for Central Repository API.
 */
public class CentralRepositoryException extends Exception
{

    /**
     * Default constructor is not allowed.
     */
    protected CentralRepositoryException()
    {
    }

    /**
     * Constructs an exception instance with a textual information.
     *
     * @param message human readable information about the exception.
     */
    public CentralRepositoryException(String message)
    {
        super(message);
    }

}
