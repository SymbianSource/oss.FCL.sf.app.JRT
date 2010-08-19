/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface indicating changes in the playback state.
*
*/


package com.nokia.mid.sound;

/**
 * <p>
 * This interface is used by applications which need to receive events
 * that indicate changes in the playback state of the Sound objects.
 * </p>
 * @see com.nokia.mid.sound.Sound
 * @version 1.1
 * @since 1.0
 */

public interface SoundListener
{

    /**
     *  Called when playback state of an Sound has been changed.
     *  Listener will be notified when playback has been started
     *  or stopped.
     *  @see com.nokia.mid.sound.Sound#setSoundListener(SoundListener listener)
     *  @see com.nokia.mid.sound.Sound#SOUND_PLAYING
     *  @see com.nokia.mid.sound.Sound#SOUND_STOPPED
     *  @param sound the sound object this event relates to
     *  @param event the sound changed event, SOUND_PLAYING or SOUND_STOPPED
     *  @since 1.0
     */
    public abstract void soundStateChanged(Sound sound, int event);

}
