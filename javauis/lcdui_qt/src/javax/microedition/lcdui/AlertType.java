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
package javax.microedition.lcdui;

/**
 * Implementation of LCDUI <code>AlertType</code> class.
 */
public class AlertType extends Object {

    /**
     * Alarm Alert type.
     */
    public static final AlertType ALARM = new AlertType();

    /**
     * Confirmation Alert type.
     */
    public static final AlertType CONFIRMATION = new AlertType();

    /**
     * Error Alert type.
     */
    public static final AlertType ERROR = new AlertType();

    /**
     * Info Alert type.
     */
    public static final AlertType INFO = new AlertType();

    /**
     * Warning Alert type.
     */
    public static final AlertType WARNING = new AlertType();

    /**
     * Protected constructor for subclasses.
     */
    protected AlertType() {
    }

    /**
     * Play Alert sound for the user.
     *
     * @param display on which the sound should be played
     * @return true is the sound was played, false otherwise.
     */
    public boolean playSound(Display display) {
        if (display == null) {
            throw new NullPointerException(
                    MsgRepository.ALERTTYPE_EXCEPTION_PLAYSOUND_NULL_POINTER);
        }
        // TODO: eSWT support required
        return false;
    }

}
