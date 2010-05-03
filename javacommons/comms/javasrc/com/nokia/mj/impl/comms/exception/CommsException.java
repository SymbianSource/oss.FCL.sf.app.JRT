/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.comms.exception;

public class CommsException extends RuntimeException
{

    // Reference to the exception which caused this exception.
    private Throwable iRootException = null;

    /**
     * Constructor
     *
     * @param aMsg The error message associated with this exception
     */
    public CommsException(String aMsg)
    {
        super(aMsg);
    }

    /**
     * Constructor
     *
     * @param aMsg The error message associated with this exception
     * @param aRootException The exception which caused this exception
     */
    public CommsException(String aMsg, Throwable aRootException)
    {
        super(aMsg);
        iRootException = aRootException;
    }

    /**
     * Returns the root exception associated with this exception
     *
     * @return The root exception associated with this exception
     */
    public Throwable getRootException()
    {
        return iRootException;
    }
}
