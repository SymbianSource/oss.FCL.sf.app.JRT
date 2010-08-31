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
* Description: Control for routing the player's audio output.
*
*/

package com.nokia.mid.media;

/**
 * <code>AudioOutput</code> 
 * Encapsulates current audio output mode and output device(s) used. 
 * <p>
 * <p>
 * @since 1.4
 * <p> 
 */
public interface AudioOutput
{
   /**
    * Get active output mode.
    * <p>
    * The different modes are defined in AudioOutputControl class.
    *
    * @return The current audio output mode or -1 if unable to determine
    *
    * @exception SecurityException with message "Permission denied" if the
    * called does not have permission to set the audio routing.
    */
    public int getActiveOutputMode();
    
    /**
     * Get supported output devices
     * <p>
     *  returns array of audio output device types if it is supported
     * otherwise returns "null" 
    */
    public int[] getOutputDevices();
}
