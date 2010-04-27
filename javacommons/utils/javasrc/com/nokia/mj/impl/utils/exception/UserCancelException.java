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

package com.nokia.mj.impl.utils.exception;

/**
 * Exception for indicating that user has cancelled operation.
 * Security framework throws this exception when user cancels
 * security prompt dialog.
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public class UserCancelException extends RuntimeException
{

    // Exception which indicates more detailed cancellation reason.
    private Throwable iRootException = null;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Constructor
     *
     * @param aMsg the message for this exception
     */
    public UserCancelException(String aMsg)
    {
        super(aMsg);
    }

    /**
     * Constructor
     *
     * @param aMsg the message for this exception
     * @param aRootException the exception which indicates more detailed
     * cancellation reason
     */
    public UserCancelException(String aMsg, Throwable aRootException)
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

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append(super.toString());
        if (getRootException() != null)
        {
            buf.append("\nRootException:\n");
            buf.append(getRootException().toString());
        }
        return buf.toString();
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */
}
