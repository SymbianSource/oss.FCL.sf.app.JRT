/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.rt.midp;

import java.util.Hashtable;

/**
 * An excetion used for cancelling the start up. The cancel can be a result of
 * an error or the MIDlet start up denied by the user (e.g. auto invocation).
 * If it is denied by the user, then this exception is not considered as an
 * error.
 *
 * @author Nokia Corporation
 * @version $Rev$
 */
final class StartupException extends RuntimeException
{
    private boolean mIsError;

    /*** ----------------------------- PACKAGE ---------------------------- */

    /**
     * Will construct StartupException to be considered as an error.
     * @param text Info about the problem.
     */
    StartupException(String text)
    {
        this(text, true);
    }

    /**
     * Will construct StartupException with possiblity for defining the
     * fatality.
     * @param text Info about the problem.
     * @param isError Should the exception be considered as an error.
     */
    StartupException(String text, boolean isError)
    {
        super(text);
        mIsError = isError;
    }

    /**
     * A getter for whether the exception should be considered as an error.
     * @return true if error.
     */
    boolean isError()
    {
        return mIsError;
    }
}
