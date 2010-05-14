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
* Description:  Manager.playTone implementation
*
*/

package com.nokia.microedition.media.tone;

import javax.microedition.media.Manager;
import javax.microedition.media.Player;
import javax.microedition.media.PlayerListener;
import javax.microedition.media.control.ToneControl;
import javax.microedition.media.MediaException;
import java.io.IOException;
import java.io.ByteArrayInputStream;
import java.util.Vector;

/**
 * Manager.playTone implementation
 *
 */
public class PlayToneImpl implements PlayerListener
{

    // MEMBERS
    private static final String TONE_SEQUENCE_CONTENT_TYPE = "audio/x-tone-seq";
    private static final int TONE_SEQUENCE_VERSION = 1;
    private static final int TONE_SEQUENCE_RESOLUTION = 64;
    private static final int TONE_SEQUENCE_TEMPO = 30;
    private static final int DURATION_DIVIDE = 240000;
    private static final String CANNOT_PLAY_TONE = "Cannot play tone";

    // Holds tone players
    private Vector iPlayers = new Vector();

    /**
     * Constructor
     */
    public PlayToneImpl()
    {
    }

    /**
     * Play tone.
     * @see javax.microedition.media.Manager.playTone()
     */
    synchronized public void playTone(int aNote, int aDuration, int aVolume)
    throws MediaException
    {
        if (aVolume < MidiToneConstants.TONE_MIN_VOLUME)
        {
            aVolume = MidiToneConstants.TONE_MIN_VOLUME;
        }
        else if (aVolume > MidiToneConstants.TONE_MAX_VOLUME)
        {
            aVolume = MidiToneConstants.TONE_MAX_VOLUME;
        }

        if (aNote > MidiToneConstants.TONE_MAX_NOTE ||
                aNote < MidiToneConstants.TONE_MIN_NOTE)
        {
            throw new IllegalArgumentException("Note is out of range, " +
                                               "valid range is 0 <= Note <= 127");
        }

        if (aDuration <= 0)
        {
            throw new IllegalArgumentException("Duration must be positive");
        }

        int duration = aDuration * TONE_SEQUENCE_RESOLUTION *
                       TONE_SEQUENCE_TEMPO / DURATION_DIVIDE;

        if (duration < MidiToneConstants.TONE_SEQUENCE_NOTE_MIN_DURATION)
        {
            duration = MidiToneConstants.TONE_SEQUENCE_NOTE_MIN_DURATION;
        }
        else if (duration > MidiToneConstants.TONE_SEQUENCE_NOTE_MAX_DURATION)
        {
            duration = MidiToneConstants.TONE_SEQUENCE_NOTE_MAX_DURATION;
        }

        byte[] sequence =
        {
            ToneControl.VERSION, TONE_SEQUENCE_VERSION,
            ToneControl.TEMPO, TONE_SEQUENCE_TEMPO,
            ToneControl.RESOLUTION, TONE_SEQUENCE_RESOLUTION,
            ToneControl.SET_VOLUME, (byte)aVolume,
            (byte)aNote, (byte)duration
        };

        Player p = null;
        try
        {
            p = Manager.createPlayer(
                    new ByteArrayInputStream(sequence),
                    TONE_SEQUENCE_CONTENT_TYPE);
        }
        catch (IOException ioe)
        {
            iPlayers.removeElement(p);
            throw new MediaException(CANNOT_PLAY_TONE + " " + ioe.getMessage());
        }

        p.addPlayerListener(this);
        iPlayers.addElement(p);
        try
        {
            p.start();
        }
        catch (MediaException me)
        {
            iPlayers.removeElement(p);
            throw me;
        }
    }

    /**
     * @see javax.microedition.media.PlayerListener.playerUpdate()
     */
    public void playerUpdate(Player aPlayer, java.lang.String aEvent,
                             java.lang.Object aEventData)
    {
        if ((aEvent == END_OF_MEDIA) || (aEvent == ERROR))
        {
            aPlayer.close();
            iPlayers.removeElement(aPlayer);
        }
    }
}

// End of File
