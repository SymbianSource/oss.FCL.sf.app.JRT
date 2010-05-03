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


package javax.microedition.midlet;

/**
 * An exception for the MIDlet to inform that it doesn't want to do the state
 * change requested by the life cycle.
 * @see MIDP spec for further details.
 *
 * @author Nokia Corporation
 * @version $Rev$
 */
public class MIDletStateChangeException extends Exception
{

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
    * The default constructor of the MIDletStateChangeException without any
    * message.
    */
    public MIDletStateChangeException()
    {
    }

    /**
    * The constructor of the MIDletStateChangeException with a detailed
    * message.
    * @param message The detailed message.
    */
    public MIDletStateChangeException(String message)
    {
        super(message);
    }
}
