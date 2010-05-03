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
 * This exception thrown in the case the MIDlet doesn't want to do the
 * requested state change.
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public class IllegalStateException extends RuntimeException
{
    /**
     *  The default constructor.
     */
    public IllegalStateException()
    {
        super();
    }

    /**
     *  Constructor with the specified message.
     *
     * @param s The message associated with the exception.
     */
    public IllegalStateException(String s)
    {
        super(s);
    }
}
